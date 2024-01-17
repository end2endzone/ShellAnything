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

#ifndef SA_ENUMS_H
#define SA_ENUMS_H

#include "shellanything/export.h"
#include "shellanything/config.h"

namespace shellanything
{

  enum FIND_BY_NAME_FLAGS
  {
    FIND_BY_NAME_NONE = 0,
    FIND_BY_NAME_EXPANDS = 1,
    FIND_BY_NAME_CASE_INSENSITIVE = 2,
    FIND_BY_NAME_ALL = (-1),
  };

  enum KEYB_MODIFIER_ID
  {
    KMID_INVALID = 0,
    KMID_CTRL,
    KMID_ALT,
    KMID_SHIFT,
  };

  enum KEYB_TOGGLE_ID
  {
    KTID_INVALID = 0,
    KTID_CAPS_LOCK,
    KTID_SCROLL_LOCK,
    KTID_NUM_LOCK,
  };

} //namespace shellanything

#endif //SA_ENUMS_H
