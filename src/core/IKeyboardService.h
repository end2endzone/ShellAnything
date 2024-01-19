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

#ifndef SA_IKEYBOARD_SERVICE_H
#define SA_IKEYBOARD_SERVICE_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include "Enums.h"

#include <string>

namespace shellanything
{
  /// <summary>
  /// Abstract keyboard service class.
  /// Used to decouple the core from Windows Operating System API.
  /// </summary>
  class SHELLANYTHING_EXPORT IKeyboardService
  {
  public:
    IKeyboardService(); 
    virtual ~IKeyboardService();

  private:
    // Disable and copy constructor, dtor and copy operator
    IKeyboardService(const IKeyboardService&);
    IKeyboardService& operator=(const IKeyboardService&);
  public:

    /// <summary>
    /// Reads the state of a keyboard modifier key.
    /// </summary>
    /// <param name="key">The id of a matching keyboard modifier key.</param>
    /// <returns>Returns true if the given key is UP. Returns false otherwise.</returns>
    virtual bool IsModifierKeyUp(KEYB_MODIFIER_ID key) const = 0;

    /// <summary>
    /// Reads the state of a keyboard modifier key.
    /// </summary>
    /// <param name="key">The id of a matching keyboard modifier key.</param>
    /// <returns>Returns true if the given key is DOWN. Returns false otherwise.</returns>
    virtual bool IsModifierKeyDown(KEYB_MODIFIER_ID key) const = 0;

    /// <summary>
    /// Reads the state of a keyboard toggle key.
    /// </summary>
    /// <param name="key">The id of a matching keyboard toggle key.</param>
    /// <returns>Returns true if the given key is ON. Returns false otherwise.</returns>
    virtual bool IsToggleStateOn(KEYB_TOGGLE_ID state) const = 0;

    /// <summary>
    /// Reads the state of a keyboard toggle key.
    /// </summary>
    /// <param name="key">The id of a matching keyboard toggle key.</param>
    /// <returns>Returns true if the given key is OFF. Returns false otherwise.</returns>
    virtual bool IsToggleStateOff(KEYB_TOGGLE_ID state) const = 0;

  };

} //namespace shellanything

#endif //SA_IKEYBOARD_SERVICE_H
