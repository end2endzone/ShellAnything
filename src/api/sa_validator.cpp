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

#include "shellanything/sa_validator.h"
#include "Validator.h"
#include "sa_types_private.h"
#include "sa_string_private.h"

using namespace shellanything;

sa_validator_immutable_t sa_validator_to_immutable(sa_validator_t* validator)
{
  sa_validator_immutable_t output;
  output.opaque = validator->opaque;
  return output;
}

int sa_validator_is_attribute_inversed(sa_validator_immutable_t* validator, const char* name)
{
  if (name == NULL)
    return 0;
  bool inversed = AS_CLASS_VALIDATOR(validator)->IsInversed(name);
  if (inversed)
    return 1;
  return 0;
}

int sa_validator_validate(sa_validator_immutable_t* validator, sa_context_immutable_t* ctx)
{
  if (ctx == NULL)
    return 0;
  const Context* context = AS_CLASS_CONTEXT(ctx);
  bool valid = AS_CLASS_VALIDATOR(validator)->Validate(*context);
  if (valid)
    return 1;
  return 0;
}

int sa_validator_get_max_files(sa_validator_immutable_t* validator)
{
  int value = AS_CLASS_VALIDATOR(validator)->GetMaxFiles();
  return value;
}

void sa_validator_set_max_files(sa_validator_t* validator, int max_files)
{
  AS_CLASS_VALIDATOR(validator)->SetMaxFiles(max_files);
}

int sa_validator_get_max_directories(sa_validator_immutable_t* validator)
{
  int value = AS_CLASS_VALIDATOR(validator)->GetMaxDirectories();
  return value;
}

void sa_validator_set_max_directories(sa_validator_t* validator, int max_directories)
{
  AS_CLASS_VALIDATOR(validator)->SetMaxDirectories(max_directories);
}

sa_error_t sa_validator_get_properties_buffer(sa_validator_immutable_t* validator, int* length, char* buffer, size_t size)
{
  if (length)
    *length = -1;
  const std::string& properties = AS_CLASS_VALIDATOR(validator)->GetProperties();
  sa_error_t result = sa_cstr_copy_buffer(buffer, size, length, properties);
  return result;
}

sa_error_t sa_validator_get_properties_string(sa_validator_immutable_t* validator, sa_string_t* str)
{
  const std::string& properties = AS_CLASS_VALIDATOR(validator)->GetProperties();
  sa_cstr_copy_string(str, properties);
  return SA_ERROR_SUCCESS;
}

void sa_validator_set_properties(sa_validator_t* validator, const char* value)
{
  AS_CLASS_VALIDATOR(validator)->SetProperties(value);
}

sa_error_t sa_validator_get_file_extensions_buffer(sa_validator_immutable_t* validator, int* length, char* buffer, size_t size)
{
  if (length)
    *length = -1;
  const std::string& file_extensions = AS_CLASS_VALIDATOR(validator)->GetFileExtensions();
  sa_error_t result = sa_cstr_copy_buffer(buffer, size, length, file_extensions);
  return result;
}

sa_error_t sa_validator_get_file_extensions_string(sa_validator_immutable_t* validator, sa_string_t* str)
{
  const std::string& file_extensions = AS_CLASS_VALIDATOR(validator)->GetFileExtensions();
  sa_cstr_copy_string(str, file_extensions);
  return SA_ERROR_SUCCESS;
}

void sa_validator_set_file_extensions(sa_validator_t* validator, const char* value)
{
  AS_CLASS_VALIDATOR(validator)->SetFileExtensions(value);
}

sa_error_t sa_validator_get_file_exists_buffer(sa_validator_immutable_t* validator, int* length, char* buffer, size_t size)
{
  if (length)
    *length = -1;
  const std::string& file_exists = AS_CLASS_VALIDATOR(validator)->GetFileExists();
  sa_error_t result = sa_cstr_copy_buffer(buffer, size, length, file_exists);
  return result;
}

sa_error_t sa_validator_get_file_exists_string(sa_validator_immutable_t* validator, sa_string_t* str)
{
  const std::string& file_exists = AS_CLASS_VALIDATOR(validator)->GetFileExists();
  sa_cstr_copy_string(str, file_exists);
  return SA_ERROR_SUCCESS;
}

void sa_validator_set_file_exists(sa_validator_t* validator, const char* value)
{
  AS_CLASS_VALIDATOR(validator)->SetFileExists(value);
}

sa_error_t sa_validator_get_class_buffer(sa_validator_immutable_t* validator, int* length, char* buffer, size_t size)
{
  if (length)
    *length = -1;
  const std::string& value = AS_CLASS_VALIDATOR(validator)->GetClass();
  sa_error_t result = sa_cstr_copy_buffer(buffer, size, length, value);
  return result;
}

