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
#include "shellanything/Context.h"
#include "shellanything/ActionExecute.h"
#include "PropertyManager.h"
#include "rapidassist/testing.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/user.h"
#include "rapidassist/timing.h"
#include "rapidassist/environment.h"

#include <Windows.h>

namespace shellanything { namespace test
{

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

    //execute the action
    ActionExecute ae;
    ae.SetPath("C:\\Windows\\System32\\calc.exe");
    ae.SetBaseDir("");
    ae.SetArguments("");

    bool executed = ae.Execute(c);
    ASSERT_TRUE( executed );

    //cleanup
    ra::timing::Millisleep(500);
    system("cmd.exe /c taskkill /IM calc.exe >NUL 2>NUL");
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

    //execute the action
    ActionExecute ae;
    ae.SetPath("calc.exe");
    ae.SetBaseDir("C:\\Windows\\System32");
    ae.SetArguments("");

    bool executed = ae.Execute(c);
    ASSERT_TRUE( executed );

    //cleanup
    ra::timing::Millisleep(500);
    system("cmd.exe /c taskkill /IM calc.exe >NUL 2>NUL");
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

    //execute the action
    ActionExecute ae;
    ae.SetPath("cmd.exe");
    ae.SetBaseDir(temp_dir);
    ae.SetArguments(arguments);

    bool executed = ae.Execute(c);
    ASSERT_TRUE( executed );

    //Wait for the copy to complete, with a timeout
    static const double timeout_time = 5000; //ms
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

    //cleanup
    ra::filesystem::DeleteFileUtf8(destination_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestActionExecute, testVerb)
  {
    //Skip this test if run on AppVeyor as it requires administrative (elevated) privileges.
    if (ra::testing::IsAppVeyor() ||
        ra::testing::IsJenkins() ||
        ra::testing::IsTravis())
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

    std::string temp_dir = ra::filesystem::GetTemporaryDirectory();
    std::string batch_file_filename = ra::testing::GetTestQualifiedName() + ".bat";
    std::string result_file_filename = ra::testing::GetTestQualifiedName() + ".txt";
    std::string batch_file_path = temp_dir + "\\" + batch_file_filename;
    std::string result_file_path = temp_dir + "\\" + result_file_filename;
    std::string arguments = "/c " + batch_file_filename + " >" + result_file_filename;

    //This is a batch file that prints ADMIN if it is run in elevated mode or prints FAIL otherwise.
    //Inspired from https://stackoverflow.com/questions/4051883/batch-script-how-to-check-for-admin-rights
    const std::string content = 
      "@echo off\n"
      "net session >nul 2>&1\n"
      "if %errorLevel% == 0 (\n"
      "  echo ADMIN\n"
      ") else (\n"
      "  echo FAIL\n"
      ")\n";
    ASSERT_TRUE( ra::filesystem::WriteTextFile(batch_file_path, content) );

    //execute the action
    ActionExecute ae;
    ae.SetPath("cmd.exe");
    ae.SetBaseDir(temp_dir);
    ae.SetArguments(arguments);
    ae.SetVerb("runas");

    bool executed = ae.Execute(c);
    ASSERT_TRUE( executed );

    //Wait for the operation to complete, with a timeout
    static const double timeout_time = 5000; //ms
    bool result_file_found = false;
    double timer_start = ra::timing::GetMillisecondsTimer();
    double time_elapsed = ra::timing::GetMillisecondsTimer() - timer_start;
    while(!result_file_found && time_elapsed <= timeout_time)
    {
      result_file_found = ra::filesystem::FileExists(result_file_path.c_str());
      ra::timing::Millisleep(500); //allow process to complete
      time_elapsed = ra::timing::GetMillisecondsTimer() - timer_start; //evaluate elapsed time again
    }

    //Validate arguments
    ASSERT_TRUE(result_file_found);

    //Read the result file
    std::string result;
    ASSERT_TRUE( ra::filesystem::ReadTextFile(result_file_path, result) );
    ra::strings::Replace(result, ra::environment::GetLineSeparator(), "");
    ra::strings::Replace(result, "\n", "");
    static const std::string EXPECTED_RESULT = "ADMIN";
    ASSERT_EQ(EXPECTED_RESULT, result);

    //cleanup
    ra::filesystem::DeleteFileUtf8(batch_file_path.c_str());
    ra::filesystem::DeleteFileUtf8(result_file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
