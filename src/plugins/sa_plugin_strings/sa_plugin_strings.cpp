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
#include <ctime>
#include <algorithm>
#include <map>
#include <ctype.h>

#define EXPORT_API __declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#if 0
}  // do not indent code inside extern C
#endif
#endif

static const char* PLUGIN_NAME_IDENTIFIER = "sa_plugin_strings";

// Declare attributes for substr command
// <substr offset="7" count="4" value="Behind Blue Eyes" property="color" />
XML_ATTR SUBSTR_ATTRIBUTES[] =
{
  {"count",       SA_XML_ATTR_MANDATORY, NULL, NULL},
  {"value",       SA_XML_ATTR_MANDATORY, NULL, NULL},
  {"property",    SA_XML_ATTR_MANDATORY, NULL, NULL},
  {"offset",      SA_XML_ATTR_OPTINAL,   NULL, NULL},
};
size_t SUBSTR_ATTRIBUTES_COUNT = sizeof(SUBSTR_ATTRIBUTES) / sizeof(SUBSTR_ATTRIBUTES[0]);

// Declare attributes for strlen command
// <strlen value="${foobar}" property="foobar.length" />
XML_ATTR STRLEN_ATTRIBUTES[] =
{
  {"value",     SA_XML_ATTR_MANDATORY, NULL, NULL},
  {"property",  SA_XML_ATTR_MANDATORY, NULL, NULL},
};
size_t STRLEN_ATTRIBUTES_COUNT = sizeof(STRLEN_ATTRIBUTES) / sizeof(STRLEN_ATTRIBUTES[0]);

// Declare attributes for strreplace command
// <strreplace text="Everything is in the right place" token="right" value="wrong" property="inverse" />
XML_ATTR STRREPLACE_ATTRIBUTES[] =
{
  {"text",      SA_XML_ATTR_MANDATORY, NULL, NULL},
  {"token",     SA_XML_ATTR_MANDATORY, NULL, NULL},
  {"value",     SA_XML_ATTR_MANDATORY, NULL, NULL},
  {"property",  SA_XML_ATTR_MANDATORY, NULL, NULL},
};
size_t STRREPLACE_ATTRIBUTES_COUNT = sizeof(STRREPLACE_ATTRIBUTES) / sizeof(STRREPLACE_ATTRIBUTES[0]);

// Declare attributes for strreplace command
// <struppercase value="${foobar}" property="foobar" />
XML_ATTR STRUPPERCASE_ATTRIBUTES[] =
{
  {"value",     SA_XML_ATTR_MANDATORY, NULL, NULL},
  {"property",  SA_XML_ATTR_MANDATORY, NULL, NULL},
};
size_t STRUPPERCASE_ATTRIBUTES_COUNT = sizeof(STRUPPERCASE_ATTRIBUTES) / sizeof(STRUPPERCASE_ATTRIBUTES[0]);

// Declare attributes for strreplace command
// <strlowercase value="${foobar}" property="foobar" />
XML_ATTR STRLOWERCASE_ATTRIBUTES[] =
{
  {"value",     SA_XML_ATTR_MANDATORY, NULL, NULL},
  {"property",  SA_XML_ATTR_MANDATORY, NULL, NULL},
};
size_t STRLOWERCASE_ATTRIBUTES_COUNT = sizeof(STRLOWERCASE_ATTRIBUTES) / sizeof(STRLOWERCASE_ATTRIBUTES[0]);

// Declare attributes for strreplace command
// <strfind text="You shall not pass!" value="!" property="exclamation.offset" />
XML_ATTR STRFIND_ATTRIBUTES[] =
{
  {"text",      SA_XML_ATTR_MANDATORY, NULL, NULL},
  {"value",     SA_XML_ATTR_MANDATORY, NULL, NULL},
  {"property",  SA_XML_ATTR_MANDATORY, NULL, NULL},
  {"offset",    SA_XML_ATTR_OPTINAL,   NULL, NULL},
};
size_t STRFIND_ATTRIBUTES_COUNT = sizeof(STRFIND_ATTRIBUTES) / sizeof(STRFIND_ATTRIBUTES[0]);




