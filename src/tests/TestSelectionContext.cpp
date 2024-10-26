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

#include "TestSelectionContext.h"
#include "App.h"
#include "SelectionContext.h"
#include "PropertyManager.h"
#include "SaUtils.h"

#include "rapidassist/process.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/testing.h"
#include "rapidassist/errors.h"

#include "LockFile.h"

namespace shellanything
{
  namespace test
  {
    size_t CountLines(const std::string& value)
    {
      size_t count = 0;
      if (!value.empty())
        count = 1;

      for (size_t i = 0; i < value.size(); i++)
      {
        if (value[i] == '\n')
          count++;
      }

      return count;
    }

    //--------------------------------------------------------------------------------------------------
    void TestSelectionContext::SetUp()
    {
    }
    //--------------------------------------------------------------------------------------------------
    void TestSelectionContext::TearDown()
    {
      // Force default properties back to their default values.
      PropertyManager& pmgr = PropertyManager::GetInstance();
      pmgr.Clear();
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSelectionContext, testDefaults)
    {
      SelectionContext c;
      ASSERT_EQ(0, c.GetNumFiles());
      ASSERT_EQ(0, c.GetNumDirectories());
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSelectionContext, testFileDirectoryAutoDetect)
    {
      SelectionContext c;
#ifdef _WIN32
      {
        StringList elements;
        elements.push_back("C:\\Windows\\System32\\kernel32.dll");
        elements.push_back("C:\\Windows\\System32\\cmd.exe");
        elements.push_back("C:\\Windows\\System32\\notepad.exe");
        elements.push_back("C:\\Windows\\System32\\services.msc");
        elements.push_back("C:\\Program Files");
        elements.push_back("C:\\Users");
        elements.push_back("C:\\Windows");
        c.SetElements(elements);
      }
#else
      //TODO: complete with known path to files
#endif

      ASSERT_EQ(4, c.GetNumFiles());
      ASSERT_EQ(3, c.GetNumDirectories());
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSelectionContext, testCopy)
    {
      SelectionContext c;
#ifdef _WIN32
      {
        StringList elements;
        elements.push_back("C:\\Windows\\System32\\kernel32.dll");
        elements.push_back("C:\\Windows\\System32\\cmd.exe");
        elements.push_back("C:\\Windows\\System32\\notepad.exe");
        elements.push_back("C:\\Windows\\System32\\services.msc");
        elements.push_back("C:\\Program Files");
        elements.push_back("C:\\Users");
        elements.push_back("C:\\Windows");
        c.SetElements(elements);
      }
#else
      //TODO: complete with known path to files
#endif

      ASSERT_EQ(4, c.GetNumFiles());
      ASSERT_EQ(3, c.GetNumDirectories());

      //operator =
      SelectionContext c2;
      c2 = c;

      //assert both equals
      ASSERT_EQ(c.GetElements().size(), c2.GetElements().size());
      ASSERT_EQ(c.GetNumFiles(), c2.GetNumFiles());
      ASSERT_EQ(c.GetNumDirectories(), c2.GetNumDirectories());
      for (size_t i = 0; i < c.GetElements().size(); i++)
      {
        const std::string& c1_element = c.GetElements()[i];
        const std::string& c2_element = c2.GetElements()[i];
        ASSERT_EQ(c1_element, c2_element);
      }

      //copy constructor
      SelectionContext c3(c);

      //assert both equals
      ASSERT_EQ(c.GetElements().size(), c3.GetElements().size());
      ASSERT_EQ(c.GetNumFiles(), c3.GetNumFiles());
      ASSERT_EQ(c.GetNumDirectories(), c3.GetNumDirectories());
      for (size_t i = 0; i < c.GetElements().size(); i++)
      {
        const std::string& c1_element = c.GetElements()[i];
        const std::string& c3_element = c3.GetElements()[i];
        ASSERT_EQ(c1_element, c3_element);
      }

    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSelectionContext, testRegisterUnregisterProperties)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();

