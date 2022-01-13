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

#ifndef SA_DEFAULTSETTINGS_H
#define SA_DEFAULTSETTINGS_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include "Action.h"

namespace shellanything
{

  /// <summary>
  /// The DefaultSettings class defines the default values when a configuration .
  /// </summary>
  class SHELLANYTHING_EXPORT DefaultSettings
  {
  public:

    DefaultSettings();
    virtual ~DefaultSettings();

  private:
    // Disable copy constructor and copy operator
    DefaultSettings(const DefaultSettings&);
    DefaultSettings& operator=(const DefaultSettings&);
  public:

    /// <summary>
    /// Add a new Action to the DefaultSettings. The DefaultSettings instance takes ownership of the action.
    /// </summary>
    /// <param name="action">The given action to add to the DefaultSettings instance</param>
    void AddAction(Action * action);
    
    /// <summary>
    /// Get the list of action of the menu.
    /// </summary>
    const Action::ActionPtrList & GetActions() const;

  private:
    Action::ActionPtrList mActions;
  };

} //namespace shellanything

#endif //SA_DEFAULTSETTINGS_H
