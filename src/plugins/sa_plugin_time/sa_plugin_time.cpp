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
#include <string>
#include <ctime>
#include <Windows.h>

#define EXPORT_API __declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#endif

  // do not indent code inside extern C
#if 0
}
#endif

static const char* PLUGIN_NAME_IDENTIFIER = "sa_plugin_time";
static const char* START_TIME_ATTR = "start_time";
static const char* END_TIME_ATTR = "end_time";

std::tm get_current_time()
{
  std::time_t current_time = std::time(0);   // get time now
  std::tm now = *std::localtime(&current_time);
  return now;
}

bool is_digit(char c)
{
  if (c >= '0' && c <= '9')
    return true;
  return false;
}

bool is_valid_time(const std::string& value)
{
  if (value.size() != 5)
    return false;
  if (value[2] != ':')
    return false;
  if (
    !is_digit(value[0]) ||
    !is_digit(value[1]) ||
    !is_digit(value[3]) ||
    !is_digit(value[4]))
    return false;
  return true;
}

sa_error_t parse_time(const char* attribute_name, const char* value, std::tm* output)
{
  if (value == NULL)
  {
    sa_logging_print_format(SA_LOG_LEVEL_ERROR, PLUGIN_NAME_IDENTIFIER, "Attribute '%s' is not found.", attribute_name);
    return SA_ERROR_INVALID_ARGUMENTS;
  }
  else if (value[0] == '\0')
  {
    sa_logging_print_format(SA_LOG_LEVEL_ERROR, PLUGIN_NAME_IDENTIFIER, "Attribute '%s' is empty.", attribute_name);
    return SA_ERROR_INVALID_ARGUMENTS;
  }
  else if (output == NULL)
    return SA_ERROR_INVALID_ARGUMENTS;

  std::string input = value;
  if (!is_valid_time(input))
  {
    sa_logging_print_format(SA_LOG_LEVEL_ERROR, PLUGIN_NAME_IDENTIFIER, "Unknown time format: '%s'.", value);
    return SA_ERROR_INVALID_ARGUMENTS;
  }

  std::string hour_str = input.substr(0, 2);
  std::string min_str = input.substr(3, 2);
  int hour = atoi(hour_str.c_str());
  int min = atoi(min_str.c_str());

  //we only care about hour and minute.
  *output = get_current_time();
  output->tm_hour = hour;
  output->tm_min = min;

  return SA_ERROR_SUCCESS;
}

bool is_between_now(const std::tm& start_time, const std::tm& end_time)
{
  std::tm tmp_now = get_current_time();
  std::tm tmp_a = start_time;
  std::tm tmp_b = end_time;

  std::time_t now = std::mktime(&tmp_now);
  std::time_t a = std::mktime(&tmp_a);
  std::time_t b = std::mktime(&tmp_b);
  if (a <= now && now <= b)
    return true;
  return false;
}

sa_boolean sa_plugin_time_validate_time_of_day()
{
  sa_selection_context_immutable_t* ctx = sa_plugin_validation_get_selection_context();
  sa_property_store_immutable_t* store = sa_plugin_validation_get_property_store();

  const char* start_time_str = sa_property_store_get_property_cstr(store, START_TIME_ATTR);
  const char* end_time_str = sa_property_store_get_property_cstr(store, END_TIME_ATTR);

  //initialize both times to "now"
  std::tm start_time = { 0 };
  std::tm end_time = { 0 };

  //apply the hour/min values
  sa_error_t parse_result;
  parse_result = parse_time(START_TIME_ATTR, start_time_str, &start_time);
  if (parse_result != SA_ERROR_SUCCESS)
    return 0;
  parse_result = parse_time(END_TIME_ATTR, end_time_str, &end_time);
  if (parse_result != SA_ERROR_SUCCESS)
    return 0;

  // compare against "now"
  std::tm now = get_current_time();
  bool valid = is_between_now(start_time, end_time);
  if (valid)
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
  // register validation function for 'start_time' and 'end_time' custom attributes
  static const char* time_attributes[] = {
    START_TIME_ATTR,
    END_TIME_ATTR,
  };
  static const size_t time_attributes_count = sizeof(time_attributes) / sizeof(time_attributes[0]);
  sa_plugin_validation_attributes_func validate_func = &sa_plugin_time_validate_time_of_day;
  sa_error_t result = sa_plugin_register_validation_attributes(time_attributes, time_attributes_count, validate_func);
  if (result != SA_ERROR_SUCCESS)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Failed registering validation function for attributes '%s' and '%s'.", time_attributes[0], time_attributes[1]);
    return result;
  }

  return SA_ERROR_SUCCESS;
}

void test()
{
#define ASSERT(expression) if (!(expression)) { printf("The assertion '%s' at line %d has failed.\n", #expression, __LINE__); exit(1); }

  printf("%s()\n", __FUNCTION__);

  //wait for a "safe" time in minutes
  std::tm now = get_current_time();
  while (now.tm_min <= 1 || now.tm_min >= 58)
  {
    Sleep(1000);
    now = get_current_time();
  }

  std::tm a = now;
  std::tm b = now;

  // assert exactly now
  a = now;
  b = now;
  ASSERT(is_between_now(a, b) == true);

  // exactly in between
  a = now;
  b = now;
  a.tm_min--;
  b.tm_min++;
  ASSERT(is_between_now(a, b) == true);

  // too early
  a = now;
  b = now;
  a.tm_min -= 2;
  b.tm_min -= 1;
  ASSERT(is_between_now(a, b) == false);

  // too late
  a = now;
  b = now;
  a.tm_min += 1;
  b.tm_min += 2;
  ASSERT(is_between_now(a, b) == false);

  printf("All tests in function '%s' has passed!\n", __FUNCTION__);
#undef ASSERT
}

// do not indent code inside extern C
#if 0
{
#endif

#ifdef __cplusplus
}
#endif
