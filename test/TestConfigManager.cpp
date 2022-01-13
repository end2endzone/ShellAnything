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
#include "Workspace.h"
#include "ConfigManager.h"
#include "Context.h"

#include "rapidassist/testing.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/environment.h"
#include "rapidassist/timing.h"

namespace shellanything { namespace test
{
  static const Configuration * INVALID_CONFIGURATION = NULL;
 
  const char * ToBooleanString(bool value)
  {
    if (value)
      return "true";
    return "false";
  }

  Context GetContextSingleFile()
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

  Context GetContextSingleDirectory()
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

  void QueryAllMenusRecursive(Menu * menu, Menu::MenuPtrList & list)
  {
    if (menu == NULL)
      return;

    list.push_back(menu);

    Menu::MenuPtrList submenus = menu->GetSubMenus();
    for(size_t i=0; i<submenus.size(); i++)
    {
      Menu * submenu = submenus[i];
      QueryAllMenusRecursive(submenu, list);
    }
  }

  void QueryAllMenusRecursive(Configuration * config, Menu::MenuPtrList & list)
  {
    if (config == NULL)
      return;

    Menu::MenuPtrList menus = config->GetMenus();
    for(size_t i=0; i<menus.size(); i++)
    {
      Menu * menu = menus[i];
      QueryAllMenusRecursive(menu, list);
    }
  }
  
  Menu * QuerySubMenu(Menu * parent, size_t index)
  {
    if (parent == NULL)
      return NULL;

    Menu::MenuPtrList menus = parent->GetSubMenus();
    if (index >= menus.size())
      return NULL; //out of bounds

    Menu * m = menus[index];
    return m;
  }

  Menu * QuerySubMenu(Configuration * config, size_t index)
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
 
    //Creating a temporary workspace for the test execution.
    Workspace workspace;
    ASSERT_FALSE(workspace.GetBaseDirectory().empty());

    //Define the path to the template configuration file
    static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
    std::string test_name = ra::testing::GetTestQualifiedName();
    std::string template_source_path = std::string("test_files") + path_separator + test_name + ".xml";

    //Import the required files into the workspace
    ASSERT_TRUE(workspace.ImportFileUtf8(template_source_path.c_str()));

    //Setup ConfigManager to read files from workspace
    cmgr.ClearSearchPath();
    cmgr.AddSearchPath(workspace.GetBaseDirectory());
    cmgr.Refresh();
 
    //ASSERT the file is loaded
    Configuration::ConfigurationPtrList configs = cmgr.GetConfigurations();
    ASSERT_EQ( 1, configs.size() );
 
    //ASSERT a single menu is available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ( 1, menus.size() );
 
    //Import another file into the workspace
    static const std::string CONFIG_XML = ""
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
    bool file_write = ra::filesystem::WriteTextFile(workspace.GetFullPathUtf8("another.xml"), CONFIG_XML);
    ASSERT_TRUE(file_write);
 
    //Refresh the ConfigurationManager to see if it picked up the new file
    cmgr.Refresh();
 
