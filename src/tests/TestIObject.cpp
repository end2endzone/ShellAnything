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

#include "TestIObject.h"
#include "ConfigManager.h"
#include "ConfigFile.h"

#include "rapidassist/strings.h"
#include "rapidassist/testing.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/environment.h"
#include "rapidassist/timing.h"

#include "Workspace.h"
#include "QuickLoader.h"

namespace shellanything
{
  namespace test
  {
    inline std::string SanitizeWord(std::string& value);

    bool IsInstanceAddress(const std::string& value)
    {
      // smalest address accepted would be 0x1234
      if (value.size() <= 6)
        return false;
      // check prefix
      bool prefix_valid = (value[0] == '0' && value[1] == 'x');
      if (!prefix_valid)
        return false;
      return true;
    }

    bool IsConfigFilePath(const std::string& value)
    {
      // Check that first letter is a drive letter
      if (!value.empty())
      {
        char drive_letter = value[0];
        bool valid = (
          ('a' <= drive_letter && drive_letter <= 'z') ||
          ('A' <= drive_letter && drive_letter <= 'Z')
          );
        if (!valid)
          return false;
      }

      // Check for an absolute path
      if (value.size() >= 3)
      {
        bool is_absolute_path = (value[1] == ':' && value[2] == '\\');
        if (!is_absolute_path)
          return false;
      }

      std::string tmp_copy = value;
      SanitizeWord(tmp_copy);

      // Check file extension
      std::string file_extension = ra::filesystem::GetFileExtention(tmp_copy);
      file_extension = ra::strings::Uppercase(file_extension);
      if (file_extension != "XML")
        return false;

      return true;
    }

    inline std::string SanitizeWord(std::string& value)
    {
      // Sanitize endding of word. Words are split by ' ' but sometime end with ',' character
      std::string output = ra::strings::TrimRight(value, ',');
      return output;
    }

    void SanatizeAddresses(std::string& value)
    {
      // Cleanup the input string
      std::string tmp_copy = value;
      ra::strings::Replace(tmp_copy, "\r", "");

      // Split by line
      ra::strings::StringVector lines = ra::strings::Split(tmp_copy, "\n");

      ra::strings::StringVector addresses;

      // Search each line
      for (size_t i = 0; i < lines.size(); i++)
      {
        const std::string& line = lines[i];

        // Split by words
        ra::strings::StringVector words = ra::strings::Split(line, " ");

        // Flush empty words at the beginning of a line
        while (words.size() > 0 && words[0].empty())
        {
          words.erase(words.begin());
        }

        // Skip the first word, it is expected to be a class name
        if (words.size() > 0)
          words.erase(words.begin());

        // Check format of second word.
        // That should be our instance value
        if (words.size() > 0)
        {
          std::string second_word_in_line = words[0];

          // Sanitize word before check.
          second_word_in_line = SanitizeWord(second_word_in_line);

          if (IsInstanceAddress(second_word_in_line))
            addresses.push_back(second_word_in_line);
        }
      }

      // Proceed with the search and replace in given value
      for (size_t i = 0; i < addresses.size(); i++)
      {
        const std::string& addr = addresses[i];
        ra::strings::Replace(value, addr, "0xdeadbeef");
      }
    }

    void SanatizeConfigPaths(std::string& value)
    {
      // Cleanup the input string
      std::string tmp_copy = value;
      ra::strings::Replace(tmp_copy, "\r", "");

      // Split by line
      ra::strings::StringVector lines = ra::strings::Split(tmp_copy, "\n");

      ra::strings::StringVector paths;

      // Search each line
      for (size_t i = 0; i < lines.size(); i++)
      {
        const std::string& line = lines[i];

        // Split by words
        ra::strings::StringVector words = ra::strings::Split(line, " ");

        // Flush empty words at the beginning of a line
        while (words.size() > 0 && words[0].empty())
        {
          words.erase(words.begin());
        }

        // Check first word
        std::string first_word;
        if (words.size() > 0)
        {
          first_word = words[0];
        }
        if (first_word != "ConfigFile")
          continue; //next line

        // Skip the first word, it is expected to be a class name
        if (words.size() > 0)
          words.erase(words.begin());

        // Skip the second word, it is the instance address
        if (words.size() > 0)
          words.erase(words.begin());

        // Check format of third word.
        // That should be our ConfigFile path
        if (words.size() > 0)
        {
          std::string third_word_in_line = words[0];

          // Sanitize word before check.
          third_word_in_line = SanitizeWord(third_word_in_line);

          if (IsConfigFilePath(third_word_in_line))
            paths.push_back(third_word_in_line);
        }
      }

      // Proceed with the search and replace in given value
      for (size_t i = 0; i < paths.size(); i++)
      {
        const std::string& path = paths[i];
        ra::strings::Replace(value, path, "C:\\temp\\foobar.xml");
      }
    }


