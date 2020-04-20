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

#include "rapidassist/testing.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/environment.h"
#include "rapidassist/timing.h"

namespace shellanything { namespace test
{
  static const Configuration * INVALID_CONFIGURATION = NULL;
 
  const char * toString(bool value)
  {
    if (value)
      return "true";
    return "false";
  }

  Context getContextSingleFile()
  {
    Context c;
    Context::ElementList elements;
#ifdef _WIN32
    elements.push_back("C:\\Windows\\System32\\notepad.exe" );
#else
    //TODO: complete with known path to files
#endif
    c.SetElements(elements);

    return c;
  }

  Context getContextSingleDirectory()
  {
    Context c;
    Context::ElementList elements;
#ifdef _WIN32
    elements.push_back("C:\\Program Files (x86)" );
#else
    //TODO: complete with known path to files
#endif
    c.SetElements(elements);

    return c;
  }

  void queryAllMenusRecursive(Menu * menu, Menu::MenuPtrList & list)
  {
    if (menu == NULL)
      return;

    list.push_back(menu);

    Menu::MenuPtrList submenus = menu->GetSubMenus();
    for(size_t i=0; i<submenus.size(); i++)
    {
      Menu * submenu = submenus[i];
      queryAllMenusRecursive(submenu, list);
    }
  }

  void queryAllMenusRecursive(Configuration * config, Menu::MenuPtrList & list)
  {
    if (config == NULL)
      return;

    Menu::MenuPtrList menus = config->GetMenus();
    for(size_t i=0; i<menus.size(); i++)
    {
      Menu * menu = menus[i];
      queryAllMenusRecursive(menu, list);
    }
  }
  
  Menu * querySubMenu(Menu * parent, size_t index)
  {
    if (parent == NULL)
      return NULL;

    Menu::MenuPtrList menus = parent->GetSubMenus();
    if (index >= menus.size())
      return NULL; //out of bounds

    Menu * m = menus[index];
    return m;
  }

  Menu * querySubMenu(Configuration * config, size_t index)
  {
    if (config == NULL)
      return NULL;

    Menu::MenuPtrList menus = config->GetMenus();
    if (index >= menus.size())
      return NULL; //out of bounds

    Menu * m = menus[index];
    return m;
  }

