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

#ifndef SA_KEYBOARD_SERVICE_H
#define SA_KEYBOARD_SERVICE_H

#include "sa_windows_export.h"
#include "IKeyboardService.h"

namespace shellanything
{
  /// <summary>
  /// Win32 implementation class of IWindowsKeyboardService.
  /// </summary>
  class SA_WINDOWS_EXPORT WindowsKeyboardService : public virtual IKeyboardService
  {
  public:
    WindowsKeyboardService();
    virtual ~WindowsKeyboardService();

  private:
    // Disable and copy constructor, dtor and copy operator
    WindowsKeyboardService(const WindowsKeyboardService&);
    WindowsKeyboardService& operator=(const WindowsKeyboardService&);
  public:

    /// <summary>
    /// Reads the state of a keyboard modifier key.
    /// </summary>
    /// <param name="key">The id of a matching keyboard modifier key.</param>
    /// <returns>Returns true if the given key is UP. Returns false otherwise.</returns>
    virtual bool IsModifierKeyUp(KEYB_MODIFIER_ID key) const;

    /// <summary>
    /// Reads the state of a keyboard modifier key.
    /// </summary>
    /// <param name="key">The id of a matching keyboard modifier key.</param>
    /// <returns>Returns true if the given key is DOWN. Returns false otherwise.</returns>
    virtual bool IsModifierKeyDown(KEYB_MODIFIER_ID key) const;

    /// <summary>
    /// Reads the state of a keyboard toggle key.
    /// </summary>
    /// <param name="key">The id of a matching keyboard toggle key.</param>
    /// <returns>Returns true if the given key is ON. Returns false otherwise.</returns>
    virtual bool IsToggleStateOn(KEYB_TOGGLE_ID state) const;

    /// <summary>
    /// Reads the state of a keyboard toggle key.
    /// </summary>
    /// <param name="key">The id of a matching keyboard toggle key.</param>
    /// <returns>Returns true if the given key is OFF. Returns false otherwise.</returns>
    virtual bool IsToggleStateOff(KEYB_TOGGLE_ID state) const;

  private:
    int ToWin32VirtualKey(KEYB_MODIFIER_ID key) const;
    int ToWin32VirtualKey(KEYB_TOGGLE_ID key) const;

  };

} //namespace shellanything

#endif //SA_KEYBOARD_SERVICE_H