      SelectionContext context;
#ifdef _WIN32
      {
        StringList elements;
        elements.push_back("C:\\Windows\\System32\\notepad.exe");
        context.SetElements(elements);
      }
#else
      //TODO: complete with known path to files
#endif

      ASSERT_FALSE(pmgr.HasProperty("selection.path"));

      //act
      context.RegisterProperties();

      //assert
      ASSERT_TRUE(pmgr.HasProperty("selection.path"));

      //act
      context.UnregisterProperties();

      //assert
      ASSERT_FALSE(pmgr.HasProperty("selection.path"));
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSelectionContext, testRegisterPropertiesSingleFile)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();

      SelectionContext context;
#ifdef _WIN32
      {
        StringList elements;
        elements.push_back("C:\\Windows\\System32\\notepad.exe");
        context.SetElements(elements);
      }
#else
      //TODO: complete with known path to files
#endif

      ASSERT_FALSE(pmgr.HasProperty("selection.path"));

      //act
      context.RegisterProperties();

      //assert
      ASSERT_TRUE(pmgr.HasProperty("selection.path"));

      const char* test_string = ""
        "selection.path"                "=${selection.path}"                "\n"
        "selection.dir"                 "=${selection.dir}"                 "\n"
        "selection.parent.path"         "=${selection.parent.path}"         "\n"
        "selection.parent.filename"     "=${selection.parent.filename}"     "\n"
        "selection.filename"            "=${selection.filename}"            "\n"
        "selection.filename.noext"      "=${selection.filename.noext}"      "\n"
        "selection.filename.extension"  "=${selection.filename.extension}"  "\n";

      const char* expected_string = ""
        "selection.path"                "=C:\\Windows\\System32\\notepad.exe\n"
        "selection.dir"                 "=C:\\Windows\\System32\n"
        "selection.parent.path"         "=C:\\Windows\\System32\n"
        "selection.parent.filename"     "=System32\n"
        "selection.filename"            "=notepad.exe\n"
        "selection.filename.noext"      "=notepad\n"
        "selection.filename.extension"  "=exe\n";

      std::string actual_string = pmgr.Expand(test_string);

      ASSERT_EQ(std::string(expected_string), actual_string);

      context.UnregisterProperties();
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSelectionContext, testRegisterPropertiesSingleDirectory)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();

      SelectionContext context;
#ifdef _WIN32
      {
        StringList elements;
        elements.push_back("C:\\");
        context.SetElements(elements);
      }
#else
      //TODO: complete with known path to files
#endif

      ASSERT_FALSE(pmgr.HasProperty("selection.dir.count"));
      ASSERT_FALSE(pmgr.HasProperty("selection.dir.empty"));

      //act
      context.RegisterProperties();

      //assert
      ASSERT_TRUE(pmgr.HasProperty("selection.dir.count"));
      ASSERT_TRUE(pmgr.HasProperty("selection.dir.empty"));

      std::string selection_dir_count = pmgr.Expand("${selection.dir.count}");
      std::string selection_dir_empty = pmgr.Expand("${selection.dir.empty}");

      ASSERT_TRUE(ra::strings::IsNumeric(selection_dir_count.c_str()));
      ASSERT_EQ(std::string("false"), selection_dir_empty);

      context.UnregisterProperties();
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSelectionContext, testRegisterPropertiesEmptyDirectory)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();

      //create an empty directory
      std::string temp_dir = ra::filesystem::GetTemporaryDirectory();
      std::string empty_dir = temp_dir + ra::testing::GetTestQualifiedName();
      ASSERT_TRUE(ra::filesystem::CreateDirectory(empty_dir.c_str()));

      SelectionContext context;
      StringList elements;
      elements.push_back(empty_dir);
      context.SetElements(elements);

      ASSERT_FALSE(pmgr.HasProperty("selection.dir.count"));
      ASSERT_FALSE(pmgr.HasProperty("selection.dir.empty"));

      //act
      context.RegisterProperties();

      //assert
      ASSERT_TRUE(pmgr.HasProperty("selection.dir.count"));
      ASSERT_TRUE(pmgr.HasProperty("selection.dir.empty"));

      std::string selection_dir_count = pmgr.Expand("${selection.dir.count}");
      std::string selection_dir_empty = pmgr.Expand("${selection.dir.empty}");

      ASSERT_EQ(std::string("0"), selection_dir_count);
      ASSERT_EQ(std::string("true"), selection_dir_empty);

      context.UnregisterProperties();

      //cleanup
      ra::filesystem::DeleteDirectory(empty_dir.c_str());
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSelectionContext, testRegisterPropertiesMultipleFiles)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();
      pmgr.Clear();

      SelectionContext context;