void parse_string(const char* str, size_t& value)
{
  std::stringstream ss(str);
  ss >> value;
}

void to_string(const size_t& value, std::string& str)
{
  std::stringstream ss;
  ss << value;
  str = ss.str();
}

void string_replace(std::string& text, const std::string& token, const std::string& value)
{
  size_t start_pos = 0;
  size_t find_pos = std::string::npos;
  do
  {
    find_pos = text.find(token, start_pos);
    if (find_pos != std::string::npos)
    {
      text.replace(find_pos, token.length(), value);
      start_pos = find_pos + value.length();
    }
  } while (find_pos != -1);
}








sa_error_t substr_event_create(sa_action_event_t evnt)
{
  const char* name = sa_plugin_action_get_name();
  const char* xml = sa_plugin_action_get_xml();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  sa_property_store_immutable_t store_immutable = sa_property_store_to_immutable(store);
  XML_ATTR* attrs = SUBSTR_ATTRIBUTES;
  size_t count = SUBSTR_ATTRIBUTES_COUNT;

  sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Creating action '%s'.", name);

  sa_xml_attr_list_init(attrs, count);
  sa_error_t result = sa_xml_parse_attr_list_store(xml, attrs, count, store);
  if (result != SA_ERROR_SUCCESS)
    return result;

  return SA_ERROR_SUCCESS;
}

sa_error_t substr_event_execute(sa_action_event_t evnt)
{
  const char* action_name = sa_plugin_action_get_name();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  sa_property_store_immutable_t store_immutable = sa_property_store_to_immutable(store);
  XML_ATTR* attrs = SUBSTR_ATTRIBUTES;
  size_t count = SUBSTR_ATTRIBUTES_COUNT;

  // Expand attributes
  sa_xml_attr_list_update(attrs, count, &store_immutable);

  // Get expanded attribute values individually
  const char* pid_str = attrs[0].tmp_expanded;
  const char* filename = attrs[1].tmp_expanded;

  // Get expanded attribute values individually
  const char* attr_count_str = attrs[0].tmp_expanded;
  const std::string attr_value = attrs[1].tmp_expanded;
  const char* attr_property = attrs[2].tmp_expanded;
  const char* attr_offset = attrs[3].tmp_expanded;

  // Convert string values to numeric values
  size_t attr_count = 0;
  parse_string(attr_count_str, attr_count);
  size_t offset = 0;
  if (attr_offset != NULL)
    parse_string(attr_offset, offset);

  // Execute this action
  // <substr offset="7" count="4" value="Behind Blue Eyes" property="color" />
  std::string output;
  if (offset < attr_value.size())
    output = attr_value.substr(offset, attr_count);

  sa_properties_set(attr_property, output.c_str());

  // Cleanup allocated c strings
  sa_xml_attr_list_cleanup(attrs, count);

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
    return SA_ERROR_SUCCESS;
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

  sa_property_store_immutable_t store_immutable = sa_property_store_to_immutable(store);
  XML_ATTR* attrs = STRLEN_ATTRIBUTES;
  size_t count = STRLEN_ATTRIBUTES_COUNT;

  sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Creating action '%s'.", name);

  sa_xml_attr_list_init(attrs, count);
  sa_error_t result = sa_xml_parse_attr_list_store(xml, attrs, count, store);
  if (result != SA_ERROR_SUCCESS)
    return result;

  return SA_ERROR_SUCCESS;
}

