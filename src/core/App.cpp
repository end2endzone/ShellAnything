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

#include "App.h"
#include "LoggerHelper.h"
#include "ConfigManager.h"
#include "PropertyManager.h"
#include "Environment.h"

#include "rapidassist/process_utf8.h"
#include "rapidassist/user_utf8.h"
#include "rapidassist/environment_utf8.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/unicode.h"

#include "shellanything/version.h"

#include "SaUtils.h"

#include <string.h>

namespace shellanything
{
  static const std::string app_name = "ShellAnything";
  static const std::string app_version = SHELLANYTHING_VERSION;


  App::App() :
    mLogger(NULL),
    mRegistry(NULL),
    mClipboard(NULL),
    mKeyboard(NULL)
  {
  }

  App::~App()
  {
  }

  App& App::GetInstance()
  {
    static App _instance;
    return _instance;
  }

  const std::string& App::GetApplicationPath()
  {
    return mApplicationPath;
  }

  void App::SetApplicationPath(const std::string& value)
  {
    mApplicationPath = value;

    //Issue #124. Define property 'application.path'.
    shellanything::PropertyManager& pmgr = shellanything::PropertyManager::GetInstance();
    pmgr.SetProperty("application.path", mApplicationPath);
  }

  void App::ClearServices()
  {
    mLogger = NULL;
    mRegistry = NULL;
    mClipboard = NULL;
    mKeyboard = NULL;
  }

  void App::SetLoggerService(ILoggerService* logger)
  {
    mLogger = logger;
  }

  ILoggerService* App::GetLoggerService()
  {
    return mLogger;
  }

  void App::SetRegistryService(IRegistryService* instance)
  {
    mRegistry = instance;
  }

  IRegistryService* App::GetRegistryService()
  {
    return mRegistry;
  }

  void App::SetClipboardService(IClipboardService* instance)
  {
    mClipboard = instance;
  }

  IClipboardService* App::GetClipboardService()
  {
    return mClipboard;
  }

  void App::SetKeyboardService(IKeyboardService* instance)
  {
    mKeyboard = instance;
  }

  IKeyboardService* App::GetKeyboardService()
  {
    return mKeyboard;
  }

  void App::SetRandomService(IRandomService* instance)
  {
    mRandom = instance;
  }

  IRandomService* App::GetRandomService()
  {
    return mRandom;
  }

  void App::SetIconResolutionService(IIconResolutionService* instance)
  {
    mIconResolutionService = instance;
  }

  IIconResolutionService* App::GetIconResolutionService()
  {
    return mIconResolutionService;
  }

  void App::SetProcessLauncherService(IProcessLauncherService* instance)
  {
    mProcessLauncherService = instance;
  }

  IProcessLauncherService* App::GetProcessLauncherService()
  {
    return mProcessLauncherService;
  }

  bool App::IsTestingEnvironment()
  {
    std::string process_path = ra::process::GetCurrentProcessPathUtf8();
    if (process_path.find("sa.tests") != std::string::npos)
      return true;
    return false;
  }

  std::string App::GetLegacyLogsDirectory()
  {
    //get home directory of the user
    std::string home_dir = ra::user::GetHomeDirectoryUtf8();
    std::string legacy_dir = home_dir + "\\" + app_name + "\\Logs";
    return legacy_dir;
  }

  std::string App::GetLogDirectory()
  {
    //Issue #10 - Change the log directory if run from the unit tests executable
    if (IsTestingEnvironment())
    {
      //This DLL is executed by the unit tests.

      //Create 'test_logs' directory under the current executable.
      //When running tests from a developer environment, the 'test_logs' directory is expected to have write access.
      std::string log_dir = ra::process::GetCurrentProcessDirUtf8();
      if (!log_dir.empty())
      {
        log_dir.append("\\test_logs");
        if (IsValidLogDirectory(log_dir))
          return log_dir;
      }

      //Issue #60 - Unit tests cannot execute from installation directory.
      //If unit tests are executed from the installation directory,
      //the 'test_logs' directory under the current executable is denied write access.
      log_dir = ra::environment::GetEnvironmentVariableUtf8("TEMP");
      if (!log_dir.empty())
      {
        log_dir.append("\\test_logs");
        if (IsValidLogDirectory(log_dir))
          return log_dir;
      }
    }

    //This DLL is most probably executed by the shell (File Explorer).

    //By default, GLOG will output log files in %TEMP% directory.

    // Issue #108. Log files directory can be overriden with an option.
    Environment& env = Environment::GetInstance();
    if (env.IsOptionSet(Environment::SYSTEM_LOGS_DIR_OVERRIDE_ENVIRONMENT_VARIABLE_NAME))
    {
      std::string log_dir = env.GetOptionValue(Environment::SYSTEM_LOGS_DIR_OVERRIDE_ENVIRONMENT_VARIABLE_NAME);
      if (IsValidLogDirectory(log_dir))
        return log_dir;
    }

    // Issue #108. Log files should be stored in %LOCALAPPDATA%\ShellAnything\logs
    std::string localappdata_dir = ra::environment::GetEnvironmentVariableUtf8("LOCALAPPDATA");
    if (!localappdata_dir.empty() && ra::filesystem::DirectoryExistsUtf8(localappdata_dir.c_str()))
    {
      std::string log_dir = localappdata_dir + "\\" + app_name + "\\logs";
      if (IsValidLogDirectory(log_dir))
        return log_dir;
    }

    // Fallback to %USERPROFILE%\ShellAnything\logs
    std::string home_dir = ra::user::GetHomeDirectoryUtf8();
    if (!home_dir.empty() && ra::filesystem::DirectoryExistsUtf8(home_dir.c_str()))
    {
      //We got the %USERPROFILE% directory.
      std::string log_dir = home_dir + "\\" + app_name + "\\logs";
      if (IsValidLogDirectory(log_dir))
        return log_dir;
    }

    //Failed getting HOME directory.
    //Fallback to using %TEMP%.
    std::string temp_dir = ra::environment::GetEnvironmentVariableUtf8("TEMP");
    return temp_dir;
  }

