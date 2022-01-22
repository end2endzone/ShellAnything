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

#ifndef SA_API_PROPERTIES_H
#define SA_API_PROPERTIES_H

#include "shellanything/sa_string.h"
#include "shellanything/sa_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Clears all the registered properties.
/// Note that environement variable properties are still registered to the manager.
/// </summary>
void sa_properties_clear();

/// <summary>
/// Delete the given property.
/// </summary>
/// <param name="name">The name of the property to delete.</param>
void sa_properties_delete(const char* name);

/// <summary>
/// Check if a property exists.
/// An empty property value is defined as 'existing'.
/// </summary>
/// <param name="name">The name of the property to check.</param>
/// <returns>Returns 1 if the property exists. Returns 0 otherwise.</returns>
int sa_properties_exists(const char* name);

/// <summary>
/// Set the value of the given property name.
/// </summary>
/// <param name="name">The name of the property to set.</param>
/// <param name="value">The new value of the property.</param>
void sa_properties_set(const char* name, const char* value);

/// <summary>
/// Get the value of the given property name.
/// </summary>
/// <remarks>
/// The property_length is updated even if the given buffer is too small.
/// </remarks>
/// <param name="name">The name of the property to get.</param>
/// <param name="property_length">The length of the property value in bytes.</param>
/// <param name="buffer">The output buffer for the property value.</param>
/// <param name="buffer_size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns a non-zero on error.</returns>
sa_error_t sa_properties_get_buffer(const char* name, int* property_length, char* buffer, size_t buffer_size);

/// <summary>
/// Get the value of the given property name.
/// </summary>
/// <param name="name">The name of the property to get.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns a non-zero on error.</returns>
sa_error_t sa_properties_get_string(const char* name, sa_string_t* str);

/// <summary>
/// Expand the given string by replacing property variable reference by the actual variable's value.
/// The syntax of a property variable reference is the following: `${variable-name}` where `variable-name` is the name of a variable.
/// </summary>
/// <remarks>
/// The string is expanded until there are no change in the given string.
/// The expanded_length is updated even if the given buffer is too small.
/// </remarks>
/// <param name="value">The given value to expand.</param>
/// <param name="expanded_length">The length of the expanded value in bytes.</param>
/// <param name="buffer">The output buffer for the expanded value.</param>
/// <param name="buffer_size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns a non-zero on error.</returns>
sa_error_t sa_properties_expand_buffer(const char* value, int* expanded_length, char* buffer, size_t buffer_size);

/// <summary>
/// Expand the given string by replacing property variable reference by the actual variable's value.
/// The syntax of a property variable reference is the following: `${variable-name}` where `variable-name` is the name of a variable.
/// </summary>
/// <remarks>
/// The string is expanded until there are no change in the given string.
/// </remarks>
/// <param name="value">The given value to expand.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns a non-zero on error.</returns>
sa_error_t sa_properties_expand_string(const char* value, sa_string_t* str);

/// <summary>
/// Expand the given string by replacing property variable reference by the actual variable's value.
/// The syntax of a property variable reference is the following: `${variable-name}` where `variable-name` is the name of a variable.
/// </summary>
/// <remarks>
/// The string is expanded in a single pass. Some property reference may not be expanded after the first pass.
/// The expanded_length is updated even if the given buffer is too small.
/// </remarks>
/// <param name="value">The given value to expand.</param>
/// <param name="expanded_length">The length of the expanded value in bytes.</param>
/// <param name="buffer">The output buffer for the expanded value.</param>
/// <param name="buffer_size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns a non-zero on error.</returns>
sa_error_t sa_properties_expand_once_buffer(const char* value, int* expanded_length, char* buffer, size_t buffer_size);

/// <summary>
/// Expand the given string by replacing property variable reference by the actual variable's value.
/// The syntax of a property variable reference is the following: `${variable-name}` where `variable-name` is the name of a variable.
/// </summary>
/// <remarks>
/// The string is expanded in a single pass. Some property reference may not be expanded after the first pass.
/// </remarks>
/// <param name="value">The given value to expand.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns a non-zero on error.</returns>
sa_error_t sa_properties_expand_once_string(const char* value, sa_string_t* str);

#ifdef __cplusplus
}
#endif

#endif //SA_API_PROPERTIES_H
