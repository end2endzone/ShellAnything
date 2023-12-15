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

#ifndef SA_TEST_QUICK_LOADER_H
#define SA_TEST_QUICK_LOADER_H

#include <string>
#include "Workspace.h"
#include "Menu.h"
#include "SelectionContext.h"

namespace shellanything
{

  /// <summary>
  /// Create a class for quickly manipulating configuration files and menu.
  /// </summary>
  class QuickLoader
  {
  public:
    QuickLoader();
    virtual ~QuickLoader();

    /// <summary>
    /// Set the current Workspace class instance for handling files.
    /// </summary>
    /// <param name="workspace">A valid Workspace instance.</param>
    void SetWorkspace(Workspace* workspace);

    /// <summary>
    /// Get the current Workspace class instance for handling files.
    /// </summary>
    /// <returns>Returns the current Workspace instance that is set.</returns>
    Workspace* GetWorkspace();

    /// <summary>
    /// Delete all Configuration File that are found in the current workspace.
    /// </summary>
    /// <returns>Returns the current Workspace instance that is set.</returns>
    bool DeleteConfigurationFilesInWorkspace();

    /// <summary>
    /// Load a Configuration File. Unload any previous configurations from the system.
    /// </summary>
    /// <param name="path">The path to the Configuration File.</param>
    /// <returns>Returns true if the configuration file was loaded. Returns false otherwise.</returns>
    bool ImportAndLoadConfigurationFile(const std::string& path);

    /// <summary>
    /// Search within 'test_files' directory for a Configuration File that matches the name of the current unit test and load it.
    /// </summary>
    /// <returns>Returns true if the configuration file was loaded. Returns false otherwise.</returns>
    bool LoadCurrentTestConfigurationFile();

    /// <summary>
    /// Finds a loaded Menu pointer by a given name. The first menu that matches is returned.
    /// </summary>
    /// <param name="name">The name of the menu.</param>
    /// <param name="expand">The should the menu name be expanded before comparing with the given name.</param>
    /// <returns>Returns a Menu pointer if a match is found. Returns NULL otherwise.</returns>
    Menu* FindMenuByName(const std::string& name, bool expands = false);

    /// <summary>
    /// Executes all action of the given menu.
    /// </summary>
    /// <param name="menu">The menu to execute all actions.</param>
    /// <returns>Returns true when all menu action was executed succesfully. Returns false otherwise.</returns>
    bool ExecuteActions(const Menu* menu, const SelectionContext& context);

  private:
    Workspace* mWorkspace;
  };

} //namespace shellanything

#endif //SA_TEST_QUICK_LOADER_H
