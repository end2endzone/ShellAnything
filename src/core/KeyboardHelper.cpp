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

#include "KeyboardHelper.h"
#include "rapidassist/strings.h"
#include <string>

namespace shellanything
{

  const char* KeyboardHelper::ToString(KEYB_MODIFIER_ID value)
  {
    switch (value)
    {
    case KMID_CTRL:
      return "CTRL";
    case KMID_ALT:
      return "ALT";
    case KMID_SHIFT:
      return "SHIFT";
    };
    return "";
  }

  KEYB_MODIFIER_ID KeyboardHelper::ParseKeyboardModifierId(const std::string& value)
  {
    const std::string value_upper = ra::strings::Uppercase(value);
    if (value_upper == ToString(KMID_CTRL))
      return KMID_CTRL;
    if (value_upper == ToString(KMID_ALT))
      return KMID_ALT;
    if (value_upper == ToString(KMID_SHIFT))
      return KMID_SHIFT;
    return KMID_INVALID;
  }

  const char* KeyboardHelper::ToString(KEYB_TOGGLE_ID value)
  {
    switch (value)
    {
    case KTID_CAPS_LOCK:
      return "CAPS";
    case KTID_SCROLL_LOCK:
      return "SCROLL";
    case KTID_NUM_LOCK:
      return "NUM";
    };
    return "";
  }

  KEYB_TOGGLE_ID KeyboardHelper::ParseKeyboardToggleId(const std::string& value)
  {
    const std::string value_upper = ra::strings::Uppercase(value);
    if (value_upper == ToString(KTID_CAPS_LOCK))
      return KTID_CAPS_LOCK;
    if (value_upper == ToString(KTID_SCROLL_LOCK))
      return KTID_SCROLL_LOCK;
    if (value_upper == ToString(KTID_NUM_LOCK))
      return KTID_NUM_LOCK;
    return KTID_INVALID;
  }

} //namespace shellanything
