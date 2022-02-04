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

#ifndef SA_PLATFORM_H
#define SA_PLATFORM_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include "rapidassist/strings.h"

namespace shellanything
{

  /// <summary>
  /// Show an error message encoded in ansi to the user.
  /// </summary>
  /// <param name="title">The caption title of the window.</param>
  /// <param name="message">The message to display to the user.</param>
  SHELLANYTHING_EXPORT void ShowErrorMessage(const std::string & title, const std::string & message);

  /// <summary>
  /// Show an error message encoded in utf8 to the user.
  /// </summary>
  /// <param name="title">The caption title of the window.</param>
  /// <param name="message">The message to display to the user.</param>
  SHELLANYTHING_EXPORT void ShowErrorMessageUtf8(const std::string & title, const std::string & message);

} //namespace shellanything

#endif //SA_ACTION_H
