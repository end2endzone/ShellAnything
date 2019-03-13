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

#include "TestValidation.h"
#include "shellanything/Validator.h"
#include "shellanything/PropertyManager.h"
#include "rapidassist/gtesthelp.h"

namespace shellanything { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestValidation::SetUp()
  {
    PropertyManager & pmgr = PropertyManager::getInstance();
    pmgr.clear();
  }
  //--------------------------------------------------------------------------------------------------
  void TestValidation::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidation, testMaxFiles)
  {
    Context c;
    c.filesCount = 4;
    c.directoryCount = 0;
    c.elements.push_back("C:\\Windows\\System32\\kernel32.dll");
    c.elements.push_back("C:\\Windows\\System32\\cmd.exe");
    c.elements.push_back("C:\\Windows\\System32\\notepad.exe");
    c.elements.push_back("C:\\Windows\\System32\\services.msc");

    Validator v;

    //assert default
    ASSERT_TRUE( v.isValid(c) );

    //assert higher value
    v.setMaxFiles(c.filesCount + 1);
    ASSERT_TRUE( v.isValid(c) );

    //assert exact value
    v.setMaxFiles(c.filesCount);
    ASSERT_TRUE( v.isValid(c) );

    //assert lower value
    v.setMaxFiles(c.filesCount - 1);
    ASSERT_FALSE( v.isValid(c) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidation, testMaxDirectories)
  {
    Context c;
    c.filesCount = 0;
    c.directoryCount = 3;
    c.elements.push_back("C:\\Program Files");
    c.elements.push_back("C:\\Users");
    c.elements.push_back("C:\\Windows");

    Validator v;

    //assert default
    ASSERT_TRUE( v.isValid(c) );

    //assert higher value
    v.setMaxDirectories(c.directoryCount + 1);
    ASSERT_TRUE( v.isValid(c) );

    //assert exact value
    v.setMaxDirectories(c.directoryCount);
    ASSERT_TRUE( v.isValid(c) );

    //assert lower value
    v.setMaxDirectories(c.directoryCount - 1);
    ASSERT_FALSE( v.isValid(c) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidation, testProperties)
  {
    Context c;
    c.filesCount = 4;
    c.directoryCount = 0;
    c.elements.push_back("C:\\Windows\\System32\\kernel32.dll");
    c.elements.push_back("C:\\Windows\\System32\\cmd.exe");
    c.elements.push_back("C:\\Windows\\System32\\notepad.exe");
    c.elements.push_back("C:\\Windows\\System32\\services.msc");

    Validator v;

    //assert default
    ASSERT_TRUE( v.isValid(c) );

    PropertyManager & pmgr = PropertyManager::getInstance();
    std::string property_name = ra::gtesthelp::getTestQualifiedName();

    //assert failure when property is not defined
    v.setProperties(property_name);
    ASSERT_FALSE( v.isValid(c) );

    //assert success when property is defined
    v.setProperties(property_name);
    pmgr.setProperty(property_name, "defined");
    ASSERT_TRUE( v.isValid(c) );

    //assert failure when at least one property is not defined
    v.setProperties(property_name + ";foo");
    ASSERT_FALSE( v.isValid(c) );

    //assert success when all properties are defined
    pmgr.setProperty("foo", "bar");
    ASSERT_TRUE( v.isValid(c) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidation, testFileExtensions)
  {
    Context c;
    c.filesCount = 4;
    c.directoryCount = 0;
    c.elements.push_back("C:\\Windows\\System32\\kernel32.dll");
    c.elements.push_back("C:\\Windows\\System32\\cmd.exe");
    c.elements.push_back("C:\\Windows\\System32\\notepad.exe");
    c.elements.push_back("C:\\Windows\\System32\\services.msc");

    Validator v;

    //assert default
    ASSERT_TRUE( v.isValid(c) );

    //assert failure when no file extension is matching
    v.setFileExtensions("foo");
    ASSERT_FALSE( v.isValid(c) );

    //assert failure when a single file extension is matching
    v.setFileExtensions("dll");
    ASSERT_FALSE( v.isValid(c) );

    //assert success when all properties are defined
    v.setFileExtensions("dll;exe;msc");
    ASSERT_TRUE( v.isValid(c) );
    v.setFileExtensions(""); //random order
    ASSERT_TRUE( v.isValid(c) );

    //assert success when more file extensions are allowed
    v.setFileExtensions("ini;txt;bat;doc;msc;dll;exe;xls;");
    ASSERT_TRUE( v.isValid(c) );
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
