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
#include <ctime>
#include <algorithm>
#include <map>
#include <ctype.h>

#define EXPORT_API __declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#endif

static const char* PLUGIN_NAME_IDENTIFIER = "sa_plugin_strings";
static const sa_boolean ATTR_OPTINAL = 0;
static const sa_boolean ATTR_MANDATORY = 1;
static const sa_string_t INVALID_SA_STRING = { 0 };

struct XML_ATTR
{
  const char* name;
  sa_boolean mandatory;
  const char* value;
  sa_string_t str;
};

// Declare attributes for substr command
// <substr offset="7" count="4" value="Behind Blue Eyes" property="color" />
XML_ATTR SUBSTR_ATTRIBUTES[] =
{
  {"count",       ATTR_MANDATORY, NULL, INVALID_SA_STRING},
  {"value",       ATTR_MANDATORY, NULL, INVALID_SA_STRING},
  {"property",    ATTR_MANDATORY, NULL, INVALID_SA_STRING},
  {"offset",      ATTR_OPTINAL,   NULL, INVALID_SA_STRING},
};
size_t SUBSTR_ATTRIBUTES_COUNT = sizeof(SUBSTR_ATTRIBUTES) / sizeof(SUBSTR_ATTRIBUTES[0]);

// Declare attributes for strlen command
// <strlen value="${foobar}" property="foobar.length" />
XML_ATTR STRLEN_ATTRIBUTES[] =
{
  {"value",     ATTR_MANDATORY, NULL, INVALID_SA_STRING},
  {"property",  ATTR_MANDATORY, NULL, INVALID_SA_STRING},
};
size_t STRLEN_ATTRIBUTES_COUNT = sizeof(STRLEN_ATTRIBUTES) / sizeof(STRLEN_ATTRIBUTES[0]);

// Declare attributes for strreplace command
// <strreplace text="Everything is in the right place" token="right" value="wrong" property="inverse" />
XML_ATTR STRREPLACE_ATTRIBUTES[] =
{
  {"text",      ATTR_MANDATORY, NULL, INVALID_SA_STRING},
  {"token",     ATTR_MANDATORY, NULL, INVALID_SA_STRING},
  {"value",     ATTR_MANDATORY, NULL, INVALID_SA_STRING},
  {"property",  ATTR_MANDATORY, NULL, INVALID_SA_STRING},
};
size_t STRREPLACE_ATTRIBUTES_COUNT = sizeof(STRREPLACE_ATTRIBUTES) / sizeof(STRREPLACE_ATTRIBUTES[0]);

// Declare attributes for strreplace command
// <struppercase value="${foobar}" property="foobar" />
XML_ATTR STRUPPERCASE_ATTRIBUTES[] =
{
  {"value",     ATTR_MANDATORY, NULL, INVALID_SA_STRING},
  {"property",  ATTR_MANDATORY, NULL, INVALID_SA_STRING},
};
size_t STRUPPERCASE_ATTRIBUTES_COUNT = sizeof(STRUPPERCASE_ATTRIBUTES) / sizeof(STRUPPERCASE_ATTRIBUTES[0]);

// Declare attributes for strreplace command
// <strlowercase value="${foobar}" property="foobar" />
XML_ATTR STRLOWERCASE_ATTRIBUTES[] =
{
  {"value",     ATTR_MANDATORY, NULL, INVALID_SA_STRING},
  {"property",  ATTR_MANDATORY, NULL, INVALID_SA_STRING},
};
size_t STRLOWERCASE_ATTRIBUTES_COUNT = sizeof(STRLOWERCASE_ATTRIBUTES) / sizeof(STRLOWERCASE_ATTRIBUTES[0]);

// Declare attributes for strreplace command
// <strfind text="You shall not pass!" value="!" property="exclamation.offset" />
XML_ATTR STRFIND_ATTRIBUTES[] =
{
  {"text",      ATTR_MANDATORY, NULL, INVALID_SA_STRING},
  {"value",     ATTR_MANDATORY, NULL, INVALID_SA_STRING},
  {"property",  ATTR_MANDATORY, NULL, INVALID_SA_STRING},
  {"offset",    ATTR_OPTINAL,   NULL, INVALID_SA_STRING},
};
size_t STRFIND_ATTRIBUTES_COUNT = sizeof(STRFIND_ATTRIBUTES) / sizeof(STRFIND_ATTRIBUTES[0]);




