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
#include "shellanything/sa_selection_context.h"

#define EXPORT_API __declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#endif

static const char* PLUGIN_NAME_IDENTIFIER = "sa_plugin_demo";

sa_boolean sa_plugin_demo_validate(sa_selection_context_immutable_t* ctx, sa_property_store_immutable_t* store)
{
  size_t count = sa_property_store_get_property_count(store);

  sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "%s() {", __FUNCTION__);
  sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "count = %zu", count);

  if (count == 0)
    return 1; // valid

  // create a buffer to get all property names in the store
  size_t max_name_length = 1024;
  char** names = new char* [count];
  if (names == NULL)
    return 0;
  for (size_t i = 0; i < count; i++)
  {
    names[i] = new char [max_name_length];
    if (names[i] == NULL)
      return 0;
  }

  // get all property names in the store.
  sa_error_t result = sa_property_store_get_properties(store, names, count, max_name_length);
  if (result != SA_ERROR_SUCCESS)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Failed getting all property names in the property store.");
    return 0;
  }

  // print property names and values of the given store
  sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Attributes: {");
  for (size_t i = 0; i < count; i++)
  {
    const char* property_name = names[i];
    const char * property_value = sa_property_store_get_property_c_str(store, property_name);

    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "  %s = %s", property_name, property_value);
  }
  sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "}");

  // memory cleanup
  for (size_t i = 0; i < count; i++)
    delete[] names[i];
  delete[] names;

  return 1; // always valid
}

EXPORT_API sa_error_t sa_plugin_initialize(sa_version_info_t* version)
{
  if (version->major == 0 && version->minor < 8) // this plugin is designed for version 0.8.0 and over
    return SA_ERROR_NOT_SUPPORTED;
  return SA_ERROR_SUCCESS;
}

EXPORT_API sa_error_t sa_plugin_register()
{
  // register validation function for 'demo1' and 'demo2' custom attributes
  static const char* time_attributes[] = {
    "demo1",
    "demo2",
  };
  static const size_t time_attributes_count = sizeof(time_attributes) / sizeof(time_attributes[0]);
  sa_plugin_attribute_validate_func validate_func = &sa_plugin_demo_validate;
  sa_error_t result = sa_plugin_register_attribute_validation(time_attributes, time_attributes_count, validate_func);
  if (result != SA_ERROR_SUCCESS)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Failed registering validation function for attributes '%s' and '%s'.", time_attributes[0], time_attributes[1]);
    return result;
  }

  return SA_ERROR_SUCCESS;
}

#ifdef __cplusplus
}
#endif
