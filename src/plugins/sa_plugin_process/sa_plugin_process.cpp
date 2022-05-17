/**********************************************************************************
 * MIT License
 *
 * Copyright (c) 2018 Antoine Beauchamp
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#include <cstdio>
#include "shellanything/sa_logging.h"
#include "shellanything/sa_error.h"
#include "shellanything/sa_plugin.h"
#include "shellanything/sa_property_store.h"
#include "shellanything/sa_properties.h"
#include <string>
#include <sstream>      // std::stringstream, std::stringbuf
#include <map>
#include <Windows.h>
#include <psapi.h>
#pragma comment( lib, "psapi.lib" )
#include <Tlhelp32.h>

#include "rapidassist/undef_windows_macros.h"
#include "rapidassist/process.h"

#define EXPORT_API __declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#endif

static const char* PLUGIN_NAME_IDENTIFIER = "sa_plugin_process";
static const char* ATTR_PROCESS_RUNNING_FILENAME = "process_filename";
static const char* ATTR_PROCESS_RUNNING_PID = "process_pid";
static const sa_boolean ATTR_OPTINAL = 0;
static const sa_boolean ATTR_MANDATORY = 1;
static const int ATTR_GROUP_ANY = -1;
static const sa_string_t INVALID_SA_STRING = { 0 };
static const DWORD INVALID_PROCESS_ID = 0;

struct XML_ATTR
{
  const char* name;
  sa_boolean mandatory;
  int group;
  const char* value;
  sa_string_t str;
};

// Declare attributes for killprocess action
// <killprocess pid="${processid}" />
// <killprocess filename="${process.exe.name}" />
XML_ATTR KILLPROCESS_ATTRIBUTES[] =
{
  {"pid",       ATTR_OPTINAL, 1, NULL, INVALID_SA_STRING},
  {"filename",  ATTR_OPTINAL, 1, NULL, INVALID_SA_STRING},
};
size_t KILLPROCESS_ATTRIBUTES_COUNT = sizeof(KILLPROCESS_ATTRIBUTES) / sizeof(KILLPROCESS_ATTRIBUTES[0]);

// Declare attributes for terminateprocess action
// <terminateprocess pid="${processid}" />
// <terminateprocess filename="${process.exe.name}" />
XML_ATTR TERMINATEPROCESS_ATTRIBUTES[] =
{
  {"pid",       ATTR_OPTINAL, 1, NULL, INVALID_SA_STRING},
  {"filename",  ATTR_OPTINAL, 1, NULL, INVALID_SA_STRING},
};
size_t TERMINATEPROCESS_ATTRIBUTES_COUNT = sizeof(TERMINATEPROCESS_ATTRIBUTES) / sizeof(TERMINATEPROCESS_ATTRIBUTES[0]);




void parse_string_size_t(const char* str, size_t & value)
{
  std::stringstream ss(str);
  ss >> value;
}

void parse_string_dword(const char* str, DWORD& value)
{
  std::stringstream ss(str);
  ss >> value;
}

void to_string(const size_t& value, std::string & str)
{
  std::stringstream ss;
  ss << value;
  str = ss.str();
}

void string_replace(std::string & text, const std::string & token, const std::string & value)
{
  size_t start_pos = 0;
  size_t find_pos = std::string::npos;
  do {
    find_pos = text.find(token, start_pos);
    if (find_pos != std::string::npos) {
      text.replace(find_pos, token.length(), value);
      start_pos = find_pos + value.length();
    }
  } while (find_pos != -1);
}

void replace_entities(std::string& str)
{
  typedef std::map<std::string /*name*/, std::string /*value*/> EntityMap;
  static EntityMap entities;
  
  // Is entities list not already filled ?
  if (entities.empty())
  {
    // https://en.wikipedia.org/wiki/List_of_XML_and_HTML_character_entity_references#List_of_predefined_entities_in_XML
    entities["quot"] = "\"";
    entities["amp"] = "&";
    entities["apos"] = "'";
    entities["lt"] = "<";
    entities["gt"] = ">";
  }

  std::string accu;
  bool capture = false;
  for (size_t i = 0; i < str.size(); i++)
  {
    char c = str[i];
    if (c == '&')
    {
      accu.clear();
      capture = true;
    }
    else if (c == ';' && !accu.empty())
    {
      EntityMap::const_iterator it = entities.find(accu);
      bool found = (it != entities.end());
      if (found)
      {
        const std::string& entity_value = entities[accu];

        // move i back to the ampersand offset
        i -= (accu.size() + 1);
        
        // proceed with search and replace
        size_t length = accu.size() + 2;
        str.replace(str.begin() + i, str.begin() + i + length, entity_value);

        //reset
        accu.clear();
        capture = false;
      }
    }
    else if (capture)
    {
      accu.append(1, c);
    }
  }
}

