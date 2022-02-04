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

#include "shellanything/sa_string.h"
#include "sa_string_private.h"

void sa_cstr_copy_string(sa_string_t* dest_str, const std::string& src_str)
{
  sa_string_free(dest_str);
  sa_string_alloc(dest_str, src_str.size());
  if (sa_string_valid(dest_str)) {
    memcpy(dest_str->buffer, src_str.c_str(), src_str.size()+1); // +1 to copy terminating \0 character
    dest_str->length = src_str.size();
  }
}

sa_error_t sa_cstr_copy_buffer(char* dest_buffer, size_t dest_size, int* length, const std::string& src_str)
{
  if (dest_buffer == NULL || dest_size == NULL)
    return SA_ERROR_UNKNOWN;
  if (length != NULL)
    *length = (int)src_str.length();
  const size_t minimum_buffer_size = (src_str.size() + 1);
  if (dest_size < minimum_buffer_size)
    return SA_ERROR_BUFFER_TOO_SMALL;
  strcpy(dest_buffer, src_str.c_str());
  return SA_ERROR_SUCCESS;
}

void sa_cstr_copy_truncate_buffer(char* dest_buffer, size_t dest_size, const char* src_buffer)
{
  if (dest_buffer == NULL || dest_size == 0)
    return;

  // reset first and last by of the buffer
  dest_buffer[0] = '\0';
  dest_buffer[dest_size - 1] = '\0';

  char* dest = dest_buffer;
  const char* src = src_buffer;
  size_t length = dest_size - 1; // -1 to prevent overwriting the last character of the destination buffer
  while (src[0] != '\0' && length > 0)
  {
    dest[0] = src[0];
    dest++;
    src++;
    length--;
  }

  // end the string
  dest[0] = '\0';
}

void sa_cstr_copy_truncate_string(char* dest_buffer, size_t dest_size, sa_string_t* str)
{
  if (str)
    sa_cstr_copy_truncate_buffer(dest_buffer, dest_size, str->buffer);
}
