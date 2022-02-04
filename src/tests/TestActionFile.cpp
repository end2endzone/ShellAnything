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

#include "TestActionFile.h"
#include "Context.h"
#include "ActionFile.h"
#include "PropertyManager.h"
#include "rapidassist/testing.h"
#include "rapidassist/filesystem_utf8.h"

#include <Windows.h>

namespace shellanything { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestActionFile::SetUp()
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
    pmgr.Clear();
  }
  //--------------------------------------------------------------------------------------------------
  void TestActionFile::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestActionFile, testEncodingUtf8)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    const std::string selected_file_path_utf8 = "C:\\TEMP\\\303\251cole"; //school in french, encoded as utf-8

    //Create a valid context
    Context c;
    Context::ElementList elements;
    elements.push_back(selected_file_path_utf8);
    c.SetElements(elements);

    c.RegisterProperties();

    const std::string separator = ra::filesystem::GetPathSeparatorStr();
    const std::string filename = ra::testing::GetTestQualifiedName() + ".txt";
    const std::string temp_dir = ra::filesystem::GetTemporaryDirectory();
    const std::string path = temp_dir + separator + filename;

    //cleanup
    ra::filesystem::DeleteFileUtf8(path.c_str());

    //execute the action
    ActionFile af;
    af.SetPath(path);
    af.SetEncoding("utf-8");
    af.SetText("${selection.path}");

    bool executed = af.Execute(c);
    ASSERT_TRUE( executed );

    ASSERT_TRUE( ra::filesystem::FileExistsUtf8(path.c_str()) ) << "File not found: " << path;

    const size_t file_size = ra::filesystem::GetFileSize(path.c_str());
    const size_t expected_size = selected_file_path_utf8.size();
    ASSERT_EQ(expected_size, file_size);

    //cleanup
    ra::filesystem::DeleteFileUtf8(path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestActionFile, testEncodingAnsi)
  {
    //validate user's active code page is compatible with U+00E9 character (E with Acute) 
    UINT acp = GetACP();
    bool is_compatible_acp = 
      (acp == 850) || //https://en.wikipedia.org/wiki/Code_page_850
      (acp == 1252);  //https://en.wikipedia.org/wiki/Windows-1252
    if (!is_compatible_acp)
    {
      printf("Skipped. Incompatible active code page: %u\n", acp);
      return;
    }

    PropertyManager & pmgr = PropertyManager::GetInstance();

    const std::string selected_file_path_utf8 = "C:\\TEMP\\\303\251cole"; //school in french, encoded as utf-8

    //Create a valid context
    Context c;
    Context::ElementList elements;
    elements.push_back(selected_file_path_utf8);
    c.SetElements(elements);

    c.RegisterProperties();

    const std::string separator = ra::filesystem::GetPathSeparatorStr();
    const std::string filename = ra::testing::GetTestQualifiedName() + ".txt";
    const std::string temp_dir = ra::filesystem::GetTemporaryDirectory();
    const std::string path = temp_dir + separator + filename;

    //cleanup
    ra::filesystem::DeleteFileUtf8(path.c_str());

    //execute the action
    ActionFile af;
    af.SetPath(path);
    af.SetEncoding("ansi");
    af.SetText("${selection.path}");

    bool executed = af.Execute(c);
    ASSERT_TRUE( executed );

    ASSERT_TRUE( ra::filesystem::FileExistsUtf8(path.c_str()) ) << "File not found: " << path;

    const size_t file_size = ra::filesystem::GetFileSize(path.c_str());
    const size_t expected_size = selected_file_path_utf8.size() - 1; //a character encoded as 2 bytes should be encoded as a single byte.
    ASSERT_EQ(expected_size, file_size);

    //cleanup
    ra::filesystem::DeleteFileUtf8(path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestActionFile, testEncodingUnicode)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    const std::string selected_file_path_utf8 = "C:\\TEMP\\\303\251cole"; //school in french, encoded as utf-8

    //Create a valid context
    Context c;
    Context::ElementList elements;
    elements.push_back(selected_file_path_utf8);
    c.SetElements(elements);

    c.RegisterProperties();

    const std::string separator = ra::filesystem::GetPathSeparatorStr();
    const std::string filename = ra::testing::GetTestQualifiedName() + ".txt";
    const std::string temp_dir = ra::filesystem::GetTemporaryDirectory();
    const std::string path = temp_dir + separator + filename;

    //cleanup
    ra::filesystem::DeleteFileUtf8(path.c_str());

    //execute the action
    ActionFile af;
    af.SetPath(path);
    af.SetEncoding("unicode");
    af.SetText("${selection.path}");

    bool executed = af.Execute(c);
    ASSERT_TRUE( executed );

    ASSERT_TRUE( ra::filesystem::FileExistsUtf8(path.c_str()) ) << "File not found: " << path;

    const size_t file_size = ra::filesystem::GetFileSize(path.c_str());
    const size_t expected_size = (selected_file_path_utf8.size() - 1) * 2; //a character encoded as 2 bytes should be encoded as a single byte and then the whole string should be doubled.
    ASSERT_EQ(expected_size, file_size);

    //cleanup
    ra::filesystem::DeleteFileUtf8(path.c_str());
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