    //--------------------------------------------------------------------------------------------------
    void TestIObject::SetUp()
    {
      //Delete the configurations which source files are deleted
      ConfigManager& cmgr = ConfigManager::GetInstance();
      cmgr.Refresh();

      //Delete the source file of all remaining Configuration instance
      ConfigFile::ConfigFilePtrList configs = cmgr.GetConfigFiles();
      for (size_t i = 0; i < configs.size(); i++)
      {
        ConfigFile* config = configs[i];
        if (config)
        {
          const std::string& file_path = config->GetFilePath();
          ASSERT_TRUE(ra::filesystem::DeleteFile(file_path.c_str())) << "Failed deleting file '" << file_path << "'.";
        }
      }

      //Now that all configuration files are deleted, refresh again
      cmgr.Refresh();

      //ASSERT that no files are loaded
      ASSERT_EQ(0, cmgr.GetConfigFiles().size());
    }
    //--------------------------------------------------------------------------------------------------
    void TestIObject::TearDown()
    {
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestIObject, testToLongString)
    {
      ConfigManager& cmgr = ConfigManager::GetInstance();

      //Creating a temporary workspace for the test execution.
      Workspace workspace;
      ASSERT_FALSE(workspace.GetBaseDirectory().empty());
      ASSERT_TRUE(workspace.IsEmpty());

      //Import the required files into the workspace
      static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
      std::string test_name = ra::testing::GetTestQualifiedName();
      std::string template_source_path = std::string("test_files") + path_separator + test_name + ".xml";
      ASSERT_TRUE(workspace.ImportFileUtf8(template_source_path.c_str()));

      //Wait to make sure that the next file copy/modification will not have the same timestamp
      ra::timing::Millisleep(1500);

      //Setup ConfigManager to read files from workspace
      cmgr.ClearSearchPath();
      cmgr.AddSearchPath(workspace.GetBaseDirectory());
      cmgr.Refresh();

      //ASSERT the file is loaded
      ConfigFile::ConfigFilePtrList configs = cmgr.GetConfigFiles();
      ASSERT_EQ(1, configs.size());

      //Create a valid context
      SelectionContext c;
      StringList elements;
      elements.push_back("C:\\Windows\\System32\\calc.exe");
      c.SetElements(elements);

      c.RegisterProperties();

      //Force a full update based on the selection
      cmgr.Update(c);

      //Get actual
      std::string longstring_actual;
      cmgr.ToLongString(longstring_actual, 0);
      ra::strings::Replace(longstring_actual, "\r\n", "\n"); // Replace CRLF by LF to display properly in GoogleTest diff view

      //Get expected long string from file
      std::string expected_source_path = std::string("test_files") + path_separator + test_name + ".expected.txt";
      ASSERT_TRUE(ra::filesystem::FileExists(expected_source_path.c_str())) << "File '" << expected_source_path << "' does not exists!";
      std::string longstring_expected;
      bool file_read = ra::filesystem::ReadFile(expected_source_path, longstring_expected);
      ASSERT_TRUE(file_read) << "Unable to read file '" << expected_source_path << "'";
      ra::strings::Replace(longstring_expected, "\r\n", "\n"); // Replace CRLF by LF to display properly in GoogleTest diff view

      //Filter out addresses instance and config file path from both strings to allow compare
      SanatizeAddresses(longstring_actual);
      SanatizeAddresses(longstring_expected);
      SanatizeConfigPaths(longstring_actual);
      SanatizeConfigPaths(longstring_expected);

      //Do the actual comparison
      ASSERT_EQ(longstring_expected, longstring_actual);

      //Cleanup
      ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
    }
    //--------------------------------------------------------------------------------------------------

  } //namespace test
} //namespace shellanything
