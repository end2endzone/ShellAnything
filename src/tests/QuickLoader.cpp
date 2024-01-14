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

#include "QuickLoader.h"

#include "PropertyManager.h"
#include "ConfigManager.h"

#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/testing.h"
#include "rapidassist/timing.h"

#include <iostream>

namespace shellanything
{
  QuickLoader::QuickLoader()
  {
  }

  QuickLoader::~QuickLoader()
  {
    // Cleanup
    DeleteConfigurationFilesInWorkspace();
  }

  void QuickLoader::SetWorkspace(Workspace* workspace)
  {
    mWorkspace = workspace;
  }

  Workspace* QuickLoader::GetWorkspace()
  {
    return mWorkspace;
  }

  bool QuickLoader::DeleteConfigurationFilesInWorkspace()
  {
    if (!mWorkspace)
      return false;
    std::string workspace_dir = mWorkspace->GetBaseDirectory();
    if (workspace_dir.empty())
      return true;

    //Delete the configurations which source files are deleted
    ConfigManager& cmgr = ConfigManager::GetInstance();
    cmgr.Refresh();

    // For each files in workspace directory...
    ra::strings::StringVector files;
    ra::filesystem::FindFilesUtf8(files, workspace_dir.c_str());
    if (!files.empty())
    {
      std::cout << "Deleting configuration files in workspace directory '" << workspace_dir << "'." << std::endl;
    }
    for (size_t i = 0; i < files.size(); i++)
    {
      const std::string& file_path = files[i];

      // Is that a Configuration File?
      if (ConfigFile::IsValidConfigFile(file_path))
      {
        // Yes, delete it
        bool deleted = ra::filesystem::DeleteFileUtf8(file_path.c_str());
        if (!deleted)
        {
          std::cout << "Failed deleting file '" << file_path << "'." << std::endl;
          return false;
        }
      }
    }

    //Now that all configuration files are deleted, refresh again
    cmgr.Refresh();

    //Check that no files from the wprkspace are loaded
    ConfigFile::ConfigFilePtrList configs = cmgr.GetConfigFiles();
    size_t loaded_count = configs.size();
    if (loaded_count > 0)
    {
      for (size_t i = 0; i < loaded_count; i++)
      {
        ConfigFile* config = configs[i];
        const std::string & config_file_path = config->GetFilePath();
        if (config_file_path.find(workspace_dir) != std::string::npos)
        {
          std::cout << "Found a configuration file that is still loaded: '" << config_file_path << "'." << std::endl;
          return false;
        }
      }
    }

    return true;
  }

  bool QuickLoader::ImportAndLoadConfigurationFile(const std::string& path)
  {
    if (!mWorkspace)
      return false;

    std::string workspace_dir = mWorkspace->GetBaseDirectory();

    //Delete previous configurations
    bool deleted = DeleteConfigurationFilesInWorkspace();
    if (!deleted)
    {
      std::cout << "Failed deleting all previous configuration files from directory '" << workspace_dir << "'.";
      return false;
    }

    ConfigManager& cmgr = ConfigManager::GetInstance();

    //Import the required file into the workspace
    bool imported = mWorkspace->ImportAndRenameFileUtf8(path.c_str(), "config.xml");
    if (!imported)
    {
      std::cout << "Failed importing Configuration File '" << path << "' into workspace as 'config.xml'.";
      return false;
    }

    //Wait to make sure that the next file copy/modification will not have the same timestamp
    ra::timing::Millisleep(1500);

    //Setup ConfigManager to read files from workspace
    cmgr.ClearSearchPath();
    cmgr.Clear();
    cmgr.AddSearchPath(workspace_dir);
    cmgr.Refresh();

    //Check the file is loaded
    ConfigFile::ConfigFilePtrList configs = cmgr.GetConfigFiles();
    size_t count = configs.size();

    return (count == 1);
  }

  bool QuickLoader::LoadCurrentTestConfigurationFile()
  {
    if (!mWorkspace)
      return false;

    // Build a file path of a Configuration File that matches the name of the current unit test.
    static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
    std::string config_file_path = std::string("test_files") + path_separator + ra::testing::GetTestQualifiedName() + ".xml";

    if (!ra::filesystem::FileExists(config_file_path.c_str()))
      return false;

    // Import the file into the current workspace and load it.
    return ImportAndLoadConfigurationFile(config_file_path);
  }

} //namespace shellanything