sa_error_t parse_attribute_in_xml(sa_boolean log_errors, const char* xml, const char* action_name, XML_ATTR * attr, sa_property_store_t* store)
{
  std::string strxml = xml;

  std::string pattern;
  pattern.append(attr->name);
  pattern.append("=");
  pattern.append("\"");

  // Find the beginning of the attribute value
  size_t pos_start = strxml.find(pattern);
  if (pos_start == std::string::npos)
    return SA_ERROR_NOT_FOUND;
  pos_start += pattern.size();

  size_t pos_end = strxml.find("\"", pos_start);
  if (pos_end == std::string::npos)
    return SA_ERROR_NOT_FOUND;

  size_t length = pos_end - pos_start;

  // Exact attribute value
  std::string attr_value = strxml.substr(pos_start, length);

  // Replace entities
  replace_entities(attr_value);

  // Save value in property store
  sa_property_store_set_property(store, attr->name, attr_value.c_str());

  return SA_ERROR_SUCCESS;
}

sa_error_t read_attributes(const char* action_name, const char* xml, sa_property_store_t* store, size_t count, XML_ATTR* attributes)
{
  if (action_name == NULL)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Unknown action name.");
    return SA_ERROR_INVALID_ARGUMENTS;
  }
  if (xml == NULL)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "No xml data for action '%s'.", action_name);
    return SA_ERROR_INVALID_ARGUMENTS;
  }
  if (store == NULL)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "No property store available for action '%s'.", action_name);
    return SA_ERROR_INVALID_ARGUMENTS;
  }

  sa_error_t result;
  
  // For each attributes
  for (size_t i = 0; i < count; i++)
  {
    XML_ATTR& attr = attributes[i];
    
    if (attr.mandatory)
    {
      result = parse_attribute_in_xml(1, xml, action_name, &attr, store);
      if (result != SA_ERROR_SUCCESS)
      {
        sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Unable to find mandatory attribute '%s' for action '%s'.", attr.name, action_name);
        return result;
      }
    }
    else
    {
      parse_attribute_in_xml(0, xml, action_name, &attr, store);
    }
  }

  return SA_ERROR_SUCCESS;
}

sa_error_t expand_attributes(const char* action_name, sa_property_store_t* store, size_t count, XML_ATTR* attributes)
{
  if (action_name == NULL)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Unknown action name in function '%s'.", __FUNCTION__);
    return SA_ERROR_INVALID_ARGUMENTS;
  }
  if (store == NULL)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "No property store available for action '%s'.", action_name);
    return SA_ERROR_INVALID_ARGUMENTS;
  }

  sa_error_t result;
  sa_property_store_immutable_t store_immutable = sa_property_store_to_immutable(store);

  // Check mandatory attributes
  for (size_t i = 0; i < count; i++)
  {
    XML_ATTR& attr = attributes[i];
    if (attr.mandatory)
    {
      if (!sa_property_store_has_property(&store_immutable, attr.name))
      {
        sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Missing mandatory attribute '%s' for action '%s'.", attr.name, action_name);
        return SA_ERROR_NOT_FOUND;
      }
    }
  }

  // Expand all attributes
  for (size_t i = 0; i < count; i++)
  {
    XML_ATTR& attr = attributes[i];

    // Get the value of the attribute
    // Note that attr.value can be NULL if not specified in the xml configuration
    attr.value = sa_property_store_get_property_c_str(&store_immutable, attr.name);

    if (attr.value != NULL)
    {
      // Expand the attribute
      attr.str = INVALID_SA_STRING;
      sa_string_create(&attr.str);
      result = sa_properties_expand_string(attr.value, &attr.str);
      if (result != SA_ERROR_SUCCESS)
      {
        sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Failed to expand value of attribute '%s'.", attr.name);
        return result;
      }
    }
    else
    {
      // Fill with an empty string
      void sa_string_create_from_cstr(sa_string_t * str, const char* value);

    }
  }

  return SA_ERROR_SUCCESS;
}

