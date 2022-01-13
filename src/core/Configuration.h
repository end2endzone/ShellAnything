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

#ifndef SA_CONFIGURATION_H
#define SA_CONFIGURATION_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include "Menu.h"
#include "DefaultSettings.h"
#include <stdint.h>

namespace shellanything
{

  /// <summary>
  /// A configuration holds mutiple Menu instances.
  /// </summary>
  class SHELLANYTHING_EXPORT Configuration
  {
  public:
    /// <summary>
    /// A list of Configuration pointer.
    /// </summary>
    typedef std::vector<Configuration*> ConfigurationPtrList;

    Configuration();
    virtual ~Configuration();

  private:
    // Disable copy constructor and copy operator
    Configuration(const Configuration&);
    Configuration& operator=(const Configuration&);
  public:

    /// <summary>
    /// Load a configuration file.
    /// </summary>
    /// <param name="path">The file path to load</param>
    /// <param name="error">The error desription if the file cannot be loaded.</param>
    /// <returns>Returns a valid Configuration pointer if the file can be loaded. Returns NULL otherwise.</returns>
    static Configuration * LoadFile(const std::string & path, std::string & error);

    /// <summary>
    /// Detect if a given file is a valid configuration file.
    /// </summary>
    /// <param name="path">The file path to load</param>
    /// <returns>Returns true if the file is a valid configuration file. Returns false otherwise.</returns>
    static bool IsValidConfigFile(const std::string & path);

    /// <summary>
    /// Returns the file path of this configuration.
    /// </summary>
    const std::string & GetFilePath() const;

    /// <summary>
    /// Set the file path of this configuration.
    /// </summary>
    void SetFilePath(const std::string & file_path);

    /// <summary>
    /// Returns the configuration file's modified date.
    /// </summary>
    const uint64_t & GetFileModifiedDate() const;

    /// <summary>
    /// Set the configuration file's modified date.
    /// </summary>
    void SetFileModifiedDate(const uint64_t & file_modified_date);

    /// <summary>
    /// Recursively calls Menu::update() on all menus loaded by the configuration manager.
    /// </summary>
    void Update(const Context & context);

    /// <summary>
    /// Apply the configuration's default properties.
    /// </summary>
    void ApplyDefaultSettings();

    /// <summary>
    /// Finds a loaded Menu that have the given command_id assigned.
    /// </summary>
    /// <param name="command_id">The search command id value.</param>
    /// <returns>Returns a Menu pointer if a match is found. Returns NULL otherwise.</returns>
    Menu * FindMenuByCommandId(const uint32_t & command_id);

    /// <summary>
    /// Assign unique command ids to all menus loaded by the configuration manager.
    /// </summary>
    /// <param name="first_command_id">The first command id available.</param>
    /// <returns>Returns the next available command id. Returns first_command_id if it failed assigning a command id.</returns>
    uint32_t AssignCommandIds(const uint32_t & first_command_id);

    /// <summary>
    /// Get the list of menu pointers handled by the configuration.
    /// </summary>
    Menu::MenuPtrList GetMenus();

    /// <summary>
    /// Set a new DefaultSettings instance to the Configuration. The Configuration instance takes ownership of the instance.
    /// </summary>
    /// <param name="defaults">The given DefaultSettings to add to the configuration</param>
    void SetDefaultSettings(DefaultSettings * defaults);

    /// <summary>
    /// Get the DefaultSettings instance of the Configuration.
    /// </summary>
    /// <returns>Returns the DefaultSettings instance of the Configuration. Returns NULL if no DefaultSettings is set.</returns>
    const DefaultSettings * GetDefaultSettings() const;

    /// <summary>
    /// Add a Menu to this Configuration. The Configuration takes ownership of the Menu.
    /// </summary>
    /// <param name="menu">The Menu to add.</param>
    void AddMenu(Menu * menu);

  private:
    //methods
    void DeleteChildren();
    void DeleteChild(Menu* menu);

    DefaultSettings * mDefaults;
    uint64_t mFileModifiedDate;
    std::string mFilePath;
    Menu::MenuPtrList mMenus;
  };

} //namespace shellanything

#endif //SA_CONFIGURATION_H
