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

#ifndef SA_UNICODE_H
#define SA_UNICODE_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include <string>
#include <vector>

namespace shellanything
{

  /// <summary>
  /// Calculate the size in bytes of the first utf-8 code point in the given utf-8 encoded string.
  /// </summary>
  /// <param name="str">The utf-8 encoded string that must be calculated.</param>
  /// <returns>Returns the size in bytes of the first utf-8 code point. Returns 0 if the given string is not properly utf-8 encoded.</returns>
  SHELLANYTHING_EXPORT size_t GetCodePointLengthUtf8(const char * str);

  /// <summary>
  /// Calculate the size in bytes of the utf-8 code point at the given offset in the given string.
  /// </summary>
  /// <param name="str">The utf-8 encoded string that must be calculated.</param>
  /// <param name="offset">The offset in bytes in the string where a the code point is located.</param>
  /// <returns>Returns the size in bytes of the utf-8 code point at the given offset in the given string. Returns 0 if the given string is not properly utf-8 encoded.</returns>
  inline size_t GetCodePointLengthUtf8(const char * str, size_t offset) { return GetCodePointLengthUtf8(&str[offset]); }

  /// <summary>
  /// Calculate the number of character (code points) in an utf-8 encoded string.
  /// Note that number of characters is always lower or equals to the number of bytes in a string.
  /// </summary>
  /// <param name="str">The utf-8 encoded string that must be calculated.</param>
  /// <returns>Returns the number of character (code points) in an utf-8 encoded string. Returns 0 if the given string is not properly utf-8 encoded.</returns>
  SHELLANYTHING_EXPORT size_t GetLengthUtf8(const char * str);

  /// <summary>
  /// Returns a substring from a given utf-8 encoded string.
  /// The function is code points aware which means that it cannot cut "in the middle" of a character.
  /// </summary>
  /// <param name="str">The utf-8 encoded input string.</param>
  /// <param name="start">The index in code points where the substring must start. Set to 0 to start at the beginning of the string.</param>
  /// <param name="length">The length in code points of the output substring.</param>
  /// <returns>Returns a substring from a given utf-8 encoded string. Returns an empty string if the given string is not properly utf-8 encoded.</returns>
  SHELLANYTHING_EXPORT std::string SubstringUtf8(const char * str, size_t start, size_t length);

} //namespace shellanything

#endif //SA_UNICODE_H
