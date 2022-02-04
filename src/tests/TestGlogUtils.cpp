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

#include "TestGlogUtils.h"
#include "GlogUtils.h"
#include "SaUtils.h"

#include "ErrorManager.h"
#include <Windows.h>
#undef GetEnvironmentVariable
#undef DeleteFile
#undef CreateDirectory
#undef CopyFile
#undef CreateFile

#include "rapidassist/timing.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/errors.h"

static const std::string  EMPTY_STRING;
static const std::wstring EMPTY_WIDE_STRING;

std::string GetLogDirectorySafe()
{
  std::string log_dir = shellanything::GetLogDirectory();
  printf("Using log directory '%s'.\n", log_dir.c_str());

  //verify if the directory is available
  if (ra::filesystem::DirectoryExists(log_dir.c_str()))
    printf("Directory is found.\n");
  else
  {
    printf("WARNING: Directory is NOT found!\n");

    //creating directory
    printf("Creating directory '%s'.\n", log_dir.c_str());

    //Use 'ra::filesystem::CreateDirectory()' instead of 'ra::filesystem::CreateDirectory()' because
    //of RapidAssist issue #27: https://github.com/end2endzone/RapidAssist/issues/27
    bool created = ra::filesystem::CreateDirectory(log_dir.c_str());
    if (created)
      printf("Directory created.\n");
    else
      printf("WARNING: Failed creating directory!\n");

    if (ra::filesystem::DirectoryExists(log_dir.c_str()))
      printf("Directory is found.\n");
  }

  return log_dir;
}

namespace shellanything { namespace test
{
  std::string CreateFakeLog(int level, const std::string & date, const std::string & time, uint32_t process_id)
  {
    std::string log_dir = GetLogDirectorySafe();
    std::string filename = GetLogFilename(level, date, time, process_id);

    std::string path = log_dir + "\\" + filename;

    //create the file
    printf("Creating file '%s'.\n", path.c_str());
    FILE * f = fopen(path.c_str(), "w");
    if (!f)
    {
      printf("Failed creating log file '%s'.\n", path.c_str());
      return "";
    }

    fputs("This is a fake log file!", f);

    fclose(f);
    return path;    
  }

  //--------------------------------------------------------------------------------------------------
  void TestGlogUtils::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestGlogUtils::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGlogUtils, testTestingEnvironment)
  {
    bool testing = IsTestingEnvironment();

    ASSERT_TRUE( testing );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGlogUtils, testValidLogDirectory)
  {
    std::string log_dir = GetLogDirectorySafe();
    printf("Using log directory '%s'.\n", log_dir.c_str());

    //assert a folder was decided
    ASSERT_FALSE( log_dir.empty() );

    //assert the directory can be created
    if (!ra::filesystem::DirectoryExists(log_dir.c_str()))
    {
      ASSERT_TRUE( ra::filesystem::CreateDirectory(log_dir.c_str()) ) << "Failed creating directory '" << log_dir << "'.";
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGlogUtils, testLogDirectoryCreation)
  {
    ASSERT_FALSE( GetLogDirectorySafe().empty() );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGlogUtils, testGetLogDateTime)
  {
    GLOG_DATETIME invalid_dt = GetInvalidLogDateTime();

    //test from filename
    {
      GLOG_DATETIME dt = GetFileDateTime("sa.shellextension-d.dll.MYPCNAME.JohnSmith.log.INFO.20190503-180615.14920.log");

      ASSERT_EQ(2019, dt.year   );
      ASSERT_EQ(  05, dt.month  );
      ASSERT_EQ(  03, dt.day    );
      ASSERT_EQ(  18, dt.hour   );
      ASSERT_EQ(  06, dt.minute );
      ASSERT_EQ(  15, dt.second );
    }

    //test from file path
    {
      GLOG_DATETIME dt = GetFileDateTime("C:\\Users\\JohnSmith\\ShellAnything\\Logs\\sa.shellextension-d.dll.MYPCNAME.JohnSmith.log.INFO.20190503-180615.14920.log");

      ASSERT_EQ(2019, dt.year   );
      ASSERT_EQ(  05, dt.month  );
      ASSERT_EQ(  03, dt.day    );
      ASSERT_EQ(  18, dt.hour   );
      ASSERT_EQ(  06, dt.minute );
      ASSERT_EQ(  15, dt.second );
    }

  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGlogUtils, testCleanup)
  {
    //create old files
    std::string log_dir = GetLogDirectorySafe();
    printf("Using log directory '%s'.\n", log_dir.c_str());

    tm now = ra::timing::GetLocalTime();
    now.tm_year += 1900;
    now.tm_mon += 1; //from [0,11] range to [1,12]

    std::string  past_date = ra::strings::Format("%04d0102", now.tm_year-1);
    std::string today_date = ra::strings::Format("%04d%02d%02d", now.tm_year, now.tm_mon, now.tm_mday);

    //create 3 "old" files
    std::string file1 = CreateFakeLog(0, past_date, "123456", 7890);
    std::string file2 = CreateFakeLog(1, past_date, "123456", 7890);
    std::string file3 = CreateFakeLog(2, past_date, "123456", 7890);
    ASSERT_TRUE( ra::filesystem::FileExists(file1.c_str()) ) << "File not found: '" << file1 << "'.";
    ASSERT_TRUE( ra::filesystem::FileExists(file2.c_str()) ) << "File not found: '" << file2 << "'.";
    ASSERT_TRUE( ra::filesystem::FileExists(file3.c_str()) ) << "File not found: '" << file3 << "'.";
    
    //create an "actual" log file
    std::string file4 = CreateFakeLog(0, today_date, "123456", 4567);
    ASSERT_TRUE( ra::filesystem::FileExists(file4.c_str()) );

    //act
    shellanything::DeletePreviousLogs();

    //assert the "old" files were deleted
    ASSERT_FALSE( ra::filesystem::FileExists(file1.c_str()) );
    ASSERT_FALSE( ra::filesystem::FileExists(file2.c_str()) );
    ASSERT_FALSE( ra::filesystem::FileExists(file3.c_str()) );

    //assert "actual" log file is still found
    ASSERT_TRUE( ra::filesystem::FileExists(file4.c_str()) );

    //cleanup
    ASSERT_TRUE( ra::filesystem::DeleteFile(file4.c_str()) );
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