sa_error_t strlen_event_execute(sa_action_event_t evnt)
{
  const char* action_name = sa_plugin_action_get_name();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  sa_property_store_immutable_t store_immutable = sa_property_store_to_immutable(store);
  XML_ATTR* attrs = STRLEN_ATTRIBUTES;
  size_t count = STRLEN_ATTRIBUTES_COUNT;

  // Expand attributes
  sa_xml_attr_list_update(attrs, count, &store_immutable);

  // Get expanded attribute values individually
  const std::string attr_value = attrs[0].tmp_expanded;
  const char* attr_property = attrs[1].tmp_expanded;

  // Execute this action
  // <strlen value="${sa_plugin_strings.name}" property="sa_plugin_strings.strlen" />

  size_t length = attr_value.size();
  std::string output;
  to_string(length, output);

  sa_properties_set(attr_property, output.c_str());

  // Cleanup allocated c strings
  sa_xml_attr_list_cleanup(attrs, count);

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
    return SA_ERROR_SUCCESS;
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

  sa_property_store_immutable_t store_immutable = sa_property_store_to_immutable(store);
  XML_ATTR* attrs = STRREPLACE_ATTRIBUTES;
  size_t count = STRREPLACE_ATTRIBUTES_COUNT;

  sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Creating action '%s'.", name);

  sa_xml_attr_list_init(attrs, count);
  sa_error_t result = sa_xml_parse_attr_list_store(xml, attrs, count, store);
  if (result != SA_ERROR_SUCCESS)
    return result;

  return SA_ERROR_SUCCESS;
}

sa_error_t strreplace_event_execute(sa_action_event_t evnt)
{
  const char* action_name = sa_plugin_action_get_name();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  sa_property_store_immutable_t store_immutable = sa_property_store_to_immutable(store);
  XML_ATTR* attrs = STRREPLACE_ATTRIBUTES;
  size_t count = STRREPLACE_ATTRIBUTES_COUNT;

  // Expand attributes
  sa_xml_attr_list_update(attrs, count, &store_immutable);

  // Get expanded attribute values individually
  std::string attr_text = attrs[0].tmp_expanded;
  const char* attr_token = attrs[1].tmp_expanded;
  const char* attr_value = attrs[2].tmp_expanded;
  const char* attr_property = attrs[3].tmp_expanded;

  // Execute this action
  // <strreplace text="Everything is in the right place" token="right" value="wrong" property="inverse" />
  string_replace(attr_text, attr_token, attr_value);

  sa_properties_set(attr_property, attr_text.c_str());

  // Cleanup allocated c strings
  sa_xml_attr_list_cleanup(attrs, count);

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
    return SA_ERROR_SUCCESS;
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

  sa_property_store_immutable_t store_immutable = sa_property_store_to_immutable(store);
  XML_ATTR* attrs = STRUPPERCASE_ATTRIBUTES;
  size_t count = STRUPPERCASE_ATTRIBUTES_COUNT;

  sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Creating action '%s'.", name);

  sa_xml_attr_list_init(attrs, count);
  sa_error_t result = sa_xml_parse_attr_list_store(xml, attrs, count, store);
  if (result != SA_ERROR_SUCCESS)
    return result;

  return SA_ERROR_SUCCESS;
}

sa_error_t struppercase_event_execute(sa_action_event_t evnt)
{
  const char* action_name = sa_plugin_action_get_name();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  sa_property_store_immutable_t store_immutable = sa_property_store_to_immutable(store);
  XML_ATTR* attrs = STRUPPERCASE_ATTRIBUTES;
  size_t count = STRUPPERCASE_ATTRIBUTES_COUNT;

  // Expand attributes
  sa_xml_attr_list_update(attrs, count, &store_immutable);

  // Get expanded attribute values individually
  const char* attr_value = attrs[0].tmp_expanded;
  const char* attr_property = attrs[1].tmp_expanded;

  // Execute this action
  // <struppercase value="${foobar}" property="foobar" />
  std::string output;
  output = attr_value;
  std::transform(output.begin(), output.end(), output.begin(), toupper);

  sa_properties_set(attr_property, output.c_str());

  // Cleanup allocated c strings
  sa_xml_attr_list_cleanup(attrs, count);

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
    return SA_ERROR_SUCCESS;
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

  sa_property_store_immutable_t store_immutable = sa_property_store_to_immutable(store);
  XML_ATTR* attrs = STRLOWERCASE_ATTRIBUTES;
  size_t count = STRLOWERCASE_ATTRIBUTES_COUNT;

  sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Creating action '%s'.", name);

  sa_xml_attr_list_init(attrs, count);
  sa_error_t result = sa_xml_parse_attr_list_store(xml, attrs, count, store);
  if (result != SA_ERROR_SUCCESS)
    return result;

  return SA_ERROR_SUCCESS;
}

