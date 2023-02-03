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
#include "stdlib.h"

#include "rapidassist/undef_windows_macros.h"
#include "rapidassist/strings.h"

#define EXPORT_API __declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#if 0
}  // do not indent code inside extern C
#endif
#endif

static const char* PLUGIN_NAME_IDENTIFIER = "sa_plugin_test_data";
static const char* PLUGIN_STATUS_PROPERTY_NAME = "sa_plugin_test_data.status";
static const char* ATTR_ID = "id";

const char* to_hex_string(void* address, char* buffer, size_t size)
{
  if (size == 0)
    return NULL;
  memset(buffer, 0, size);

  std::string output;
  char hex_str[64];
  size_t value = (size_t)address;
  for (int i = 0; i < sizeof(value); i++)
  {
    uint8_t low = value & 0xFF;
    sprintf(hex_str, "%02x", low);
    output.insert(0, hex_str);
    value = value >> 8;
  }
  output.insert(0, "0x");

  //copy to output
  if (output.size() <= size)
    memcpy(buffer, output.c_str(), output.size() + 1);
  return buffer;
}

// Declare attributes for sa_plugin_test_data action
// <sa_plugin_test_data id="${processid}" />
XML_ATTR SA_PLUGIN_TEST_DATA_ATTRIBUTES[] =
{
  {"id", SA_XML_ATTR_MANDATORY, 1, NULL, NULL},
};
size_t SA_PLUGIN_TEST_DATA_ATTRIBUTES_COUNT = sizeof(SA_PLUGIN_TEST_DATA_ATTRIBUTES) / sizeof(SA_PLUGIN_TEST_DATA_ATTRIBUTES[0]);

struct MY_TEST_DATA_STRUCT
{
  int id;
  int exec_count;
};

const char* get_property_name_prefix(MY_TEST_DATA_STRUCT* my_data, char* buffer, size_t size)
{
  if (size == 0)
    return NULL;
  memset(buffer, 0, size);
  std::string output;
  output += PLUGIN_NAME_IDENTIFIER;
  output += ".";
  output += ra::strings::ToString(my_data->id);

  //copy to output
  if (output.size() <= size)
    memcpy(buffer, output.c_str(), output.size() + 1);
  return buffer;
}

sa_error_t data_register(MY_TEST_DATA_STRUCT* my_data)
{
  sa_plugin_action_set_data(my_data);
  if (sa_plugin_action_get_data() != my_data)
    return SA_ERROR_ABORTED;

  //setup a proof that this struct exists.
  char buffer[1024];
  std::string address_property;
  address_property += get_property_name_prefix(my_data, buffer, sizeof(buffer));
  address_property += ".address";

  const char* address_value = to_hex_string(my_data, buffer, sizeof(buffer));

  //set a property to "remember" this structure
  sa_properties_set(address_property.c_str(), address_value);

  return SA_ERROR_SUCCESS;
}

sa_error_t data_validate()
{
  MY_TEST_DATA_STRUCT* my_data = (MY_TEST_DATA_STRUCT*)sa_plugin_action_get_data();
  if (my_data == NULL)
    return SA_ERROR_ABORTED;

  char buffer[256];

  std::string address_property;
  address_property += get_property_name_prefix(my_data, buffer, sizeof(buffer));
  address_property += ".address";

  char expected_address[128];
  int length = 0;
  sa_properties_get_buffer(address_property.c_str(), &length, expected_address, sizeof(expected_address));

  //compate pointer of my_data with the property matching the same id...
  if (strcmp(to_hex_string(my_data, buffer, sizeof(buffer)), expected_address))
    return SA_ERROR_ABORTED;

  return SA_ERROR_SUCCESS;
}

MY_TEST_DATA_STRUCT* data_new_copy(MY_TEST_DATA_STRUCT** my_data)
{
  //create a new instance
  MY_TEST_DATA_STRUCT* my_data2 = (MY_TEST_DATA_STRUCT*)malloc(sizeof(MY_TEST_DATA_STRUCT));

  //copy properties
  (*my_data2) = (**my_data);

  //delete the old one
  memset(*my_data, 0, sizeof(MY_TEST_DATA_STRUCT));
  free(*my_data);
  *my_data = NULL;

  return my_data2;
}

