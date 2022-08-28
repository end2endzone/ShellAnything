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

#ifndef SA_API_STRING_H
#define SA_API_STRING_H

#include "shellanything/sa_types.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#if 0
}  // do not indent code inside extern C
#endif
#endif

/// <summary>
/// Initialize a string with NULL values
/// </summary>
/// <param name="str">A pointer to a string</param>
void sa_string_init(sa_string_t* str);

/// <summary>
/// Validate a sa_string_t structure
/// </summary>
/// <param name="str">A pointer to a string</param>
sa_boolean sa_string_valid(sa_string_t* str);

/// <summary>
/// Create a new string.
/// </summary>
/// <param name="str">A pointer to a string</param>
void sa_string_create(sa_string_t* str);

/// <summary>
/// Create a new string initialized with the given cstr value
/// </summary>
/// <param name="str">A pointer to a string</param>
/// <param name="value">The initial value of the string</param>
void sa_string_create_from_cstr(sa_string_t* str, const char* value);

/// <summary>
/// Create a new string initialized with the given string value
/// </summary>
/// <param name="str">A pointer to a string</param>
/// <param name="value">The initial value of the string</param>
void sa_string_create_from_str(sa_string_t* str, sa_string_t* value);

/// <summary>
/// Free an existing string.
/// </summary>
/// <param name="str">A pointer to a string</param>
void sa_string_destroy(sa_string_t* str);

/// <summary>
/// Get the length of a string.
/// </summary>
/// <param name="str">A pointer to a string</param>
/// <returns>Returns the length of string in bytes.</returns>
size_t sa_string_get_length(sa_string_t* str);

/// <summary>
/// Get the value of a string as a null-terminated string.
/// </summary>
/// <param name="str">A pointer to a string</param>
/// <returns>Returns a immutable buffer matching the value of a string.</returns>
const char* sa_string_get_value(sa_string_t* str);

#ifdef __cplusplus
#if 0
{  // do not indent code inside extern C
#endif
}  // extern "C"
#endif

#endif //SA_API_CONTEXT_H
