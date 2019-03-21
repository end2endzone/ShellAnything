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

#include "TestConfigManager.h"
#include "shellanything/ConfigManager.h"
#include "shellanything/PropertyManager.h"
#include "shellanything/Context.h"
#include "rapidassist/gtesthelp.h"

namespace shellanything { namespace test
{
  static const Configuration * INVALID_CONFIGURATION = NULL;
 
  size_t countLines(const std::string & value)
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
  void TestConfigManager::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestConfigManager::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testLoadFile)
  {
    ConfigManager & mgr = ConfigManager::getInstance();
    
    const std::string path = "test_files/demo.xml";
    std::string error_message = ra::gtesthelp::getTestQualifiedName(); //init error message to an unexpected string
    Configuration * config = mgr.loadFile(path, error_message);

    ASSERT_TRUE( error_message.empty() ) << "error_message=" << error_message;
    ASSERT_NE( INVALID_CONFIGURATION, config );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testRegisterUnregisterProperties)
  {
    ConfigManager & cmgr = ConfigManager::getInstance();
    PropertyManager & pmgr = PropertyManager::getInstance();
   
    Context context;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      context.setElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif
 
    ASSERT_FALSE( pmgr.hasProperty("selection.path") );
 
    //act
    cmgr.registerProperties(context);
 
    //assert
    ASSERT_TRUE( pmgr.hasProperty("selection.path") );
 
    //act
    cmgr.unregisterProperties(context);
 
    //assert
    ASSERT_FALSE( pmgr.hasProperty("selection.path") );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testRegisterPropertiesSingleFile)
  {
    ConfigManager & cmgr = ConfigManager::getInstance();
    PropertyManager & pmgr = PropertyManager::getInstance();
   
    Context context;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      context.setElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif
 
    ASSERT_FALSE( pmgr.hasProperty("selection.path") );
 
    //act
    cmgr.registerProperties(context);
 
    //assert
    ASSERT_TRUE( pmgr.hasProperty("selection.path") );
 
    const char * test_string = ""
      "selection.path"            "=${selection.path}"            "\n"
      "selection.parent.path"     "=${selection.parent.path}"     "\n"
      "selection.parent.filename" "=${selection.parent.filename}" "\n"
      "selection.filename"        "=${selection.filename}"        "\n"
      "selection.filename.noext"  "=${selection.filename.noext}"  "\n";
 
    const char * expected_string = ""
      "selection.path"            "=C:\\Windows\\System32\\notepad.exe\n"
      "selection.parent.path"     "=C:\\Windows\\System32\n"
      "selection.parent.filename" "=System32\n"
      "selection.filename"        "=notepad.exe\n"
      "selection.filename.noext"  "=notepad\n";
 
    std::string actual_string = pmgr.expand(test_string);
 
    ASSERT_EQ( std::string(expected_string), actual_string );
 
    cmgr.unregisterProperties(context);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testRegisterPropertiesMultipleFiles)
  {
    ConfigManager & cmgr = ConfigManager::getInstance();
    PropertyManager & pmgr = PropertyManager::getInstance();
   
    Context context;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\kernel32.dll");
      elements.push_back("C:\\Windows\\System32\\cmd.exe"     );
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      elements.push_back("C:\\Windows\\System32\\services.msc");
      context.setElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif
 
    ASSERT_FALSE( pmgr.hasProperty("selection.path") );
 
    //act
    cmgr.registerProperties(context);
 
    //assert
    ASSERT_TRUE( pmgr.hasProperty("selection.path") );
 
    //get all expanded properties
    std::string selection_path             = pmgr.expand("${selection.path}");
    std::string selection_parent_path      = pmgr.expand("${selection.parent.path}");
    std::string selection_parent_filename  = pmgr.expand("${selection.parent.filename}");
    std::string selection_filename         = pmgr.expand("${selection.filename}");
    std::string selection_filename_noext   = pmgr.expand("${selection.filename.noext}");
 
    //assert 1 line per element (file or directory) in context
    ASSERT_EQ( context.getElements().size(), countLines(selection_path           ) );
    ASSERT_EQ( context.getElements().size(), countLines(selection_parent_path    ) );
    ASSERT_EQ( context.getElements().size(), countLines(selection_parent_filename) );
    ASSERT_EQ( context.getElements().size(), countLines(selection_filename       ) );
    ASSERT_EQ( context.getElements().size(), countLines(selection_filename_noext ) );
 
    //assert all elements are added to the ${selection.path}
    for(size_t i=0; i<context.getElements().size(); i++)
    {
      const std::string & file_element = context.getElements()[i];
      bool file_element_found = (selection_path.find(file_element) != std::string::npos);
      ASSERT_TRUE( file_element_found ) << "selection_path=" << selection_path << "\n" << "file_element=" << file_element;
    }
  }
  //--------------------------------------------------------------------------------------------------
 
} //namespace test
} //namespace shellanything
