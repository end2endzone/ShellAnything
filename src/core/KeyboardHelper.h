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

#ifndef SA_KEYBOARD_HELPER_H
#define SA_KEYBOARD_HELPER_H

#include "IKeyboardService.h"

namespace shellanything
{

  /// <summary>
  /// Helper class for the keyboard service
  /// </summary>
  class SHELLANYTHING_EXPORT KeyboardHelper
  {
  public:

    /// <summary>
    /// Converts a KEYB_MODIFIER_ID to a string.
    /// </summary>
    /// <param name="value">The value to convert to string.</param>
    /// <returns>Returns the string representation of the given value. Returns an empty string if unknown.</returns>
    static const char* ToString(KEYB_MODIFIER_ID value);

    /// <summary>
    /// Parses a string as a KEYB_MODIFIER_ID.
    /// </summary>
    /// <param name="value">The string value to convert.</param>
    /// <returns>Returns a valid KEYB_MODIFIER_ID. Returns KMID_INVALID if string is unknown.</returns>
    static KEYB_MODIFIER_ID ParseKeyboardModifierId(const std::string& value);

    /// <summary>
    /// Converts a KEYB_TOGGLE_ID to a string.
    /// </summary>
    /// <param name="value">The value to convert to string.</param>
    /// <returns>Returns the string representation of the given value. Returns an empty string if unknown.</returns>
    static const char* ToString(KEYB_TOGGLE_ID value);

    /// <summary>
    /// Parses a string as a KEYB_TOGGLE_ID.
    /// </summary>
    /// <param name="value">The string value to convert.</param>
    /// <returns>Returns a valid KEYB_TOGGLE_ID. Returns KTID_INVALID if string is unknown.</returns>
    static KEYB_TOGGLE_ID ParseKeyboardToggleId(const std::string& value);

  };

} //namespace shellanything

#endif //SA_KEYBOARD_HELPER_H
