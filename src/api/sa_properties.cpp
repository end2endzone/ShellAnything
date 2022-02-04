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

#include "shellanything/sa_properties.h"
#include "sa_string_private.h"
#include "PropertyManager.h"

using namespace shellanything;

void sa_properties_clear()
{
  PropertyManager& pmgr = PropertyManager::GetInstance();
  pmgr.Clear();
}

void sa_properties_delete(const char* name)
{
  if (name == NULL)
    return;
  PropertyManager& pmgr = PropertyManager::GetInstance();
  pmgr.ClearProperty(name);
}

int sa_properties_exists(const char* name)
{
  if (name == NULL)
    return 0;
  PropertyManager& pmgr = PropertyManager::GetInstance();
  if (pmgr.HasProperty(name))
    return 1;
  return 0;
}

void sa_properties_set(const char* name, const char* value)
{
  if (name == NULL || value == NULL)
    return;
  PropertyManager& pmgr = PropertyManager::GetInstance();
  pmgr.SetProperty(name, value);
}

sa_error_t sa_properties_get_buffer(const char* name, int* property_length, char* buffer, size_t buffer_size)
{
  if (property_length)
    *property_length = -1;
  if (name == NULL)
    return SA_ERROR_INVALID_ARGUMENTS;
  PropertyManager& pmgr = PropertyManager::GetInstance();
  if (!sa_properties_exists(name))
    return SA_ERROR_NOT_FOUND;
  const std::string & property_value = pmgr.GetProperty(name);
  sa_error_t result = sa_cstr_copy_buffer(buffer, buffer_size, property_length, property_value);
  return result;
}

sa_error_t sa_properties_get_string(const char* name, sa_string_t* str)
{
  if (name == NULL || str == NULL)
    return SA_ERROR_INVALID_ARGUMENTS;
  PropertyManager& pmgr = PropertyManager::GetInstance();
  if (!sa_properties_exists(name))
    return SA_ERROR_NOT_FOUND;
  const std::string& property_value = pmgr.GetProperty(name);
  sa_cstr_copy_string(str, property_value);
  return SA_ERROR_SUCCESS;
}

sa_error_t sa_properties_expand_buffer(const char* value, int* expanded_length, char* buffer, size_t buffer_size)
{
  if (expanded_length)
    *expanded_length = -1;
  if (value == NULL)
    return SA_ERROR_INVALID_ARGUMENTS;
  PropertyManager& pmgr = PropertyManager::GetInstance();
  std::string expanded_value = pmgr.Expand(value);
  sa_error_t result = sa_cstr_copy_buffer(buffer, buffer_size, expanded_length, expanded_value);
  return result;
}

sa_error_t sa_properties_expand_string(const char* value, sa_string_t* str)
{
  if (value == NULL || str == NULL)
    return SA_ERROR_INVALID_ARGUMENTS;
  PropertyManager& pmgr = PropertyManager::GetInstance();
  std::string expanded_value = pmgr.Expand(value);
  sa_cstr_copy_string(str, expanded_value);
  return SA_ERROR_SUCCESS;
}

sa_error_t sa_properties_expand_once_buffer(const char* value, int* expanded_length, char* buffer, size_t buffer_size)
{
  if (expanded_length)
    *expanded_length = -1;
  if (value == NULL)
    return SA_ERROR_INVALID_ARGUMENTS;
  PropertyManager& pmgr = PropertyManager::GetInstance();
  std::string expanded_value = pmgr.ExpandOnce(value);
  sa_error_t result = sa_cstr_copy_buffer(buffer, buffer_size, expanded_length, expanded_value);
  return result;
}

sa_error_t sa_properties_expand_once_string(const char* value, sa_string_t* str)
{
  if (value == NULL || str == NULL)
    return SA_ERROR_INVALID_ARGUMENTS;
  PropertyManager& pmgr = PropertyManager::GetInstance();
  std::string expanded_value = pmgr.ExpandOnce(value);
  sa_cstr_copy_string(str, expanded_value);
  return SA_ERROR_SUCCESS;
}
