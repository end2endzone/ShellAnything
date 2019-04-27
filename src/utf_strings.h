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

  // Convert a wide Unicode string to an UTF8 string
  std::string unicode_to_utf8(const std::wstring & wstr);

  // Convert an UTF8 string to a wide Unicode String
  std::wstring utf8_to_unicode(const std::string & str);

  // Convert an wide Unicode string to ANSI string
  std::string unicode_to_ansi(const std::wstring & wstr);

  // Convert an ANSI string to a wide Unicode String
  std::wstring ansi_to_unicode(const std::string & str);

  std::string utf8_to_ansi(const std::string & str);

  std::string ansi_to_utf8(const std::string & str);

} //namespace shellanything

#endif //SA_UTF_STRINGS_H