    //ASSERT both file is loaded
    configs = cmgr.GetConfigurations();
    ASSERT_EQ( 2, configs.size() );

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testFileModifications)
  {
    ConfigManager & cmgr = ConfigManager::GetInstance();
 
    //Creating a temporary workspace for the test execution.
    Workspace workspace;
    ASSERT_FALSE(workspace.GetBaseDirectory().empty());

    //Import the required files into the workspace
    static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
    std::string template_source_path = std::string("test_files") + path_separator + ra::testing::GetTestQualifiedName() + ".xml";
    ASSERT_TRUE(workspace.ImportAndRenameFileUtf8(template_source_path.c_str(), "tmp.xml"));
    
    //Wait to make sure that the next file copy/modification will not have the same timestamp
    ra::timing::Millisleep(1500);

    //Setup ConfigManager to read files from workspace
    cmgr.ClearSearchPath();
    cmgr.AddSearchPath(workspace.GetBaseDirectory());
    cmgr.Refresh();
 
    //ASSERT the file is loaded
    Configuration::ConfigurationPtrList configs = cmgr.GetConfigurations();
    ASSERT_EQ( 1, configs.size() );
 
    //ASSERT a single menu is available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ( 1, menus.size() );
 
    //Inject another menu in the loaded xml file
 
    //Prepare XML content to add
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
 
    //Process with file search and replace
    const std::string target_path = workspace.GetFullPathUtf8("tmp.xml");
    std::string content;
    bool file_readed = ra::filesystem::ReadFile(target_path, content);
    ASSERT_TRUE(file_readed);
    ra::strings::Replace(content, "<!-- CODE INSERT LOCATION -->", MENU_XML);
    bool file_write = ra::filesystem::WriteFile(target_path, content);
    ASSERT_TRUE(file_write);
 
    //Refresh the ConfigurationManager to see if it picked up the new file
    cmgr.Refresh();
 
    //ASSERT the file is loaded
    configs = cmgr.GetConfigurations();
    ASSERT_EQ( 1, configs.size() );
 
    //ASSERT 2 menus is available
    menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ( 2, menus.size() );

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testAssignCommandId)
  {
    ConfigManager & cmgr = ConfigManager::GetInstance();

    //Creating a temporary workspace for the test execution.
    Workspace workspace;
    ASSERT_FALSE(workspace.GetBaseDirectory().empty());

    //Import the required files into the workspace
    static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
    std::string test_name = ra::testing::GetTestQualifiedName();
    std::string template_source_path1 = std::string("test_files") + path_separator + test_name + ".1.xml";
    std::string template_source_path2 = std::string("test_files") + path_separator + test_name + ".2.xml";
    ASSERT_TRUE(workspace.ImportAndRenameFileUtf8(template_source_path1.c_str(), "tmp1.xml"));
    ASSERT_TRUE(workspace.ImportAndRenameFileUtf8(template_source_path2.c_str(), "tmp2.xml"));

    //Wait to make sure that the next file copy/modification will not have the same timestamp
    ra::timing::Millisleep(1500);
 
    //Setup ConfigManager to read files from workspace
    cmgr.ClearSearchPath();
    cmgr.AddSearchPath(workspace.GetBaseDirectory());
    cmgr.Refresh();

    //ASSERT the files are loaded
    Configuration::ConfigurationPtrList configs = cmgr.GetConfigurations();
    ASSERT_EQ( 2, configs.size() );

    //Assign unique command ids
    uint32_t nextCommandId = cmgr.AssignCommandIds(101);
    ASSERT_EQ( 112, nextCommandId ); //assert 11 Menu(s) loaded by ConfigManager
 
    //Assert invalid command id
    ASSERT_EQ( (Menu*)NULL, cmgr.FindMenuByCommandId(99999999) );
 
    //Find known Menu by known command id
    Menu * wFooServiceMenu = cmgr.FindMenuByCommandId(101);
    Menu *    wRestartMenu = cmgr.FindMenuByCommandId(104);
    Menu *     wWinzipMenu = cmgr.FindMenuByCommandId(108);
    ASSERT_TRUE( wFooServiceMenu != NULL );
    ASSERT_TRUE(    wRestartMenu != NULL );
    ASSERT_TRUE(     wWinzipMenu != NULL );
    ASSERT_EQ( std::string("Foo Service"),  wFooServiceMenu->GetName() );
    ASSERT_EQ( std::string("Restart"),      wRestartMenu->GetName() );
    ASSERT_EQ( std::string("Winzip"),       wWinzipMenu->GetName() );
 
    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
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
 
    //Creating a temporary workspace for the test execution.
    Workspace workspace;
    ASSERT_FALSE(workspace.GetBaseDirectory().empty());

    //Import the required files into the workspace
    static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
    std::string test_name = ra::testing::GetTestQualifiedName();
    std::string template_source_path = std::string("test_files") + path_separator + test_name + ".xml";
    ASSERT_TRUE(workspace.ImportFileUtf8(template_source_path.c_str()));
     
    //Wait to make sure that the next file copy/modification will not have the same timestamp
    ra::timing::Millisleep(1500);

    //Setup ConfigManager to read files from workspace
    cmgr.ClearSearchPath();
    cmgr.AddSearchPath(workspace.GetBaseDirectory());
    cmgr.Refresh();
 
    //ASSERT the file is loaded
    Configuration::ConfigurationPtrList configs = cmgr.GetConfigurations();
    ASSERT_EQ( 1, configs.size() );
 
    //act
    cmgr.Clear();

    //ASSERT nothing is loaded
    configs = cmgr.GetConfigurations();
    ASSERT_EQ( 0, configs.size() );

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testParentWithoutChildren)
  {
    ConfigManager & cmgr = ConfigManager::GetInstance();
 
    //Creating a temporary workspace for the test execution.
    Workspace workspace;
    ASSERT_FALSE(workspace.GetBaseDirectory().empty());

    //Import the required files into the workspace
    static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
    std::string test_name = ra::testing::GetTestQualifiedName();
    std::string template_source_path = std::string("test_files") + path_separator + test_name + ".xml";
    ASSERT_TRUE(workspace.ImportFileUtf8(template_source_path.c_str()));
 
    //Wait to make sure that the next file copy/modification will not have the same timestamp
    ra::timing::Millisleep(1500);

    //Setup ConfigManager to read files from workspace
    cmgr.ClearSearchPath();
    cmgr.AddSearchPath(workspace.GetBaseDirectory());
    cmgr.Refresh();
 
    //ASSERT the file is loaded
    Configuration::ConfigurationPtrList configs = cmgr.GetConfigurations();
    ASSERT_EQ( 1, configs.size() );
 
    //Query first menu
    Menu::MenuPtrList menus = configs[0]->GetMenus();
    ASSERT_EQ( 1, menus.size() );
    Menu * first = menus[0];
    ASSERT_TRUE( first != NULL );

    //Update the menus based on a context with a single file
    Context context = GetContextSingleFile();
    cmgr.Update(context);

    //ASSERT top menu is visible (default option)
    ASSERT_TRUE( first->IsVisible() );

    //Update the menus based on a context with a single directory
    context = GetContextSingleDirectory();
    cmgr.Update(context);

    //ASSERT top menu is invisible (issue #4)
    ASSERT_FALSE( first->IsVisible() );

    //Get all submenus
    Menu * option1 = first;
    ASSERT_TRUE( option1 != NULL );
    Menu * option1_1 = QuerySubMenu(option1, 0);
    ASSERT_TRUE( option1_1 != NULL );
    Menu * option1_1_1 = QuerySubMenu(option1_1, 0);
    ASSERT_TRUE( option1_1_1 != NULL );
    ASSERT_FALSE( option1_1_1->IsParentMenu() ); //menu is a leaf

    //Assert all sub menus are also invisible
    ASSERT_FALSE( option1->IsVisible() );
    ASSERT_FALSE( option1_1->IsVisible() );
    ASSERT_FALSE( option1_1_1->IsVisible() );

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfigManager, testAssignCommandIdsInvalid)
  {
    ConfigManager & cmgr = ConfigManager::GetInstance();
 
    //Creating a temporary workspace for the test execution.
    Workspace workspace;
    ASSERT_FALSE(workspace.GetBaseDirectory().empty());

    //Import the required files into the workspace
    static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
    std::string test_name = ra::testing::GetTestQualifiedName();
    std::string template_source_path = std::string("test_files") + path_separator + test_name + ".xml";
    ASSERT_TRUE(workspace.ImportFileUtf8(template_source_path.c_str()));
    
    //Wait to make sure that the next file copy/modification will not have the same timestamp
    ra::timing::Millisleep(1500);
 
    //Setup ConfigManager to read files from workspace
    cmgr.ClearSearchPath();
    cmgr.AddSearchPath(workspace.GetBaseDirectory());
    cmgr.Refresh();
 
    //ASSERT the file is loaded
    Configuration::ConfigurationPtrList configs = cmgr.GetConfigurations();
    ASSERT_EQ( 1, configs.size() );
 
    //Query all menus
    Menu * option1 = QuerySubMenu(configs[0], 0);
    ASSERT_TRUE( option1 != NULL );
    Menu * option1_1 = QuerySubMenu(option1, 0);
    Menu * option1_2 = QuerySubMenu(option1, 1);
    Menu * option1_3 = QuerySubMenu(option1, 2);
    ASSERT_TRUE( option1_1 != NULL );
    ASSERT_TRUE( option1_2 != NULL );
    ASSERT_TRUE( option1_3 != NULL );
    Menu * option1_2_1    = QuerySubMenu(option1_2,   0);
    Menu * option1_2_1_1  = QuerySubMenu(option1_2_1, 0);
    ASSERT_TRUE( option1_2_1    != NULL );
    ASSERT_TRUE( option1_2_1_1  != NULL );

    //Query all menus
    Menu::MenuPtrList menus;
    QueryAllMenusRecursive(configs[0], menus);

    //Update the menus based on a context with a single file
    Context context = GetContextSingleFile();
    cmgr.Update(context);

    //Assign unique command ids
    uint32_t nextCommandId = cmgr.AssignCommandIds(101);
    ASSERT_EQ( 103, nextCommandId ); //assert 2 Menu(s) visible by ConfigManager

    //Assert that all visible menu have an assigned command id (issue #5)
    for(size_t i=0; i<menus.size(); i++)
    {
      Menu * m = menus[i];
      std::string message = ra::strings::Format("Error. Menu '%s' which visible is '%s' and command_id is '%d' was not expected.", m->GetName().c_str(), ToBooleanString(m->IsVisible()), m->GetCommandId());
      if (m->IsVisible())
        ASSERT_NE(Menu::INVALID_COMMAND_ID, m->GetCommandId() ) << message; //visible menus must have a valid command id
      else
        ASSERT_EQ(Menu::INVALID_COMMAND_ID, m->GetCommandId() ) << message; //invisible menus must have an invalid command id
    }

    //Expected assigned command ids
    ASSERT_EQ( 101, option1->GetCommandId() );
    ASSERT_EQ( 102, option1_1->GetCommandId() );
    ASSERT_EQ( Menu::INVALID_COMMAND_ID, option1_2->GetCommandId() );
    ASSERT_EQ( Menu::INVALID_COMMAND_ID, option1_3->GetCommandId() );
    ASSERT_EQ( Menu::INVALID_COMMAND_ID, option1_2_1  ->GetCommandId() );
    ASSERT_EQ( Menu::INVALID_COMMAND_ID, option1_2_1_1->GetCommandId() );

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
 
} //namespace test
} //namespace shellanything