void cleanup_attributes(const char* action_name, sa_property_store_t* store, size_t count, XML_ATTR* attributes)
{
  // Expand all attributes
  for (size_t i = 0; i < count; i++)
  {
    XML_ATTR& attr = attributes[i];
    if (sa_string_valid(&attr.str))
      sa_string_destroy(&attr.str);
    attr.str = INVALID_SA_STRING;
    attr.value = NULL;
  }
}

sa_error_t assert_mutually_exclusive_attr_group(const char* action_name, const char* xml, int group, sa_property_store_t* store, size_t attributes_count, XML_ATTR* attributes)
{
  if (action_name == NULL)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Unknown action name.");
    return SA_ERROR_INVALID_ARGUMENTS;
  }
  if (xml == NULL)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "No xml data for action '%s'.", action_name);
    return SA_ERROR_INVALID_ARGUMENTS;
  }
  if (store == NULL)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "No property store available for action '%s'.", action_name);
    return SA_ERROR_INVALID_ARGUMENTS;
  }

  // Find attribute names in group
  std::string all_names_in_group;
  for (size_t i = 0; i < attributes_count; i++)
  {
    XML_ATTR& attr = attributes[i];
    if (attr.group == group)
    {
      if (!all_names_in_group.empty())
        all_names_in_group.append(",");
      all_names_in_group.append(attr.name);
    }
  }

  // Check if group is found
  if (all_names_in_group.empty())
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Fail to identify an attribute in group %d for action '%s'.", group, action_name);
    return SA_ERROR_NOT_FOUND;
  }

  sa_property_store_immutable_t store_immutable = sa_property_store_to_immutable(store);

  // Count how many attributes are specified in group
  size_t num_specified = 0;
  std::string specified_names_in_group;
  for (size_t i = 0; i < attributes_count; i++)
  {
    XML_ATTR& attr = attributes[i];
    if (attr.group == group)
    {
      // Check if this attribute was specified (is available in the store)
      if (sa_property_store_has_property(&store_immutable, attr.name))
      {
        num_specified++;
        if (!specified_names_in_group.empty())
          specified_names_in_group.append(",");
        specified_names_in_group.append(attr.name);
      }
    }
  }

  // Assert
  if (num_specified == 0)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "One attributes of group %d for action '%s' must be specified: %s.", group, action_name, all_names_in_group.c_str());
    return SA_ERROR_VALUE_OUT_OF_BOUNDS;
  }
  else if (num_specified > 1)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "One attributes of group %d for action '%s' must be specified: %s.  The following attributes was specified: %s.", group, action_name, all_names_in_group.c_str(), specified_names_in_group.c_str());
    return SA_ERROR_VALUE_OUT_OF_BOUNDS;
  }

  return SA_ERROR_SUCCESS;
}






// https://stackoverflow.com/questions/13179410/check-whether-one-specific-process-is-running-on-windows-with-c
DWORD find_process_id_from_name(const char* process_name)
{
  // strip path
  const char* p = strrchr(process_name, '\\');
  if (p)
    process_name = p + 1;

  PROCESSENTRY32 pi;
  pi.dwSize = sizeof(pi);

  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
  if (hSnapshot == INVALID_HANDLE_VALUE)
    return 0;

  Process32First(hSnapshot, &pi);
  if (!_stricmp(process_name, pi.szExeFile))
  {
    CloseHandle(hSnapshot);
    return pi.th32ProcessID;
  }

  while (Process32Next(hSnapshot, &pi))
  {
    if (!_stricmp(process_name, pi.szExeFile))
    {
      CloseHandle(hSnapshot);
      return pi.th32ProcessID;
    }
  }

  CloseHandle(hSnapshot);
  return INVALID_PROCESS_ID;
}