#ifdef _WIN32
      {
        StringList elements;
        elements.push_back("C:\\Windows\\System32\\kernel32.dll");
        elements.push_back("C:\\Windows\\System32\\cmd.exe");
        elements.push_back("C:\\Windows\\System32\\notepad.exe");
        elements.push_back("C:\\Windows\\System32\\services.msc");
        context.SetElements(elements);
      }
#else
      //TODO: complete with known path to files
#endif

      ASSERT_FALSE(pmgr.HasProperty("selection.path"));

      //act
      context.RegisterProperties();

      //assert
      ASSERT_TRUE(pmgr.HasProperty("selection.path"));

      //get all expanded properties
      std::string selection_path = pmgr.Expand("${selection.path}");
      std::string selection_dir = pmgr.Expand("${selection.dir}");
      std::string selection_parent_path = pmgr.Expand("${selection.parent.path}");
      std::string selection_parent_filename = pmgr.Expand("${selection.parent.filename}");
      std::string selection_filename = pmgr.Expand("${selection.filename}");
      std::string selection_filename_noext = pmgr.Expand("${selection.filename.noext}");
      std::string selection_filename_ext = pmgr.Expand("${selection.filename.extension}");

      //assert 1 line per element (file or directory) in context
      ASSERT_EQ(context.GetElements().size(), CountLines(selection_path));
      ASSERT_EQ(context.GetElements().size(), CountLines(selection_dir));
      ASSERT_EQ(context.GetElements().size(), CountLines(selection_parent_path));
      ASSERT_EQ(context.GetElements().size(), CountLines(selection_parent_filename));
      ASSERT_EQ(context.GetElements().size(), CountLines(selection_filename));
      ASSERT_EQ(context.GetElements().size(), CountLines(selection_filename_noext));
      ASSERT_EQ(context.GetElements().size(), CountLines(selection_filename_ext));

      //assert all elements are added to the ${selection.path}
      for (size_t i = 0; i < context.GetElements().size(); i++)
      {
        const std::string& file_element = context.GetElements()[i];
        bool file_element_found = (selection_path.find(file_element) != std::string::npos);
        ASSERT_TRUE(file_element_found) << "selection_path=" << selection_path << "\n" << "file_element=" << file_element;
      }
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSelectionContext, testCustomMultiSelectionSeparator)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();
      pmgr.Clear();

      // Force 'selection.path' and other selection based properties to be separated by a ',' character.
      // All properties should be single-line.
      pmgr.SetProperty(SelectionContext::MULTI_SELECTION_SEPARATOR_PROPERTY_NAME, ",");

      SelectionContext context;
#ifdef _WIN32
      {
        StringList elements;
        elements.push_back("C:\\Windows\\System32\\kernel32.dll");
        elements.push_back("C:\\Windows\\System32\\cmd.exe");
        elements.push_back("C:\\Windows\\System32\\notepad.exe");
        elements.push_back("C:\\Windows\\System32\\services.msc");
        context.SetElements(elements);
      }
#else
      //TODO: complete with known path to files