void parse_string(const char* str, size_t & value)
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
    attr.value = sa_property_store_get_property_cstr(&store_immutable, attr.name);

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








sa_error_t substr_event_create(sa_action_event_t evnt)
{
  const char* name = sa_plugin_action_get_name();
  const char* xml = sa_plugin_action_get_xml();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  sa_error_t result = read_attributes(name, xml, store, SUBSTR_ATTRIBUTES_COUNT, SUBSTR_ATTRIBUTES);
  if (result != SA_ERROR_SUCCESS)
    return result;

  return SA_ERROR_SUCCESS;
}

sa_error_t substr_event_destroy(sa_action_event_t evnt)
{
  return SA_ERROR_SUCCESS;
}

sa_error_t substr_event_execute(sa_action_event_t evnt)
{
  const char* action_name = sa_plugin_action_get_name();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  // Expand attributes
  sa_error_t result = expand_attributes(action_name, store, SUBSTR_ATTRIBUTES_COUNT, SUBSTR_ATTRIBUTES);
  if (result != SA_ERROR_SUCCESS)
    return result;

  // Get expanded attribute values individually
  const char* attr_count = sa_string_get_value(&SUBSTR_ATTRIBUTES[0].str);
  const std::string attr_value = sa_string_get_value(&SUBSTR_ATTRIBUTES[1].str);
  const char* attr_property = sa_string_get_value(&SUBSTR_ATTRIBUTES[2].str);
  const char* attr_offset = sa_string_get_value(&SUBSTR_ATTRIBUTES[3].str);

  // Convert string values to numeric values
  size_t count = 0;
  parse_string(attr_count, count);
  size_t offset = 0;
  if (attr_offset != NULL)
    parse_string(attr_offset, offset);

  // Execute this action
  // <substr offset="7" count="4" value="Behind Blue Eyes" property="color" />
  std::string output;
  if (offset < attr_value.size())
    output = attr_value.substr(offset, count);

  sa_properties_set(attr_property, output.c_str());

  // Cleanup sa_string_t instances
  cleanup_attributes(action_name, store, SUBSTR_ATTRIBUTES_COUNT, SUBSTR_ATTRIBUTES);

  return SA_ERROR_SUCCESS;
}

sa_error_t substr_event(sa_action_event_t evnt)
{
  sa_error_t result;
  switch (evnt)
  {
  case SA_ACTION_EVENT_CREATE:
    result = substr_event_create(evnt);
    return result;
  case SA_ACTION_EVENT_DESTROY:
    result = substr_event_destroy(evnt);
    return result;
  case SA_ACTION_EVENT_EXECUTE:
    result = substr_event_execute(evnt);
    return result;
  default:
    // Unknown event.
    return SA_ERROR_NOT_IMPLEMENTED;
  }
}



sa_error_t strlen_event_create(sa_action_event_t evnt)
{
  const char* name = sa_plugin_action_get_name();
  const char* xml = sa_plugin_action_get_xml();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  sa_error_t result = read_attributes(name, xml, store, STRLEN_ATTRIBUTES_COUNT, STRLEN_ATTRIBUTES);
  if (result != SA_ERROR_SUCCESS)
    return result;

  return SA_ERROR_SUCCESS;
}

sa_error_t strlen_event_destroy(sa_action_event_t evnt)
{
  return SA_ERROR_SUCCESS;
}

