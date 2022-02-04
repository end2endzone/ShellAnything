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

#include "TestActionExecute.h"
#include "Context.h"
#include "ActionExecute.h"
#include "PropertyManager.h"
#include "rapidassist/testing.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/user.h"
#include "rapidassist/timing.h"
#include "rapidassist/environment.h"
#include "rapidassist/process.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <Windows.h>
#undef GetEnvironmentVariable
#undef DeleteFile
#undef CreateDirectory
#undef CopyFile
#undef CreateFile
#undef GetCurrentDirectory


namespace shellanything { namespace test
{
  void KillCalculatorProcess()
  {
    system("cmd.exe /c taskkill /IM calc.exe >NUL 2>NUL");

    // On Windows 10, calc.exe launches Calculator which is an application in the Microsoft App Store.
    //The executable path is something similar to C:\Program Files\WindowsApps\Microsoft.WindowsCalculator_10.2103.8.0_x64__8wbfmf6g6wwcr\Calculator.exe
    system("cmd.exe /c WMIC PROCESS WHERE \"ExecutablePath like '%%Microsoft.WindowsCalculator%%Calculator.exe'\" DELETE >NUL 2>NUL");
  }

  //--------------------------------------------------------------------------------------------------
  void TestActionExecute::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestActionExecute::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestActionExecute, testBasic)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    //Create a valid context
    Context c;
    Context::ElementList elements;
    elements.push_back("C:\\Windows\\System32\\calc.exe");
    c.SetElements(elements);

    c.RegisterProperties();

    //Execute the action
    ActionExecute ae;
    ae.SetPath("C:\\Windows\\System32\\calc.exe");
    ae.SetBaseDir("");
    ae.SetArguments("");

    bool executed = ae.Execute(c);
    ASSERT_TRUE( executed );

