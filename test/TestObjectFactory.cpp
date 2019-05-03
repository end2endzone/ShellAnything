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

#include "TestObjectFactory.h"
#include "shellanything/ConfigManager.h"
#include "shellanything/Context.h"
#include "Platform.h"

#include "rapidassist/gtesthelp.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/environment.h"
#include "rapidassist/time_.h"

namespace shellanything { namespace test
{
  static const Configuration * INVALID_CONFIGURATION = NULL;
 
  //--------------------------------------------------------------------------------------------------
  void TestObjectFactory::SetUp()
  {
    //Delete the configurations which source files are deleted
    ConfigManager & cmgr = ConfigManager::getInstance();
    cmgr.refresh();

    //Delete the source file of all remaining Configuration instance
    Configuration::ConfigurationPtrList configs = cmgr.getConfigurations();
    for(size_t i=0; i<configs.size(); i++)
    {
      Configuration * config = configs[i];
      if (config)
      {
        const std::string & file_path = config->getFilePath();
        ASSERT_TRUE( ra::filesystem::deleteFile(file_path.c_str()) ) << "Failed deleting file '" << file_path << "'.";
      }
    }

    //Now that all configuration files are deleted, refresh again
    cmgr.refresh();

    //ASSERT that no files are loaded
    ASSERT_EQ( 0, cmgr.getConfigurations().size() );
  }
  //--------------------------------------------------------------------------------------------------
  void TestObjectFactory::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseValidator)
  {
    ConfigManager & cmgr = ConfigManager::getInstance();
 
    static const std::string path_separator = ra::filesystem::getPathSeparatorStr();
 
    //copy test template file to a temporary subdirectory to allow editing the file during the test
    std::string test_name = ra::gtesthelp::getTestQualifiedName();
    std::string template_source_path = std::string("test_files") + path_separator + test_name + ".xml";
    std::string template_target_path = std::string("test_files") + path_separator + test_name + path_separator + "tmp.xml";
 
    //make sure the target directory exists
    std::string template_target_dir = ra::filesystem::getParentPath(template_target_path);
    ASSERT_TRUE( ra::filesystem::createFolder(template_target_dir.c_str()) ) << "Failed creating directory '" << template_target_dir << "'.";
 
    //copy the file
    ASSERT_TRUE( copyFile(template_source_path, template_target_path) ) << "Failed copying file '" << template_source_path << "' to file '" << template_target_path << "'.";
    
    //wait to make sure that the next files not dated the same date as this copy
    ra::time::millisleep(1500);

    //setup ConfigManager to read files from template_target_dir
    cmgr.clearSearchPath();
    cmgr.addSearchPath(template_target_dir);
    cmgr.refresh();
 
    //ASSERT the file is loaded
    Configuration::ConfigurationPtrList configs = cmgr.getConfigurations();
    ASSERT_EQ( 1, configs.size() );
 
    //ASSERT a 4 menus are available
    Menu::MenuPtrList menus = cmgr.getConfigurations()[0]->getMenus();
    ASSERT_EQ( 4, menus.size() );

    //assert <vasibility> tag properly parsed
    static const std::string expected_property = "bar";
    static const std::string expected_file_extension = "com;exe;bat;cmd";
    static const std::string expected_file_exists = "C:\\Users\\Public|C:\\Program Files (x86)";

    ASSERT_EQ( expected_property,       menus[00]->getVisibility().getProperties() );
    ASSERT_EQ( 5,                       menus[01]->getVisibility().getMaxFiles() );
    ASSERT_EQ( 6,                       menus[01]->getVisibility().getMaxDirectories() );
    ASSERT_EQ( expected_file_extension, menus[02]->getVisibility().getFileExtensions() );
    ASSERT_EQ( expected_file_exists,    menus[03]->getVisibility().getFileExists() );

    //cleanup
    ASSERT_TRUE( ra::filesystem::deleteFile(template_target_path.c_str()) ) << "Failed deleting file '" << template_target_path << "'.";
  }
  //--------------------------------------------------------------------------------------------------
 
} //namespace test
} //namespace shellanything