bool process_id_exists(DWORD pid)
{
  PROCESSENTRY32 pi;
  pi.dwSize = sizeof(pi);

  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
  if (hSnapshot == INVALID_HANDLE_VALUE)
    return 0;

  Process32First(hSnapshot, &pi);
  if (pi.th32ProcessID == pid)
  {
    CloseHandle(hSnapshot);
    return true;
  }

  while (Process32Next(hSnapshot, &pi))
  {
    if (pi.th32ProcessID == pid)
    {
      CloseHandle(hSnapshot);
      return true;
    }
  }

  CloseHandle(hSnapshot);
  return false;
}

sa_error_t kill_process_by_pid(DWORD pid)
{
  bool success = false;
  HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
  if (hProcess) {
    success = (TerminateProcess(hProcess, 255) != 0);
    CloseHandle(hProcess);
  }
  if (success)
    return SA_ERROR_SUCCESS;
  return SA_ERROR_ACCESS_DENIED;
}

sa_error_t terminate_process_by_pid(DWORD pid)
{
  //ask the process to exit gracefully allowing a maximum of 60 seconds to close
  ULONGLONG time_start = GetTickCount64();
  bool terminated = ra::process::Terminate(pid);
  ULONGLONG time_end = GetTickCount64();
  ULONGLONG elapsed_ms = time_end - time_start;
  if (!terminated)
  {
    if (elapsed_ms >= 58000)
    {
      return SA_ERROR_TIME_OUT;
    }
    return SA_ERROR_ACCESS_DENIED;
  }

  return SA_ERROR_SUCCESS;
}




sa_error_t killprocess_event_create(sa_action_event_t evnt)
{
  const char* name = sa_plugin_action_get_name();
  const char* xml = sa_plugin_action_get_xml();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  sa_error_t result = read_attributes(name, xml, store, KILLPROCESS_ATTRIBUTES_COUNT, KILLPROCESS_ATTRIBUTES);
  if (result != SA_ERROR_SUCCESS)
    return result;

  result = assert_mutually_exclusive_attr_group(name, xml, 1, store, KILLPROCESS_ATTRIBUTES_COUNT, KILLPROCESS_ATTRIBUTES);
  if (result != SA_ERROR_SUCCESS)
    return result;

  return SA_ERROR_SUCCESS;
}

sa_error_t killprocess_event_destroy(sa_action_event_t evnt)
{
  return SA_ERROR_SUCCESS;
}

sa_error_t killprocess_event_execute(sa_action_event_t evnt)
{
  const char* action_name = sa_plugin_action_get_name();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  // Expand attributes
  sa_error_t result = expand_attributes(action_name, store, KILLPROCESS_ATTRIBUTES_COUNT, KILLPROCESS_ATTRIBUTES);
  if (result != SA_ERROR_SUCCESS)
    return result;

  // Get expanded attribute values individually
  const char* pid_str = sa_string_get_value(&KILLPROCESS_ATTRIBUTES[0].str);
  const char* filename = sa_string_get_value(&KILLPROCESS_ATTRIBUTES[1].str);

  // Convert string values to numeric values
  DWORD pid = INVALID_PROCESS_ID;
  if (pid_str != NULL)
    parse_string_dword(pid_str, pid);

  result = SA_ERROR_SUCCESS;

  // Execute this action
  // <killprocess pid="${processid}" />
  // <killprocess filename="${process.exe.name}" />
  if (pid != INVALID_PROCESS_ID)
  {
    if (process_id_exists(pid))
    {
      result = kill_process_by_pid(pid);
    }
  }
  if (filename != NULL && result == SA_ERROR_SUCCESS)
  {
    DWORD pid_tmp = find_process_id_from_name(filename);
    while (pid_tmp != INVALID_PROCESS_ID)
    {
      result = kill_process_by_pid(pid);
      if (result != SA_ERROR_SUCCESS)
        break;

      // next process with the same name
      pid_tmp = find_process_id_from_name(filename);
    }
  }

  // Cleanup sa_string_t instances
  cleanup_attributes(action_name, store, KILLPROCESS_ATTRIBUTES_COUNT, KILLPROCESS_ATTRIBUTES);

  return result;
}

