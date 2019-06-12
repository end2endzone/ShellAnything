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

#include "shellanything/Node.h"
#include "shellanything/Menu.h"
#include <stdint.h>

namespace shellanything
{

  /// <summary>
  /// A configuration holds mutiple Menu instances.
  /// </summary>
  class Configuration : public Node
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
    static Configuration * loadFile(const std::string & path, std::string & error);

    /// <summary>
    /// Detect if a given file is a valid configuration file.
    /// </summary>
    /// <param name="path">The file path to load</param>
    /// <returns>Returns true if the file is a valid configuration file. Returns false otherwise.</returns>
    static bool isValidConfigFile(const std::string & path);

    /// <summary>
    /// Returns the file path of this configuration.
    /// </summary>
    const std::string & getFilePath() const;

    /// <summary>
    /// Set the file path of this configuration.
    /// </summary>
    void setFilePath(const std::string & iFilePath);

    /// <summary>
    /// Returns the configuration file's modified date.
    /// </summary>
    const uint64_t & getFileModifiedDate() const;

    /// <summary>
    /// Set the configuration file's modified date.
    /// </summary>
    void setFileModifiedDate(const uint64_t & iFileModifiedDate);

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
    /// Get the list of menu pointers handled by the configuration.
    /// </summary>
    Menu::MenuPtrList getMenus();

  private:
    uint64_t mFileModifiedDate;
    std::string mFilePath;
  };

} //namespace shellanything

#endif //SA_CONFIGURATION_H