  std::string App::GetLegacyConfigurationsDirectory()
  {
    //get home directory of the user
    std::string home_dir = ra::user::GetHomeDirectoryUtf8();
    std::string legacy_dir = home_dir + "\\" + app_name;
    return legacy_dir;
  }

  std::string App::GetConfigurationsDirectory()
  {
    // Issue #108. Configuration Files directory can be overriden with an option.
    Environment& env = Environment::GetInstance();
    if (env.IsOptionSet(Environment::SYSTEM_CONFIGURATIONS_DIR_OVERRIDE_ENVIRONMENT_VARIABLE_NAME))
    {
      std::string config_dir = env.GetOptionValue(Environment::SYSTEM_CONFIGURATIONS_DIR_OVERRIDE_ENVIRONMENT_VARIABLE_NAME);
      if (IsValidConfigDirectory(config_dir))
        return config_dir;
    }

    //get home directory of the user
    std::string home_dir = ra::user::GetHomeDirectoryUtf8();
    std::string app_dir = home_dir + "\\" + app_name;
    std::string config_dir = app_dir + +"\\configurations";

    if (IsValidConfigDirectory(config_dir))
      return config_dir;

    return config_dir;
  }

  std::string App::GetBinDirectory()
  {
    const std::string module_path = GetCurrentModulePathUtf8();
    const std::string bin_dir = ra::filesystem::GetParentPath(module_path);
    return bin_dir;
  }

  std::string App::GetInstallDirectory()
  {
    const std::string bin_dir = GetBinDirectory();
    const std::string install_dir = ra::filesystem::GetParentPath(bin_dir);
    return install_dir;
  }

  bool App::Start()
  {
    SetupGlobalProperties();

    InitConfigManager();

    return true;
  }

  bool App::IsValidLogDirectory(const std::string& path)
  {
    //Issue #60 - Unit tests cannot execute from installation directory.

    //Check if the directory already exists
    if (!ra::filesystem::DirectoryExistsUtf8(path.c_str()))
    {
      //Try to create the directory.
      bool created = ra::filesystem::CreateDirectoryUtf8(path.c_str());
      if (!created)
        return false;
    }

    //Validate that directory path is writable.
    bool write_access = HasDirectoryWriteAccessUtf8(path);
    if (!write_access)
      return false; //Write to directory is denied.

    //Write to directory is granted.
    return true;
  }

  bool App::IsValidConfigDirectory(const std::string& path)
  {
    // Config directory must be accessible for reading.
    // Write access is optional

    //Check if the directory already exists
    if (!ra::filesystem::DirectoryExistsUtf8(path.c_str()))
    {
      //Try to create the directory.
      bool created = ra::filesystem::CreateDirectoryUtf8(path.c_str());
      if (!created)
        return false;
    }

    //Validate that directory path is readable.
    bool write_access = HasDirectoryReadAccessUtf8(path);
    if (!write_access)
      return false; //Write to directory is denied.

    //Write to directory is granted.
    return true;
  }

  void App::InstallDefaultConfigurations(const std::string& dest_dir)
  {
    const std::string install_dir = shellanything::App::GetInstallDirectory();

    static const char* default_files[] = {
      "default.xml",
      "shellanything.xml",
    };
    static const size_t num_files = sizeof(default_files) / sizeof(default_files[0]);

    SA_LOG(INFO) << "Installing default configurations files.";

    // Copy default files one by one to the user's directory
    for (size_t i = 0; i < num_files; i++)
    {
      const char* filename = default_files[i];
      std::string source_path = install_dir + "\\resources\\configurations\\" + filename;
      std::string target_path = dest_dir + "\\" + filename;

      SA_LOG(INFO) << "Installing configuration file: " << target_path;
      bool installed = ra::filesystem::CopyFileUtf8(source_path, target_path);
      if (!installed)
      {
        SA_LOG(ERROR) << "Failed coping file '" << source_path << "' to target file '" << target_path << "'.";
      }
    }
  }