sa_error_t strlen_event_execute(sa_action_event_t evnt)
{
  const char* action_name = sa_plugin_action_get_name();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  // Expand attributes
  sa_error_t result = expand_attributes(action_name, store, STRLEN_ATTRIBUTES_COUNT, STRLEN_ATTRIBUTES);
  if (result != SA_ERROR_SUCCESS)
    return result;

  // Get expanded attribute values individually
  const std::string attr_value = sa_string_get_value(&STRLEN_ATTRIBUTES[0].str);
  const char* attr_property = sa_string_get_value(&STRLEN_ATTRIBUTES[1].str);

  // Execute this action
  // <strlen value="${sa_plugin_strings.name}" property="sa_plugin_strings.strlen" />

  size_t length = attr_value.size();
  std::string output;
  to_string(length, output);

  sa_properties_set(attr_property, output.c_str());

  // Cleanup sa_string_t instances
  cleanup_attributes(action_name, store, STRLEN_ATTRIBUTES_COUNT, STRLEN_ATTRIBUTES);

  return SA_ERROR_SUCCESS;
}

sa_error_t strlen_event(sa_action_event_t evnt)
{
  sa_error_t result;
  switch (evnt)
  {
  case SA_ACTION_EVENT_CREATE:
    result = strlen_event_create(evnt);
    return result;
  case SA_ACTION_EVENT_DESTROY:
    result = strlen_event_destroy(evnt);
    return result;
  case SA_ACTION_EVENT_EXECUTE:
    result = strlen_event_execute(evnt);
    return result;
  default:
    // Unknown event.
    return SA_ERROR_NOT_IMPLEMENTED;
  }
}



sa_error_t strreplace_event_create(sa_action_event_t evnt)
{
  const char* name = sa_plugin_action_get_name();
  const char* xml = sa_plugin_action_get_xml();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  sa_error_t result = read_attributes(name, xml, store, STRREPLACE_ATTRIBUTES_COUNT, STRREPLACE_ATTRIBUTES);
  if (result != SA_ERROR_SUCCESS)
    return result;

  return SA_ERROR_SUCCESS;
}

sa_error_t strreplace_event_destroy(sa_action_event_t evnt)
{
  return SA_ERROR_SUCCESS;
}

sa_error_t strreplace_event_execute(sa_action_event_t evnt)
{
  const char* action_name = sa_plugin_action_get_name();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  // Expand attributes
  sa_error_t result = expand_attributes(action_name, store, STRREPLACE_ATTRIBUTES_COUNT, STRREPLACE_ATTRIBUTES);
  if (result != SA_ERROR_SUCCESS)
    return result;

  // Get expanded attribute values individually
  std::string attr_text = sa_string_get_value(&STRREPLACE_ATTRIBUTES[0].str);
  const std::string attr_token = sa_string_get_value(&STRREPLACE_ATTRIBUTES[1].str);
  const std::string attr_value = sa_string_get_value(&STRREPLACE_ATTRIBUTES[2].str);
  const char* attr_property = sa_string_get_value(&STRREPLACE_ATTRIBUTES[3].str);

  // Execute this action
  // <strreplace text="Everything is in the right place" token="right" value="wrong" property="inverse" />
  string_replace(attr_text, attr_token, attr_value);

  sa_properties_set(attr_property, attr_text.c_str());

  // Cleanup sa_string_t instances
  cleanup_attributes(action_name, store, STRREPLACE_ATTRIBUTES_COUNT, STRREPLACE_ATTRIBUTES);

  return SA_ERROR_SUCCESS;
}

sa_error_t strreplace_event(sa_action_event_t evnt)
{
  sa_error_t result;
  switch (evnt)
  {
  case SA_ACTION_EVENT_CREATE:
    result = strreplace_event_create(evnt);
    return result;
  case SA_ACTION_EVENT_DESTROY:
    result = strreplace_event_destroy(evnt);
    return result;
  case SA_ACTION_EVENT_EXECUTE:
    result = strreplace_event_execute(evnt);
    return result;
  default:
    // Unknown event.
    return SA_ERROR_NOT_IMPLEMENTED;
  }
}



sa_error_t struppercase_event_create(sa_action_event_t evnt)
{
  const char* name = sa_plugin_action_get_name();
  const char* xml = sa_plugin_action_get_xml();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  sa_error_t result = read_attributes(name, xml, store, STRUPPERCASE_ATTRIBUTES_COUNT, STRUPPERCASE_ATTRIBUTES);
  if (result != SA_ERROR_SUCCESS)
    return result;

  return SA_ERROR_SUCCESS;
}

