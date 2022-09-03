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
#include "sa_private_casting.h"

void sa_string_init(sa_string_t* str)
{
  memset(str, 0, sizeof(sa_string_t));
}

sa_boolean sa_string_valid(sa_string_t* str)
{
  if (str)
    return str->opaque != NULL;
  return false;
}

void sa_string_create(sa_string_t* str)
{
  str->opaque = new std::string();
}

void sa_string_create_from_cstr(sa_string_t* str, const char* value)
{
  str->opaque = new std::string(value);
}

void sa_string_create_from_str(sa_string_t* str, sa_string_t* value)
{
  str->opaque = new std::string(*AS_CLASS_STRING(str));
}

void sa_string_destroy(sa_string_t* str)
{
  if (sa_string_valid(str))
    delete str->opaque;
  sa_string_init(str);
}

size_t sa_string_get_length(sa_string_t* str)
{
  size_t length = 0;
  if (sa_string_valid(str))
    length = AS_CLASS_STRING(str)->length();
  return length;
}

const char* sa_string_get_value(sa_string_t* str)
{
  const char* value = NULL;
  if (sa_string_valid(str))
    value = AS_CLASS_STRING(str)->c_str();
  return value;
}