#endif

      ASSERT_FALSE(pmgr.HasProperty("selection.path"));

      //act
      context.RegisterProperties();

      //assert
      ASSERT_TRUE(pmgr.HasProperty("selection.path"));

      //get all expanded properties
      std::string selection_path = pmgr.Expand("${selection.path}");
      std::string selection_dir = pmgr.Expand("${selection.dir}");
      std::string selection_parent_path = pmgr.Expand("${selection.parent.path}");
      std::string selection_parent_filename = pmgr.Expand("${selection.parent.filename}");
      std::string selection_filename = pmgr.Expand("${selection.filename}");
      std::string selection_filename_noext = pmgr.Expand("${selection.filename.noext}");
      std::string selection_filename_ext = pmgr.Expand("${selection.filename.extension}");

      //assert 1 line for each properties
      ASSERT_EQ(1, CountLines(selection_path));
      ASSERT_EQ(1, CountLines(selection_dir));
      ASSERT_EQ(1, CountLines(selection_parent_path));
      ASSERT_EQ(1, CountLines(selection_parent_filename));
      ASSERT_EQ(1, CountLines(selection_filename));
      ASSERT_EQ(1, CountLines(selection_filename_noext));
      ASSERT_EQ(1, CountLines(selection_filename_ext));

      //assert specific values of each property
      ASSERT_EQ("C:\\Windows\\System32\\kernel32.dll,C:\\Windows\\System32\\cmd.exe,C:\\Windows\\System32\\notepad.exe,C:\\Windows\\System32\\services.msc", selection_path);
      ASSERT_EQ("C:\\Windows\\System32,C:\\Windows\\System32,C:\\Windows\\System32,C:\\Windows\\System32", selection_dir);
      ASSERT_EQ("C:\\Windows\\System32,C:\\Windows\\System32,C:\\Windows\\System32,C:\\Windows\\System32", selection_parent_path);
      ASSERT_EQ("System32,System32,System32,System32", selection_parent_filename);
      ASSERT_EQ("kernel32.dll,cmd.exe,notepad.exe,services.msc", selection_filename);
      ASSERT_EQ("kernel32,cmd,notepad,services", selection_filename_noext);
      ASSERT_EQ("dll,exe,exe,msc", selection_filename_ext);
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSelectionContext, testSelectionDrive)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();
      pmgr.Clear();

      // Force 'selection.path' and other selection based properties to be separated by a ',' character.
      // This is easier for testing
      pmgr.SetProperty(SelectionContext::MULTI_SELECTION_SEPARATOR_PROPERTY_NAME, ",");

      SelectionContext context;
#ifdef _WIN32
      {
        StringList elements;
        elements.push_back("c:\\windows\\system32\\kernel32.dll");
        elements.push_back("C:\\Windows\\System32\\cmd.exe");
        elements.push_back("C:\\Windows\\System32\\notepad.exe");
        context.SetElements(elements);
      }
#else
      //TODO: complete with known path to files
#endif

      ASSERT_FALSE(pmgr.HasProperty("selection.drive.path"));
      ASSERT_FALSE(pmgr.HasProperty("selection.drive.letter"));

      //act
      context.RegisterProperties();

      //assert
      ASSERT_TRUE(pmgr.HasProperty("selection.drive.path"));
      ASSERT_TRUE(pmgr.HasProperty("selection.drive.letter"));

      ASSERT_EQ("c:\\,C:\\,C:\\", pmgr.GetProperty("selection.drive.path"));
      ASSERT_EQ("c,C,C", pmgr.GetProperty("selection.drive.letter"));
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSelectionContext, testSelectionDriveNetwork)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();
      pmgr.Clear();

      // Force 'selection.path' and other selection based properties to be separated by a ',' character.
      // This is easier for testing
      pmgr.SetProperty(SelectionContext::MULTI_SELECTION_SEPARATOR_PROPERTY_NAME, ",");

      SelectionContext context;
#ifdef _WIN32
      {
        StringList elements;
        elements.push_back("\\\\localhost\\public\\foo.dat");
        elements.push_back("\\\\localhost\\public\\bar.dat");
        context.SetElements(elements);
      }
#else
      //TODO: complete with known path to files
