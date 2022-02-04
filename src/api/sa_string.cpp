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
#include "shellanything/sa_memory.h"
#include <string>

void sa_cstr_copy_string(sa_string_t* str, std::string& from)
{
  sa_string_free(str);
  sa_string_alloc(str, from.size());
  if (sa_string_valid(str)) {
    memcpy(str->buffer, from.c_str(), from.size()+1); // +1 to copy terminating \0 character
    str->length = from.size();
  }
}

void sa_string_init(sa_string_t* str)
{
  if (sa_string_valid(str))
    memset(str, 0, sizeof(sa_string_t));
}

void sa_string_alloc(sa_string_t* str, size_t size)
{
  // free old value
  if (sa_string_valid(str)) {
    sa_string_free(str);
  }

  // zeroize the string
  sa_string_init(str);

  // allocate a bew buffer for the string
  str->buffer = (char*)sa_memory_alloc(size);
  if (str->buffer) {
    str->size = size;
    str->length = 0;
  }
}

void sa_string_free(sa_string_t* str)
{
  if (sa_string_valid(str)) {
    sa_memory_free(str->buffer);
  }
  sa_string_init(str);
}

int sa_string_valid(sa_string_t* str)
{
  if (str == NULL || str->buffer == NULL || str->size == 0)
    return 0;
  return 1;
}
