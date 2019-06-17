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

#ifndef SA_DEFAULTS_H
#define SA_DEFAULTS_H

#include "shellanything/Action.h"
#include <string>
#include <vector>
#include <stdint.h>

namespace shellanything
{

  /// <summary>
  /// The Defaults class defines the default values when a configuration .
  /// </summary>
  class Defaults
  {
  public:

    Defaults();
    virtual ~Defaults();

  private:
    // Disable copy constructor and copy operator
    Defaults(const Defaults&);
    Defaults& operator=(const Defaults&);
  public:

    /// <summary>
    /// Add a new Action to the Defaults. The Defaults instance takes ownership of the action.
    /// </summary>
    /// <param name="action">The given action to add to the defaults</param>
    void addAction(Action * action);
    
    /// <summary>
    /// Get the list of action of the menu.
    /// </summary>
    const Action::ActionPtrList & getActions() const;

  private:
    Action::ActionPtrList mActions;
  };

} //namespace shellanything

#endif //SA_DEFAULTS_H