sa_error_t killprocess_event(sa_action_event_t evnt)
{
  sa_error_t result;
  switch (evnt)
  {
  case SA_ACTION_EVENT_CREATE:
    result = killprocess_event_create(evnt);
    return result;
  case SA_ACTION_EVENT_DESTROY:
    result = killprocess_event_destroy(evnt);
    return result;
  case SA_ACTION_EVENT_EXECUTE:
    result = killprocess_event_execute(evnt);
    return result;
  default:
    // Unknown event.
    return SA_ERROR_NOT_IMPLEMENTED;
  }
}




sa_error_t terminateprocess_event_create(sa_action_event_t evnt)
{
  const char* name = sa_plugin_action_get_name();
  const char* xml = sa_plugin_action_get_xml();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  sa_error_t result = read_attributes(name, xml, store, TERMINATEPROCESS_ATTRIBUTES_COUNT, TERMINATEPROCESS_ATTRIBUTES);
  if (result != SA_ERROR_SUCCESS)
    return result;

  result = assert_mutually_exclusive_attr_group(name, xml, 1, store, TERMINATEPROCESS_ATTRIBUTES_COUNT, TERMINATEPROCESS_ATTRIBUTES);
  if (result != SA_ERROR_SUCCESS)
    return result;

  return SA_ERROR_SUCCESS;
}

sa_error_t terminateprocess_event_destroy(sa_action_event_t evnt)
{
  return SA_ERROR_SUCCESS;
}

sa_error_t terminateprocess_event_execute(sa_action_event_t evnt)
{
  const char* action_name = sa_plugin_action_get_name();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  // Expand attributes
  sa_error_t result = expand_attributes(action_name, store, TERMINATEPROCESS_ATTRIBUTES_COUNT, TERMINATEPROCESS_ATTRIBUTES);
  if (result != SA_ERROR_SUCCESS)
    return result;

  // Get expanded attribute values individually
  const char* pid_str = sa_string_get_value(&TERMINATEPROCESS_ATTRIBUTES[0].str);
  const char* filename = sa_string_get_value(&TERMINATEPROCESS_ATTRIBUTES[1].str);

  // Convert string values to numeric values
  DWORD pid = INVALID_PROCESS_ID;
  if (pid_str != NULL)
    parse_string_dword(pid_str, pid);

  result = SA_ERROR_SUCCESS;

  // Execute this action
  // <killprocess pid="${processid}" />
  // <killprocess filename="${process.exe.name}" />
  if (pid != INVALID_PROCESS_ID)
  {
    if (process_id_exists(pid))
    {
      result = terminate_process_by_pid(pid);
    }
  }
  if (filename != NULL && result == SA_ERROR_SUCCESS)
  {
    DWORD pid_tmp = find_process_id_from_name(filename);
    while (pid_tmp != INVALID_PROCESS_ID)
    {
      result = terminate_process_by_pid(pid);
      if (result != SA_ERROR_SUCCESS)
        break;

      // next process with the same name
      pid_tmp = find_process_id_from_name(filename);
    }
  }

  // Cleanup sa_string_t instances
  cleanup_attributes(action_name, store, TERMINATEPROCESS_ATTRIBUTES_COUNT, TERMINATEPROCESS_ATTRIBUTES);

  return result;
}

sa_error_t terminateprocess_event(sa_action_event_t evnt)
{
  sa_error_t result;
  switch (evnt)
  {
  case SA_ACTION_EVENT_CREATE:
    result = terminateprocess_event_create(evnt);
    return result;
  case SA_ACTION_EVENT_DESTROY:
    result = terminateprocess_event_destroy(evnt);
    return result;
  case SA_ACTION_EVENT_EXECUTE:
    result = terminateprocess_event_execute(evnt);
    return result;
  default:
    // Unknown event.
    return SA_ERROR_NOT_IMPLEMENTED;
  }
}




