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
#include "shellanything/sa_xml.h"
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

  // do not indent code inside extern C
#if 0
}
#endif

static const char* PLUGIN_NAME_IDENTIFIER = "sa_plugin_process";
static const char* ATTR_PROCESS_RUNNING_FILENAME = "process_filename";
static const char* ATTR_PROCESS_RUNNING_PID = "process_pid";
static const DWORD INVALID_PROCESS_ID = 0;

// Declare attributes for killprocess action
// <killprocess pid="${processid}" />
// <killprocess filename="${process.exe.name}" />
XML_ATTR KILLPROCESS_ATTRIBUTES[] =
{
  {"pid",       SA_XML_ATTR_OPTINAL, 1, NULL, NULL},
  {"filename",  SA_XML_ATTR_OPTINAL, 1, NULL, NULL},
};
size_t KILLPROCESS_ATTRIBUTES_COUNT = sizeof(KILLPROCESS_ATTRIBUTES) / sizeof(KILLPROCESS_ATTRIBUTES[0]);

// Declare attributes for terminateprocess action
// <terminateprocess pid="${processid}" />
// <terminateprocess filename="${process.exe.name}" />
XML_ATTR TERMINATEPROCESS_ATTRIBUTES[] =
{
  {"pid",       SA_XML_ATTR_OPTINAL, 1, NULL, NULL},
  {"filename",  SA_XML_ATTR_OPTINAL, 1, NULL, NULL},
};
size_t TERMINATEPROCESS_ATTRIBUTES_COUNT = sizeof(TERMINATEPROCESS_ATTRIBUTES) / sizeof(TERMINATEPROCESS_ATTRIBUTES[0]);




void parse_string_dword(const char* str, DWORD& value)
{
  std::stringstream ss(str);
  ss >> value;
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
  if (hProcess)
  {
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

  sa_property_store_immutable_t store_immutable = sa_property_store_to_immutable(store);
  XML_ATTR* attrs = KILLPROCESS_ATTRIBUTES;
  size_t count = KILLPROCESS_ATTRIBUTES_COUNT;

  sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Creating action '%s'.", name);

  sa_xml_attr_list_init(attrs, count);
  sa_error_t result = sa_xml_parse_attr_list_store(xml, attrs, count, store);
  if (result != SA_ERROR_SUCCESS)
    return result;

  result = sa_xml_attr_group_is_mutually_exclusive(attrs, count, 1, &store_immutable);
  if (result != SA_ERROR_SUCCESS)
    return result;

  return SA_ERROR_SUCCESS;
}

sa_error_t killprocess_event_execute(sa_action_event_t evnt)
{
  const char* action_name = sa_plugin_action_get_name();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  sa_property_store_immutable_t store_immutable = sa_property_store_to_immutable(store);
  XML_ATTR* attrs = KILLPROCESS_ATTRIBUTES;
  size_t count = KILLPROCESS_ATTRIBUTES_COUNT;

  // Expand attributes
  sa_xml_attr_list_update(attrs, count, &store_immutable);

  // Get expanded attribute values individually
  const char* pid_str = attrs[0].tmp_expanded;
  const char* filename = attrs[1].tmp_expanded;

  // Convert string values to numeric values
  DWORD pid = INVALID_PROCESS_ID;
  if (pid_str != NULL)
    parse_string_dword(pid_str, pid);

  sa_error_t result = SA_ERROR_SUCCESS;

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

  // Cleanup allocated c strings
  sa_xml_attr_list_cleanup(attrs, count);

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
    return SA_ERROR_SUCCESS;
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

  sa_property_store_immutable_t store_immutable = sa_property_store_to_immutable(store);
  XML_ATTR* attrs = TERMINATEPROCESS_ATTRIBUTES;
  size_t count = TERMINATEPROCESS_ATTRIBUTES_COUNT;

  sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Creating action '%s'.", name);

  sa_xml_attr_list_init(attrs, count);
  sa_error_t result = sa_xml_parse_attr_list_store(xml, attrs, count, store);
  if (result != SA_ERROR_SUCCESS)
    return result;

  result = sa_xml_attr_group_is_mutually_exclusive(attrs, count, 1, &store_immutable);
  if (result != SA_ERROR_SUCCESS)
    return result;

  return SA_ERROR_SUCCESS;
}

sa_error_t terminateprocess_event_execute(sa_action_event_t evnt)
{
  const char* action_name = sa_plugin_action_get_name();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  sa_property_store_immutable_t store_immutable = sa_property_store_to_immutable(store);
  XML_ATTR* attrs = TERMINATEPROCESS_ATTRIBUTES;
  size_t count = TERMINATEPROCESS_ATTRIBUTES_COUNT;

  // Expand attributes
  sa_xml_attr_list_update(attrs, count, &store_immutable);

  // Get expanded attribute values individually
  const char* pid_str = attrs[0].tmp_expanded;
  const char* filename = attrs[1].tmp_expanded;

  // Convert string values to numeric values
  DWORD pid = INVALID_PROCESS_ID;
  if (pid_str != NULL)
    parse_string_dword(pid_str, pid);

  sa_error_t result = SA_ERROR_SUCCESS;

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

  // Cleanup allocated c strings
  sa_xml_attr_list_cleanup(attrs, count);

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
    return SA_ERROR_SUCCESS;
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

  const char* filename = sa_property_store_get_property_cstr(store, ATTR_PROCESS_RUNNING_FILENAME);
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
  const char* filename_expanded = sa_string_get_value(&tmp_str);

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

  const char* pid_str = sa_property_store_get_property_cstr(store, ATTR_PROCESS_RUNNING_PID);
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
    sa_plugin_validation_attributes_func validate_func = &event_func; \
    result = sa_plugin_register_validation_attributes(attributes, count, validate_func); \
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

  PLUGIN_REGISTER_EVENT("killprocess", &killprocess_event);
  PLUGIN_REGISTER_EVENT("terminateprocess", &terminateprocess_event);

#undef PLUGIN_REGISTER_EVENT

  return SA_ERROR_SUCCESS;
}

// do not indent code inside extern C
#if 0
{
#endif

#ifdef __cplusplus
}
#endif
