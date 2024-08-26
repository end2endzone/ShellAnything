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

#include <stdio.h>
#include <iostream>
#include <stdio.h>

#include <gtest/gtest.h>

#include "rapidassist/testing.h"
#include "rapidassist/environment.h"
#include "rapidassist/cli.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/unicode.h"
#include "rapidassist/process_utf8.h"
#include "rapidassist/user_utf8.h"

#include "ArgumentsHandler.h"
#include "GlogUtils.h"
#include "SaUtils.h"
#include "PropertyManager.h"
#include "Workspace.h"

#include "ILoggerService.h"
#include "App.h"
#include "LoggerHelper.h"
#include "GlogLoggerService.h"
#include "WindowsRegistryService.h"
#include "WindowsClipboardService.h"
#include "TestKeyboardService.h"
#include "PcgRandomService.h"
#include "WindowsIconResolutionService.h"
#include "ConfigManager.h"

using namespace ra;
using namespace shellanything::logging;

shellanything::TestKeyboardService* keyboard_service = NULL;

int SetTestPreferedRootDirectory()
{
  std::string exec_dir = ra::process::GetCurrentProcessDirUtf8();
  std::string home_dir = ra::user::GetDocumentsDirectoryUtf8();
  std::string temp_dir = ra::filesystem::GetTemporaryDirectoryUtf8();

  // Find a suitable write directory
  std::string writable_dir;
  if (ra::filesystem::HasDirectoryWriteAccessUtf8(exec_dir.c_str()))
    writable_dir = exec_dir;
  else if (ra::filesystem::HasDirectoryWriteAccessUtf8(home_dir.c_str()))
    writable_dir = home_dir;
  else if (ra::filesystem::HasDirectoryWriteAccessUtf8(temp_dir.c_str()))
    writable_dir = temp_dir;
  else
  {
    const char* message = "Failed to find a writable directory to use for tests";

    SA_LOG(ERROR) << message;
    printf(message);
    printf("\n");

    return 1;
  }

  std::string root_dir = writable_dir + ra::filesystem::GetPathSeparatorStr() + "test_workspace";

  // Should we clean root directory first ?
  if (ra::filesystem::DirectoryExistsUtf8(root_dir.c_str()))
  {
    // Directory already exists. Clean it.
    bool deleted = ra::filesystem::DeleteDirectoryUtf8(root_dir.c_str());
    if (!deleted)
    {
      // hope for the best...
    }
  }

  // Create the root directory
  bool created = ra::filesystem::CreateDirectoryUtf8(root_dir.c_str());
  if (!created)
    return 2;

  shellanything::Workspace::SetPreferedRootDirectoryUtf8(root_dir.c_str());
  printf("Using test directory: '%s'.\n", root_dir.c_str());
  SA_LOG(INFO) << "Using test directory: '" << root_dir << "'.";

  return 0;
}

int main(int argc, char** argv)
{
  // Look for custom command line arguments
  bool has_PrintProcessSettings = ra::cli::ParseArgument("PrintProcessSettings", std::string(), argc, argv);
  if (has_PrintProcessSettings)
    return shellanything::PrintProcessSettings(argc, argv);
  if (shellanything::HasCommandLineEntryPoints(argc, argv))
  {
    return shellanything::ProcessCommandLineEntryPoints(argc, argv);
  }

  shellanything::App& app = shellanything::App::GetInstance();

  //Define application's main executable path.
  std::string exec_path = ra::process::GetCurrentProcessPathUtf8();
  app.SetApplicationPath(exec_path);

  // Initialize Google's logging library.
  glog::InitGlog();

  // Setup an active logger service in ShellAnything's core.
  shellanything::ILoggerService* logger_service = new shellanything::GlogLoggerService();
  app.SetLoggerService(logger_service);

  // Setup an active registry service in ShellAnything's core.
  shellanything::IRegistryService* registry_service = new shellanything::WindowsRegistryService();
  app.SetRegistryService(registry_service);

  // Setup an active clipboard service in ShellAnything's core.
  shellanything::IClipboardService* clipboard_service = new shellanything::WindowsClipboardService();
  app.SetClipboardService(clipboard_service);

  // Setup an active clipboard service in ShellAnything's core.
  keyboard_service = new shellanything::TestKeyboardService();
  app.SetKeyboardService(keyboard_service);

  // Setup an active random service in ShellAnything's core.
  shellanything::IRandomService* random_service = new shellanything::PcgRandomService();
  app.SetRandomService(random_service);

  // Setup an active icon resolution service in ShellAnything's core.
  shellanything::IIconResolutionService* icon_resolution_service = new shellanything::WindowsIconResolutionService();
  app.SetIconResolutionService(icon_resolution_service);

  //Issue #60 - Unit tests cannot execute from installation directory.
  //Create log directory under the current executable.
  //When running tests from a developer environment, the log directory is expected to have write access.
  //If unit tests are executed from the installation directory, the log directory under the current executable is denied write access.
  std::string log_dir = app.GetLogDirectory();
  printf("Using log directory: '%s'.\n", log_dir.c_str());

  // Setup and starting application
  app.Start();

  // Clear official Configuration Files directory from unit test scope.
  shellanything::ConfigManager& cmgr = shellanything::ConfigManager::GetInstance();
  cmgr.ClearSearchPath(); // remove official Configuration Files directory from seached directories.
  cmgr.Clear(); // remove all preloaded configurations.
  cmgr.Refresh();

  // Setup Workspace class root directory
  int exit_code = SetTestPreferedRootDirectory();
  if (exit_code != 0)
  {
    // Shutdown Google's logging library.
    glog::ShutdownGlog();
    return exit_code;
  }

  SA_LOG(INFO) << "Starting unit tests";
  SA_LOG(INFO) << __FUNCTION__ << "() - BEGIN";

  //define path for xml output report
  std::string outputXml = "xml:";
  if (!HasDirectoryWriteAccess("."))
  {
    //Issue #60 - Unit tests cannot execute from installation directory.
    //Output report in same directory as the log files
    outputXml.append(log_dir);
    outputXml.append("\\");
  }
  outputXml.append("sa.tests");
  outputXml += (ra::environment::IsProcess32Bit() ? ".x86" : ".x64");
  outputXml += (ra::environment::IsConfigurationDebug() ? ".debug" : ".release");
  outputXml += ".xml";
  ::testing::GTEST_FLAG(output) = outputXml;
  printf("Using xml report: '%s'.\n", outputXml.c_str());

  ::testing::GTEST_FLAG(filter) = "*";
  ::testing::InitGoogleTest(&argc, argv);

  int wResult = RUN_ALL_TESTS(); //Find and run all tests

  SA_LOG(INFO) << __FUNCTION__ << "() - END";

  // Shutdown Google's logging library.
  glog::ShutdownGlog();

  // Destroy services
  app.ClearServices();
  delete random_service;
  delete keyboard_service;
  delete clipboard_service;
  delete registry_service;
  delete logger_service;
  delete icon_resolution_service;
  random_service = NULL;
  keyboard_service = NULL;
  clipboard_service = NULL;
  registry_service = NULL;
  logger_service = NULL;
  icon_resolution_service = NULL;

  return wResult; // returns 0 if all the tests are successful, or 1 otherwise
}