sa_boolean validate_process_filename()
{
  sa_selection_context_immutable_t* ctx = sa_plugin_validation_get_selection_context();
  sa_property_store_immutable_t* store = sa_plugin_validation_get_property_store();

  const char* filename = sa_property_store_get_property_c_str(store, ATTR_PROCESS_RUNNING_FILENAME);
  if (filename == NULL || filename[0] == '\0')
    return 0;

  //expand
  sa_string_t tmp_str;
  sa_string_init(&tmp_str);
  sa_string_create(&tmp_str);
  sa_error_t result = sa_properties_expand_string(filename, &tmp_str);
  if (result != SA_ERROR_SUCCESS)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Failed to expand value '%s'.", filename);
    return result;
  }
  const char * filename_expanded = sa_string_get_value(&tmp_str);

  //convert
  DWORD pid = find_process_id_from_name(filename_expanded);

  //cleanup
  sa_string_destroy(&tmp_str);

  //validate
  if (pid != INVALID_PROCESS_ID)
    return 1;
  return 0;
}

sa_boolean validate_process_pid()
{
  sa_selection_context_immutable_t* ctx = sa_plugin_validation_get_selection_context();
  sa_property_store_immutable_t* store = sa_plugin_validation_get_property_store();

  const char* pid_str = sa_property_store_get_property_c_str(store, ATTR_PROCESS_RUNNING_PID);
  if (pid_str == NULL || pid_str[0] == '\0')
    return 0;

  //expand
  sa_string_t tmp_str;
  sa_string_init(&tmp_str);
  sa_string_create(&tmp_str);
  sa_error_t result = sa_properties_expand_string(pid_str, &tmp_str);
  if (result != SA_ERROR_SUCCESS)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Failed to expand value '%s'.", pid_str);
    return result;
  }
  const char* pid_expanded = sa_string_get_value(&tmp_str);

  //convert
  DWORD pid = 0;
  parse_string_dword(pid_expanded, pid);

  //cleanup
  sa_string_destroy(&tmp_str);

  //validate
  if (pid == INVALID_PROCESS_ID)
    return 0;
  if (process_id_exists(pid))
    return 1;
  return 0;
}




EXPORT_API sa_error_t sa_plugin_initialize(sa_version_info_t* version)
{
  if (version->major == 0 && version->minor < 8) // this plugin is designed for version 0.8.0 and over
    return SA_ERROR_NOT_SUPPORTED;
  return SA_ERROR_SUCCESS;
}

EXPORT_API sa_error_t sa_plugin_register()
{
  sa_error_t result;

  // register custom validation function
#define PLUGIN_REGISTER_VALIDATION(attributes, event_func) \
  { \
    static const size_t count = sizeof(attributes) / sizeof(attributes[0]); \
    sa_plugin_validate_callback_func validate_func = &event_func; \
    result = sa_plugin_register_attribute_validation(attributes, count, validate_func); \
    if (result != SA_ERROR_SUCCESS) \
    { \
      sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Failed registering validation function for attribute '%s'.", attributes[0]); \
      return result; \
    } \
  }

  static const char* filename_attributes[] = {
    ATTR_PROCESS_RUNNING_FILENAME,
  };
  static const char* pid_attributes[] = {
    ATTR_PROCESS_RUNNING_PID,
  };
  PLUGIN_REGISTER_VALIDATION(filename_attributes, validate_process_filename);
  PLUGIN_REGISTER_VALIDATION(pid_attributes, validate_process_pid);

#undef PLUGIN_REGISTER_VALIDATION

  // register custom actions
#define PLUGIN_REGISTER_EVENT(name, event_func) \
  result = sa_plugin_register_action_event(name, event_func); \
  if (result != SA_ERROR_SUCCESS) \
  { \
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Failed registering event function for action '%s'.", name); \
    return result; \
  }

  PLUGIN_REGISTER_EVENT("killprocess",        &killprocess_event);
  PLUGIN_REGISTER_EVENT("terminateprocess",   &terminateprocess_event);

#undef PLUGIN_REGISTER_EVENT

  return SA_ERROR_SUCCESS;
}

#ifdef __cplusplus
}
#endif