sa_error_t struppercase_event_destroy(sa_action_event_t evnt)
{
  return SA_ERROR_SUCCESS;
}

sa_error_t struppercase_event_execute(sa_action_event_t evnt)
{
  const char* action_name = sa_plugin_action_get_name();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  // Expand attributes
  sa_error_t result = expand_attributes(action_name, store, STRUPPERCASE_ATTRIBUTES_COUNT, STRUPPERCASE_ATTRIBUTES);
  if (result != SA_ERROR_SUCCESS)
    return result;

  // Get expanded attribute values individually
  const std::string attr_value = sa_string_get_value(&STRUPPERCASE_ATTRIBUTES[0].str);
  const char* attr_property = sa_string_get_value(&STRUPPERCASE_ATTRIBUTES[1].str);

  // Execute this action
  // <struppercase value="${foobar}" property="foobar" />
  std::string output;
  output = attr_value;
  std::transform(output.begin(), output.end(), output.begin(), toupper);

  sa_properties_set(attr_property, output.c_str());

  // Cleanup sa_string_t instances
  cleanup_attributes(action_name, store, STRUPPERCASE_ATTRIBUTES_COUNT, STRUPPERCASE_ATTRIBUTES);

  return SA_ERROR_SUCCESS;
}

sa_error_t struppercase_event(sa_action_event_t evnt)
{
  sa_error_t result;
  switch (evnt)
  {
  case SA_ACTION_EVENT_CREATE:
    result = struppercase_event_create(evnt);
    return result;
  case SA_ACTION_EVENT_DESTROY:
    result = struppercase_event_destroy(evnt);
    return result;
  case SA_ACTION_EVENT_EXECUTE:
    result = struppercase_event_execute(evnt);
    return result;
  default:
    // Unknown event.
    return SA_ERROR_NOT_IMPLEMENTED;
  }
}



sa_error_t strlowercase_event_create(sa_action_event_t evnt)
{
  const char* name = sa_plugin_action_get_name();
  const char* xml = sa_plugin_action_get_xml();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  sa_error_t result = read_attributes(name, xml, store, STRLOWERCASE_ATTRIBUTES_COUNT, STRLOWERCASE_ATTRIBUTES);
  if (result != SA_ERROR_SUCCESS)
    return result;

  return SA_ERROR_SUCCESS;
}

sa_error_t strlowercase_event_destroy(sa_action_event_t evnt)
{
  return SA_ERROR_SUCCESS;
}

sa_error_t strlowercase_event_execute(sa_action_event_t evnt)
{
  const char* action_name = sa_plugin_action_get_name();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  // Expand attributes
  sa_error_t result = expand_attributes(action_name, store, STRLOWERCASE_ATTRIBUTES_COUNT, STRLOWERCASE_ATTRIBUTES);
  if (result != SA_ERROR_SUCCESS)
    return result;

  // Get expanded attribute values individually
  const std::string attr_value = sa_string_get_value(&STRLOWERCASE_ATTRIBUTES[0].str);
  const char* attr_property = sa_string_get_value(&STRLOWERCASE_ATTRIBUTES[1].str);

  // Execute this action
  // <strlowercase value="${foobar}" property="foobar" />
  std::string output;
  output = attr_value;
  std::transform(output.begin(), output.end(), output.begin(), tolower);

  sa_properties_set(attr_property, output.c_str());

  // Cleanup sa_string_t instances
  cleanup_attributes(action_name, store, STRLOWERCASE_ATTRIBUTES_COUNT, STRLOWERCASE_ATTRIBUTES);

  return SA_ERROR_SUCCESS;
}

sa_error_t strlowercase_event(sa_action_event_t evnt)
{
  sa_error_t result;
  switch (evnt)
  {
  case SA_ACTION_EVENT_CREATE:
    result = strlowercase_event_create(evnt);
    return result;
  case SA_ACTION_EVENT_DESTROY:
    result = strlowercase_event_destroy(evnt);
    return result;
  case SA_ACTION_EVENT_EXECUTE:
    result = strlowercase_event_execute(evnt);
    return result;
  default:
    // Unknown event.
    return SA_ERROR_NOT_IMPLEMENTED;
  }
}



