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

#include "ConfigManager.h"
#include "Menu.h"
#include "LoggerHelper.h"
#include "SaUtils.h"

#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/strings.h"
#include "rapidassist/environment.h"

namespace shellanything
{

  ConfigManager::ConfigManager()
  {
  }

  ConfigManager::~ConfigManager()
  {
    DeleteChildren();
  }

  ConfigManager& ConfigManager::GetInstance()
  {
    static ConfigManager _instance;
    return _instance;
  }

  void ConfigManager::Clear()
  {
    SA_LOG(INFO) << __FUNCTION__ << "()";
    ClearSearchPath(); //remove all search path to make sure that a refresh won’t find any other configuration file
    DeleteChildren();
    Refresh(); //forces all loaded configurations to be unloaded
  }

  void ConfigManager::Refresh()
  {
    SA_DECLARE_SCOPE_LOGGER_ARGS(sli);
    ScopeLogger logger(&sli);

    //validate existing configurations
    ConfigFile::ConfigFilePtrList existing = GetConfigFiles();
    for (size_t i = 0; i < existing.size(); i++)
    {
      ConfigFile* config = existing[i];

      //compare the file's date at the load time and the current date
      const std::string& file_path = config->GetFilePath();
      const uint64_t& old_file_date = config->GetFileModifiedDate();
      const uint64_t new_file_date = ra::filesystem::GetFileModifiedDateUtf8(file_path);
      if (ra::filesystem::FileExistsUtf8(file_path.c_str()) && old_file_date == new_file_date)
      {
        //current configuration is up to date
        SA_LOG(INFO) << "Configuration file '" << file_path << "' is up to date.";
      }
      else
      {
        //file is missing or current configuration is out of date
        //forget about existing config
        SA_LOG(INFO) << "Configuration file '" << file_path << "' is missing or is not up to date. Deleting configuration.";
        DeleteChild(config);
      }
    }

    //search every known path
    for (size_t i = 0; i < mPaths.size(); i++)
    {
      const std::string& path = mPaths[i];

      SA_LOG(INFO) << "Searching configuration files in directory '" << path << "'";

      //search files in each directory
      ra::strings::StringVector files;
      bool dir_found = ra::filesystem::FindFilesUtf8(files, path.c_str());
      if (dir_found)
      {
        //search through each files for *.xml files
        for (size_t j = 0; j < files.size(); j++)
        {
          const std::string& file_path = files[j];
          if (ConfigFile::IsValidConfigFile(file_path))
          {
            //is this file already loaded ?
            if (!IsConfigFileLoaded(file_path))
            {
              SA_LOG(INFO) << "Found new configuration file '" << file_path << "'";

              //parse the file
              std::string error;
              ConfigFile* config = ConfigFile::LoadFile(file_path, error);
              if (config == NULL)
              {
                //log an error message
                SA_LOG(ERROR) << "Failed loading configuration file '" << file_path << "'. Error=" << error << ".";
              }
              else
              {
                //add to current list of configurations
                mConfigurations.push_back(config);

                //apply default properties of the configuration
                config->ApplyDefaultSettings();
              }
            }
            else
            {
              SA_LOG(INFO) << "Skipped configuration file '" << file_path << "'. File is already loaded.";
            }
          }
        }
      }
      else
      {
        //log an error message
        SA_LOG(ERROR) << "Failed searching for configuration files in directory '" << path << "'.";
      }
    }
  }

  void ConfigManager::Update(const SelectionContext& context)
  {
    SA_DECLARE_SCOPE_LOGGER_ARGS(sli);
    sli.verbose = true;
    ScopeLogger logger(&sli);

    //for each child
    ConfigFile::ConfigFilePtrList configurations = ConfigManager::GetConfigFiles();
    for (size_t i = 0; i < configurations.size(); i++)
    {
      ConfigFile* config = configurations[i];
      config->Update(context);
    }
  }

  Menu* ConfigManager::FindMenuByCommandId(const uint32_t& command_id)
  {
    //for each child
    ConfigFile::ConfigFilePtrList configurations = ConfigManager::GetConfigFiles();
    for (size_t i = 0; i < configurations.size(); i++)
    {
      ConfigFile* config = configurations[i];
      Menu* match = config->FindMenuByCommandId(command_id);
      if (match)
        return match;
    }

    return NULL;
  }

  Menu* ConfigManager::FindMenuByName(const std::string& name, FIND_BY_NAME_FLAGS flags)
  {
    //for each child
    ConfigFile::ConfigFilePtrList configurations = ConfigManager::GetConfigFiles();
    for (size_t i = 0; i < configurations.size(); i++)
    {
      ConfigFile* config = configurations[i];
      Menu* match = config->FindMenuByName(name, flags);
      if (match)
        return match;
    }

    return NULL;
  }


  uint32_t ConfigManager::AssignCommandIds(const uint32_t& first_command_id)
  {
    uint32_t nextCommandId = first_command_id;

    //for each child
    ConfigFile::ConfigFilePtrList configurations = ConfigManager::GetConfigFiles();
    for (size_t i = 0; i < configurations.size(); i++)
    {
      ConfigFile* config = configurations[i];
      nextCommandId = config->AssignCommandIds(nextCommandId);
    }

    return nextCommandId;
  }

  ConfigFile::ConfigFilePtrList ConfigManager::GetConfigFiles()
  {
    return mConfigurations;
  }

  void ConfigManager::ClearSearchPath()
  {
    mPaths.clear();
  }

  void ConfigManager::AddSearchPath(const std::string& path)
  {
    mPaths.push_back(path);
  }

  std::string ConfigManager::ToShortString() const
  {
    std::string str;
    str += "ConfigManager ";
    str += ToHexString(this);
    if (mPaths.size())
    {
      str += ", ";
      IObject::AppendObjectCount(str, "path", mPaths.size());
    }
    if (mConfigurations.size())
    {
      str += ", ";
      IObject::AppendObjectCount(str, "configuration", mConfigurations.size());
    }
    return str;
  }

  void ConfigManager::ToLongString(std::string& str, int indent) const
  {
    const bool have_children = (mConfigurations.size() > 0);
    const std::string indent_str = std::string(indent, ' ');

    const std::string short_string = ToShortString();
    str += indent_str + short_string;
    if (have_children)
    {
      str += " {\n";

      // print config children
      for (size_t i = 0; i < mConfigurations.size(); i++)
      {
        ConfigFile* config = mConfigurations[i];
        config->ToLongString(str, indent + 2);

        str += "\n";
      }

      str += indent_str + "}";
    }
  }

  bool ConfigManager::IsConfigFileLoaded(const std::string& path) const
  {
    for (size_t i = 0; i < mConfigurations.size(); i++)
    {
      const ConfigFile* config = mConfigurations[i];
      if (config != NULL && config->GetFilePath() == path)
        return true;
    }
    return false;
  }

  void ConfigManager::DeleteChildren()
  {
    // delete configurations
    for (size_t i = 0; i < mConfigurations.size(); i++)
    {
      ConfigFile* config = mConfigurations[i];
      delete config;
    }
    mConfigurations.clear();
  }

  void ConfigManager::DeleteChild(ConfigFile* config)
  {
    mConfigurations.erase(std::find(mConfigurations.begin(), mConfigurations.end(), config));
    delete config;
  }

} //namespace shellanything
