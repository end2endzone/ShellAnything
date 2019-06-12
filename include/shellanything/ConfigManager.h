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

#include "shellanything/Node.h"
#include "shellanything/Configuration.h"
#include "shellanything/Context.h"

namespace shellanything
{

  /// <summary>
  /// The ConfigManager holds mutiple Configuration instances.
  /// </summary>
  class ConfigManager
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

    static ConfigManager & getInstance();
    
    /// <summary>
    /// Get the list of Configuration pointers handled by the manager
    /// </summary>
    Configuration::ConfigurationPtrList getConfigurations();

    /// <summary>
    /// Returns true if the given path is a Configuration loaded by the manager.
    /// </summary>
    /// <param name="path">The path of a Configuration file</param>
    /// <returns>Returns true if the given path is a Configuration loaded by the manager. Returns false otherwise.</returns>
    bool isConfigFileLoaded(const std::string & path) const;

    /// <summary>
    /// Clears the configuration manager of all loaded Configuration
    /// </summary>
    void clear();

    /// <summary>
    /// Refresh the content of the configuration manager:
    /// * Reload configuration files that were modified.
    /// * Deleted loaded configurations whose file are missing.
    /// * Discover new unloaded configuration files.
    /// </summary>
    void refresh();

    /// <summary>
    /// Recursively calls Menu::update() on all menus loaded by the configuration manager.
    /// </summary>
    void update(const Context & c);

    /// <summary>
    /// Finds a loaded Menu pointer that is assigned the command id iCommandId.
    /// </summary>
    /// <param name="iCommandId">The search command id value.</param>
    /// <returns>Returns a Menu pointer if a match is found. Returns NULL otherwise.</returns>
    Menu * findMenuByCommandId(const uint32_t & iCommandId);

    /// <summary>
    /// Assign unique command id to all menus loaded by the configuration manager.
    /// </summary>
    /// <param name="iFirstCommandId">The first command id available.</param>
    /// <returns>Returns the next available command id. Returns iFirstCommandId if it failed assining command id.</returns>
    uint32_t assignCommandIds(const uint32_t & iFirstCommandId);

    /// <summary>
    /// Clears all the registered search paths
    /// </summary>
    void clearSearchPath();

    /// <summary>
    /// Add a new search path to the path list.
    /// </summary>
    /// <param name="path">The path to add to the search list.</param>
    void addSearchPath(const std::string & path);

  private:
    //attributes
    PathList mPaths;
    Node mConfigurations;
  };

} //namespace shellanything

#endif //SA_CONFIGMANAGER_H
