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

#include "WindowsKeyboardService.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

namespace shellanything
{
  static const SHORT KEY_STATUS_DOWN = (SHORT)0x8000;
  static const SHORT KEY_STATUS_TOGGLED = 0x0001;

  WindowsKeyboardService::WindowsKeyboardService()
  {
  }

  WindowsKeyboardService::~WindowsKeyboardService()
  {
  }

  bool WindowsKeyboardService::IsModifierKeyUp(KEYB_MODIFIER_ID key) const
  {
    bool pressed = IsModifierKeyDown(key);
    return !pressed;
  }

  bool WindowsKeyboardService::IsModifierKeyDown(KEYB_MODIFIER_ID key) const
  {
    int vk_key = ToWin32VirtualKey(key);
    bool pressed = (GetKeyState(vk_key) & KEY_STATUS_DOWN);
    return pressed;
  }

  bool WindowsKeyboardService::IsToggleStateOn(KEYB_TOGGLE_ID state) const
  {
    int vk_key = ToWin32VirtualKey(state);
    bool pressed = (GetKeyState(vk_key) & KEY_STATUS_TOGGLED);
    return pressed;
  }

  bool WindowsKeyboardService::IsToggleStateOff(KEYB_TOGGLE_ID state) const
  {
    bool state_on = IsToggleStateOn(state);
    return !state_on;
  }

  int WindowsKeyboardService::ToWin32VirtualKey(KEYB_MODIFIER_ID key) const
  {
    switch (key)
    {
    case KMID_CTRL:
      return VK_CONTROL;
    case KMID_ALT:
      return VK_MENU;
    case KMID_SHIFT:
      return VK_SHIFT;
    }
    return 0;
  }
  int WindowsKeyboardService::ToWin32VirtualKey(KEYB_TOGGLE_ID key) const
  {
    switch (key)
    {
    case KTID_CAPS_LOCK:
      return VK_CAPITAL;
    case KTID_SCROLL_LOCK:
      return VK_SCROLL;
    case KTID_NUM_LOCK:
      return VK_NUMLOCK;
    };
    return 0;
  }

} //namespace shellanything