    //Cleanup
    ra::timing::Millisleep(500);
    KillCalculatorProcess();
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestActionExecute, testBaseDir)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    //Create a valid context
    Context c;
    Context::ElementList elements;
    elements.push_back("C:\\Windows\\System32\\calc.exe");
    c.SetElements(elements);

    c.RegisterProperties();

    std::string home_dir = ra::user::GetHomeDirectory();

    //Execute the action
    ActionExecute ae;
    ae.SetPath("calc.exe");
    ae.SetBaseDir("C:\\Windows\\System32");
    ae.SetArguments("");

    bool executed = ae.Execute(c);
    ASSERT_TRUE( executed );

    //Cleanup
    ra::timing::Millisleep(500);
    KillCalculatorProcess();
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestActionExecute, testArguments)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    //Create a valid context
    Context c;
    Context::ElementList elements;
    elements.push_back("C:\\Windows\\System32\\calc.exe");
    c.SetElements(elements);

    c.RegisterProperties();

    std::string home_dir = ra::user::GetHomeDirectory();
    std::string temp_dir = ra::filesystem::GetTemporaryDirectory();
    std::string destination_path = temp_dir + "\\my_calc.exe";
    std::string arguments = "/c copy C:\\Windows\\System32\\calc.exe " + destination_path + ">NUL 2>NUL";

    //Execute the action
    ActionExecute ae;
    ae.SetPath("cmd.exe");
    ae.SetBaseDir(temp_dir);
    ae.SetArguments(arguments);

    bool executed = ae.Execute(c);
    ASSERT_TRUE( executed );

    //Wait for the copy to complete, with a timeout
    static const double timeout_time = 50; //seconds
    bool file_copied = false;
    double timer_start = ra::timing::GetMillisecondsTimer();
    double time_elapsed = ra::timing::GetMillisecondsTimer() - timer_start;
    while(!file_copied && time_elapsed <= timeout_time)
    {
      file_copied = ra::filesystem::FileExists(destination_path.c_str());
      ra::timing::Millisleep(500); //allow process to complete
      time_elapsed = ra::timing::GetMillisecondsTimer() - timer_start; //evaluate elapsed time again
    }

    //Validate arguments
    ASSERT_TRUE(file_copied);

    //Cleanup
    ra::filesystem::DeleteFileUtf8(destination_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestActionExecute, testVerb)
  {
    //--------------------------------------------------------------------
    //Note:
    //  The cmd.exe cannot be used for testing the verb and the basedir functionalities at the same time.
    //  Whenever cmd.exe detects it's running elevated, it ignores its launch parameters and always starts in %SystemRoot%\System32. You cannot override this behavior.
    //  For this reason, a command such as `net session >nul 2>&1` cannot be used to detect if the process is elevated or not.
    //
    //  See the following for details:
    //  https://stackoverflow.com/questions/38033790/shellexecute-a-cmd-prompt-with-a-specific-working-directory
    //--------------------------------------------------------------------


    //Skip this test if run on AppVeyor as it requires administrative (elevated) privileges.
    if (ra::testing::IsAppVeyor() ||
        ra::testing::IsJenkins() ||
        ra::testing::IsTravis() ||
        ra::testing::IsGitHubActions())
    {
      printf("Skipping tests as it requires administrative (elevated) privileges.\n");
      return;
    }

    PropertyManager & pmgr = PropertyManager::GetInstance();

    //Create a valid context
    Context c;
    Context::ElementList elements;
    elements.push_back("C:\\Windows\\System32\\calc.exe");
    c.SetElements(elements);

    c.RegisterProperties();

    std::string self_path = ra::process::GetCurrentProcessPath();
    std::string temp_dir = ra::filesystem::GetTemporaryDirectory();
    std::string output_filename = "sa.tests.ProcessSettings.txt";
    std::string output_file_path = temp_dir + "\\" + output_filename;
    std::string arguments = "--PrintProcessSettings --foobar";

    //Cleanup
    ra::filesystem::DeleteFileUtf8(output_file_path.c_str());

    //Execute the action
    ActionExecute ae;
    ae.SetPath(self_path);
    ae.SetBaseDir(temp_dir);
    ae.SetArguments(arguments);
    ae.SetVerb("runas");

    bool executed = ae.Execute(c);
    ASSERT_TRUE( executed );

    //Wait for the operation to complete, with a timeout
    static const double timeout_time = 50; //seconds
    bool result_file_found = false;
    double timer_start = ra::timing::GetMillisecondsTimer();
    double time_elapsed = ra::timing::GetMillisecondsTimer() - timer_start;
    while(!result_file_found && time_elapsed <= timeout_time)
    {
      result_file_found = ra::filesystem::FileExists(output_file_path.c_str());
      ra::timing::Millisleep(500); //allow process to complete
      time_elapsed = ra::timing::GetMillisecondsTimer() - timer_start; //evaluate elapsed time again
    }

    //Validate arguments
    ASSERT_TRUE(result_file_found);

    //Read the result file, search for the elevated process signature.
    std::string result;
    ASSERT_TRUE( ra::filesystem::ReadTextFile(output_file_path, result) );
    static const std::string PROCESS_ELEVATED_SIGNATURE = "process.elevated=1";
    ASSERT_NE(result.find(PROCESS_ELEVATED_SIGNATURE), std::string::npos);

    //Cleanup
    ra::filesystem::DeleteFileUtf8(output_file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestActionExecute, DISABLED_demoAdminCommandPromptHere)
  {
    //----------------------------------
    //Note:
    //  This is not actually a test but a demontration 
    //  of a "Open Adminitrator command prompt here" command.
    //----------------------------------


    //Skip this test if run on AppVeyor as it requires administrative (elevated) privileges.
    if (ra::testing::IsAppVeyor() ||
        ra::testing::IsJenkins() ||
        ra::testing::IsTravis())
    {
      printf("Skipping tests as it requires administrative (elevated) privileges.\n");
      return;
    }

    PropertyManager & pmgr = PropertyManager::GetInstance();

    std::string home_dir = ra::user::GetHomeDirectory();

    //Create a valid context
    Context c;
    Context::ElementList elements;
    elements.push_back(home_dir);
    c.SetElements(elements);

    c.RegisterProperties();

    //Execute the action as explained in https://stackoverflow.com/questions/38033790/shellexecute-a-cmd-prompt-with-a-specific-working-directory
    ActionExecute ae;
    ae.SetPath("cmd.exe");
    ae.SetBaseDir(home_dir);
    ae.SetArguments("/k cd /d ${selection.path}");
    ae.SetVerb("runas");

    bool executed = ae.Execute(c);
    ASSERT_TRUE( executed );
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
