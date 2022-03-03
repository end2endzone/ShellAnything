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

#ifndef SA_UPDATE_CONTEXT_H
#define SA_UPDATE_CONTEXT_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include <string>
#include <vector>

namespace shellanything
{
  class SelectionContext;
  class Configuration;
  class Menu;

  /// <summary>
  /// A singleton class that holds a reference to the actual objects that are beeing updated
  /// </summary>
  class SHELLANYTHING_EXPORT UpdateContext
  {
  public:
    UpdateContext();
    virtual ~UpdateContext();

  private:
    // Disable copy constructor and copy operator
    UpdateContext(const UpdateContext&);
    UpdateContext& operator=(const UpdateContext&);
  public:

    /// <summary>
    /// Clear all active objects that are currently set in the update context
    /// </summary>
    void Clear();

    /// <summary>
    /// Get the selection context
    /// </summary>
    /// <returns>Get the selection context. Returns NULL if no selection is set.</returns>
    const SelectionContext* GetSelection() const;

    /// <summary>
    /// Set the selection context
    /// </summary>
    /// <param name="selection">The selection</param>
    void SetSelection(const SelectionContext* selection);

    /// <summary>
    /// Get the current updated configuration
    /// </summary>
    /// <returns>Get the current updated configuration. Returns NULL if no configuration is updated.</returns>
    const Configuration* GetConfiguration() const;

    /// <summary>
    /// Set the current updated configuration
    /// </summary>
    /// <param name="configuration">The updating configuration</param>
    void SetConfiguration(const Configuration* configuration);

    /// <summary>
    /// Get the current updated menu
    /// </summary>
    /// <returns>Get the current updated menu. Returns NULL if no menu is updated.</returns>
    const Menu* GetMenu() const;

    /// <summary>
    /// Set the current updated menu
    /// </summary>
    /// <param name="menu">The updating menu</param>
    void SetMenu(const Menu* menu);

  private:
    const SelectionContext* mSelection;
    const Configuration* mConfiguration;
    const Menu* mMenu;
  };

} //namespace shellanything

#endif //SA_UPDATE_CONTEXT_H