#endif

      ASSERT_FALSE(pmgr.HasProperty("selection.drive.path"));
      ASSERT_FALSE(pmgr.HasProperty("selection.drive.letter"));

      //act
      context.RegisterProperties();

      //assert
      ASSERT_TRUE(pmgr.HasProperty("selection.drive.path"));
      ASSERT_TRUE(pmgr.HasProperty("selection.drive.letter"));

      ASSERT_EQ("", pmgr.GetProperty("selection.drive.path"));
      ASSERT_EQ("", pmgr.GetProperty("selection.drive.letter"));
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSelectionContext, testFileMagicPropertiesExe)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();

      SelectionContext context;
#ifdef _WIN32
      {
        StringList elements;
        elements.push_back(ra::process::GetCurrentProcessPath());
        context.SetElements(elements);
      }
#else
      //TODO: complete with known path to files
#endif

      ASSERT_FALSE(pmgr.HasProperty("selection.mimetype"));
      ASSERT_FALSE(pmgr.HasProperty("selection.description"));
      ASSERT_FALSE(pmgr.HasProperty("selection.charset"));
      //ASSERT_FALSE(pmgr.HasProperty("selection.libmagic_ext"));

      //act
      context.RegisterProperties();

      const char* test_string = ""
        "selection.mimetype"                "=${selection.mimetype}"      "\n"
        "selection.description"             "=${selection.description}"   "\n"
        "selection.charset"                 "=${selection.charset}"       "\n";
      //"selection.libmagic_ext"          "=${selection.extension}"     "\n"

      const char* expected_string = ""
        "selection.mimetype"                "=application/x-dosexec"                                  "\n"
        "selection.description"             "=PE32+ executable (console) x86-64, for MS Windows"      "\n"
        "selection.charset"                 "=binary"                                                 "\n";
      //"selection.libmagic_ext"          "=exe/com"                                                "\n"

    //act
      std::string actual_string = pmgr.Expand(test_string);

      ASSERT_EQ(std::string(expected_string), actual_string);

      context.UnregisterProperties();
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSelectionContext, testFileMagicPropertiesXml)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();

      const std::string install_dir = shellanything::App::GetInstallDirectory();

      SelectionContext context;
#ifdef _WIN32
      {
        StringList elements;
        elements.push_back(install_dir + "\\resources\\configurations\\default.xml");
        context.SetElements(elements);
      }
#else
      //TODO: complete with known path to files
#endif

      ASSERT_FALSE(pmgr.HasProperty("selection.mimetype"));
      ASSERT_FALSE(pmgr.HasProperty("selection.description"));
      ASSERT_FALSE(pmgr.HasProperty("selection.charset"));
      //ASSERT_FALSE(pmgr.HasProperty("selection.libmagic_ext"));

      //act
      context.RegisterProperties();

      const char* test_string = ""
        "selection.mimetype"                "=${selection.mimetype}"      "\n"
        "selection.description"             "=${selection.description}"   "\n"
        "selection.charset"                 "=${selection.charset}"       "\n";
      //"selection.libmagic_ext"          "=${selection.libmagic_ext}"  "\n"

      const char* expected_string = ""
        "selection.mimetype"                "=text/xml"                                                           "\n"
        "selection.description"             "=XML 1.0 document, UTF-8 Unicode text, with CRLF line terminators"   "\n"
        "selection.charset"                 "=utf-8"                                                              "\n";
      //"selection.libmagic_ext"          "=???"                                                "\n"

      //act
      std::string actual_string = pmgr.Expand(test_string);

      ASSERT_EQ(std::string(expected_string), actual_string);

      context.UnregisterProperties();
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSelectionContext, testFileMagicPropertiesEmpty)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();

      //create an empty file in a temporary directory for this test
      std::string temp_dir = ra::filesystem::GetTemporaryDirectory();
      std::string test_dir = temp_dir + ra::filesystem::GetPathSeparatorStr() + ra::testing::GetTestQualifiedName();
      std::string test_file = test_dir + ra::filesystem::GetPathSeparatorStr() + "emptyfile";
      ASSERT_TRUE(ra::filesystem::CreateDirectory(test_dir.c_str()));
      ASSERT_TRUE(ra::testing::CreateFile(test_file.c_str(), 0));

      SelectionContext context;
#ifdef _WIN32
      {
        StringList elements;
        elements.push_back(test_file);
        context.SetElements(elements);
      }
