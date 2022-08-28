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

#ifndef SA_API_PROPERTY_STORE_H
#define SA_API_PROPERTY_STORE_H

#include "shellanything/sa_types.h"
#include "shellanything/sa_string.h"
#include "shellanything/sa_error.h"

#ifdef __cplusplus
extern "C" {
#if 0
}  // do not indent code inside extern C
#endif
#endif

/// <summary>
/// Converts a mutable property store to an immutable property store.
/// </summary>
/// <param name="store">The mutable object to convert.</param>
/// <returns>Returns an immutable store</returns>
sa_property_store_immutable_t sa_property_store_to_immutable(sa_property_store_t* store);

/// <summary>
/// Clears all the registered properties.
/// </summary>
/// <param name="store">The property store structure object.</param>
void sa_property_store_clear(sa_property_store_t* store);

/// <summary>
/// Delete the given property.
/// </summary>
/// <param name="store">The property store structure object.</param>
/// <param name="name">The name of the property to delete.</param>
void sa_property_store_clear_property(sa_property_store_t* store, const char* name);

/// <summary>
/// Check if a property have been set.
/// An empty property value is defined as 'set'.
/// </summary>
/// <param name="store">The property store structure object.</param>
/// <param name="name">The name of the property to check.</param>
/// <returns>Returns 1 if the property is set. Returns 0 otherwise.</returns>
sa_boolean sa_property_store_has_property(sa_property_store_immutable_t* store, const char* name);

/// <summary>
/// Check if the properties are all set.
/// An empty property value is defined as 'set'.
/// </summary>
/// <param name="store">The property store structure object.</param>
/// <param name="properties">The array of properties to check.</param>
/// <param name="count">How many elements in the properties array.</param>
/// <returns>Returns 1 if all the properties are set. Returns 0 otherwise.</returns>
sa_boolean sa_property_store_has_properties(sa_property_store_immutable_t* store, const char* properties[], size_t count);

/// <summary>
/// Sets the value of the given property name.
/// </summary>
/// <param name="store">The property store structure object.</param>
/// <param name="name">The name of the property to set.</param>
/// <param name="value">The new value of the property.</param>
void sa_property_store_set_property(sa_property_store_t* store, const char* name, const char* value);

/// <summary>
/// Get the value of the given property name.
/// </summary>
/// <remarks>
/// The property_length is updated even if the given buffer is too small.
/// </remarks>
/// <param name="store">The property store structure object.</param>
/// <param name="name">The name of the property to get.</param>
/// <param name="length">The length of the property value in bytes.</param>
/// <param name="buffer">The output buffer for the property value.</param>
/// <param name="size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns a non-zero on error.</returns>
sa_error_t sa_property_store_get_property_buffer(sa_property_store_immutable_t* store, const char* name, int* length, char* buffer, size_t size);

/// <summary>
/// Get the value of the given property name.
/// </summary>
/// <param name="store">The property store structure object.</param>
/// <param name="name">The name of the property to get.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns a non-zero on error.</returns>
sa_error_t sa_property_store_get_property_string(sa_property_store_immutable_t* store, const char* name, sa_string_t* str);

/// <summary>
/// Get the value of the given property name.
/// </summary>
/// <param name="store">The property store structure object.</param>
/// <param name="name">The name of the property to get.</param>
/// <returns>Returns the value of the given attribute. Returns NULL if the property cannot be found.</returns>
const char* sa_property_store_get_property_cstr(sa_property_store_immutable_t* store, const char* name);

/// <summary>
/// Get the value of the given property name.
/// </summary>
/// <param name="store">The property store structure object.</param>
/// <param name="name">The name of the property to get.</param>
/// <returns>Returns a valid string value. Must be free with sa_memory_free() to prevent memory leaks. Returns NULL if no value is defined.</returns>
const char* sa_property_store_get_property_alloc(sa_property_store_immutable_t* store, const char* name);

/// <summary>
/// Counts how many properties are registered in the store.
/// </summary>
/// <param name="store">The property store structure object.</param>
/// <returns>Returns how many properties are registered in the store.</returns>
size_t sa_property_store_get_property_count(sa_property_store_immutable_t* store);

/// <summary>
/// Returns whether the list container is empty
/// </summary>
/// <param name="store">The property store structure object.</param>
/// <returns>Returns 1 if the store is empty. Returns 0 otherwise.</returns>
sa_boolean sa_property_store_is_empty(sa_property_store_immutable_t* store);

/// <summary>
/// Get the list of properties in the store
/// </summary>
/// <param name="store">The property store structure object.</param>
/// <param name="names">The output list of properties</param>
/// <param name="count">How many elements are in the names array.</param>
/// <param name="max_name_length">The maximum length of each name in the names array.</param>
/// <returns>Returns 0 on success. Returns a non-zero on error.</returns>
sa_error_t sa_property_store_get_properties(sa_property_store_immutable_t* store, char* names[], size_t count, size_t max_name_length);

/// <summary>
/// Find the list of properties which are not in the store.
/// </summary>
/// <param name="store">The property store structure object.</param>
/// <param name="input_names">The list of property names to test.</param>
/// <param name="count_input_names">How many elements are in the input_names array.</param>
/// <param name="output_names">The output list of property names which are not in the store.</param>
/// <param name="count_output_names">How many elements are in the output_names array.</param>
/// <param name="output_names_max_length">The maximum length of each name in the output_names array.</param>
/// <returns>Returns 0 on success. Returns a non-zero on error.</returns>
sa_error_t sa_property_store_find_missing_properties(sa_property_store_immutable_t* store, const char* input_names[], size_t count_input_names, char* output_names[], size_t count_output_names, size_t output_names_max_length);

#ifdef __cplusplus
#if 0
{  // do not indent code inside extern C
#endif
}  // extern "C"
#endif

#endif //SA_API_PROPERTY_STORE_H