sa_error_t strlowercase_event_execute(sa_action_event_t evnt)
{
  const char* action_name = sa_plugin_action_get_name();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  sa_property_store_immutable_t store_immutable = sa_property_store_to_immutable(store);
  XML_ATTR* attrs = STRLOWERCASE_ATTRIBUTES;
  size_t count = STRLOWERCASE_ATTRIBUTES_COUNT;

  // Expand attributes
  sa_xml_attr_list_update(attrs, count, &store_immutable);

  // Get expanded attribute values individually
  const char* attr_value = attrs[0].tmp_expanded;
  const char* attr_property = attrs[1].tmp_expanded;

  // Execute this action
  // <strlowercase value="${foobar}" property="foobar" />
  std::string output;
  output = attr_value;
  std::transform(output.begin(), output.end(), output.begin(), tolower);

  sa_properties_set(attr_property, output.c_str());

  // Cleanup allocated c strings
  sa_xml_attr_list_cleanup(attrs, count);

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
    return SA_ERROR_SUCCESS;
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

  sa_property_store_immutable_t store_immutable = sa_property_store_to_immutable(store);
  XML_ATTR* attrs = STRFIND_ATTRIBUTES;
  size_t count = STRFIND_ATTRIBUTES_COUNT;

  sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Creating action '%s'.", name);

  sa_xml_attr_list_init(attrs, count);
  sa_error_t result = sa_xml_parse_attr_list_store(xml, attrs, count, store);
  if (result != SA_ERROR_SUCCESS)
    return result;

  return SA_ERROR_SUCCESS;
}

sa_error_t strfind_event_execute(sa_action_event_t evnt)
{
  const char* action_name = sa_plugin_action_get_name();
  sa_property_store_t* store = sa_plugin_action_get_property_store();

  sa_property_store_immutable_t store_immutable = sa_property_store_to_immutable(store);
  XML_ATTR* attrs = STRFIND_ATTRIBUTES;
  size_t count = STRFIND_ATTRIBUTES_COUNT;

  // Expand attributes
  sa_xml_attr_list_update(attrs, count, &store_immutable);

  // Get expanded attribute values individually
  const std::string attr_text = attrs[0].tmp_expanded;
  const char* attr_value = attrs[1].tmp_expanded;
  const char* attr_property = attrs[2].tmp_expanded;
  const char* attr_offset = attrs[3].tmp_expanded;

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

  // Cleanup allocated c strings
  sa_xml_attr_list_cleanup(attrs, count);

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
    return SA_ERROR_SUCCESS;
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

EXPORT_API sa_error_t sa_plugin_terminate()
{
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
  PLUGIN_REGISTER_EVENT("substr", &substr_event);
  PLUGIN_REGISTER_EVENT("strreplace", &strreplace_event);
  PLUGIN_REGISTER_EVENT("strlen", &strlen_event);
  PLUGIN_REGISTER_EVENT("struppercase", &struppercase_event);
  PLUGIN_REGISTER_EVENT("strlowercase", &strlowercase_event);
  PLUGIN_REGISTER_EVENT("strfind", &strfind_event);

  return SA_ERROR_SUCCESS;
}

#ifdef __cplusplus
#if 0
{  // do not indent code inside extern C
#endif
}  // extern "C"
#endif