#else
      //TODO: complete with known path to files
#endif

      ASSERT_FALSE(pmgr.HasProperty("selection.mimetype"));
      ASSERT_FALSE(pmgr.HasProperty("selection.description"));
      ASSERT_FALSE(pmgr.HasProperty("selection.charset"));
      //ASSERT_FALSE(pmgr.HasProperty("selection.libmagic_ext"));

      //act
      context.RegisterProperties();

      const char* test_string = ""
        "selection.mimetype"                "=${selection.mimetype}"      "\n"
        "selection.description"             "=${selection.description}"   "\n"
        "selection.charset"                 "=${selection.charset}"       "\n";
      //"selection.libmagic_ext"          "=${selection.libmagic_ext}"  "\n"

      const char* expected_string = ""
        "selection.mimetype"                "=inode/x-empty"    "\n"
        "selection.description"             "=empty"            "\n"
        "selection.charset"                 "=binary"           "\n";

      //act
      std::string actual_string = pmgr.Expand(test_string);

      ASSERT_EQ(std::string(expected_string), actual_string);

      context.UnregisterProperties();

      //cleanup
      ra::filesystem::DeleteDirectory(test_dir.c_str());
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSelectionContext, testFileMagicPropertiesUnknown)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();

      //create a "data" file in a temporary directory for this test
      std::string temp_dir = ra::filesystem::GetTemporaryDirectory();
      std::string test_dir = temp_dir + ra::filesystem::GetPathSeparatorStr() + ra::testing::GetTestQualifiedName();
      std::string test_file = test_dir + ra::filesystem::GetPathSeparatorStr() + "datafile";
      ASSERT_TRUE(ra::filesystem::CreateDirectory(test_dir.c_str()));
      ASSERT_TRUE(ra::testing::CreateFile(test_file.c_str(), 293)); //293 is an random number grater than 255 so that all possible bytes exists in the file

      SelectionContext context;
#ifdef _WIN32
      {
        StringList elements;
        elements.push_back(test_file);
        context.SetElements(elements);
      }
#else
      //TODO: complete with known path to files
#endif

      ASSERT_FALSE(pmgr.HasProperty("selection.mimetype"));
      ASSERT_FALSE(pmgr.HasProperty("selection.description"));
      ASSERT_FALSE(pmgr.HasProperty("selection.charset"));
      //ASSERT_FALSE(pmgr.HasProperty("selection.libmagic_ext"));

      //act
      context.RegisterProperties();

      const char* test_string = ""
        "selection.mimetype"                "=${selection.mimetype}"      "\n"
        "selection.description"             "=${selection.description}"   "\n"
        "selection.charset"                 "=${selection.charset}"       "\n";
      //"selection.libmagic_ext"          "=${selection.libmagic_ext}"  "\n"

      const char* expected_string = ""
        "selection.mimetype"                "=application/octet-stream"   "\n"
        "selection.description"             "=data"                       "\n"
        "selection.charset"                 "=binary"                     "\n";

      //act
      std::string actual_string = pmgr.Expand(test_string);

      ASSERT_EQ(std::string(expected_string), actual_string);

      context.UnregisterProperties();

      //cleanup
      ra::filesystem::DeleteDirectory(test_dir.c_str());
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSelectionContext, testFileMagicPropertiesFileLockedByFopen)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();

      //create a "data" file in a temporary directory for this test
      std::string temp_dir = ra::filesystem::GetTemporaryDirectory();
      std::string test_dir = temp_dir + ra::filesystem::GetPathSeparatorStr() + ra::testing::GetTestQualifiedName();
      std::string test_file = test_dir + ra::filesystem::GetPathSeparatorStr() + "locked-by-fsopen";
      ASSERT_TRUE(ra::filesystem::CreateDirectory(test_dir.c_str()));
      ASSERT_TRUE(ra::testing::CreateFile(test_file.c_str(), 293));

      FILE* f = fopen_lock(test_file.c_str(), "ab");
      ASSERT_TRUE(f != NULL);

      SelectionContext context;
