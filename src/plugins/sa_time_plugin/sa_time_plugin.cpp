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
#include "shellanything/sa_plugins.h"

#define EXPORT_API __declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#endif

static const char* PLUGIN_NAME_IDENTIFIER = "sa_time_plugin";

int sa_time_plugin_validate_time_of_day(const char** names, const char** values, const int** flags, size_t count)
{
  sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "%s(), received %z names : {", __FUNCTION__, count);
  for (size_t i = 0; i < count; i++)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "  %s --> %s", names[i], values[i]);
  }
  sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "}");

  return 1; // always valid
}

EXPORT_API sa_error_t sa_plugin_initialize(sa_version_info_t* version)
{
  if (version->major != 1)
    return SA_ERROR_NOT_SUPPORTED;
  return SA_ERROR_SUCCESS;
}

EXPORT_API sa_error_t sa_plugin_register()
{
  // register validation function for 'start_time' and 'end_time' custom attributes
  static const char* time_attributes[] = {
    "start_time",
    "end_time",
  };
  static const size_t time_attributes_count = sizeof(time_attributes) / sizeof(time_attributes[0]);
  sa_error_t result = sa_plugins_register_attribute_validation(time_attributes, time_attributes_count, &sa_time_plugin_validate_time_of_day);
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
