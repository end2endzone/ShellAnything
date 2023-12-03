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

#include "rapidassist/process.h"
#include "rapidassist/user.h"
#include "rapidassist/environment.h"
#include "rapidassist/filesystem.h"

#include "SaUtils.h"

#include <string.h>

namespace shellanything
{
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

} //namespace shellanything
