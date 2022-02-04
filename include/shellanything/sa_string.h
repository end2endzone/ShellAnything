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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sa_string_t  {
  char* buffer;
  size_t size;    // size of the allocated buffer.
  size_t length;  // length of the string in bytes.
} sa_string_t;

/// <summary>
/// Initialize a string with NULL values
/// </summary>
/// <param name="str">A pointer to a string</param>
void sa_string_init(sa_string_t* str);

/// <summary>
/// Allocate a new string based on the given size
/// </summary>
/// <param name="str">A pointer to a string</param>
/// <param name="size">The size of the string buffer in bytes.</param>
void sa_string_alloc(sa_string_t* str, size_t size);

/// <summary>
/// Free an existing string.
/// </summary>
/// <param name="str">A pointer to a string</param>
void sa_string_free(sa_string_t* str);

/// <summary>
/// Check if a string is valid
/// </summary>
/// <param name="str">A pointer to a string</param>
/// <returns>Returns 1 when the given string is valid. Returns 0 otherwise.</returns>
int sa_string_valid(sa_string_t* str);

#ifdef __cplusplus
}
#endif

#endif //SA_API_CONTEXT_H
