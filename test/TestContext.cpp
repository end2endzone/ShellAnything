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

#include "TestContext.h"
#include "shellanything/Context.h"
#include "PropertyManager.h"

namespace shellanything { namespace test
{
  size_t CountLines(const std::string & value)
  {
    size_t count = 0;
    if (!value.empty())
      count = 1;
 
    for(size_t i=0; i<value.size(); i++)
    {
      if (value[i] == '\n')
        count++;
    }
 
    return count;
  }
 
  //--------------------------------------------------------------------------------------------------
  void TestContext::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestContext::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestContext, testDefaults)
  {
    Context c;
    ASSERT_EQ( 0, c.GetNumFiles() );
    ASSERT_EQ( 0, c.GetNumDirectories() );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestContext, testFileDirectoryAutoDetect)
  {
    Context c;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\kernel32.dll");
      elements.push_back("C:\\Windows\\System32\\cmd.exe"     );
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      elements.push_back("C:\\Windows\\System32\\services.msc");
      elements.push_back("C:\\Program Files");
      elements.push_back("C:\\Users"        );
      elements.push_back("C:\\Windows"      );
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    ASSERT_EQ( 4, c.GetNumFiles() );
    ASSERT_EQ( 3, c.GetNumDirectories() );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestContext, testCopy)
  {
    Context c;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\kernel32.dll");
      elements.push_back("C:\\Windows\\System32\\cmd.exe"     );
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      elements.push_back("C:\\Windows\\System32\\services.msc");
      elements.push_back("C:\\Program Files");
      elements.push_back("C:\\Users"        );
      elements.push_back("C:\\Windows"      );
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    ASSERT_EQ( 4, c.GetNumFiles() );
    ASSERT_EQ( 3, c.GetNumDirectories() );

    //operator =
    Context c2;
    c2 = c;

    //assert both equals
    ASSERT_EQ( c.GetElements().size() , c2.GetElements().size() );
    ASSERT_EQ( c.GetNumFiles()        , c2.GetNumFiles()        );
    ASSERT_EQ( c.GetNumDirectories()  , c2.GetNumDirectories()  );
    for(size_t i=0; i<c.GetElements().size(); i++)
    {
      const std::string & c1_element =  c.GetElements()[i];
      const std::string & c2_element = c2.GetElements()[i];
      ASSERT_EQ(c1_element, c2_element);
    }

    //copy constructor
    Context c3(c);

    //assert both equals
    ASSERT_EQ( c.GetElements().size() , c3.GetElements().size() );
    ASSERT_EQ( c.GetNumFiles()        , c3.GetNumFiles()        );
    ASSERT_EQ( c.GetNumDirectories()  , c3.GetNumDirectories()  );
    for(size_t i=0; i<c.GetElements().size(); i++)
    {
      const std::string & c1_element =  c.GetElements()[i];
      const std::string & c3_element = c3.GetElements()[i];
      ASSERT_EQ(c1_element, c3_element);
    }

  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestContext, testRegisterUnregisterProperties)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
   
    Context context;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      context.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif
 
    ASSERT_FALSE( pmgr.HasProperty("selection.path") );
 
    //act
    context.RegisterProperties();
 
    //assert
    ASSERT_TRUE( pmgr.HasProperty("selection.path") );
 
    //act
    context.UnregisterProperties();
 
    //assert
    ASSERT_FALSE( pmgr.HasProperty("selection.path") );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestContext, testRegisterPropertiesSingleFile)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
   
    Context context;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      context.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif
 
    ASSERT_FALSE( pmgr.HasProperty("selection.path") );
 
    //act
    context.RegisterProperties();
 
    //assert
    ASSERT_TRUE( pmgr.HasProperty("selection.path") );
 
    const char * test_string = ""
      "selection.path"                "=${selection.path}"                "\n"
      "selection.parent.path"         "=${selection.parent.path}"         "\n"
      "selection.parent.filename"     "=${selection.parent.filename}"     "\n"
      "selection.filename"            "=${selection.filename}"            "\n"
      "selection.filename.noext"      "=${selection.filename.noext}"      "\n"
      "selection.filename.extension"  "=${selection.filename.extension}"  "\n";
 
    const char * expected_string = ""
      "selection.path"                "=C:\\Windows\\System32\\notepad.exe\n"
      "selection.parent.path"         "=C:\\Windows\\System32\n"
      "selection.parent.filename"     "=System32\n"
      "selection.filename"            "=notepad.exe\n"
      "selection.filename.noext"      "=notepad\n"
      "selection.filename.extension"  "=exe\n";
 
    std::string actual_string = pmgr.Expand(test_string);
 
    ASSERT_EQ( std::string(expected_string), actual_string );
 
    context.UnregisterProperties();
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestContext, testRegisterPropertiesMultipleFiles)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
   
    Context context;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\kernel32.dll");
      elements.push_back("C:\\Windows\\System32\\cmd.exe"     );
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      elements.push_back("C:\\Windows\\System32\\services.msc");
      context.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif
 
    ASSERT_FALSE( pmgr.HasProperty("selection.path") );
 
    //act
    context.RegisterProperties();
 
    //assert
    ASSERT_TRUE( pmgr.HasProperty("selection.path") );
 
    //get all expanded properties
    std::string selection_path             = pmgr.Expand("${selection.path}");
    std::string selection_parent_path      = pmgr.Expand("${selection.parent.path}");
    std::string selection_parent_filename  = pmgr.Expand("${selection.parent.filename}");
    std::string selection_filename         = pmgr.Expand("${selection.filename}");
    std::string selection_filename_noext   = pmgr.Expand("${selection.filename.noext}");
    std::string selection_filename_ext     = pmgr.Expand("${selection.filename.extension}");
 
    //assert 1 line per element (file or directory) in context
    ASSERT_EQ( context.GetElements().size(), CountLines(selection_path           ) );
    ASSERT_EQ( context.GetElements().size(), CountLines(selection_parent_path    ) );
    ASSERT_EQ( context.GetElements().size(), CountLines(selection_parent_filename) );
    ASSERT_EQ( context.GetElements().size(), CountLines(selection_filename       ) );
    ASSERT_EQ( context.GetElements().size(), CountLines(selection_filename_noext ) );
    ASSERT_EQ( context.GetElements().size(), CountLines(selection_filename_ext   ) );
 
    //assert all elements are added to the ${selection.path}
    for(size_t i=0; i<context.GetElements().size(); i++)
    {
      const std::string & file_element = context.GetElements()[i];
      bool file_element_found = (selection_path.find(file_element) != std::string::npos);
      ASSERT_TRUE( file_element_found ) << "selection_path=" << selection_path << "\n" << "file_element=" << file_element;
    }
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
