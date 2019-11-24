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

#include "TestValidator.h"
#include "shellanything/Validator.h"
#include "shellanything/Context.h"
#include "PropertyManager.h"
#include "rapidassist/testing.h"

namespace shellanything { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestValidator::SetUp()
  {
    PropertyManager & pmgr = PropertyManager::getInstance();
    pmgr.clear();
  }
  //--------------------------------------------------------------------------------------------------
  void TestValidator::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testValidByDefault)
  {
    Context c;
    Validator v;

    //assert default
    ASSERT_TRUE( v.validate(c) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testMaxFiles)
  {
    Context c;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\kernel32.dll");
      elements.push_back("C:\\Windows\\System32\\cmd.exe"     );
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      elements.push_back("C:\\Windows\\System32\\services.msc");
      c.setElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    Validator v;

    //assert default
    ASSERT_TRUE( v.validate(c) );

    //assert higher value
    v.setMaxFiles(c.getNumFiles() + 1);
    ASSERT_TRUE( v.validate(c) );

    //assert exact value
    v.setMaxFiles(c.getNumFiles());
    ASSERT_TRUE( v.validate(c) );

    //assert lower value
    v.setMaxFiles(c.getNumFiles() - 1);
    ASSERT_FALSE( v.validate(c) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testMaxDirectories)
  {
    Context c;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Program Files");
      elements.push_back("C:\\Users"        );
      elements.push_back("C:\\Windows"      );
      c.setElements(elements);
    }
#else
    //TODO: complete with known path to directories
#endif

    Validator v;

    //assert default
    ASSERT_TRUE( v.validate(c) );

    //assert higher value
    v.setMaxDirectories(c.getNumDirectories() + 1);
    ASSERT_TRUE( v.validate(c) );

    //assert exact value
    v.setMaxDirectories(c.getNumDirectories());
    ASSERT_TRUE( v.validate(c) );

    //assert lower value
    v.setMaxDirectories(c.getNumDirectories() - 1);
    ASSERT_FALSE( v.validate(c) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testProperties)
  {
    Context c;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\kernel32.dll");
      elements.push_back("C:\\Windows\\System32\\cmd.exe"     );
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      elements.push_back("C:\\Windows\\System32\\services.msc");
      c.setElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    Validator v;

    //assert default
    ASSERT_TRUE( v.validate(c) );

    PropertyManager & pmgr = PropertyManager::getInstance();
    std::string property_name = ra::testing::GetTestQualifiedName();

    //assert failure when property is not defined
    v.setProperties(property_name);
    ASSERT_FALSE( v.validate(c) );

    //assert success when property is defined
    v.setProperties(property_name);
    pmgr.setProperty(property_name, "defined");
    ASSERT_TRUE( v.validate(c) );

    //assert failure when at least one property is not defined
    v.setProperties(property_name + ";foo");
    ASSERT_FALSE( v.validate(c) );

    //assert success when all properties are defined
    pmgr.setProperty("foo", "bar");
    ASSERT_TRUE( v.validate(c) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testFileExtensions)
  {
    Context c;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\kernel32.dll");
      elements.push_back("C:\\Windows\\System32\\cmd.exe"     );
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      elements.push_back("C:\\Windows\\System32\\services.msc");
      c.setElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    Validator v;

    //assert default
    ASSERT_TRUE( v.validate(c) );

    //assert failure when no file extension is matching
    v.setFileExtensions("foo");
    ASSERT_FALSE( v.validate(c) );

    //assert failure when a single file extension is matching
    v.setFileExtensions("dll");
    ASSERT_FALSE( v.validate(c) );

    //assert success when all properties are defined
    v.setFileExtensions("dll;exe;msc");
    ASSERT_TRUE( v.validate(c) );
    v.setFileExtensions(""); //random order
    ASSERT_TRUE( v.validate(c) );

    //assert success when more file extensions are allowed
    v.setFileExtensions("ini;txt;bat;doc;msc;dll;exe;xls;");
    ASSERT_TRUE( v.validate(c) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testFileExists)
  {
    Context c;
 
#ifdef _WIN32
    const std::string file_path = "C:\\Windows\\System32\\kernel32.dll";
    const std::string dir_path = "C:\\Program Files (x86)";
#else
    //TODO: complete with known path to files
#endif
 
    Validator v;
 
    //assert default
    ASSERT_TRUE( v.validate(c) );
 
    //assert failure when a specified file/directory is not found
    v.setFileExists("foo");
    ASSERT_FALSE( v.validate(c) );
 
    //assert success if the specified file is found
    v.setFileExists(file_path);
    ASSERT_TRUE( v.validate(c) );
 
    //assert success if the specified directory is found
    v.setFileExists(dir_path);
    ASSERT_TRUE( v.validate(c) );
 
    //assert success if multiple element is specified
    v.setFileExists(file_path + ";" + dir_path);
    ASSERT_TRUE( v.validate(c) );
 
    //assert failure if the last element is not found
    v.setFileExists(file_path + ";" + dir_path + ";foo");
    ASSERT_FALSE( v.validate(c) );
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