sa_error_t sa_validator_get_class_string(sa_validator_immutable_t* validator, sa_string_t* str)
{
  const std::string& value = AS_CLASS_VALIDATOR(validator)->GetClass();
  sa_cstr_copy_string(str, value);
  return SA_ERROR_SUCCESS;
}

void sa_validator_set_class(sa_validator_t* validator, const char* value)
{
  AS_CLASS_VALIDATOR(validator)->SetClass(value);
}

sa_error_t sa_validator_get_pattern_buffer(sa_validator_immutable_t* validator, int* length, char* buffer, size_t size)
{
  if (length)
    *length = -1;
  const std::string& value = AS_CLASS_VALIDATOR(validator)->GetPattern();
  sa_error_t result = sa_cstr_copy_buffer(buffer, size, length, value);
  return result;
}

sa_error_t sa_validator_get_pattern_string(sa_validator_immutable_t* validator, sa_string_t* str)
{
  const std::string& value = AS_CLASS_VALIDATOR(validator)->GetPattern();
  sa_cstr_copy_string(str, value);
  return SA_ERROR_SUCCESS;
}

void sa_validator_set_pattern(sa_validator_t* validator, const char* value)
{
  AS_CLASS_VALIDATOR(validator)->SetPattern(value);
}

sa_error_t sa_validator_get_exprtk_buffer(sa_validator_immutable_t* validator, int* length, char* buffer, size_t size)
{
  if (length)
    *length = -1;
  const std::string& value = AS_CLASS_VALIDATOR(validator)->GetExprtk();
  sa_error_t result = sa_cstr_copy_buffer(buffer, size, length, value);
  return result;
}

sa_error_t sa_validator_get_exprtk_string(sa_validator_immutable_t* validator, sa_string_t* str)
{
  const std::string& value = AS_CLASS_VALIDATOR(validator)->GetExprtk();
  sa_cstr_copy_string(str, value);
  return SA_ERROR_SUCCESS;
}

void sa_validator_set_exprtk(sa_validator_t* validator, const char* value)
{
  AS_CLASS_VALIDATOR(validator)->SetExprtk(value);
}

sa_error_t sa_validator_get_istrue_buffer(sa_validator_immutable_t* validator, int* length, char* buffer, size_t size)
{
  if (length)
    *length = -1;
  const std::string& value = AS_CLASS_VALIDATOR(validator)->GetIsTrue();
  sa_error_t result = sa_cstr_copy_buffer(buffer, size, length, value);
  return result;
}

sa_error_t sa_validator_get_istrue_string(sa_validator_immutable_t* validator, sa_string_t* str)
{
  const std::string& value = AS_CLASS_VALIDATOR(validator)->GetIsTrue();
  sa_cstr_copy_string(str, value);
  return SA_ERROR_SUCCESS;
}

void sa_validator_set_istrue(sa_validator_t* validator, const char* value)
{
  AS_CLASS_VALIDATOR(validator)->SetIsTrue(value);
}

sa_error_t sa_validator_get_isfalse_buffer(sa_validator_immutable_t* validator, int* length, char* buffer, size_t size)
{
  if (length)
    *length = -1;
  const std::string& value = AS_CLASS_VALIDATOR(validator)->GetIsFalse();
  sa_error_t result = sa_cstr_copy_buffer(buffer, size, length, value);
  return result;
}

sa_error_t sa_validator_get_isfalse_string(sa_validator_immutable_t* validator, sa_string_t* str)
{
  const std::string& value = AS_CLASS_VALIDATOR(validator)->GetIsFalse();
  sa_cstr_copy_string(str, value);
  return SA_ERROR_SUCCESS;
}

void sa_validator_set_isfalse(sa_validator_t* validator, const char* value)
{
  AS_CLASS_VALIDATOR(validator)->SetIsFalse(value);
}

sa_error_t sa_validator_get_inverse_buffer(sa_validator_immutable_t* validator, int* length, char* buffer, size_t size)
{
  if (length)
    *length = -1;
  const std::string& value = AS_CLASS_VALIDATOR(validator)->GetInserve();
  sa_error_t result = sa_cstr_copy_buffer(buffer, size, length, value);
  return result;
}

sa_error_t sa_validator_get_inverse_string(sa_validator_immutable_t* validator, sa_string_t* str)
{
  const std::string& value = AS_CLASS_VALIDATOR(validator)->GetInserve();
  sa_cstr_copy_string(str, value);
  return SA_ERROR_SUCCESS;
}

void sa_validator_set_inverse(sa_validator_t* validator, const char* value)
{
  AS_CLASS_VALIDATOR(validator)->SetInserve(value);
}