sa_error_t sa_plugin_test_data_event_create(sa_action_event_t evnt)
{
  const char* name = sa_plugin_action_get_name();
  const char* xml = sa_plugin_action_get_xml();

  // assert data from a previous parse should not be available.
  void* previous_data = sa_plugin_action_get_data();
  if (previous_data != NULL)
    return SA_ERROR_ABORTED;

  XML_ATTR* id_attr = &SA_PLUGIN_TEST_DATA_ATTRIBUTES[0];

  //find the id attribute
  char id_str[128];
  int id_len = 0;
  sa_error_t result = sa_xml_parse_attr_buffer(xml, id_attr, &id_len, id_str, sizeof(id_str));
  if (result != SA_ERROR_SUCCESS)
    return result;

  //parsed the attribute's value
  int value = 0;
  bool parsed = ra::strings::Parse(std::string(id_str), value);
  if (!parsed)
    return SA_ERROR_INVALID_ARGUMENTS;

  //allocate a data structure
  MY_TEST_DATA_STRUCT* my_data = (MY_TEST_DATA_STRUCT*)malloc(sizeof(MY_TEST_DATA_STRUCT));
  if (my_data == NULL)
    return SA_ERROR_NOT_SUPPORTED;

  //initialize the instance
  my_data->id = value;
  my_data->exec_count = 0;

  //remember this instance!
  result = data_register(my_data);
  if (result != SA_ERROR_SUCCESS)
    return result;

  //validate integrity of my_data pointer
  result = data_validate();
  if (result != SA_ERROR_SUCCESS)
    return result;

  return SA_ERROR_SUCCESS;
}

sa_error_t sa_plugin_test_data_event_execute(sa_action_event_t evnt)
{
  const char* action_name = sa_plugin_action_get_name();

  //validate integrity of my_data pointer
  sa_error_t result = data_validate();
  if (result != SA_ERROR_SUCCESS)
    return result;

  //create a new duplicate copy of my_data
  MY_TEST_DATA_STRUCT* my_data = (MY_TEST_DATA_STRUCT*)sa_plugin_action_get_data();
  MY_TEST_DATA_STRUCT* my_data2 = data_new_copy(&my_data);
  if (my_data2 == NULL)
    return SA_ERROR_ABORTED;

  //swap
  my_data = my_data2;
  my_data2 = NULL;

  //remember this NEW instance!
  result = data_register(my_data);
  if (result != SA_ERROR_SUCCESS)
    return result;

  //validate integrity of my_data pointer
  result = data_validate();
  if (result != SA_ERROR_SUCCESS)
    return result;

  char tmp[256];
  int length = 0;

  //increase execution counters
  my_data->exec_count++;
  std::string count_property;
  count_property += get_property_name_prefix(my_data, tmp, sizeof(tmp));
  count_property += ".count";
  sa_properties_set(count_property.c_str(), ra::strings::ToString(my_data->exec_count).c_str());

  return result;
}

sa_error_t sa_plugin_test_data_event_destroy(sa_action_event_t evnt)
{
  const char* action_name = sa_plugin_action_get_name();

  //validate integrity of my_data pointer
  sa_error_t result = data_validate();
  if (result != SA_ERROR_SUCCESS)
    return result;

  MY_TEST_DATA_STRUCT* my_data = (MY_TEST_DATA_STRUCT*)sa_plugin_action_get_data();

  //define a destroyed flag
  char tmp[256];
  std::string destroy_property;
  destroy_property += get_property_name_prefix(my_data, tmp, sizeof(tmp));
  destroy_property += ".destroy";

  const char* address_value = to_hex_string(my_data, tmp, sizeof(tmp));

  sa_properties_set(destroy_property.c_str(), address_value);

  memset(my_data, 0, sizeof(MY_TEST_DATA_STRUCT));
  free(my_data);

  return SA_ERROR_SUCCESS;
}

sa_error_t sa_plugin_test_data_event(sa_action_event_t evnt)
{
  sa_error_t result;
  switch (evnt)
  {
  case SA_ACTION_EVENT_CREATE:
    result = sa_plugin_test_data_event_create(evnt);
    return result;
  case SA_ACTION_EVENT_DESTROY:
    result = sa_plugin_test_data_event_destroy(evnt);
  case SA_ACTION_EVENT_EXECUTE:
    result = sa_plugin_test_data_event_execute(evnt);
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

  // Set a property to notify the plugin is initialized
  sa_properties_set(PLUGIN_STATUS_PROPERTY_NAME, "initilized");

  return SA_ERROR_SUCCESS;
}

EXPORT_API sa_error_t sa_plugin_terminate()
{
  // Set a property to notify the plugin has terminated
  sa_properties_delete(PLUGIN_STATUS_PROPERTY_NAME);

  return SA_ERROR_SUCCESS;
}

EXPORT_API sa_error_t sa_plugin_register()
{
  sa_error_t result;

  // register custom actions
#define PLUGIN_REGISTER_EVENT(name, event_func) \
  result = sa_plugin_register_action_event(name, event_func); \
  if (result != SA_ERROR_SUCCESS) \
  { \
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Failed registering event function for action '%s'.", name); \
    return result; \
  }

  PLUGIN_REGISTER_EVENT("sa_plugin_test_data", &sa_plugin_test_data_event);

#undef PLUGIN_REGISTER_EVENT

  return SA_ERROR_SUCCESS;
}

#ifdef __cplusplus
#if 0
{  // do not indent code inside extern C
#endif
}  // extern "C"
#endif