sa_error_t strfind_event_create(sa_action_event_t evnt)
{
  const char* name = sa_plugin_action_get_name();
  const char* xml = sa_plugin_action_get_xml();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  sa_error_t result = read_attributes(name, xml, store, STRFIND_ATTRIBUTES_COUNT, STRFIND_ATTRIBUTES);
  if (result != SA_ERROR_SUCCESS)
    return result;

  return SA_ERROR_SUCCESS;
}

sa_error_t strfind_event_destroy(sa_action_event_t evnt)
{
  return SA_ERROR_SUCCESS;
}

sa_error_t strfind_event_execute(sa_action_event_t evnt)
{
  const char* action_name = sa_plugin_action_get_name();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  // Expand attributes
  sa_error_t result = expand_attributes(action_name, store, STRFIND_ATTRIBUTES_COUNT, STRFIND_ATTRIBUTES);
  if (result != SA_ERROR_SUCCESS)
    return result;

  // Get expanded attribute values individually
  const std::string attr_text = sa_string_get_value(&STRFIND_ATTRIBUTES[0].str);
  const std::string attr_value = sa_string_get_value(&STRFIND_ATTRIBUTES[1].str);
  const char* attr_property = sa_string_get_value(&STRFIND_ATTRIBUTES[2].str);
  const char* attr_offset = sa_string_get_value(&STRFIND_ATTRIBUTES[3].str);

  // Convert string values to numeric values
  size_t offset = 0;
  if (attr_offset != NULL)
    parse_string(attr_offset, offset);

  // Execute this action
  // <strfind text="You shall not pass!" value="!" property="exclamation.offset" />
  size_t pos = attr_text.find(attr_value, offset);
  std::string output;
  to_string(pos, output);

  sa_properties_set(attr_property, output.c_str());

  // Cleanup sa_string_t instances
  cleanup_attributes(action_name, store, STRFIND_ATTRIBUTES_COUNT, STRFIND_ATTRIBUTES);

  return SA_ERROR_SUCCESS;
}

sa_error_t strfind_event(sa_action_event_t evnt)
{
  sa_error_t result;
  switch (evnt)
  {
  case SA_ACTION_EVENT_CREATE:
    result = strfind_event_create(evnt);
    return result;
  case SA_ACTION_EVENT_DESTROY:
    result = strfind_event_destroy(evnt);
    return result;
  case SA_ACTION_EVENT_EXECUTE:
    result = strfind_event_execute(evnt);
    return result;
  default:
    // Unknown event.
    return SA_ERROR_NOT_IMPLEMENTED;
  }
}



EXPORT_API sa_error_t sa_plugin_initialize(sa_version_info_t* version)
{
  if (version->major == 0 && version->minor < 8) // this plugin is designed for version 0.8.0 and over
    return SA_ERROR_NOT_SUPPORTED;
  return SA_ERROR_SUCCESS;
}

#define PLUGIN_REGISTER_EVENT(name, event_func) \
  result = sa_plugin_register_action_event(name, event_func); \
  if (result != SA_ERROR_SUCCESS) \
  { \
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Failed registering event function for action '%s'.", name); \
    return result; \
  }

EXPORT_API sa_error_t sa_plugin_register()
{
  sa_error_t result;
  PLUGIN_REGISTER_EVENT("substr",        &substr_event);
  PLUGIN_REGISTER_EVENT("strreplace",    &strreplace_event);
  PLUGIN_REGISTER_EVENT("strlen",        &strlen_event);
  PLUGIN_REGISTER_EVENT("struppercase",  &struppercase_event);
  PLUGIN_REGISTER_EVENT("strlowercase",  &strlowercase_event);
  PLUGIN_REGISTER_EVENT("strfind",       &strfind_event);

  return SA_ERROR_SUCCESS;
}

#ifdef __cplusplus
}
#endif
