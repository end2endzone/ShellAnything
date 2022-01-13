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
#include "Configuration.h"
#include "Context.h"

namespace shellanything
{

  /// <summary>
  /// The ConfigManager holds mutiple Configuration instances.
  /// </summary>
  class SHELLANYTHING_EXPORT ConfigManager
  {
  private:
    ConfigManager();
    virtual ~ConfigManager();

  private:
    // Disable copy constructor and copy operator
    ConfigManager(const ConfigManager&);
    ConfigManager& operator=(const ConfigManager&);
  public:

  public:
    /// <summary>
    /// A list of string for holding configuration paths.
    /// </summary>
    typedef std::vector<std::string> PathList;

    static ConfigManager & GetInstance();
    
    /// <summary>
    /// Get the list of Configuration pointers handled by the manager
    /// </summary>
    Configuration::ConfigurationPtrList GetConfigurations();

    /// <summary>
    /// Returns true if the given path is a Configuration loaded by the manager.
    /// </summary>
    /// <param name="path">The path of a Configuration file</param>
    /// <returns>Returns true if the given path is a Configuration loaded by the manager. Returns false otherwise.</returns>
    bool IsConfigFileLoaded(const std::string & path) const;

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
    /// Recursively calls Menu::update() on all menus loaded by the configuration manager.
    /// </summary>
    void Update(const Context & context);

    /// <summary>
    /// Finds a loaded Menu pointer that is assigned the command id command_id.
    /// </summary>
    /// <param name="command_id">The search command id value.</param>
    /// <returns>Returns a Menu pointer if a match is found. Returns NULL otherwise.</returns>
    Menu * FindMenuByCommandId(const uint32_t & command_id);

    /// <summary>
    /// Assign unique command id to all menus loaded by the configuration manager.
    /// </summary>
    /// <param name="first_command_id">The first command id available.</param>
    /// <returns>Returns the next available command id. Returns first_command_id if it failed assining command id.</returns>
    uint32_t AssignCommandIds(const uint32_t & first_command_id);

    /// <summary>
    /// Clears all the registered search paths
    /// </summary>
    void ClearSearchPath();

    /// <summary>
    /// Add a new search path to the path list.
    /// </summary>
    /// <param name="path">The path to add to the search list.</param>
    void AddSearchPath(const std::string & path);

  private:
    //methods
    void DeleteChildren();
    void DeleteChild(Configuration * config);

    //attributes
    PathList mPaths;
    Configuration::ConfigurationPtrList mConfigurations;
  };

} //namespace shellanything

#endif //SA_CONFIGMANAGER_H
