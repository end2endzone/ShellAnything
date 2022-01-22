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

#include "shellanything/sa_configuration.h"
#include "Configuration.h"
#include "sa_types_private.h"
#include "sa_string_private.h"

using namespace shellanything;

sa_configuration_immutable_t sa_configuration_to_immutable(sa_configuration_t* configuration)
{
  sa_configuration_immutable_t output;
  output.opaque = configuration->opaque;
  return output;
}

sa_error_t sa_configuration_load_file_buffer(const char* path, char* error_buffer, size_t error_size, sa_configuration_t* configuration)
{
  if (path == NULL || configuration == NULL)
    return SA_ERROR_INVALID_ARGUMENTS;

  std::string tmp_error;
  shellanything::Configuration* config = shellanything::Configuration::LoadFile(path, tmp_error);
  
  // Check for error
  if (config == NULL)
  {
    // Copy the error string. Truncate the error if necessary.
    sa_cstr_copy_truncate_buffer(error_buffer, error_size, tmp_error.c_str());
    return SA_ERROR_UNKNOWN;
  }

  *configuration = AS_TYPE_CONFIGURATION(config);
  return SA_ERROR_SUCCESS;
}

sa_error_t sa_configuration_load_file_string(const char* path, sa_string_t* error_str, sa_configuration_t* configuration)
{
  if (path == NULL || configuration == NULL)
    return SA_ERROR_INVALID_ARGUMENTS;

  std::string tmp_error;
  shellanything::Configuration* config = shellanything::Configuration::LoadFile(path, tmp_error);

  // Check for error
  if (config == NULL)
  {
    // Copy the error string. Truncate the error if necessary.
    sa_cstr_copy_string(error_str, tmp_error);
    return SA_ERROR_UNKNOWN;
  }

  *configuration = AS_TYPE_CONFIGURATION(config);
  return SA_ERROR_SUCCESS;
}

int sa_configuration_is_valid_config_file(const char* path)
{
  if (path == NULL)
    return 0;
  bool valid = shellanything::Configuration::IsValidConfigFile(path);
  if (valid)
    return 1;
  return 0;
}

sa_error_t sa_configuration_get_file_path_buffer(sa_configuration_immutable_t* configuration, int* length, char* buffer, size_t size)
{
  if (length)
    *length = -1;
  const std::string& file_path = AS_CLASS_CONFIGURATION(configuration)->GetFilePath();
  sa_error_t result = sa_cstr_copy_buffer(buffer, size, length, file_path);
  return result;
}

sa_error_t sa_configuration_get_file_path_string(sa_configuration_immutable_t* configuration, sa_string_t* str)
{
  const std::string& file_path = AS_CLASS_CONFIGURATION(configuration)->GetFilePath();
  sa_cstr_copy_string(str, file_path);
  return SA_ERROR_SUCCESS;
}

void sa_configuration_set_file_path(sa_configuration_t* configuration, const char* path)
{
  AS_CLASS_CONFIGURATION(configuration)->SetFilePath(path);
}

uint64_t sa_configuration_get_file_modified_date(sa_configuration_immutable_t* configuration)
{
  uint64_t value = AS_CLASS_CONFIGURATION(configuration)->GetFileModifiedDate();
  if (value)
    return 1;
  return 0;
}

void sa_configuration_set_file_modified_date(sa_configuration_t* configuration, uint64_t newdate)
{
  AS_CLASS_CONFIGURATION(configuration)->SetFileModifiedDate(newdate);
}

sa_error_t sa_configuration_get_description_buffer(sa_configuration_immutable_t* configuration, int* length, char* buffer, size_t size)
{
  if (length)
    *length = -1;
  const std::string& path = AS_CLASS_CONFIGURATION(configuration)->GetFilePath();
  sa_error_t result = sa_cstr_copy_buffer(buffer, size, length, path);
  return result;
}

sa_error_t sa_configuration_get_description_string(sa_configuration_immutable_t* configuration, sa_string_t* str)
{
  const std::string& path = AS_CLASS_CONFIGURATION(configuration)->GetFilePath();
  sa_cstr_copy_string(str, path);
  return SA_ERROR_SUCCESS;
}

void sa_configuration_set_description(sa_configuration_t* configuration, const char* path)
{
  AS_CLASS_CONFIGURATION(configuration)->SetFilePath(path);
}

uint64_t sa_configuration_is_visible(sa_configuration_immutable_t* configuration)
{
  uint64_t filedate = AS_CLASS_CONFIGURATION(configuration)->GetFileModifiedDate();
  return filedate;
}

void sa_configuration_set_visible(sa_configuration_t* configuration, uint64_t newdate)
{
  AS_CLASS_CONFIGURATION(configuration)->SetFileModifiedDate(newdate);
}

void sa_configuration_update(sa_configuration_t* configuration, sa_context_immutable_t* ctx)
{
  AS_CLASS_CONFIGURATION(configuration)->Update(*AS_CLASS_CONTEXT(ctx));
}

size_t sa_configuration_get_menu_count(sa_configuration_t* configuration)
{
  shellanything::Menu::MenuPtrList menus = AS_CLASS_CONFIGURATION(configuration)->GetMenus();
  size_t count = menus.size();
  return count;
}

sa_error_t sa_configuration_get_menu_element(sa_configuration_t* configuration, size_t index, sa_menu_t* menu)
{
  if (menu == NULL)
    return SA_ERROR_INVALID_ARGUMENTS;
  shellanything::Menu::MenuPtrList menus = AS_CLASS_CONFIGURATION(configuration)->GetMenus();
  if (menus.empty() || index > (menus.size() - 1))
    return SA_ERROR_VALUE_OUT_OF_BOUNDS;
  shellanything::Menu* menu_element = menus[index];
  *menu = AS_TYPE_MENU(menu_element);
  return SA_ERROR_SUCCESS;
}
