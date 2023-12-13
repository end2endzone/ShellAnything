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

#include "rapidassist/process.h"
#include "rapidassist/user.h"
#include "rapidassist/environment.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/user_utf8.h"
#include "rapidassist/unicode.h"

#include "shellanything/version.h"

#include "SaUtils.h"

#include <string.h>

namespace shellanything
{
  static const std::string app_name = "ShellAnything";
  static const std::string app_version = SHELLANYTHING_VERSION;


  App::App() :
    mLogger(NULL)
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

  void App::SetLogger(ILogger* logger)
  {
    mLogger = logger;
  }

  ILogger* App::GetLogger()
  {
    return mLogger;
  }

  bool App::IsTestingEnvironment()
  {
    std::string process_path = ra::process::GetCurrentProcessPath();
    if (process_path.find("sa.tests") != std::string::npos)
      return true;
    return false;
  }

  std::string App::GetLogDirectory()
  {
    //Issue #10 - Change the log directory if run from the unit tests executable
    if (IsTestingEnvironment())
    {
      //This DLL is executed by the unit tests.

      //Create 'test_logs' directory under the current executable.
      //When running tests from a developer environment, the 'test_logs' directory is expected to have write access.
      std::string log_dir = ra::process::GetCurrentProcessDir();
      if (!log_dir.empty())
      {
        log_dir.append("\\test_logs");
        if (IsValidLogDirectory(log_dir))
          return log_dir;
      }

      //Issue #60 - Unit tests cannot execute from installation directory.
      //If unit tests are executed from the installation directory,
      //the 'test_logs' directory under the current executable is denied write access.
      log_dir = ra::environment::GetEnvironmentVariable("TEMP");
      if (!log_dir.empty())
      {
        log_dir.append("\\test_logs");
        if (IsValidLogDirectory(log_dir))
          return log_dir;
      }
    }

    //This DLL is executed by the shell (File Explorer).

    //By default, GLOG will output log files in %TEMP% directory.
    //However, I prefer to use %USERPROFILE%\ShellAnything\Logs

    std::string log_dir = ra::user::GetHomeDirectory();
    if (!log_dir.empty())
    {
      //We got the %USERPROFILE% directory.
      //Now add our custom path to it
      log_dir.append("\\ShellAnything\\Logs");
      if (IsValidLogDirectory(log_dir))
        return log_dir;
    }

    //Failed getting HOME directory.
    //Fallback to using %TEMP%.
    log_dir = ra::environment::GetEnvironmentVariable("TEMP");
    return log_dir;
  }

  std::string App::GetConfigurationsDirectory()
  {
    //get home directory of the user
    std::string home_dir = ra::user::GetHomeDirectoryUtf8();
    std::string config_dir = home_dir + "\\" + app_name;
    return config_dir;
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
    if (!ra::filesystem::DirectoryExists(path.c_str()))
    {
      //Try to create the directory.
      bool created = ra::filesystem::CreateDirectory(path.c_str());
      if (!created)
        return false;
    }

    //Validate that directory path is writable.
    bool write_access = HasDirectoryWriteAccess(path);
    if (!write_access)
      return false; //Write to directory is denied.

    //Write to directory is granted.
    return true;
  }

  void App::InstallDefaultConfigurations(const std::string& dest_dir)
  {
    std::string app_path = GetCurrentModulePathUtf8();
    std::string app_dir = ra::filesystem::GetParentPath(app_path);

    static const char* default_files[] = {
      "default.xml",
      "Microsoft Office 2003.xml",
      "Microsoft Office 2007.xml",
      "Microsoft Office 2010.xml",
      "Microsoft Office 2013.xml",
      "Microsoft Office 2016.xml",
      "shellanything.xml",
    };
    static const size_t num_files = sizeof(default_files) / sizeof(default_files[0]);

    SA_LOG(INFO) << "Installing default configurations files.";

    // Copy default files one by one to the user's directory
    for (size_t i = 0; i < num_files; i++)
    {
      const char* filename = default_files[i];
      std::string source_path = app_dir + "\\configurations\\" + filename;
      std::string target_path = dest_dir + "\\" + filename;

      SA_LOG(INFO) << "Installing configuration file: " << target_path;
      bool installed = ra::filesystem::CopyFileUtf8(source_path, target_path);
      if (!installed)
      {
        SA_LOG(ERROR) << "Failed coping file '" << source_path << "' to target file '" << target_path << "'.";
      }
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
    std::string log_dir = ra::unicode::AnsiToUtf8(GetLogDirectory());

    SA_LOG(INFO) << "HOME   directory : " << home_dir.c_str();
    SA_LOG(INFO) << "Config directory : " << config_dir.c_str();
    SA_LOG(INFO) << "LOG    directory : " << log_dir.c_str();

    //define global properties
    pmgr.SetProperty("home.directory", home_dir);
    pmgr.SetProperty("config.directory", config_dir);
    pmgr.SetProperty("log.directory", log_dir);
  }

} //namespace shellanything
