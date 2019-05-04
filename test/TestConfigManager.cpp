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
  void TestConfigManager::SetUp()
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
  void TestConfigManager::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testLoadFile)
  {
    ConfigManager & mgr = ConfigManager::getInstance();
    
    const std::string path = "test_files/default.xml";
    std::string error_message = ra::gtesthelp::getTestQualifiedName(); //init error message to an unexpected string
    Configuration * config = mgr.loadFile(path, error_message);

    ASSERT_TRUE( error_message.empty() ) << "error_message=" << error_message;
    ASSERT_NE( INVALID_CONFIGURATION, config );

    //cleanup
    delete config;
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testDetectNewFile)
  {
    ConfigManager & cmgr = ConfigManager::getInstance();
 
    static const std::string path_separator = ra::filesystem::getPathSeparatorStr();
 
    //copy test template file to a temporary subdirectory to allow editing the file during the test
    std::string test_name = ra::gtesthelp::getTestQualifiedName();
    std::string template_source_path = std::string("test_files") + path_separator + test_name + ".xml";
    std::string template_target_path1 = std::string("test_files") + path_separator + test_name + path_separator + "tmp1.xml";
    std::string template_target_path2 = std::string("test_files") + path_separator + test_name + path_separator + "tmp2.xml";
 
    //make sure the target directory exists
    std::string template_target_dir = ra::filesystem::getParentPath(template_target_path1);
    ASSERT_TRUE( ra::filesystem::createFolder(template_target_dir.c_str()) ) << "Failed creating directory '" << template_target_dir << "'.";
 
    //copy the file
    ASSERT_TRUE( copyFile(template_source_path, template_target_path1) ) << "Failed copying file '" << template_source_path << "' to file '" << template_target_path1 << "'.";
    
    //setup ConfigManager to read files from template_target_dir
    cmgr.clearSearchPath();
    cmgr.addSearchPath(template_target_dir);
    cmgr.refresh();
 
    //ASSERT the file is loaded
    Configuration::ConfigurationPtrList configs = cmgr.getConfigurations();
    ASSERT_EQ( 1, configs.size() );
 
    //ASSERT a single menu is available
    Menu::MenuPtrList menus = cmgr.getConfigurations()[0]->getMenus();
    ASSERT_EQ( 1, menus.size() );
 
    //create another file in the target directory
    std::string CONFIG_XML = ""
      "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
      "<root>\n"
      "  <shell>\n"
      "    <menu name=\"Start WordPad\">\n"
      "      <actions>\n"
      "        <exec path=\"C:\\windows\\system32\\write.exe\" />\n"
      "      </actions>\n"
      "    </menu>\n"
      "  </shell>\n"
      "</root>\n";
    static const std::string LINE_SEPARATOR = ra::environment::getLineSeparator();
    if (LINE_SEPARATOR != "\n")
    {
      ra::strings::replace(CONFIG_XML, "\n", LINE_SEPARATOR);
    }
    bool fileWrite = writeFile(template_target_path2, CONFIG_XML);
    ASSERT_TRUE(fileWrite);
 
    //refresh the ConfigurationManager to see if it picked up the new file
    cmgr.refresh();
 
    //ASSERT both file is loaded
    configs = cmgr.getConfigurations();
    ASSERT_EQ( 2, configs.size() );

    //cleanup
    ASSERT_TRUE( ra::filesystem::deleteFile(template_target_path1.c_str()) ) << "Failed deleting file '" << template_target_path1 << "'.";
    ASSERT_TRUE( ra::filesystem::deleteFile(template_target_path2.c_str()) ) << "Failed deleting file '" << template_target_path2 << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testFileModifications)
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
 
    //ASSERT a single menu is available
    Menu::MenuPtrList menus = cmgr.getConfigurations()[0]->getMenus();
    ASSERT_EQ( 1, menus.size() );
 
    //inject another menu in the loaded xml file
 
    //prepare XML content to add
    std::string MENU_XML = "<menu name=\"Start notepad.exe\">\n"
      "      <actions>\n"
      "        <exec path=\"C:\\windows\\system32\\notepad.exe\" />\n"
      "      </actions>\n"
      "    </menu>";
    static const std::string LINE_SEPARATOR = ra::environment::getLineSeparator();
    if (LINE_SEPARATOR != "\n")
    {
      ra::strings::replace(MENU_XML, "\n", LINE_SEPARATOR);
    }
 
    //process with file search and replace
    std::string content;
    bool fileReaded = readFile(template_target_path, content);
    ASSERT_TRUE(fileReaded);
    ra::strings::replace(content, "<!-- CODE INSERT LOCATION -->", MENU_XML);
    bool fileWrite = writeFile(template_target_path, content);
    ASSERT_TRUE(fileWrite);
 
    //refresh the ConfigurationManager to see if it picked up the new file
    cmgr.refresh();
 
    //ASSERT the file is loaded
    configs = cmgr.getConfigurations();
    ASSERT_EQ( 1, configs.size() );
 
    //ASSERT 2 menus is available
    menus = cmgr.getConfigurations()[0]->getMenus();
    ASSERT_EQ( 2, menus.size() );

    //cleanup
    ASSERT_TRUE( ra::filesystem::deleteFile(template_target_path.c_str()) ) << "Failed deleting file '" << template_target_path << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testAssignCommandId)
  {
    ConfigManager & cmgr = ConfigManager::getInstance();
    static const std::string path_separator = ra::filesystem::getPathSeparatorStr();
    //copy test template file to a temporary subdirectory to allow editing the file during the test
    std::string test_name = ra::gtesthelp::getTestQualifiedName();
    std::string template_source_path1 = std::string("test_files") + path_separator + test_name + ".1.xml";
    std::string template_source_path2 = std::string("test_files") + path_separator + test_name + ".2.xml";
    std::string template_target_path1 = std::string("test_files") + path_separator + test_name + path_separator + "tmp.1.xml";
    std::string template_target_path2 = std::string("test_files") + path_separator + test_name + path_separator + "tmp.2.xml";
    //make sure the target directory exists
    std::string template_target_dir = ra::filesystem::getParentPath(template_target_path1);
    ASSERT_TRUE( ra::filesystem::createFolder(template_target_dir.c_str()) ) << "Failed creating directory '" << template_target_dir << "'.";
    //copy the files
    ASSERT_TRUE( copyFile(template_source_path1, template_target_path1) ) << "Failed copying file '" << template_source_path1 << "' to file '" << template_target_path1 << "'.";
    ASSERT_TRUE( copyFile(template_source_path2, template_target_path2) ) << "Failed copying file '" << template_source_path2 << "' to file '" << template_target_path2 << "'.";
   
    //wait to make sure that the next files not dated the same date as this copy
    ra::time::millisleep(1500);
 
    //setup ConfigManager to read files from template_target_dir
    cmgr.clearSearchPath();
    cmgr.addSearchPath(template_target_dir);
    cmgr.refresh();
    //ASSERT the files are loaded
    Configuration::ConfigurationPtrList configs = cmgr.getConfigurations();
    ASSERT_EQ( 2, configs.size() );
    //assign unique command ids
    uint32_t nextCommandId = cmgr.assignCommandIds(101);
    ASSERT_EQ( 112, nextCommandId ); //assert 11 Menu(s) loaded by ConfigManager
 
    //assert invalid command id
    ASSERT_EQ( (Menu*)NULL, cmgr.findMenuByCommandId(99999999) );
 
    //find known Menu by known command id
    Menu * wFooServiceMenu = cmgr.findMenuByCommandId(101);
    Menu *    wRestartMenu = cmgr.findMenuByCommandId(104);
    Menu *     wWinzipMenu = cmgr.findMenuByCommandId(108);
    ASSERT_TRUE( wFooServiceMenu != NULL );
    ASSERT_TRUE(    wRestartMenu != NULL );
    ASSERT_TRUE(     wWinzipMenu != NULL );
    ASSERT_EQ( std::string("Foo Service"),  wFooServiceMenu->getName() );
    ASSERT_EQ( std::string("Restart"),      wRestartMenu->getName() );
    ASSERT_EQ( std::string("Winzip"),       wWinzipMenu->getName() );
 
    //cleanup
    ASSERT_TRUE( ra::filesystem::deleteFile(template_target_path1.c_str()) ) << "Failed deleting file '" << template_target_path1 << "'.";
    ASSERT_TRUE( ra::filesystem::deleteFile(template_target_path2.c_str()) ) << "Failed deleting file '" << template_target_path2 << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testDescription)
  {
    ConfigManager & mgr = ConfigManager::getInstance();
    
    const std::string path = "test_files/default.xml";
    std::string error_message = ra::gtesthelp::getTestQualifiedName(); //init error message to an unexpected string
    Configuration * config = mgr.loadFile(path, error_message);

    ASSERT_TRUE( error_message.empty() ) << "error_message=" << error_message;
    ASSERT_NE( INVALID_CONFIGURATION, config );

    //search for an menu with "Run" and "parameters" in title
    Menu::MenuPtrList menus = config->getMenus();
    Menu * run_with_params = NULL;
    for(size_t i=0; i<menus.size(); i++)
    {
      Menu * menu = menus[i];
      const std::string & name = menu->getName();
      if (name.find("Run") != std::string::npos && name.find("parameters") != std::string::npos)
      {
        //found our Menu
        run_with_params = menu;
      }
    }
    ASSERT_TRUE( run_with_params != NULL );

    //look for a description
    const std::string & desc = run_with_params->getDescription();
    ASSERT_EQ(std::string(""), desc);

    //cleanup
    delete config;
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testIsConfigurationFile)
  {
    ConfigManager & mgr = ConfigManager::getInstance();

    static const char * files[] = {
      "test_files\\default.xml",
      "test_files\\Microsoft Office 2003.xml",
      "test_files\\Microsoft Office 2007.xml",
      "test_files\\Microsoft Office 2010.xml",
      "test_files\\Microsoft Office 2013.xml",
      "test_files\\Microsoft Office 2016.xml",
      "test_files\\samples.xml",
      "test_files\\TestConfigManager.testAssignCommandId.1.xml",
      "test_files\\TestConfigManager.testAssignCommandId.2.xml",
      "test_files\\TestConfigManager.testDetectNewFile.xml",
      "test_files\\TestConfigManager.testFileModifications.xml",
      "test_files\\TestObjectFactory.testParseValidator.xml",
      "test_files\\tests.xml",
      "test_files\\WinDirStat.xml",
    };
    const size_t num_files = sizeof(files)/sizeof(files[0]);

    //for each test files
    for(size_t i=0; i<num_files; i++)
    {
      const std::string path = files[i];
      ASSERT_TRUE( ra::filesystem::fileExists(path.c_str()) );
      ASSERT_TRUE( ConfigManager::isConfigurationFile(path) );
    }
  }
  //--------------------------------------------------------------------------------------------------
 
} //namespace test
} //namespace shellanything
