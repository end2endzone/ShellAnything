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

#ifndef SA_BASE_ACTION_H
#define SA_BASE_ACTION_H

#include "IAction.h"

namespace shellanything
{

  /// <summary>
  /// Base Action class.
  /// </summary>
  class SHELLANYTHING_EXPORT BaseAction : public virtual IAction
  {
  public:
    BaseAction();
    virtual ~BaseAction();

    /// <summary>
    /// Get the parent menu.
    /// </summary>
    /// <returns>Returns a pointer to the parent menu. Returns NULL if the object has no parent menu.</returns>
    virtual Menu* GetParentMenu();
    virtual const Menu* GetParentMenu() const;

    /// <summary>
    /// Set the parent menu.
    /// </summary>
    /// <param name="menu">The parent of this menu</param>
    virtual void SetParentMenu(Menu* menu);

  private:
    Menu* mParentMenu;
  };

} //namespace shellanything

#endif //SA_BASE_ACTION_H