#ifdef _WIN32
      {
        StringList elements;
        elements.push_back(test_file);
        context.SetElements(elements);
      }
#else
      //TODO: complete with known path to files
#endif

      ASSERT_FALSE(pmgr.HasProperty("selection.mimetype"));
      ASSERT_FALSE(pmgr.HasProperty("selection.description"));
      ASSERT_FALSE(pmgr.HasProperty("selection.charset"));
      //ASSERT_FALSE(pmgr.HasProperty("selection.libmagic_ext"));

      //act
      context.RegisterProperties();

      const char* test_string = ""
        "selection.mimetype"                "=${selection.mimetype}"      "\n"
        "selection.description"             "=${selection.description}"   "\n"
        "selection.charset"                 "=${selection.charset}"       "\n";
      //"selection.libmagic_ext"          "=${selection.libmagic_ext}"  "\n"

      const char* expected_string = ""
        "selection.mimetype"                "=writable, executable, regular file, no read permission"   "\n"
        "selection.description"             "=writable, executable, regular file, no read permission"   "\n"
        "selection.charset"                 "=writable, executable, regular file, no read permission"   "\n";

      //act
      std::string actual_string = pmgr.Expand(test_string);

      //make sure to unlock the file before reaching a failed assertion
      fclose_lock(f);
      f = NULL;

      ASSERT_EQ(std::string(expected_string), actual_string);

      context.UnregisterProperties();

      //cleanup
      ra::filesystem::DeleteDirectory(test_dir.c_str());
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSelectionContext, testFileMagicPropertiesFileLockedByFileLock)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();

      //create a "data" file in a temporary directory for this test
      std::string temp_dir = ra::filesystem::GetTemporaryDirectory();
      std::string test_dir = temp_dir + ra::filesystem::GetPathSeparatorStr() + ra::testing::GetTestQualifiedName();
      std::string test_file = test_dir + ra::filesystem::GetPathSeparatorStr() + "locked-by-win32.lockfile";
      ASSERT_TRUE(ra::filesystem::CreateDirectory(test_dir.c_str()));
      ASSERT_TRUE(ra::testing::CreateFile(test_file.c_str(), 293));

      // lock the file
      FILE_LOCK_HANDLE * hLock = NULL;
      bool locked = file_lock(test_file.c_str(), &hLock);
      ASSERT_TRUE(locked);

      SelectionContext context;
#ifdef _WIN32
      {
        StringList elements;
        elements.push_back(test_file);
        context.SetElements(elements);
      }
#else
      //TODO: complete with known path to files
#endif

      ASSERT_FALSE(pmgr.HasProperty("selection.mimetype"));
      ASSERT_FALSE(pmgr.HasProperty("selection.description"));
      ASSERT_FALSE(pmgr.HasProperty("selection.charset"));
      //ASSERT_FALSE(pmgr.HasProperty("selection.libmagic_ext"));

      //act
      context.RegisterProperties();

      const char* test_string = ""
        "selection.mimetype"                "=${selection.mimetype}"      "\n"
        "selection.description"             "=${selection.description}"   "\n"
        "selection.charset"                 "=${selection.charset}"       "\n";
      //"selection.libmagic_ext"          "=${selection.libmagic_ext}"  "\n"

      const char* expected_string = ""
        "selection.mimetype"                "=writable, executable, regular file, no read permission"   "\n"
        "selection.description"             "=writable, executable, regular file, no read permission"   "\n"
        "selection.charset"                 "=writable, executable, regular file, no read permission"   "\n";

      //act
      std::string actual_string = pmgr.Expand(test_string);

      //make sure to unlock the file before reaching a failed assertion
      bool unlocked = file_unlock(&hLock);
      ASSERT_TRUE(unlocked);

      ASSERT_EQ(std::string(expected_string), actual_string);

      context.UnregisterProperties();

      //cleanup
      ra::filesystem::DeleteDirectory(test_dir.c_str());
    }
    //--------------------------------------------------------------------------------------------------

  } //namespace test
} //namespace shellanything