  //--------------------------------------------------------------------------------------------------
  void TestConfigManager::SetUp()
  {
    //Delete the configurations which source files are deleted
    ConfigManager & cmgr = ConfigManager::GetInstance();
    cmgr.Refresh();

    //Delete the source file of all remaining Configuration instance
    Configuration::ConfigurationPtrList configs = cmgr.GetConfigurations();
    for(size_t i=0; i<configs.size(); i++)
    {
      Configuration * config = configs[i];
      if (config)
      {
        const std::string & file_path = config->GetFilePath();
        ASSERT_TRUE( ra::filesystem::DeleteFile(file_path.c_str()) ) << "Failed deleting file '" << file_path << "'.";
      }
    }

    //Now that all configuration files are deleted, refresh again
    cmgr.Refresh();

    //ASSERT that no files are loaded
    ASSERT_EQ( 0, cmgr.GetConfigurations().size() );
  }
  //--------------------------------------------------------------------------------------------------
  void TestConfigManager::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testDetectNewFile)
  {
    ConfigManager & cmgr = ConfigManager::GetInstance();
 
    static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
 
    //copy test template file to a temporary subdirectory to allow editing the file during the test
    std::string test_name = ra::testing::GetTestQualifiedName();
    std::string template_source_path = std::string("test_files") + path_separator + test_name + ".xml";
    std::string template_target_path1 = std::string("test_files") + path_separator + test_name + path_separator + "tmp1.xml";
    std::string template_target_path2 = std::string("test_files") + path_separator + test_name + path_separator + "tmp2.xml";
 
    //make sure the target directory exists
    std::string template_target_dir = ra::filesystem::GetParentPath(template_target_path1);
    ASSERT_TRUE( ra::filesystem::CreateDirectory(template_target_dir.c_str()) ) << "Failed creating directory '" << template_target_dir << "'.";
 
    //copy the file
    ASSERT_TRUE( ra::filesystem::CopyFile(template_source_path, template_target_path1) ) << "Failed copying file '" << template_source_path << "' to file '" << template_target_path1 << "'.";
    
    //setup ConfigManager to read files from template_target_dir
    cmgr.ClearSearchPath();
    cmgr.AddSearchPath(template_target_dir);
    cmgr.Refresh();
 
    //ASSERT the file is loaded
    Configuration::ConfigurationPtrList configs = cmgr.GetConfigurations();
    ASSERT_EQ( 1, configs.size() );
 
    //ASSERT a single menu is available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
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
    static const std::string LINE_SEPARATOR = ra::environment::GetLineSeparator();
    if (LINE_SEPARATOR != "\n")
    {
      ra::strings::Replace(CONFIG_XML, "\n", LINE_SEPARATOR);
    }
    bool fileWrite = ra::filesystem::WriteFile(template_target_path2, CONFIG_XML);
    ASSERT_TRUE(fileWrite);
 
    //refresh the ConfigurationManager to see if it picked up the new file
    cmgr.Refresh();
 
    //ASSERT both file is loaded
    configs = cmgr.GetConfigurations();
    ASSERT_EQ( 2, configs.size() );

    //cleanup
    ASSERT_TRUE( ra::filesystem::DeleteFile(template_target_path1.c_str()) ) << "Failed deleting file '" << template_target_path1 << "'.";
    ASSERT_TRUE( ra::filesystem::DeleteFile(template_target_path2.c_str()) ) << "Failed deleting file '" << template_target_path2 << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testFileModifications)
  {
    ConfigManager & cmgr = ConfigManager::GetInstance();
 
    static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
 
    //copy test template file to a temporary subdirectory to allow editing the file during the test
    std::string test_name = ra::testing::GetTestQualifiedName();
    std::string template_source_path = std::string("test_files") + path_separator + test_name + ".xml";
    std::string template_target_path = std::string("test_files") + path_separator + test_name + path_separator + "tmp.xml";
 
    //make sure the target directory exists
    std::string template_target_dir = ra::filesystem::GetParentPath(template_target_path);
    ASSERT_TRUE( ra::filesystem::CreateDirectory(template_target_dir.c_str()) ) << "Failed creating directory '" << template_target_dir << "'.";
 
    //copy the file
    ASSERT_TRUE( ra::filesystem::CopyFile(template_source_path, template_target_path) ) << "Failed copying file '" << template_source_path << "' to file '" << template_target_path << "'.";
    
    //wait to make sure that the next files not dated the same date as this copy
    ra::timing::Millisleep(1500);

    //setup ConfigManager to read files from template_target_dir
    cmgr.ClearSearchPath();
    cmgr.AddSearchPath(template_target_dir);
    cmgr.Refresh();
 
    //ASSERT the file is loaded
    Configuration::ConfigurationPtrList configs = cmgr.GetConfigurations();
    ASSERT_EQ( 1, configs.size() );
 
    //ASSERT a single menu is available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ( 1, menus.size() );
 
    //inject another menu in the loaded xml file
 
    //prepare XML content to add
    std::string MENU_XML = "<menu name=\"Start notepad.exe\">\n"
      "      <actions>\n"
      "        <exec path=\"C:\\windows\\system32\\notepad.exe\" />\n"
      "      </actions>\n"
      "    </menu>";
    static const std::string LINE_SEPARATOR = ra::environment::GetLineSeparator();
    if (LINE_SEPARATOR != "\n")
    {
      ra::strings::Replace(MENU_XML, "\n", LINE_SEPARATOR);
    }
 
    //process with file search and replace
    std::string content;
    bool fileReaded = ra::filesystem::ReadFile(template_target_path, content);
    ASSERT_TRUE(fileReaded);
    ra::strings::Replace(content, "<!-- CODE INSERT LOCATION -->", MENU_XML);
    bool fileWrite = ra::filesystem::WriteFile(template_target_path, content);
    ASSERT_TRUE(fileWrite);
 
    //refresh the ConfigurationManager to see if it picked up the new file
    cmgr.Refresh();
 
    //ASSERT the file is loaded
    configs = cmgr.GetConfigurations();
    ASSERT_EQ( 1, configs.size() );
 
    //ASSERT 2 menus is available
    menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ( 2, menus.size() );

    //cleanup
    ASSERT_TRUE( ra::filesystem::DeleteFile(template_target_path.c_str()) ) << "Failed deleting file '" << template_target_path << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testAssignCommandId)
  {
    ConfigManager & cmgr = ConfigManager::GetInstance();
    static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
    //copy test template file to a temporary subdirectory to allow editing the file during the test
    std::string test_name = ra::testing::GetTestQualifiedName();
    std::string template_source_path1 = std::string("test_files") + path_separator + test_name + ".1.xml";
    std::string template_source_path2 = std::string("test_files") + path_separator + test_name + ".2.xml";
    std::string template_target_path1 = std::string("test_files") + path_separator + test_name + path_separator + "tmp.1.xml";
    std::string template_target_path2 = std::string("test_files") + path_separator + test_name + path_separator + "tmp.2.xml";
    //make sure the target directory exists
    std::string template_target_dir = ra::filesystem::GetParentPath(template_target_path1);
    ASSERT_TRUE( ra::filesystem::CreateDirectory(template_target_dir.c_str()) ) << "Failed creating directory '" << template_target_dir << "'.";
    //copy the files
    ASSERT_TRUE( ra::filesystem::CopyFile(template_source_path1, template_target_path1) ) << "Failed copying file '" << template_source_path1 << "' to file '" << template_target_path1 << "'.";
    ASSERT_TRUE( ra::filesystem::CopyFile(template_source_path2, template_target_path2) ) << "Failed copying file '" << template_source_path2 << "' to file '" << template_target_path2 << "'.";
   
    //wait to make sure that the next files not dated the same date as this copy
    ra::timing::Millisleep(1500);
 
    //setup ConfigManager to read files from template_target_dir
    cmgr.ClearSearchPath();
    cmgr.AddSearchPath(template_target_dir);
    cmgr.Refresh();
    //ASSERT the files are loaded
    Configuration::ConfigurationPtrList configs = cmgr.GetConfigurations();
    ASSERT_EQ( 2, configs.size() );
    //assign unique command ids
    uint32_t nextCommandId = cmgr.AssignCommandIds(101);
    ASSERT_EQ( 112, nextCommandId ); //assert 11 Menu(s) loaded by ConfigManager
 
    //assert invalid command id
    ASSERT_EQ( (Menu*)NULL, cmgr.FindMenuByCommandId(99999999) );
 
    //find known Menu by known command id
    Menu * wFooServiceMenu = cmgr.FindMenuByCommandId(101);
    Menu *    wRestartMenu = cmgr.FindMenuByCommandId(104);
    Menu *     wWinzipMenu = cmgr.FindMenuByCommandId(108);
    ASSERT_TRUE( wFooServiceMenu != NULL );
    ASSERT_TRUE(    wRestartMenu != NULL );
    ASSERT_TRUE(     wWinzipMenu != NULL );
    ASSERT_EQ( std::string("Foo Service"),  wFooServiceMenu->GetName() );
    ASSERT_EQ( std::string("Restart"),      wRestartMenu->GetName() );
    ASSERT_EQ( std::string("Winzip"),       wWinzipMenu->GetName() );
 
    //cleanup
    ASSERT_TRUE( ra::filesystem::DeleteFile(template_target_path1.c_str()) ) << "Failed deleting file '" << template_target_path1 << "'.";
    ASSERT_TRUE( ra::filesystem::DeleteFile(template_target_path2.c_str()) ) << "Failed deleting file '" << template_target_path2 << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testDescription)
  {
    ConfigManager & mgr = ConfigManager::GetInstance();
    
    const std::string path = "configurations/default.xml";
    std::string error_message = ra::testing::GetTestQualifiedName(); //init error message to an unexpected string
    Configuration * config = Configuration::LoadFile(path, error_message);

    ASSERT_TRUE( error_message.empty() ) << "error_message=" << error_message;
    ASSERT_NE( INVALID_CONFIGURATION, config );

    //search for an menu with "Run" and "parameters" in title
    Menu::MenuPtrList menus = config->GetMenus();
    Menu * run_with_params = NULL;
    for(size_t i=0; i<menus.size(); i++)
    {
      Menu * menu = menus[i];
      const std::string & name = menu->GetName();
      if (name.find("Run") != std::string::npos && name.find("parameters") != std::string::npos)
      {
        //found our Menu
        run_with_params = menu;
      }
    }
    ASSERT_TRUE( run_with_params != NULL );

    //look for a description
    const std::string & desc = run_with_params->GetDescription();
    ASSERT_EQ(std::string(""), desc);

    //cleanup
    delete config;
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testClear)
  {
    ConfigManager & cmgr = ConfigManager::GetInstance();
 
    static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
 
    //copy test template file to a temporary subdirectory to allow editing the file during the test
    std::string test_name = ra::testing::GetTestQualifiedName();
    std::string template_source_path = std::string("test_files") + path_separator + test_name + ".xml";
    std::string template_target_path = std::string("test_files") + path_separator + test_name + path_separator + "tmp.xml";
 
    //make sure the target directory exists
    std::string template_target_dir = ra::filesystem::GetParentPath(template_target_path);
    ASSERT_TRUE( ra::filesystem::CreateDirectory(template_target_dir.c_str()) ) << "Failed creating directory '" << template_target_dir << "'.";
 
    //copy the file
    ASSERT_TRUE( ra::filesystem::CopyFile(template_source_path, template_target_path) ) << "Failed copying file '" << template_source_path << "' to file '" << template_target_path << "'.";
    
    //wait to make sure that the next files not dated the same date as this copy
    ra::timing::Millisleep(1500);

    //setup ConfigManager to read files from template_target_dir
    cmgr.ClearSearchPath();
    cmgr.AddSearchPath(template_target_dir);
    cmgr.Refresh();
 
    //ASSERT the file is loaded
    Configuration::ConfigurationPtrList configs = cmgr.GetConfigurations();
    ASSERT_EQ( 1, configs.size() );
 
    //act
    cmgr.Clear();

    //ASSERT nothing is loaded
    configs = cmgr.GetConfigurations();
    ASSERT_EQ( 0, configs.size() );

    //cleanup
    ASSERT_TRUE( ra::filesystem::DeleteFile(template_target_path.c_str()) ) << "Failed deleting file '" << template_target_path << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testParentWithoutChildren)
  {
    ConfigManager & cmgr = ConfigManager::GetInstance();
 
    static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
 
    //copy test template file to a temporary subdirectory to allow editing the file during the test
    std::string test_name = ra::testing::GetTestQualifiedName();
    std::string template_source_path = std::string("test_files") + path_separator + test_name + ".xml";
    std::string template_target_path = std::string("test_files") + path_separator + test_name + path_separator + "tmp.xml";
 
    //make sure the target directory exists
    std::string template_target_dir = ra::filesystem::GetParentPath(template_target_path);
    ASSERT_TRUE( ra::filesystem::CreateDirectory(template_target_dir.c_str()) ) << "Failed creating directory '" << template_target_dir << "'.";
 
    //copy the file
    ASSERT_TRUE( ra::filesystem::CopyFile(template_source_path, template_target_path) ) << "Failed copying file '" << template_source_path << "' to file '" << template_target_path << "'.";
    
    //wait to make sure that the next files not dated the same date as this copy
    ra::timing::Millisleep(1500);

    //setup ConfigManager to read files from template_target_dir
    cmgr.ClearSearchPath();
    cmgr.AddSearchPath(template_target_dir);
    cmgr.Refresh();
 
    //ASSERT the file is loaded
    Configuration::ConfigurationPtrList configs = cmgr.GetConfigurations();
    ASSERT_EQ( 1, configs.size() );
 
    //query first menu
    Menu::MenuPtrList menus = configs[0]->GetMenus();
    ASSERT_EQ( 1, menus.size() );
    Menu * first = menus[0];
    ASSERT_TRUE( first != NULL );

    //update the menus based on a context with a single file
    Context context = getContextSingleFile();
    cmgr.Update(context);

    //ASSERT top menu is visible (default option)
    ASSERT_TRUE( first->IsVisible() );

    //update the menus based on a context with a single directory
    context = getContextSingleDirectory();
    cmgr.Update(context);

    //ASSERT top menu is invisible (issue #4)
    ASSERT_FALSE( first->IsVisible() );

    //get all submenus
    Menu * option1 = first;
    ASSERT_TRUE( option1 != NULL );
    Menu * option1_1 = querySubMenu(option1, 0);
    ASSERT_TRUE( option1_1 != NULL );
    Menu * option1_1_1 = querySubMenu(option1_1, 0);
    ASSERT_TRUE( option1_1_1 != NULL );
    ASSERT_FALSE( option1_1_1->IsParentMenu() ); //menu is a leaf

    //assert all sub menus are also invisible
    ASSERT_FALSE( option1->IsVisible() );
    ASSERT_FALSE( option1_1->IsVisible() );
    ASSERT_FALSE( option1_1_1->IsVisible() );

    //cleanup
    ASSERT_TRUE( ra::filesystem::DeleteFile(template_target_path.c_str()) ) << "Failed deleting file '" << template_target_path << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testAssignCommandIdsInvalid)
  {
    ConfigManager & cmgr = ConfigManager::GetInstance();
 
    static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
 
    //copy test template file to a temporary subdirectory to allow editing the file during the test
    std::string test_name = ra::testing::GetTestQualifiedName();
    std::string template_source_path = std::string("test_files") + path_separator + test_name + ".xml";
    std::string template_target_path = std::string("test_files") + path_separator + test_name + path_separator + "tmp.xml";
 
    //make sure the target directory exists
    std::string template_target_dir = ra::filesystem::GetParentPath(template_target_path);
    ASSERT_TRUE( ra::filesystem::CreateDirectory(template_target_dir.c_str()) ) << "Failed creating directory '" << template_target_dir << "'.";
 
    //copy the file
    ASSERT_TRUE( ra::filesystem::CopyFile(template_source_path, template_target_path) ) << "Failed copying file '" << template_source_path << "' to file '" << template_target_path << "'.";
    
    //wait to make sure that the next files not dated the same date as this copy
    ra::timing::Millisleep(1500);

    //setup ConfigManager to read files from template_target_dir
    cmgr.ClearSearchPath();
    cmgr.AddSearchPath(template_target_dir);
    cmgr.Refresh();
 
    //ASSERT the file is loaded
    Configuration::ConfigurationPtrList configs = cmgr.GetConfigurations();
    ASSERT_EQ( 1, configs.size() );
 
    //query all menus
    Menu * option1 = querySubMenu(configs[0], 0);
    ASSERT_TRUE( option1 != NULL );
    Menu * option1_1 = querySubMenu(option1, 0);
    Menu * option1_2 = querySubMenu(option1, 1);
    Menu * option1_3 = querySubMenu(option1, 2);
    ASSERT_TRUE( option1_1 != NULL );
    ASSERT_TRUE( option1_2 != NULL );
    ASSERT_TRUE( option1_3 != NULL );
    Menu * option1_2_1    = querySubMenu(option1_2,   0);
    Menu * option1_2_1_1  = querySubMenu(option1_2_1, 0);
    ASSERT_TRUE( option1_2_1    != NULL );
    ASSERT_TRUE( option1_2_1_1  != NULL );

    //query all menus
    Menu::MenuPtrList menus;
    queryAllMenusRecursive(configs[0], menus);

    //update the menus based on a context with a single file
    Context context = getContextSingleFile();
    cmgr.Update(context);

    //assign unique command ids
    uint32_t nextCommandId = cmgr.AssignCommandIds(101);
    ASSERT_EQ( 103, nextCommandId ); //assert 2 Menu(s) visible by ConfigManager

    //assert that all visible menu have an assigned command id (issue #5)
    for(size_t i=0; i<menus.size(); i++)
    {
      Menu * m = menus[i];
      std::string message = ra::strings::Format("Error. Menu '%s' which visible is '%s' and command_id is '%d' was not expected.", m->GetName().c_str(), toString(m->IsVisible()), m->GetCommandId());
      if (m->IsVisible())
        ASSERT_NE(Menu::INVALID_COMMAND_ID, m->GetCommandId() ) << message; //visible menus must have a valid command id
      else
        ASSERT_EQ(Menu::INVALID_COMMAND_ID, m->GetCommandId() ) << message; //invisible menus must have an invalid command id
    }

    //expected assigned command ids
    ASSERT_EQ( 101, option1->GetCommandId() );
    ASSERT_EQ( 102, option1_1->GetCommandId() );
    ASSERT_EQ( Menu::INVALID_COMMAND_ID, option1_2->GetCommandId() );
    ASSERT_EQ( Menu::INVALID_COMMAND_ID, option1_3->GetCommandId() );
    ASSERT_EQ( Menu::INVALID_COMMAND_ID, option1_2_1  ->GetCommandId() );
    ASSERT_EQ( Menu::INVALID_COMMAND_ID, option1_2_1_1->GetCommandId() );

    //cleanup
    ASSERT_TRUE( ra::filesystem::DeleteFile(template_target_path.c_str()) ) << "Failed deleting file '" << template_target_path << "'.";
  }
  //--------------------------------------------------------------------------------------------------
 
} //namespace test
} //namespace shellanything
