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

#ifndef SA_CONFIGMANAGER_H
#define SA_CONFIGMANAGER_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include "IObject.h"
#include "StringList.h"
#include "ConfigFile.h"
#include "SelectionContext.h"
#include "Enums.h"

namespace shellanything
{

  /// <summary>
  /// The ConfigManager holds mutiple ConfigFile instances.
  /// </summary>
  class SHELLANYTHING_EXPORT ConfigManager : public virtual IObject
  {
  private:
    ConfigManager();
    virtual ~ConfigManager();

  private:
    // Disable copy constructor and copy operator
    ConfigManager(const ConfigManager&);
    ConfigManager& operator=(const ConfigManager&);

  public:
    static ConfigManager& GetInstance();

    /// <summary>
    /// Get the list of ConfigFile pointers handled by the manager
    /// </summary>
    ConfigFile::ConfigFilePtrList GetConfigFiles();

    /// <summary>
    /// Returns true if the given path is a ConfigFile loaded by the manager.
    /// </summary>
    /// <param name="path">The path of a Configuration file</param>
    /// <returns>Returns true if the given path is a ConfigFile loaded by the manager. Returns false otherwise.</returns>
    bool IsConfigFileLoaded(const std::string& path) const;

    /// <summary>
    /// Clears the configuration manager of all loaded Configuration
    /// </summary>
    void Clear();

    /// <summary>
    /// Refresh the content of the configuration manager:
    /// * Reload configuration files that were modified.
    /// * Deleted loaded configurations whose file are missing.
    /// * Discover new unloaded configuration files.
    /// </summary>
    void Refresh();

    /// <summary>
    /// Recursively update all loaded configurations.
    /// </summary>
    /// <param name="context">The selection context</param>
    void Update(const SelectionContext& context);

    /// <summary>
    /// Finds a loaded Menu pointer that is assigned the command id command_id.
    /// </summary>
    /// <param name="command_id">The search command id value.</param>
    /// <returns>Returns a Menu pointer if a match is found. Returns NULL otherwise.</returns>
    Menu* FindMenuByCommandId(const uint32_t& command_id);

    /// <summary>
    /// Finds a loaded Menu pointer by a given name. The first menu that matches the given name is returned.
    /// </summary>
    /// <param name="name">The name of the menu.</param>
    /// <param name="flags">The flags for searching by name.</param>
    /// <returns>Returns a Menu pointer if a match is found. Returns NULL otherwise.</returns>
    Menu* FindMenuByName(const std::string& name, FIND_BY_NAME_FLAGS flags = FIND_BY_NAME_NONE);

    /// <summary>
    /// Assign unique command id to all menus loaded by the configuration manager.
    /// </summary>
    /// <param name="first_command_id">The first command id available.</param>
    /// <returns>Returns the next available command id. Returns first_command_id if it failed assining command id.</returns>
    uint32_t AssignCommandIds(const uint32_t& first_command_id);

    /// <summary>
    /// Clears all the registered search paths
    /// </summary>
    void ClearSearchPath();

    /// <summary>
    /// Add a new search path to the path list.
    /// </summary>
    /// <param name="path">The path to add to the search list.</param>
    void AddSearchPath(const std::string& path);

    // IObject methods
    virtual std::string ToShortString() const;
    virtual void ToLongString(std::string& str, int indent) const;

  private:
    //methods
    void DeleteChildren();
    void DeleteChild(ConfigFile* config);

    //attributes
    StringList mPaths;
    ConfigFile::ConfigFilePtrList mConfigurations;
  };

} //namespace shellanything

#endif //SA_CONFIGMANAGER_H
