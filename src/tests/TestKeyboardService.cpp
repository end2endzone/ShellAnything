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

#include "TestKeyboardService.h"

namespace shellanything
{
  TestKeyboardService::TestKeyboardService()
  {
  }

  TestKeyboardService::~TestKeyboardService()
  {
  }

  void TestKeyboardService::Clear()
  {
    KeyboardModifierMap empty_modifiers;
    modifiers = empty_modifiers;

    KeyboardToggleMap empty_toggles;
    toggles = empty_toggles;
  }

  void TestKeyboardService::SetModifierKeyDown(KEYB_MODIFIER_ID key, bool value)
  {
    modifiers[key] = value;
  }

  void TestKeyboardService::SetToggleStateOn(KEYB_TOGGLE_ID state, bool value)
  {
    toggles[state] = value;
  }

  bool TestKeyboardService::IsModifierKeyUp(KEYB_MODIFIER_ID key) const
  {
    bool pressed = IsModifierKeyDown(key);
    return !pressed;
  }

  bool TestKeyboardService::IsModifierKeyDown(KEYB_MODIFIER_ID key) const
  {
    KeyboardModifierMap::const_iterator it = modifiers.find(key);
    bool found = (it != modifiers.end());
    if (found)
    {
      const bool is_down = it->second;
      return is_down;
    }
    return false;
  }

  bool TestKeyboardService::IsToggleStateOn(KEYB_TOGGLE_ID state) const
  {
    KeyboardToggleMap::const_iterator it = toggles.find(state);
    bool found = (it != toggles.end());
    if (found)
    {
      const bool is_on = it->second;
      return is_on;
    }
    return false;
  }

  bool TestKeyboardService::IsToggleStateOff(KEYB_TOGGLE_ID state) const
  {
    bool state_on = IsToggleStateOn(state);
    return !state_on;
  }

} //namespace shellanything
