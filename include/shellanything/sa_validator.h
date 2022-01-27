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

#ifndef SA_API_VALIDATOR_H
#define SA_API_VALIDATOR_H

#include "shellanything/sa_error.h"
#include "shellanything/sa_context.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sa_validator_t  {
  void* opaque;
} sa_validator_t;

typedef struct sa_validator_immutable_t  {
  void* opaque;
} sa_validator_immutable_t;

/// <summary>
/// Converts a mutable validator to an immutable validator.
/// </summary>
/// <param name="validator">The mutable object to convert.</param>
/// <returns>Returns an immutable validator</returns>
sa_validator_immutable_t sa_validator_to_immutable(sa_validator_t* validator);

/// <summary>
/// Returns 1 if the given attribute name is in the list of inversed attributes.
/// The accepted attribute names matches the list of attributes of <validity> or <visibility> elements in a Configuration File.
/// See the 'User Manual' for details.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="name">The name of the attribute. The name is case sensitive.</param>
/// <returns>Returns 1 if the given attribute name is in the list of inversed attributes. Returns 0 otherwise.</returns>
int sa_validator_is_attribute_inversed(sa_validator_immutable_t* validator, const char* name);

/// <summary>
/// Validate the given context against a set of constraints.
/// The possible constraints includes a minimum/maximum number of files/directories selected,
/// a list of properties that must be defined,
/// a list of file extensions,
/// ...
/// Note: this function is used to enable or disable a menu.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="ctx">The context used for validating.</param>
/// <returns>Returns 1 if the given context is valid against the set of constraints. Returns 0 otherwise.</returns>
int sa_validator_validate(sa_validator_immutable_t* validator, sa_context_immutable_t* ctx);

/// <summary>
/// Get the maxfiles parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <returns>Return the value for the maxfiles parameters.</returns>
int sa_validator_get_max_files(sa_validator_immutable_t* validator);

/// <summary>
/// Set the maxfiles parameter
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="max_files">The new parameter value</param>
void sa_validator_set_max_files(sa_validator_t* validator, int max_files);

/// <summary>
/// Get the maxdirectories parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <returns>Return the value for the maxdirectories parameters.</returns>
int sa_validator_get_max_directories(sa_validator_immutable_t* validator);

/// <summary>
/// Set the maxdirectories parameter
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="max_directories">The new parameter value</param>
void sa_validator_set_max_directories(sa_validator_t* validator, int max_directories);

/// <summary>
/// Get the properties parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="length">The length of the output value in bytes.</param>
/// <param name="buffer">The output buffer for the value.</param>
/// <param name="size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_validator_get_properties_buffer(sa_validator_immutable_t* validator, int* length, char* buffer, size_t size);

/// <summary>
/// Get the properties parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_validator_get_properties_string(sa_validator_immutable_t* validator, sa_string_t* str);

/// <summary>
/// Set the properties parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="value">The new parameter value.</param>
void sa_validator_set_properties(sa_validator_t* validator, const char* value);

/// <summary>
/// Get the file_extensions parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="length">The length of the output value in bytes.</param>
/// <param name="buffer">The output buffer for the value.</param>
/// <param name="size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_validator_get_file_extensions_buffer(sa_validator_immutable_t* validator, int* length, char* buffer, size_t size);

/// <summary>
/// Get the file_extensions parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_validator_get_file_extensions_string(sa_validator_immutable_t* validator, sa_string_t* str);

/// <summary>
/// Set the file_extensions parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="value">The new parameter value.</param>
void sa_validator_set_file_extensions(sa_validator_t* validator, const char* value);

/// <summary>
/// Get the file_exists parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="length">The length of the output value in bytes.</param>
/// <param name="buffer">The output buffer for the value.</param>
/// <param name="size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_validator_get_file_exists_buffer(sa_validator_immutable_t* validator, int* length, char* buffer, size_t size);

