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
#include "Validator.h"
#include "Menu.h"
#include "Context.h"
#include "PropertyManager.h"
#include "rapidassist/testing.h"
#include "rapidassist/process.h"

namespace shellanything { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestValidator::SetUp()
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
    pmgr.Clear();
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
    ASSERT_TRUE( v.Validate(c) );
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
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    Validator v;

    //assert default
    ASSERT_TRUE( v.Validate(c) );

    //assert higher value
    v.SetMaxFiles(c.GetNumFiles() + 1);
    ASSERT_TRUE( v.Validate(c) );

    //assert exact value
    v.SetMaxFiles(c.GetNumFiles());
    ASSERT_TRUE( v.Validate(c) );

    //assert lower value
    v.SetMaxFiles(c.GetNumFiles() - 1);
    ASSERT_FALSE( v.Validate(c) );
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
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to directories
#endif

    Validator v;

    //assert default
    ASSERT_TRUE( v.Validate(c) );

    //assert higher value
    v.SetMaxDirectories(c.GetNumDirectories() + 1);
    ASSERT_TRUE( v.Validate(c) );

    //assert exact value
    v.SetMaxDirectories(c.GetNumDirectories());
    ASSERT_TRUE( v.Validate(c) );

    //assert lower value
    v.SetMaxDirectories(c.GetNumDirectories() - 1);
    ASSERT_FALSE( v.Validate(c) );
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
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    Validator v;

    //assert default
    ASSERT_TRUE( v.Validate(c) );

    PropertyManager & pmgr = PropertyManager::GetInstance();
    std::string property_name = ra::testing::GetTestQualifiedName();

    //assert failure when property is not defined
    v.SetProperties(property_name);
    ASSERT_FALSE( v.Validate(c) );

    //assert success when property is defined
    v.SetProperties(property_name);
    pmgr.SetProperty(property_name, "defined");
    ASSERT_TRUE( v.Validate(c) );

    //assert failure when at least one property is not defined
    v.SetProperties(property_name + SA_PROPERTIES_ATTR_SEPARATOR_STR "foo");
    ASSERT_FALSE( v.Validate(c) );

    //assert success when all properties are defined
    pmgr.SetProperty("foo", "bar");
    ASSERT_TRUE( v.Validate(c) );
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
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    Validator v;

    //assert default
    ASSERT_TRUE( v.Validate(c) );

    //assert failure when no file extension is matching
    v.SetFileExtensions("foo");
    ASSERT_FALSE( v.Validate(c) );

    //assert failure when a single file extension is matching
    v.SetFileExtensions("dll");
    ASSERT_FALSE( v.Validate(c) );

    //assert success when all file extensions are matching
    v.SetFileExtensions("dll" SA_FILEEXTENSION_ATTR_SEPARATOR_STR "exe" SA_FILEEXTENSION_ATTR_SEPARATOR_STR "msc");
    ASSERT_TRUE( v.Validate(c) );
    v.SetFileExtensions("exe" SA_FILEEXTENSION_ATTR_SEPARATOR_STR "dll" SA_FILEEXTENSION_ATTR_SEPARATOR_STR "msc"); //random order
    ASSERT_TRUE( v.Validate(c) );

    //assert success when more file extensions are allowed
    std::string extensions = "ini;txt;bat;doc;msc;dll;exe;xls;";
    ra::strings::Replace(extensions, ";", SA_FILEEXTENSION_ATTR_SEPARATOR_STR);
    v.SetFileExtensions(extensions);
    ASSERT_TRUE( v.Validate(c) );

    //assert failure when multiple files are selected and a single extension is missing
    v.SetFileExtensions("dll" SA_FILEEXTENSION_ATTR_SEPARATOR_STR "exe"); //missing msc file extension
    ASSERT_FALSE( v.Validate(c) );
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
    ASSERT_TRUE( v.Validate(c) );
 
    //assert failure when a specified file/directory is not found
    v.SetFileExists("foo");
    ASSERT_FALSE( v.Validate(c) );
 
    //assert success if the specified file is found
    v.SetFileExists(file_path);
    ASSERT_TRUE( v.Validate(c) );
 
    //assert success if the specified directory is found
    v.SetFileExists(dir_path);
    ASSERT_TRUE( v.Validate(c) );
 
    //assert success if all elements exists
    v.SetFileExists(file_path + SA_EXISTS_ATTR_SEPARATOR_STR + dir_path);
    ASSERT_TRUE( v.Validate(c) );
 