  void App::ClearLegacyConfigurationDirectory()
  {
    const std::string legacy_config_dir = GetLegacyConfigurationsDirectory();
    const std::string config_dir = GetConfigurationsDirectory();
    if (legacy_config_dir == config_dir)
      return; // nothing to do

    // Search for xml files directly under legacy_dir
    ra::strings::StringVector files;
    static const int depth = 0; // Do not search recursively
    bool success = ra::filesystem::FindFilesUtf8(files, legacy_config_dir.c_str(), depth);
    if (!success)
      return; // aborted

    // for each file found
    for (size_t i = 0; i < files.size(); i++)
    {
      const std::string& file_path = files[i];

      // Is that a configuration file ?
      if (ConfigFile::IsValidConfigFile(file_path))
      {
        // It does not belongs there. Move it to the new configuration directory.

        std::string file_name = ra::filesystem::GetFilename(file_path.c_str());
        std::string old_path = file_path;
        std::string new_path = config_dir + "\\" + file_name;

        SA_LOG(INFO) << "Moving legacy configuration file '" << old_path << "' to '" << new_path << "'.";
        bool moved = RenameFileUtf8(old_path, new_path);
        if (!moved)
        {
          SA_LOG(ERROR) << "Failed moving configuration file '" << old_path << "' to target file '" << new_path << "'.";
        }
      }
    }
  }

  void App::ClearLegacyLogsDirectory()
  {
    const std::string legacy_logs_dir = GetLegacyLogsDirectory();

    // Search for log files directly under legacy_logs_dir
    ra::strings::StringVector files;
    static const int depth = 0; // Do not search recursively
    bool success = ra::filesystem::FindFilesUtf8(files, legacy_logs_dir.c_str(), depth);
    if (!success)
      return; // aborted

    // for each file found
    for (size_t i = 0; i < files.size(); i++)
    {
      const std::string& file_path = files[i];

      // Is that a configuration file ?
      if (LoggerHelper::IsValidLogFile(file_path))
      {
        // It does not belongs there.
        // Delete the file
        SA_LOG(INFO) << "Deleting old legacy log file '" << file_path << "'.";
        bool deleted = ra::filesystem::DeleteFileUtf8(file_path.c_str());
        if (!deleted)
        {
          SA_LOG(ERROR) << "Failed deleting old legacy log file '" << file_path << "'.";
        }
      }
    }

    // Check if the directory is empty.
    // We need to make this check before calling ra::filesystel::DeleteDirectory() because the
    // DeleteDirectory function will automatically delete remaining files in order to delete the directory.
    // We need to make sure the directory is empty first.
    bool empty = ra::filesystem::IsDirectoryEmptyUtf8(legacy_logs_dir);
    if (empty)
    {
      // Now it is safe to delete the directory
      SA_LOG(INFO) << "Deleting old legacy log directory '" << legacy_logs_dir << "'.";
      ra::filesystem::DeleteDirectoryUtf8(legacy_logs_dir.c_str());
    }
    else
    {
      SA_LOG(ERROR) << "Skipped deleting old legacy log directory '" << legacy_logs_dir << "'. The directory is not empty. The directory likely contains files that are not log files.";
    }
  }

  void App::InitConfigManager()
  {
    shellanything::ConfigManager& cmgr = shellanything::ConfigManager::GetInstance();

    std::string config_dir = GetConfigurationsDirectory();

    bool first_run = IsFirstApplicationRun(app_name, app_version);
    if (first_run)
    {
      SA_LOG(INFO) << "First application launch.";
      ClearLegacyConfigurationDirectory(); // Issue #108 moved Configuration Files directory to a new location.
      ClearLegacyLogsDirectory(); // Issue #108 delete previous logs directory.
      InstallDefaultConfigurations(config_dir);
    }

    //setup ConfigManager to read files from config_dir
    cmgr.ClearSearchPath();
    cmgr.AddSearchPath(config_dir);
    cmgr.Refresh();
  }

  void App::SetupGlobalProperties()
  {
    shellanything::PropertyManager& pmgr = shellanything::PropertyManager::GetInstance();

    //get home directory of the user
    std::string home_dir = ra::user::GetHomeDirectoryUtf8();
    std::string config_dir = GetConfigurationsDirectory();
    std::string log_dir = GetLogDirectory();

    SA_LOG(INFO) << "HOME   directory : " << home_dir.c_str();
    SA_LOG(INFO) << "CONFIG directory : " << config_dir.c_str();
    SA_LOG(INFO) << "LOG    directory : " << log_dir.c_str();

    //define global properties
    pmgr.SetProperty("home.directory", home_dir);
    pmgr.SetProperty("config.directory", config_dir);
    pmgr.SetProperty("log.directory", log_dir);
  }

} //namespace shellanything