/// <summary>
/// Get the file_extensions parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_validator_get_file_exists_string(sa_validator_immutable_t* validator, sa_string_t* str);

/// <summary>
/// Set the file_extensions parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="value">The new parameter value.</param>
void sa_validator_set_file_exists(sa_validator_t* validator, const char* value);

/// <summary>
/// Get the class parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="length">The length of the output value in bytes.</param>
/// <param name="buffer">The output buffer for the value.</param>
/// <param name="size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_validator_get_class_buffer(sa_validator_immutable_t* validator, int* length, char* buffer, size_t size);

/// <summary>
/// Get the file_extensions parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_validator_get_class_string(sa_validator_immutable_t* validator, sa_string_t* str);

/// <summary>
/// Set the file_extensions parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="value">The new parameter value.</param>
void sa_validator_set_class(sa_validator_t* validator, const char* value);

/// <summary>
/// Get the pattern parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="length">The length of the output value in bytes.</param>
/// <param name="buffer">The output buffer for the value.</param>
/// <param name="size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_validator_get_pattern_buffer(sa_validator_immutable_t* validator, int* length, char* buffer, size_t size);

/// <summary>
/// Get the pattern parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_validator_get_pattern_string(sa_validator_immutable_t* validator, sa_string_t* str);

/// <summary>
/// Set the pattern parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="value">The new parameter value.</param>
void sa_validator_set_pattern(sa_validator_t* validator, const char* value);

/// <summary>
/// Get the exprtk parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="length">The length of the output value in bytes.</param>
/// <param name="buffer">The output buffer for the value.</param>
/// <param name="size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_validator_get_exprtk_buffer(sa_validator_immutable_t* validator, int* length, char* buffer, size_t size);

/// <summary>
/// Get the exprtk parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_validator_get_exprtk_string(sa_validator_immutable_t* validator, sa_string_t* str);

/// <summary>
/// Set the exprtk parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="value">The new parameter value.</param>
void sa_validator_set_exprtk(sa_validator_t* validator, const char* value);

/// <summary>
/// Get the istrue parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="length">The length of the output value in bytes.</param>
/// <param name="buffer">The output buffer for the value.</param>
/// <param name="size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_validator_get_istrue_buffer(sa_validator_immutable_t* validator, int* length, char* buffer, size_t size);

/// <summary>
/// Get the istrue parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_validator_get_istrue_string(sa_validator_immutable_t* validator, sa_string_t* str);

/// <summary>
/// Set the istrue parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="value">The new parameter value.</param>
void sa_validator_set_istrue(sa_validator_t* validator, const char* value);

/// <summary>
/// Get the isfalse parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="length">The length of the output value in bytes.</param>
/// <param name="buffer">The output buffer for the value.</param>
/// <param name="size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_validator_get_isfalse_buffer(sa_validator_immutable_t* validator, int* length, char* buffer, size_t size);

/// <summary>
/// Get the isfalse parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_validator_get_isfalse_string(sa_validator_immutable_t* validator, sa_string_t* str);

/// <summary>
/// Set the isfalse parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="value">The new parameter value.</param>
void sa_validator_set_isfalse(sa_validator_t* validator, const char* value);

/// <summary>
/// Get the inverse parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="length">The length of the output value in bytes.</param>
/// <param name="buffer">The output buffer for the value.</param>
/// <param name="size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_validator_get_inverse_buffer(sa_validator_immutable_t* validator, int* length, char* buffer, size_t size);

/// <summary>
/// Get the inverse parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_validator_get_inverse_string(sa_validator_immutable_t* validator, sa_string_t* str);

/// <summary>
/// Set the inverse parameter.
/// </summary>
/// <param name="validator">The validator structure object.</param>
/// <param name="value">The new parameter value.</param>
void sa_validator_set_inverse(sa_validator_t* validator, const char* value);

#ifdef __cplusplus
}
#endif

#endif //SA_API_VALIDATOR_H
