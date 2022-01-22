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

#ifndef SA_API_STRING_PRIVATE_H
#define SA_API_STRING_PRIVATE_H

#include "shellanything/sa_error.h"

#include <string>

/// <summary>
/// Copy a std::string to a sa_string_t.
/// </summary>
/// <param name="dest_str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <param name="src_str">The source string to copy from</param>
void sa_cstr_copy_string(sa_string_t* dest_str, const std::string& src_str);

/// <summary>
/// Copy a std::string to a buffer.
/// </summary>
/// <remarks>
/// The length is updated even if the given buffer is too small.
/// </remarks>
/// <param name="dest_buffer">The destination buffer.</param>
/// <param name="dest_size">The destination buffer size in bytes.</param>
/// <param name="length">The length of a source string</param>
/// <returns>Returns 0 on success. Returns a non-zero on error.</returns>
sa_error_t sa_cstr_copy_buffer(char* dest_buffer, size_t dest_size, int* length, const std::string& src_str);

/// <summary>
/// Copy an input string to a local buffer. The input string is truncated if the destination buffer is too small.
/// </summary>
/// <param name="dest_buffer"></param>
/// <param name="dest_size"></param>
/// <param name="src_buffer"></param>
void sa_cstr_copy_truncate_buffer(char* dest_buffer, size_t dest_size, const char* src_buffer);

/// <summary>
/// Copy an input string to a local buffer. The input string is truncated if the destination buffer is too small.
/// </summary>
/// <param name="dest_buffer"></param>
/// <param name="dest_size"></param>
/// <param name="src_buffer"></param>
void sa_cstr_copy_truncate_string(char* dest_buffer, size_t dest_size, sa_string_t* str);

#endif //SA_API_STRING_PRIVATE_H
