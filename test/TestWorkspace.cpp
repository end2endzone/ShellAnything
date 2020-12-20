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

#include "TestWorkspace.h"
#include "Workspace.h"
#include "rapidassist/process_utf8.h"
#include "rapidassist/filesystem_utf8.h"

namespace shellanything { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestWorkspace::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestWorkspace::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWorkspace, testDefaultBaseDirectory)
  {
    Workspace ws;

    std::string base_dir = ws.GetBaseDirectory();

    //If the automatic workspace created failed to create, the base directory will be empty.
    ASSERT_FALSE(base_dir.empty());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWorkspace, testCleanup)
  {
    Workspace ws;

    std::string base_dir = ws.GetBaseDirectory();
    ASSERT_FALSE(base_dir.empty());

    //Should be able to cleanup the directory
    ASSERT_TRUE(ws.Cleanup());

    //After cleanup, the base directory should be empty/invalid
    ASSERT_TRUE(ws.GetBaseDirectory().empty());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWorkspace, testGetFullPathUtf8)
  {
    Workspace ws;
    const std::string base_dir = ws.GetBaseDirectory();

    ASSERT_EQ(base_dir + "", ws.GetFullPathUtf8(""));
    ASSERT_EQ(base_dir + "\\friends.txt", ws.GetFullPathUtf8("friends.txt"));
    ASSERT_EQ(base_dir + "\\friends.txt", ws.GetFullPathUtf8("\\friends.txt"));
    ASSERT_EQ(base_dir + "\\countries\\canada\\image001.jpg", ws.GetFullPathUtf8("countries\\canada\\image001.jpg"));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWorkspace, testImportFileUtf8)
  {
    Workspace ws;
    const std::string base_dir = ws.GetBaseDirectory();

    //define the path of a local file
    std::string install_dir = ra::process::GetCurrentProcessDir();
    std::string source = install_dir + "\\test_files\\samples.xml";

    ASSERT_TRUE(ra::filesystem::FileExistsUtf8(source.c_str()));

    const std::string target_dir = "foo";
    bool imported = ws.ImportFileUtf8(source.c_str(), target_dir.c_str());
    ASSERT_TRUE(imported);

    //Search for the imported file
    const std::string target_path = target_dir + "\\samples.xml";
    const std::string absolute_path = ws.GetFullPathUtf8(target_path.c_str());
    ASSERT_TRUE(ra::filesystem::FileExistsUtf8(absolute_path.c_str())) << "File '" << absolute_path << "' is not found!";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWorkspace, testImportAndRenameFileUtf8)
  {
    Workspace ws;
    const std::string base_dir = ws.GetBaseDirectory();

    //define the path of a local file
    std::string install_dir = ra::process::GetCurrentProcessDir();
    std::string source = install_dir + "\\test_files\\samples.xml";

    ASSERT_TRUE(ra::filesystem::FileExistsUtf8(source.c_str()));

    const std::string relative_path = "foo\\bar.xml";
    bool imported = ws.ImportAndRenameFileUtf8(source.c_str(), relative_path.c_str());
    ASSERT_TRUE(imported);

    //Search for the imported file
    const std::string absolute_path = ws.GetFullPathUtf8(relative_path.c_str());
    ASSERT_TRUE(ra::filesystem::FileExistsUtf8(absolute_path.c_str())) << "File '" << absolute_path << "' is not found!";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWorkspace, testImportDirectoryUtf8)
  {
    Workspace ws;
    const std::string base_dir = ws.GetBaseDirectory();

    //define the path of a local file
    std::string install_dir = ra::process::GetCurrentProcessDir();
    std::string source = install_dir + "\\test_files";

    ASSERT_TRUE(ra::filesystem::DirectoryExistsUtf8(source.c_str()));

    const std::string relative_path = "foo\\bar";
    bool imported = ws.ImportDirectoryUtf8(source.c_str(), relative_path.c_str());
    ASSERT_TRUE(imported);

    //Search for the imported directory
    const std::string absolute_path = ws.GetFullPathUtf8(relative_path.c_str());
    ASSERT_TRUE(ra::filesystem::DirectoryExistsUtf8(absolute_path.c_str()));

    //Search for sub files 
    static const char * files[] = {
      "foo\\bar\\test_files\\issue036.xml",
      "foo\\bar\\test_files\\issue040.xml",
      "foo\\bar\\test_files\\issue041.xml",
      "foo\\bar\\test_files\\issue044.xml",
      "foo\\bar\\test_files\\samples.xml" ,
    };
    static const size_t num_files = sizeof(files)/sizeof(files[0]);
    for(size_t i=0; i<num_files; i++)
    {
      const std::string absolute_path = ws.GetFullPathUtf8(files[i]);
      ASSERT_TRUE(ra::filesystem::FileExistsUtf8(absolute_path.c_str())) << "File '" << absolute_path << "' is not found!";
    }
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