    //assert failure if the last element is not found
    v.SetFileExists(file_path + SA_EXISTS_ATTR_SEPARATOR_STR + dir_path + SA_EXISTS_ATTR_SEPARATOR_STR + "foo");
    ASSERT_FALSE( v.Validate(c) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testClass)
  {
    Context c;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\kernel32.dll");
      elements.push_back("C:\\Windows\\System32\\cmd.exe"     );
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      elements.push_back("C:\\Windows\\System32\\services.msc");
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    Validator v;

    //assert default
    ASSERT_TRUE( v.Validate(c) );

    //assert failure when no file extension is matching
    v.SetClass(".foo");
    ASSERT_FALSE( v.Validate(c) );

    //assert success when a single file extension is matching
    v.SetClass(".dll");
    ASSERT_FALSE( v.Validate(c) );

    //assert success when all file extensions are matching
    v.SetClass(".dll" SA_CLASS_ATTR_SEPARATOR_STR ".exe" SA_CLASS_ATTR_SEPARATOR_STR ".msc");
    ASSERT_TRUE( v.Validate(c) );
    v.SetClass(".exe" SA_CLASS_ATTR_SEPARATOR_STR ".dll" SA_CLASS_ATTR_SEPARATOR_STR ".msc"); //random order
    ASSERT_TRUE( v.Validate(c) );

    //assert success when more file extensions are allowed
    std::string extensions = ".ini;.txt;.bat;.doc;.msc;.dll;.exe;.xls;";
    ra::strings::Replace(extensions, ";", SA_CLASS_ATTR_SEPARATOR_STR);
    v.SetClass(extensions);
    ASSERT_TRUE( v.Validate(c) );

    //assert success when using 'file'
    v.SetClass("file");
    ASSERT_TRUE( v.Validate(c) );

    //assert failure when using 'folder'
    v.SetClass("folder");
    ASSERT_FALSE( v.Validate(c) );

    //assert success when using 'drive'
    v.SetClass("drive");
    ASSERT_TRUE( v.Validate(c) );

    //assert success when using 'drive:fixed'
    v.SetClass("drive:fixed");
    ASSERT_TRUE( v.Validate(c) );

    //assert failure when using 'drive:network'
    v.SetClass("drive:network");
    ASSERT_FALSE( v.Validate(c) );

    //assert 'at least one' class must match
    //If multiple classes are specified, at least one class must match for the validation to be successful.
    v.SetClass("folder" SA_CLASS_ATTR_SEPARATOR_STR "drive:network" SA_CLASS_ATTR_SEPARATOR_STR "drive:fixed"); // folder and drive:network fails but drive:fixed matches
    ASSERT_TRUE( v.Validate(c) );

    //Set only folders
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32");
      elements.push_back("C:\\Windows\\Fonts"     );
      elements.push_back("C:\\Windows\\SysWOW64" );
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    //assert failure when using 'file'
    v.SetClass("file");
    ASSERT_FALSE( v.Validate(c) );

    //assert success when using 'folder'
    v.SetClass("folder");
    ASSERT_TRUE( v.Validate(c) );

    //assert success when using 'drive'
    v.SetClass("drive");
    ASSERT_TRUE( v.Validate(c) );

    //assert failure when using 'drive:network'
    v.SetClass("drive:network");
    ASSERT_FALSE( v.Validate(c) );

#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("\\\\localhost\\public\\foo.dat" );
      elements.push_back("\\\\localhost\\public\\bar.dat" );
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    //assert failure when using 'file'
    v.SetClass("file");
    ASSERT_FALSE( v.Validate(c) );

    //assert failure when using 'drive'
    v.SetClass("drive");
    ASSERT_FALSE( v.Validate(c) );

    //assert failure when using 'drive:fixed'
    v.SetClass("drive:fixed");
    ASSERT_FALSE( v.Validate(c) );

    //assert success when using 'drive:network'
    v.SetClass("drive:network");
    ASSERT_TRUE( v.Validate(c) );

    //Set a mix of files and folders (issue #97, issue97)
    //If multiple files are selected, the class of each file must match at least one allowed classes for the validation to be successful.
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32");
      elements.push_back("C:\\Windows\\notepad.exe");
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    //assert success when using 'file' or 'folder'
    v.SetClass("file" SA_CLASS_ATTR_SEPARATOR_STR "directory");
    ASSERT_TRUE(v.Validate(c));

  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testPattern)
  {
    Context c;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\kernel32.dll");
      elements.push_back("C:\\Windows\\System32\\cmd.exe"     );
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      elements.push_back("C:\\Windows\\System32\\services.msc");
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    Validator v;

    //assert default
    ASSERT_TRUE( v.Validate(c) );

    //assert failure when no pattern is matching
    v.SetPattern("foo");
    ASSERT_FALSE( v.Validate(c) );

    //assert failure when patterns is matching only a single file
    v.SetPattern("*cmd.exe");
    ASSERT_FALSE( v.Validate(c) );

    //assert success when patterns are matching all files
    v.SetPattern("*.dll" SA_PATTERN_ATTR_SEPARATOR_STR "*.exe" SA_PATTERN_ATTR_SEPARATOR_STR "*.msc");
    ASSERT_TRUE( v.Validate(c) );
    v.SetPattern("*.exe" SA_PATTERN_ATTR_SEPARATOR_STR "*.dll" SA_PATTERN_ATTR_SEPARATOR_STR "*.msc"); //random order
    ASSERT_TRUE( v.Validate(c) );

    //assert success when more than required patterns are provided
    std::string pattern = "*e*;*.dll;*.exe;*.msc;*a*;";
    ra::strings::Replace(pattern, ";", SA_PATTERN_ATTR_SEPARATOR_STR);
    v.SetPattern(pattern);
    ASSERT_TRUE( v.Validate(c) );

    //assert failure when multiple files are selected and a single pattern is missing
    v.SetPattern("*.dll" SA_PATTERN_ATTR_SEPARATOR_STR "*.exe"); //missing msc file extension
    ASSERT_FALSE( v.Validate(c) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testExprtk)
  {
    Context c;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\kernel32.dll");
      elements.push_back("C:\\Windows\\System32\\cmd.exe"     );
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      elements.push_back("C:\\Windows\\System32\\services.msc");
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif
    c.RegisterProperties();

    PropertyManager & pmgr = PropertyManager::GetInstance();

    Validator v;

    //assert default
    ASSERT_TRUE( v.Validate(c) );

    //assert an expression that resolve as true
    v.SetExprtk("5>1");
    ASSERT_TRUE( v.Validate(c) );

    //assert an expression that resolve as false
    v.SetExprtk("5<1");
    ASSERT_FALSE( v.Validate(c) );

    //assert expression with spaces
    v.SetExprtk("5 > 1");
    ASSERT_TRUE( v.Validate(c) );

    //assert a string expression with ==
    v.SetExprtk("'test'=='test'");
    ASSERT_TRUE( v.Validate(c) );
    v.SetExprtk("'foo'=='bar'");
    ASSERT_FALSE( v.Validate(c) );

    //assert a string expression with !=
    v.SetExprtk("'test'!='test'");
    ASSERT_FALSE( v.Validate(c) );
    v.SetExprtk("'foo'!='bar'");
    ASSERT_TRUE( v.Validate(c) );

    //assert an expression with properties
    pmgr.SetProperty("foo", "bar");
    v.SetExprtk("'${foo}'=='bar'");
    ASSERT_TRUE( v.Validate(c) );
    v.SetExprtk("'bar'=='${foo}'");
    ASSERT_TRUE( v.Validate(c) );

    //assert an invalid expression (string with numeric value)
    v.SetExprtk("'5'==5");
    ASSERT_FALSE( v.Validate(c) );
    v.SetExprtk("'5'!=5");
    ASSERT_FALSE( v.Validate(c) );

    //assert logical 'and' operator
    v.SetExprtk("10 > 6 and 10 > 7");
    ASSERT_TRUE( v.Validate(c) );
    v.SetExprtk("10 > 6 and 10 > 70");
    ASSERT_FALSE( v.Validate(c) );
    v.SetExprtk("10 > 6 & 10 > 7");
    ASSERT_TRUE( v.Validate(c) );
    v.SetExprtk("10 > 6 & 10 > 70");
    ASSERT_FALSE( v.Validate(c) );

    //assert logical 'or' operator
    v.SetExprtk("1 > 6 or 10 > 7");
    ASSERT_TRUE( v.Validate(c) );
    v.SetExprtk("1 > 6 or 10 > 70");
    ASSERT_FALSE( v.Validate(c) );
    v.SetExprtk("1 > 6 | 10 > 7");
    ASSERT_TRUE( v.Validate(c) );
    v.SetExprtk("1 > 6 | 10 > 70");
    ASSERT_FALSE( v.Validate(c) );

    //assert logical 'not' operator
    v.SetExprtk("not(6==10)");
    ASSERT_TRUE( v.Validate(c) );
    v.SetExprtk("not(6!=10)");
    ASSERT_FALSE( v.Validate(c) );

    //assert string operator: 'in'
    v.SetExprtk("'abc' in 'abcdefgh'");
    ASSERT_TRUE( v.Validate(c) );
    v.SetExprtk("'zzz' in 'abcdefgh'");
    ASSERT_FALSE( v.Validate(c) );

    //assert string operator: 'ilike'
    v.SetExprtk("'abcDeFgH' ilike 'a??d*h'");
    ASSERT_TRUE( v.Validate(c) );
    v.SetExprtk("'accH' ilike 'a??d*h'");
    ASSERT_FALSE( v.Validate(c) );

    //assert string operator: string interval (substring)
    v.SetExprtk("var x := 'abcdefgh'; x[2:4] == 'cde'");
    ASSERT_TRUE( v.Validate(c) );
    v.SetExprtk("var x := 'abcdefgh'; 'abcdefgh'[1:3] == 'zzz'");
    ASSERT_FALSE( v.Validate(c) );

    //assert string operator: size (length)
    v.SetExprtk("'abc'[] == 3");
    ASSERT_TRUE( v.Validate(c) );
    v.SetExprtk("'abc'[] == 4");
    ASSERT_FALSE( v.Validate(c) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testIsInversed)
  {
    Validator v;
 
    // Invalid attribute name
    ASSERT_FALSE( v.IsInversed(NULL) );
    ASSERT_FALSE( v.IsInversed("") );

    v.SetInserve("foo");

    // Wrong attribute name
    ASSERT_FALSE( v.IsInversed("bar") );

    // Good attribute name;
    ASSERT_TRUE( v.IsInversed("foo") );

    // Longer attribute name
    ASSERT_FALSE( v.IsInversed("foobar") );

    // Shorter name
    ASSERT_FALSE( v.IsInversed("barfoo") );



    v.SetInserve("foobar");

    // Searching subsets of the inserve string
    ASSERT_FALSE( v.IsInversed("foo") );
    ASSERT_FALSE( v.IsInversed("bar") );
    ASSERT_FALSE( v.IsInversed("ooba") );



    v.SetInserve("foo" SA_INVERSE_ATTR_SEPARATOR_STR "bar" SA_INVERSE_ATTR_SEPARATOR_STR "baz");

    // Search first, middle, last attribute names
    ASSERT_TRUE( v.IsInversed("foo") );
    ASSERT_TRUE( v.IsInversed("bar") );
    ASSERT_TRUE( v.IsInversed("baz") );

    // Search is case sensitive
    ASSERT_FALSE( v.IsInversed("fOo") );
    ASSERT_FALSE( v.IsInversed("bAr") );
    ASSERT_FALSE( v.IsInversed("bAz") );


    // Assert the search go beyond its first match.
    // There is a substring match at bart and bars but they should then
    // be rejected. The last 'bar' should be where the match occurs.
    v.SetInserve("bart" SA_INVERSE_ATTR_SEPARATOR_STR "bars" SA_INVERSE_ATTR_SEPARATOR_STR "bar");
    ASSERT_TRUE( v.IsInversed("bar") );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testMaxFilesInversed)
  {
    Context c;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\kernel32.dll");
      elements.push_back("C:\\Windows\\System32\\cmd.exe"     );
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      elements.push_back("C:\\Windows\\System32\\services.msc");
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    Validator v;
    v.SetInserve("maxfiles"); // now the maxfiles attribute means "more than x"

    //assert default
    ASSERT_FALSE( v.Validate(c) ); // matches <visibility inverse="maxfiles" /> inversing maxfiles without specifying a maxfiles attribute...

    //assert higher value
    v.SetMaxFiles(c.GetNumFiles() + 1);
    ASSERT_FALSE( v.Validate(c) );

    //assert exact value
    v.SetMaxFiles(c.GetNumFiles());
    ASSERT_FALSE( v.Validate(c) );

    //assert lower value
    v.SetMaxFiles(c.GetNumFiles() - 1);
    ASSERT_TRUE( v.Validate(c) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testMaxDirectoriesInversed)
  {
    Context c;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Program Files");
      elements.push_back("C:\\Users"        );
      elements.push_back("C:\\Windows"      );
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to directories
#endif

    Validator v;
    v.SetInserve("maxfolders"); // now the maxfolders attribute means "more than x"

    //assert default
    ASSERT_FALSE( v.Validate(c) ); // matches <visibility inverse="maxfolders" /> inversing maxfolders without specifying a maxfolders attribute...

    //assert higher value
    v.SetMaxDirectories(c.GetNumDirectories() + 1);
    ASSERT_FALSE( v.Validate(c) );

    //assert exact value
    v.SetMaxDirectories(c.GetNumDirectories());
    ASSERT_FALSE( v.Validate(c) );

    //assert lower value
    v.SetMaxDirectories(c.GetNumDirectories() - 1);
    ASSERT_TRUE( v.Validate(c) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testPropertiesInversed)
  {
    Context c;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\kernel32.dll");
      elements.push_back("C:\\Windows\\System32\\cmd.exe"     );
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      elements.push_back("C:\\Windows\\System32\\services.msc");
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    Validator v;
    v.SetInserve("properties"); // now the properties attribute means "valid when the property is not defined or empty"

    //assert default
    ASSERT_TRUE( v.Validate(c) );

    PropertyManager & pmgr = PropertyManager::GetInstance();
    std::string property_name = ra::testing::GetTestQualifiedName();

    //assert success when property is not defined
    pmgr.Clear();
    v.SetProperties(property_name);
    ASSERT_TRUE( v.Validate(c) );

    //assert failure when property is defined
    pmgr.Clear();
    pmgr.SetProperty(property_name, "defined");
    v.SetProperties(property_name);
    ASSERT_FALSE( v.Validate(c) );

    //assert failure when at least one property is defined
    pmgr.Clear();
    pmgr.SetProperty(property_name, "defined"); // property_name is defined
    v.SetProperties(property_name + SA_PROPERTIES_ATTR_SEPARATOR_STR "foo"); // foo is not defined
    ASSERT_FALSE( v.Validate(c) );

    //assert failure when all properties are defined
    pmgr.Clear();
    pmgr.SetProperty(property_name, "defined");
    pmgr.SetProperty("foo", "bar");
    v.SetProperties(property_name + SA_PROPERTIES_ATTR_SEPARATOR_STR "foo"); // all defined
    ASSERT_FALSE( v.Validate(c) );

    //assert success when all properties are not defined
    pmgr.Clear();
    v.SetProperties(property_name + SA_PROPERTIES_ATTR_SEPARATOR_STR "foo"); // all not defined
    ASSERT_TRUE( v.Validate(c) );

    // If multiple properties are specified, all properties must be empty or not defined for the validation to be successful.
    pmgr.Clear();
    v.SetProperties("foo" SA_PROPERTIES_ATTR_SEPARATOR_STR "bar" SA_PROPERTIES_ATTR_SEPARATOR_STR "baz");
    ASSERT_TRUE( v.Validate(c) );

    pmgr.SetProperty(property_name, "defined");
    v.SetProperties("foo" SA_PROPERTIES_ATTR_SEPARATOR_STR
                    "bar" SA_PROPERTIES_ATTR_SEPARATOR_STR
                    "baz" SA_PROPERTIES_ATTR_SEPARATOR_STR + property_name);
    ASSERT_FALSE( v.Validate(c) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testFileExtensionsInversed)
  {
    Context c;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\kernel32.dll");
      elements.push_back("C:\\Windows\\System32\\cmd.exe"     );
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      elements.push_back("C:\\Windows\\System32\\services.msc");
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    Validator v;
    v.SetInserve("fileextensions");

    //assert default
    ASSERT_TRUE( v.Validate(c) );

    //assert success when no file extension is matching
    v.SetFileExtensions("foo");
    ASSERT_TRUE( v.Validate(c) );

    //assert failure when a single file extension is matching
    v.SetFileExtensions("dll");
    ASSERT_FALSE( v.Validate(c) );

    //assert failure when all file extensions are defined
    v.SetFileExtensions("dll" SA_FILEEXTENSION_ATTR_SEPARATOR_STR "exe" SA_FILEEXTENSION_ATTR_SEPARATOR_STR "msc");
    ASSERT_FALSE( v.Validate(c) );
    v.SetFileExtensions("exe" SA_FILEEXTENSION_ATTR_SEPARATOR_STR "dll" SA_FILEEXTENSION_ATTR_SEPARATOR_STR "msc"); //random order
    ASSERT_FALSE( v.Validate(c) );

    //assert failure when more file extensions are matching
    std::string extensions = "ini;txt;bat;doc;msc;dll;exe;xls;";
    ra::strings::Replace(extensions, ";", SA_FILEEXTENSION_ATTR_SEPARATOR_STR);
    v.SetFileExtensions(extensions);
    ASSERT_FALSE( v.Validate(c) );

    // If multiple file extensions are specified, no extension must match for the validation to be successful.
    v.SetFileExtensions("aaa" SA_FILEEXTENSION_ATTR_SEPARATOR_STR
                        "bbb" SA_FILEEXTENSION_ATTR_SEPARATOR_STR
                        "ccc" SA_FILEEXTENSION_ATTR_SEPARATOR_STR);
    ASSERT_TRUE( v.Validate(c) );
    v.SetFileExtensions("aaa" SA_FILEEXTENSION_ATTR_SEPARATOR_STR
                        "bbb" SA_FILEEXTENSION_ATTR_SEPARATOR_STR
                        "exe" SA_FILEEXTENSION_ATTR_SEPARATOR_STR
                        "ccc" SA_FILEEXTENSION_ATTR_SEPARATOR_STR);
    ASSERT_FALSE( v.Validate(c) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testFileExistsInversed)
  {
    Context c;
 
#ifdef _WIN32
    const std::string file_path = "C:\\Windows\\System32\\kernel32.dll";
    const std::string dir_path = "C:\\Program Files (x86)";
#else
    //TODO: complete with known path to files
#endif
 
    Validator v;
    v.SetInserve("exists");
 
    //assert default
    ASSERT_TRUE( v.Validate(c) );
 
    //assert success when a specified file/directory is not found
    v.SetFileExists("foo");
    ASSERT_TRUE( v.Validate(c) );
 
    //assert failure if the specified file is found
    v.SetFileExists(file_path);
    ASSERT_FALSE( v.Validate(c) );
 
    //assert failure if the specified directory is found
    v.SetFileExists(dir_path);
    ASSERT_FALSE( v.Validate(c) );
 
    //assert failure if all elements exists
    v.SetFileExists(file_path + SA_EXISTS_ATTR_SEPARATOR_STR + dir_path);
    ASSERT_FALSE( v.Validate(c) );
 
    //assert failure if at least one of all elements is found
    std::string exists;
    exists += "bar" SA_EXISTS_ATTR_SEPARATOR_STR;
    exists += dir_path + SA_EXISTS_ATTR_SEPARATOR_STR;
    exists += "foo";
    v.SetFileExists(exists);
    ASSERT_FALSE( v.Validate(c) );

    exists += "bar" SA_EXISTS_ATTR_SEPARATOR_STR;
    exists += file_path + SA_EXISTS_ATTR_SEPARATOR_STR;
    exists += "foo";
    v.SetFileExists(exists);
    ASSERT_FALSE( v.Validate(c) );
 
    // If multiple files are specified, all files must not exists on the system for the validation to be successful.
    v.SetFileExists("foo" SA_EXISTS_ATTR_SEPARATOR_STR "bar" SA_EXISTS_ATTR_SEPARATOR_STR "baz");
    ASSERT_TRUE( v.Validate(c) );
    v.SetFileExists("foo" SA_EXISTS_ATTR_SEPARATOR_STR
                    "bar" SA_EXISTS_ATTR_SEPARATOR_STR
                    "C:\\Windows\\System32\\kernel32.dll" SA_EXISTS_ATTR_SEPARATOR_STR
                    "baz");
    ASSERT_FALSE( v.Validate(c) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testPatternInversed)
  {
    Context c;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\kernel32.dll");
      elements.push_back("C:\\Windows\\System32\\cmd.exe"     );
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      elements.push_back("C:\\Windows\\System32\\services.msc");
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    Validator v;
    v.SetInserve("pattern");

    //assert default
    ASSERT_TRUE( v.Validate(c) );

    //assert success when no pattern is matching
    v.SetPattern("foo");
    ASSERT_TRUE( v.Validate(c) );

    //assert failure when a pattern is matching a single file
    v.SetPattern("*.dll");
    ASSERT_FALSE( v.Validate(c) );

    //assert failure when pattern is matching all files
    v.SetPattern("*.dll" SA_PATTERN_ATTR_SEPARATOR_STR "*.exe" SA_PATTERN_ATTR_SEPARATOR_STR "*.msc");
    ASSERT_FALSE( v.Validate(c) );
    v.SetPattern("*.exe" SA_PATTERN_ATTR_SEPARATOR_STR "*.dll" SA_PATTERN_ATTR_SEPARATOR_STR "*.msc"); //random order
    ASSERT_FALSE( v.Validate(c) );

    //assert failure when more than required patterns are provided
    std::string pattern = "*e*;*.dll;*.exe;*.msc;*a*;";
    ra::strings::Replace(pattern, ";", SA_PATTERN_ATTR_SEPARATOR_STR);
    v.SetPattern(pattern);
    ASSERT_FALSE( v.Validate(c) );

    // If multiple patterns are specified, no pattern must match for the validation to be successful.
    v.SetPattern( "*.foo" SA_PATTERN_ATTR_SEPARATOR_STR
                  "*.bar" SA_PATTERN_ATTR_SEPARATOR_STR
                  "*.baz" SA_PATTERN_ATTR_SEPARATOR_STR);
    ASSERT_TRUE( v.Validate(c) );
    v.SetPattern( "*.foo" SA_PATTERN_ATTR_SEPARATOR_STR
                  "*.exe" SA_PATTERN_ATTR_SEPARATOR_STR
                  "*.bar" SA_PATTERN_ATTR_SEPARATOR_STR
                  "*.baz" SA_PATTERN_ATTR_SEPARATOR_STR);
    ASSERT_FALSE( v.Validate(c) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testExprtkInversed)
  {
    Context c;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\kernel32.dll");
      elements.push_back("C:\\Windows\\System32\\cmd.exe"     );
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      elements.push_back("C:\\Windows\\System32\\services.msc");
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif
    c.RegisterProperties();

    PropertyManager & pmgr = PropertyManager::GetInstance();

    Validator v;
    v.SetInserve("exprtk");

    //assert default
    ASSERT_TRUE( v.Validate(c) );

    //assert an expression that resolve as true
    v.SetExprtk("5>1");
    ASSERT_FALSE( v.Validate(c) );

    //assert an expression that resolve as false
    v.SetExprtk("5<1");
    ASSERT_TRUE( v.Validate(c) );

    //assert expression with spaces
    v.SetExprtk("5 > 1");
    ASSERT_FALSE( v.Validate(c) );

    //assert a string expression with ==
    v.SetExprtk("'test'=='test'");
    ASSERT_FALSE( v.Validate(c) );
    v.SetExprtk("'foo'=='bar'");
    ASSERT_TRUE( v.Validate(c) );

    //assert a string expression with !=
    v.SetExprtk("'test'!='test'");
    ASSERT_TRUE( v.Validate(c) );
    v.SetExprtk("'foo'!='bar'");
    ASSERT_FALSE( v.Validate(c) );

    //assert an expression with properties
    pmgr.SetProperty("foo", "bar");
    v.SetExprtk("'${foo}'=='bar'");
    ASSERT_FALSE( v.Validate(c) );
    v.SetExprtk("'bar'=='${foo}'");
    ASSERT_FALSE( v.Validate(c) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testInversedAll)
  {
    Validator v;

    v.SetInserve("all");
    ASSERT_TRUE( v.IsInversed("maxfiles") );
    ASSERT_TRUE( v.IsInversed("maxfolders") );
    ASSERT_TRUE( v.IsInversed("fileextensions") );
    ASSERT_TRUE( v.IsInversed("exists") );
    ASSERT_TRUE( v.IsInversed("properties") );

    v.SetInserve("foo;all");
    ASSERT_TRUE( v.IsInversed("maxfiles") );
    ASSERT_TRUE( v.IsInversed("maxfolders") );
    ASSERT_TRUE( v.IsInversed("fileextensions") );
    ASSERT_TRUE( v.IsInversed("exists") );
    ASSERT_TRUE( v.IsInversed("properties") );

    v.SetInserve("all;foo");
    ASSERT_TRUE( v.IsInversed("maxfiles") );
    ASSERT_TRUE( v.IsInversed("maxfolders") );
    ASSERT_TRUE( v.IsInversed("fileextensions") );
    ASSERT_TRUE( v.IsInversed("exists") );
    ASSERT_TRUE( v.IsInversed("properties") );

    v.SetInserve("foo;all;bar");
    ASSERT_TRUE( v.IsInversed("maxfiles") );
    ASSERT_TRUE( v.IsInversed("maxfolders") );
    ASSERT_TRUE( v.IsInversed("fileextensions") );
    ASSERT_TRUE( v.IsInversed("exists") );
    ASSERT_TRUE( v.IsInversed("properties") );

  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testMultipleValidator)
  {
    Context c;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\kernel32.dll");
      elements.push_back("C:\\Windows\\System32\\cmd.exe"     );
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      elements.push_back("C:\\Windows\\System32\\services.msc");
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    Menu menu;

    //assert default
    menu.Update(c);
    ASSERT_TRUE( menu.IsVisible() );
    ASSERT_TRUE( menu.IsEnabled() );

    //
    Validator * v1 = new Validator();
    Validator * v2 = new Validator();
    menu.AddVisibility(v1);
    menu.AddVisibility(v2);

    //assert 'or' operator between each Validator
    v1->SetFileExtensions("dll" SA_FILEEXTENSION_ATTR_SEPARATOR_STR "exe" SA_FILEEXTENSION_ATTR_SEPARATOR_STR "msc");
    v2->SetFileExtensions("");
    menu.Update(c);
    ASSERT_TRUE( menu.IsVisible() );

    //assert 'or' operator when swapping validators
    v1->SetFileExtensions("");
    v2->SetFileExtensions("dll" SA_FILEEXTENSION_ATTR_SEPARATOR_STR "exe" SA_FILEEXTENSION_ATTR_SEPARATOR_STR "msc");
    menu.Update(c);
    ASSERT_TRUE( menu.IsVisible() );

    //assert validators are not complementary
    v1->SetFileExtensions("dll");
    v2->SetFileExtensions("exe" SA_FILEEXTENSION_ATTR_SEPARATOR_STR "msc");
    menu.Update(c);
    ASSERT_FALSE( menu.IsVisible() );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testIsTrueStatement)
  {
    ASSERT_TRUE(Validator::IsTrue("true"));
    ASSERT_TRUE(Validator::IsTrue("tRue"));
    ASSERT_TRUE(Validator::IsTrue("TRUE"));

    ASSERT_TRUE(Validator::IsTrue("yes"));
    ASSERT_TRUE(Validator::IsTrue("OK"));
    ASSERT_TRUE(Validator::IsTrue("oN"));
    ASSERT_TRUE(Validator::IsTrue("1"));

    ASSERT_FALSE(Validator::IsTrue("foo"));

    //test with system.true
    PropertyManager & pmgr = PropertyManager::GetInstance();
    pmgr.Clear();
    ASSERT_FALSE(Validator::IsTrue("bar"));
    pmgr.SetProperty(PropertyManager::SYSTEM_TRUE_PROPERTY_NAME, "BaR");
    ASSERT_TRUE(Validator::IsTrue("bar"));
    pmgr.Clear();
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testIsFalseStatement)
  {
    ASSERT_TRUE(Validator::IsFalse("false"));
    ASSERT_TRUE(Validator::IsFalse("fAlse"));
    ASSERT_TRUE(Validator::IsFalse("FALSE"));

    ASSERT_TRUE(Validator::IsFalse("no"));
    ASSERT_TRUE(Validator::IsFalse("fail"));
    ASSERT_TRUE(Validator::IsFalse("oFF"));
    ASSERT_TRUE(Validator::IsFalse("0"));

    ASSERT_FALSE(Validator::IsFalse("bar"));

    //test with system.true
    PropertyManager & pmgr = PropertyManager::GetInstance();
    pmgr.Clear();
    ASSERT_FALSE(Validator::IsFalse("foo"));
    pmgr.SetProperty(PropertyManager::SYSTEM_FALSE_PROPERTY_NAME, "fOo");
    ASSERT_TRUE(Validator::IsFalse("foo"));
    pmgr.Clear();
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testIsTrue)
  {
    Context c;
    Context::ElementList elements;
    elements.push_back( ra::process::GetCurrentProcessPath() );
    c.SetElements(elements);

    Validator v;

    //assert default
    ASSERT_TRUE(v.Validate(c));

    //assert failure when a false/unkown statement is specified
    v.SetIsTrue("no");
    ASSERT_FALSE(v.Validate(c));
    v.SetIsTrue("foo");
    ASSERT_FALSE(v.Validate(c));

    //assert success if the statement evaluates to true
    static const char * statements[] = {
      "true",
      "tRue",
      "TRUE",
      "yes",
      "OK",
      "oN",
      "1",
    };
    static const size_t num_statements = sizeof(statements) / sizeof(statements[0]);
    for (size_t i = 0; i < num_statements; i++)
    {
      const char * statement = statements[i];
      v.SetIsTrue(statement);
      ASSERT_TRUE(v.Validate(c)) << "Statement '" << statement << "' is expected to evaluate to true";
    }

    //test with multiple values
    std::string istrue;

    //assert success if all statements evaluates to true
    istrue.clear();
    istrue += "yes";
    istrue += SA_ISTRUE_ATTR_SEPARATOR_STR;
    istrue += "true";
    istrue += SA_ISTRUE_ATTR_SEPARATOR_STR;
    istrue += "on";
    v.SetIsTrue(istrue);
    ASSERT_TRUE(v.Validate(c));

    //assert failure if the last element does not evaluates to true
    //if multiple values are specified, all values must evaluate to true for the validation to be successful.
    istrue.clear();
    istrue += "yes";
    istrue += SA_ISTRUE_ATTR_SEPARATOR_STR;
    istrue += "true";
    istrue += SA_ISTRUE_ATTR_SEPARATOR_STR;
    istrue += "foo";
    v.SetIsTrue(istrue);
    ASSERT_FALSE(v.Validate(c));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testIsFalse)
  {
    Context c;
    Context::ElementList elements;
    elements.push_back( ra::process::GetCurrentProcessPath() );
    c.SetElements(elements);

    Validator v;

    //assert default
    ASSERT_TRUE(v.Validate(c));

    //assert failure when a true/unkown statement is specified
    v.SetIsFalse("yes");
    ASSERT_FALSE(v.Validate(c));
    v.SetIsFalse("foo");
    ASSERT_FALSE(v.Validate(c));

    //assert success if the statement evaluates to false
    static const char * statements[] = {
      "false",
      "faLSe",
      "FALSE",
      "no",
      "FAIL",
      "oFF",
      "0",
    };
    static const size_t num_statements = sizeof(statements) / sizeof(statements[0]);
    for (size_t i = 0; i < num_statements; i++)
    {
      const char * statement = statements[i];
      v.SetIsFalse(statement);
      ASSERT_TRUE(v.Validate(c)) << "Statement '" << statement << "' is expected to evaluate to false";
    }

    //test with multiple values
    std::string isfalse;

    //assert success if all statements evaluates to true
    isfalse.clear();
    isfalse += "no";
    isfalse += SA_ISTRUE_ATTR_SEPARATOR_STR;
    isfalse += "false";
    isfalse += SA_ISTRUE_ATTR_SEPARATOR_STR;
    isfalse += "off";
    v.SetIsFalse(isfalse);
    ASSERT_TRUE(v.Validate(c));

    //assert failure if the last element does not evaluates to false
    //if multiple values are specified, all values must evaluate to false for the validation to be successful.
    isfalse.clear();
    isfalse += "no";
    isfalse += SA_ISTRUE_ATTR_SEPARATOR_STR;
    isfalse += "false";
    isfalse += SA_ISTRUE_ATTR_SEPARATOR_STR;
    isfalse += "foo";
    v.SetIsFalse(isfalse);
    ASSERT_FALSE(v.Validate(c));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testIsEmpty)
  {
    Context c;
    Context::ElementList elements;
    elements.push_back( ra::process::GetCurrentProcessPath() );
    c.SetElements(elements);

    Validator v;

    //assert default
    ASSERT_TRUE(v.Validate(c));

    v.SetIsEmpty("");   // same as no value specified
    ASSERT_TRUE(v.Validate(c));

    //assert failure when a non-empty value is specified
    v.SetIsEmpty("foo");
    ASSERT_FALSE(v.Validate(c));

    v.SetIsEmpty("${property-that-does-not-exist}"); // similar to literal string "${property-that-does-not-exist}"
    ASSERT_FALSE(v.Validate(c));

    //create an empty property
    PropertyManager& pmgr = PropertyManager::GetInstance();
    pmgr.SetProperty("an-empty-property", "");

    v.SetIsEmpty("${an-empty-property}"); // expands to an empty string
    ASSERT_TRUE(v.Validate(c));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestValidator, testIsEmptyInversed)
  {
    Context c;
    Context::ElementList elements;
    elements.push_back( ra::process::GetCurrentProcessPath() );
    c.SetElements(elements);

    Validator v;

    v.SetInserve("isempty");

    //assert default
    ASSERT_TRUE(v.Validate(c));

    v.SetIsEmpty("");   // same as no value specified
    ASSERT_TRUE(v.Validate(c));

    //assert failure when a non-empty value is specified
    v.SetIsEmpty("foo");
    ASSERT_TRUE(v.Validate(c));

    v.SetIsEmpty("${property-that-does-not-exist}"); // similar to literal string "${property-that-does-not-exist}"
    ASSERT_TRUE(v.Validate(c));

    //create an empty property
    PropertyManager& pmgr = PropertyManager::GetInstance();
    pmgr.SetProperty("an-empty-property", "");

    v.SetIsEmpty("${an-empty-property}"); // expands to an empty string
    ASSERT_FALSE(v.Validate(c));
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace shellanything
