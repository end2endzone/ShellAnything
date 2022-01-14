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

#include "TestIcon.h"
#include "Icon.h"
#include "Context.h"
#include "PropertyManager.h"
#include "GlogUtils.h"
#include "rapidassist/filesystem.h"

namespace shellanything { namespace test
{
  int CountString(const std::string & text, const std::string & token)
  {
    int count = 0;
    size_t offset = 0;
    size_t pos = 0;
    do
    {
      pos = text.find(token, offset);
      if (pos != std::string::npos)
      {
        count++;
        offset = pos + 1;
      }
    } while (pos != std::string::npos);
    return count;
  }

  //--------------------------------------------------------------------------------------------------
  void TestIcon::SetUp()
  {
    //delete all previous log to be able to identify the current log file
    static const int MAX_AGE_SECONDS = -1;
    DeletePreviousLogs(MAX_AGE_SECONDS);
  }
  //--------------------------------------------------------------------------------------------------
  void TestIcon::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestIcon, testResolveFileExtensionIcon)
  {
    Icon icon;
    icon.SetFileExtension("txt");
 
    //act
    icon.ResolveFileExtensionIcon();

    ASSERT_TRUE( icon.GetFileExtension().empty() );
    ASSERT_FALSE( icon.GetPath().empty() );
    ASSERT_NE( Icon::INVALID_ICON_INDEX, icon.GetIndex() );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestIcon, testResolveMultipleFileExtensionIcon) // issue #98
  {
    // Solve icon for txt files
    Icon icon_txt;
    icon_txt.SetFileExtension("txt");
    icon_txt.ResolveFileExtensionIcon();
    ASSERT_TRUE(icon_txt.GetFileExtension().empty());
    ASSERT_FALSE(icon_txt.GetPath().empty());
    ASSERT_NE(Icon::INVALID_ICON_INDEX, icon_txt.GetIndex());

    //get the separator for multiple values
    shellanything::PropertyManager & pmgr = shellanything::PropertyManager::GetInstance();
    const std::string separator = pmgr.GetProperty(Context::MULTI_SELECTION_SEPARATOR_PROPERTY_NAME);

    //build a list of multiple file extensions
    std::string file_extensions;
    file_extensions += "txt";
    file_extensions += separator;
    file_extensions += "xml";
    file_extensions += separator;
    file_extensions += "jpg";
    file_extensions += separator;
    file_extensions += "mp3";
    file_extensions += separator;
    file_extensions += "exe";
    file_extensions += separator;
    file_extensions += "dll";
    file_extensions += separator;
    file_extensions += "mp4";

    Icon icon;
    icon.SetFileExtension(file_extensions);

    //act
    icon.ResolveFileExtensionIcon();

    ASSERT_TRUE(icon.GetFileExtension().empty());
    ASSERT_EQ(icon_txt.GetPath(), icon.GetPath());
    ASSERT_EQ(icon_txt.GetIndex(), icon.GetIndex());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestIcon, testMultipleResolveFailures) // issue #98
  {
    static const std::string UNKNOWN_FILE_EXTENSION = "123456789";

    Icon icon;
    icon.SetFileExtension(UNKNOWN_FILE_EXTENSION);

    //act
    icon.ResolveFileExtensionIcon();

    //assert to unknown file type
    static const std::string  UNKNOWN_ICON_FILE_PATH = "C:\\Windows\\system32\\imageres.dll";
    static const int          UNKNOWN_ICON_INDEX = 2;
    ASSERT_TRUE(icon.GetFileExtension().empty());
    ASSERT_EQ(UNKNOWN_ICON_FILE_PATH, icon.GetPath());
    ASSERT_EQ(UNKNOWN_ICON_INDEX,     icon.GetIndex());

    //act (issue #98)
    for (int i = 0; i < 500; i++)
    {
      //this should create multiple log entries if the feature is not properly implemented.
      Icon tmp_icon;
      tmp_icon.SetFileExtension(UNKNOWN_FILE_EXTENSION);
      tmp_icon.ResolveFileExtensionIcon();
    }

    //get log files content
    std::string log_dir = GetLogDirectory();
    ra::strings::StringVector files;
    bool find_success = ra::filesystem::FindFiles(files, log_dir.c_str());
    ASSERT_TRUE(find_success);
    for (size_t i = 0; i < files.size(); i++)
    {
      const std::string & path = files[i];
      if (IsLogFile(path))
      {
        std::string content;
        bool read = ra::filesystem::ReadTextFile(path, content);
        ASSERT_TRUE(read) << "Failed to read log file: " << path;

        int count = CountString(content, UNKNOWN_FILE_EXTENSION);
        ASSERT_LE(count, 1) << "Log file '" << path << "' contains " << count << " references to '" << UNKNOWN_FILE_EXTENSION << "'.";
      }
    }
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
