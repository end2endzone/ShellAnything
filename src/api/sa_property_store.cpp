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

#include "shellanything/sa_property_store.h"
#include "PropertyStore.h"
#include "sa_private_casting.h"
#include "sa_string_private.h"

using namespace shellanything;

sa_property_store_immutable_t sa_property_store_to_immutable(sa_property_store_t* store)
{
  sa_property_store_immutable_t output;
  output.opaque = store->opaque;
  return output;
}

void sa_property_store_clear(sa_property_store_t* store)
{
  AS_CLASS_PROPERTY_STORE(store)->Clear();
}

void sa_property_store_clear_property(sa_property_store_t* store, const char* name)
{
  AS_CLASS_PROPERTY_STORE(store)->ClearProperty(name);
}

sa_boolean sa_property_store_has_property(sa_property_store_immutable_t* store, const char* name)
{
  bool found = AS_CLASS_PROPERTY_STORE(store)->HasProperty(name);
  if (found)
    return 1;
  return 0;
}

sa_boolean sa_property_store_has_properties(sa_property_store_immutable_t* store, const char* properties[], size_t count)
{
  StringList tmp;
  for (size_t i = 0; i < count; i++)
  {
    const char* name = properties[i];
    tmp.push_back(name);
  }

  bool found = AS_CLASS_PROPERTY_STORE(store)->HasProperties(tmp);
  if (found)
    return 1;
  return 0;
}

void sa_property_store_set_property(sa_property_store_t* store, const char* name, const char* value)
{
  AS_CLASS_PROPERTY_STORE(store)->SetProperty(name, value);
}

const char* sa_property_store_get_property_cstr(sa_property_store_immutable_t* store, const char* name)
{
  const shellanything::PropertyStore* store_class = AS_CLASS_PROPERTY_STORE(store);
  bool found = store_class->HasProperty(name);
  if (!found)
    return NULL;
  const std::string& value = store_class->GetProperty(name);
  return value.c_str();
}

const char* sa_property_store_get_property_alloc(sa_property_store_immutable_t* store, const char* name)
{
  const shellanything::PropertyStore* store_class = AS_CLASS_PROPERTY_STORE(store);
  bool found = store_class->HasProperty(name);
  if (!found)
    return NULL;
  const std::string& value = store_class->GetProperty(name);
  const char* output = value.c_str();
  return _strdup(output);
}

sa_error_t sa_property_store_get_property_buffer(sa_property_store_immutable_t* store, const char* name, int* length, char* buffer, size_t size)
{
  if (length)
    *length = -1;
  const std::string& value = AS_CLASS_PROPERTY_STORE(store)->GetProperty(name);
  sa_error_t result = sa_cstr_copy_buffer(buffer, size, length, value);
  return result;
}

sa_error_t sa_property_store_get_property_string(sa_property_store_immutable_t* store, const char* name, sa_string_t* str)
{
  const std::string& value = AS_CLASS_PROPERTY_STORE(store)->GetProperty(name);
  sa_string_copy_stdstr(str, value);
  return SA_ERROR_SUCCESS;
}

size_t sa_property_store_get_property_count(sa_property_store_immutable_t* store)
{
  size_t count = AS_CLASS_PROPERTY_STORE(store)->GetPropertyCount();
  return count;
}

sa_boolean sa_property_store_is_empty(sa_property_store_immutable_t* store)
{
  bool is_empty = AS_CLASS_PROPERTY_STORE(store)->IsEmpty();
  if (is_empty)
    return 1;
  return 0;
}

sa_error_t sa_property_store_get_properties(sa_property_store_immutable_t* store, char* names[], size_t count, size_t max_name_length)
{
  shellanything::StringList local_names;
  AS_CLASS_PROPERTY_STORE(store)->GetProperties(local_names);
  for (size_t i = 0; i < local_names.size(); i++)
  {
    if (i >= count)
      return SA_ERROR_VALUE_OUT_OF_BOUNDS;
    const std::string& value = local_names[i];
    if (value.length() >= max_name_length)
      return SA_ERROR_BUFFER_TOO_SMALL;
    strcpy(names[i], value.c_str()); // length should not be a problem since we already validated value.length() < max_name_length.
  }
  return SA_ERROR_SUCCESS;
}

sa_error_t sa_property_store_find_missing_properties(sa_property_store_immutable_t* store, const char* input_names[], size_t count_input_names, char* output_names[], size_t count_output_names, size_t output_names_max_length)
{
  shellanything::StringList local_input_names;

  // copy input_names to local_input_names
  for (size_t i = 0; i < count_input_names; i++)
  {
    local_input_names.push_back(input_names[i]);
  }

  // find actual missing properties
  shellanything::StringList local_output_names;
  AS_CLASS_PROPERTY_STORE(store)->FindMissingProperties(local_input_names, local_output_names);

  // copy local_output_names to output_names
  for (size_t i = 0; i < local_output_names.size(); i++)
  {
    if (i >= count_output_names)
      return SA_ERROR_VALUE_OUT_OF_BOUNDS;
    const std::string& value = local_output_names[i];
    if (value.length() >= output_names_max_length)
      return SA_ERROR_BUFFER_TOO_SMALL;
    strcpy(output_names[i], value.c_str()); // length should not be a problem since we already validated value.length() < output_names_max_length.
  }
  return SA_ERROR_SUCCESS;
}
