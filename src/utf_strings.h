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

#ifndef SA_UTF_STRINGS_H
#define SA_UTF_STRINGS_H

#include <string>
#include <vector>

namespace shellanything
{

  /// <summary>
  /// Convert a wide unicode string to an utf8 string
  /// </summary>
  /// <param name="wstr">The unicode string that must be converted.</param>
  /// <returns>Returns an utf8 encoded string.</returns>
  std::string unicode_to_utf8(const std::wstring & wstr);

  /// <summary>
  /// Convert an utf8 string to a wide unicode string
  /// </summary>
  /// <param name="str">The utf8 string that must be converted.</param>
  /// <returns>Returns an unicode encoded string.</returns>
  std::wstring utf8_to_unicode(const std::string & str);

  /// <summary>
  /// Convert an wide unicode string to ansi string
  /// </summary>
  /// <param name="str">The unicode string that must be converted.</param>
  /// <returns>Returns an ansi encoded string.</returns>
  std::string unicode_to_ansi(const std::wstring & wstr);

  /// <summary>
  /// Convert an ansi string to a wide unicode string
  /// </summary>
  /// <param name="str">The ansi string that must be converted.</param>
  /// <returns>Returns an unicode encoded string.</returns>
  std::wstring ansi_to_unicode(const std::string & str);

  /// <summary>
  /// Convert an utf8 string to an ansi string
  /// </summary>
  /// <param name="str">The utf8 string that must be converted.</param>
  /// <returns>Returns an ansi encoded string.</returns>
  std::string utf8_to_ansi(const std::string & str);

  /// <summary>
  /// Convert an ansi string to an utf8 string
  /// </summary>
  /// <param name="str">The ansi string that must be converted.</param>
  /// <returns>Returns a utf8 encoded string.</returns>
  std::string ansi_to_utf8(const std::string & str);

} //namespace shellanything

#endif //SA_UTF_STRINGS_H


