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

#include "TestPlugins.h"
#include "Workspace.h"
#include "ConfigManager.h"
#include "PropertyManager.h"
#include "SelectionContext.h"
#include "SelectionContext.h"

#include "rapidassist/testing.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/environment.h"
#include "rapidassist/timing.h"
#include "rapidassist/process.h"

namespace shellanything { namespace test
{
  static const Configuration * INVALID_CONFIGURATION = NULL;
 
  //--------------------------------------------------------------------------------------------------
  void TestPlugins::SetUp()
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
  void TestPlugins::TearDown()
  {
    //Delete the configurations which source files are deleted
    ConfigManager& cmgr = ConfigManager::GetInstance();
    cmgr.Refresh();

    //Delete the source file of all remaining Configuration instance
    Configuration::ConfigurationPtrList configs = cmgr.GetConfigurations();
    for (size_t i = 0; i < configs.size(); i++)
    {
      Configuration* config = configs[i];
      if (config)
      {
        const std::string& file_path = config->GetFilePath();
        ASSERT_TRUE(ra::filesystem::DeleteFile(file_path.c_str())) << "Failed deleting file '" << file_path << "'.";
      }
    }

    //Now that all configuration files are deleted, refresh again
    cmgr.ClearSearchPath();
    cmgr.Refresh();

    //ASSERT that no files are loaded
    ASSERT_EQ(0, cmgr.GetConfigurations().size());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPlugins, testProcess)
  {
    ConfigManager& cmgr = ConfigManager::GetInstance();

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
    ASSERT_EQ(1, configs.size());

    Configuration* config0 = cmgr.GetConfigurations()[0];

    //ASSERT all plugins were loaded
    for (size_t i = 0; i < config0->GetPlugins().size(); i++)
    {
      const Plugin* plugin = config0->GetPlugins()[i];
      ASSERT_TRUE(plugin->IsLoaded()) << "The plugin '" << plugin->GetPath() << "' is not loaded.";
    }

    //Get menus
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ(4, menus.size());
    Menu* menu0 = menus[0];
    Menu* menu1 = menus[1];
    Menu* menu2 = menus[2];
    Menu* menu3 = menus[3];
    ASSERT_TRUE(menu0 != NULL);
    ASSERT_TRUE(menu1 != NULL);
    ASSERT_TRUE(menu2 != NULL);
    ASSERT_TRUE(menu3 != NULL);

    //ASSERT all expected content is parsed
    for (size_t i = 0; i < menus.size(); i++)
    {
      Menu* menu = menus[i];

      //ASSERT menus have a visibility statement parsed.
      ASSERT_EQ(0, menu->GetValidityCount());
      ASSERT_EQ(1, menu->GetVisibilityCount());

      //ASSERT menus have an action
      const IAction::ActionPtrList& actions = menu->GetActions();
      ASSERT_EQ(1, actions.size());
    }

    PropertyManager& pmgr = PropertyManager::GetInstance();
    
    //Disable process id property
    pmgr.SetProperty("sa_plugin_process.pid", "");
    
    //Kill all instances of notepad.exe
    printf("Killing notepad.exe processes...\n");
    system("taskkill /F /IM notepad.exe");
    printf("done.\n");

    //Set all menus visible
    for (size_t i = 0; i < menus.size(); i++)
    {
      Menu* menu = menus[i];
      menu->SetVisible(true);
    }

    //Force an update to call the plugin
    SelectionContext c;
#ifdef _WIN32
    {
      StringList elements;
      elements.push_back("C:\\Windows\\System32\\cmd.exe");
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif
    config0->Update(c);

    //ASSERT all menus are now invisible
    for (size_t i = 0; i < menus.size(); i++)
    {
      Menu* menu = menus[i];
      ASSERT_FALSE(menu->IsVisible()) << "menu[" << i << "] named '" << menu->GetName() << "' should be invisible";
    }

    //Start notepad
    printf("Starting notepad.exe...\n");
    ra::process::processid_t notepad_pid = ra::process::StartProcess("C:\\Windows\\System32\\notepad.exe");
    ra::timing::Millisleep(2000);
    ASSERT_NE(0, notepad_pid);
    printf("done.\n");

    //Set all menus visible
    for (size_t i = 0; i < menus.size(); i++)
    {
      Menu* menu = menus[i];
      menu->SetVisible(true);
    }

    //Update menus again
    config0->Update(c);

    //ASSERT that half of menus are now invisible
    ASSERT_TRUE (menu0->IsVisible()) << "Menu named '" << menu0->GetName() << "' should be visible";
    ASSERT_FALSE(menu1->IsVisible()) << "Menu named '" << menu1->GetName() << "' should be invisible";
    ASSERT_TRUE (menu2->IsVisible()) << "Menu named '" << menu2->GetName() << "' should be visible";
    ASSERT_FALSE(menu3->IsVisible()) << "Menu named '" << menu3->GetName() << "' should be invisible";

    //Set all menus invisible
    for (size_t i = 0; i < menus.size(); i++)
    {
      Menu* menu = menus[i];
      menu->SetVisible(false);
    }

    std::string notepad_pid_str = ra::strings::ToString(notepad_pid);
    pmgr.SetProperty("sa_plugin_process.pid", notepad_pid_str);

    //Update menus again
    config0->Update(c);

    //ASSERT all menus are now visible
    for (size_t i = 0; i < menus.size(); i++)
    {
      Menu* menu = menus[i];
      ASSERT_TRUE(menu->IsVisible()) << "menu[" << i << "] named '" << menu->GetName() << "' should be visible";
    }

    //Cleanup
    ra::process::Kill(notepad_pid);
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPlugins, testServices)
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
 
    Configuration * config0 = cmgr.GetConfigurations()[0];

    //ASSERT all plugins were loaded
    for (size_t i = 0; i < config0->GetPlugins().size(); i++)
    {
      const Plugin* plugin = config0->GetPlugins()[i];
      ASSERT_TRUE(plugin->IsLoaded()) << "The plugin '" << plugin->GetPath() << "' is not loaded.";
    }
    
    //Get menus
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ(1, menus.size());
    Menu* menu0 = menus[0];
    ASSERT_TRUE(menu0 != NULL);

    // Force an update to call the plugin
    SelectionContext c;
#ifdef _WIN32
    {
      StringList elements;
      elements.push_back("C:\\Windows\\System32\\cmd.exe");
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    //Define the list of expected properties
    ra::strings::StringVector expected_properties;
    expected_properties.push_back("sa_plugin_services.WinDefend.status");
    expected_properties.push_back("sa_plugin_services.Dhcp.status");
    expected_properties.push_back("sa_plugin_services.Dnscache.status");
    expected_properties.push_back("sa_plugin_services.OneDrive_Updater_Service.status");
    expected_properties.push_back("sa_plugin_services.wuauserv.status");
    expected_properties.push_back("sa_plugin_services.Fax.status");
    expected_properties.push_back("sa_plugin_services.msiserver.status");
    expected_properties.push_back("sa_plugin_services.WebClient.status");
    expected_properties.push_back("sa_plugin_services.aaaa.status");

    //Clear expected properties
    PropertyManager& pmgr = PropertyManager::GetInstance();
    for (size_t i = 0; i < expected_properties.size(); i++)
    {
      const std::string& property_name = expected_properties[i];
      pmgr.ClearProperty(property_name);
    }

    config0->Update(c);

    //ASSERT the expected properties are set
    for (size_t i = 0; i < expected_properties.size(); i++)
    {
      const std::string& property_name = expected_properties[i];
      ASSERT_TRUE(pmgr.HasProperty(property_name)) << "The expected property '" << property_name << "' is not found.";
      //const std::string& value = pmgr.GetProperty(property_name);
      //printf("Found property '%s' with value '%s'.\n", property_name.c_str(), value.c_str());
    }

    //ASSERT expected process status
    static const std::string& STATUS_RUNNING = "running";
    static const std::string& STATUS_STOPPED = "stopped";
    static const std::string& STATUS_EMPTY   = "";
    ASSERT_EQ(STATUS_RUNNING, pmgr.GetProperty("sa_plugin_services.Dhcp.status"));
    ASSERT_EQ(STATUS_STOPPED, pmgr.GetProperty("sa_plugin_services.WebClient.status"));
    ASSERT_EQ(STATUS_EMPTY  , pmgr.GetProperty("sa_plugin_services.aaaa.status"));

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPlugins, testStrings)
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
 
    Configuration * config0 = cmgr.GetConfigurations()[0];

    //ASSERT all plugins were loaded
    for (size_t i = 0; i < config0->GetPlugins().size(); i++)
    {
      const Plugin* plugin = config0->GetPlugins()[i];
      ASSERT_TRUE(plugin->IsLoaded()) << "The plugin '" << plugin->GetPath() << "' is not loaded.";
    }
    
    //Get menus
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ(1, menus.size());
    Menu* menu0 = menus[0];
    ASSERT_TRUE(menu0 != NULL);

    //ASSERT all actions were parsed
    const IAction::ActionPtrList & actions = menu0->GetActions();
    ASSERT_EQ(16, actions.size());

    // Force an update to call the plugin
    SelectionContext c;
#ifdef _WIN32
    {
      StringList elements;
      elements.push_back("C:\\Windows\\System32\\cmd.exe");
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    //Define the list of expected properties
    ra::strings::StringVector expected_properties;
    expected_properties.push_back("sa_plugin_strings.numbers");
    expected_properties.push_back("sa_plugin_strings.letters");
    expected_properties.push_back("sa_plugin_strings.strlen");
    expected_properties.push_back("sa_plugin_strings.father");
    expected_properties.push_back("sa_plugin_strings.uppercase");
    expected_properties.push_back("sa_plugin_strings.lowercase");
    expected_properties.push_back("sa_plugin_strings.roads.1");
    expected_properties.push_back("sa_plugin_strings.roads.2");
    expected_properties.push_back("sa_plugin_strings.roads.3");
    expected_properties.push_back("sa_plugin_strings.demo.offset");
    expected_properties.push_back("sa_plugin_strings.demo.lenght");
    expected_properties.push_back("sa_plugin_strings.demo.username");
    expected_properties.push_back("sa_plugin_strings.demo.message");

    //Clear expected properties
    PropertyManager& pmgr = PropertyManager::GetInstance();
    for (size_t i = 0; i < expected_properties.size(); i++)
    {
      const std::string& property_name = expected_properties[i];
      pmgr.ClearProperty(property_name);
    }

    // Execute menu actions
    for (size_t i = 0; i < actions.size(); i++)
    {
      const shellanything::IAction* action = actions[i];
      ASSERT_TRUE(action != NULL);
      bool success = action->Execute(c);
      ASSERT_EQ(true, success);
    }

    //ASSERT the expected properties are set
    for (size_t i = 0; i < expected_properties.size(); i++)
    {
      const std::string& property_name = expected_properties[i];
      ASSERT_TRUE(pmgr.HasProperty(property_name)) << "The expected property '" << property_name << "' is not found.";
      //const std::string& value = pmgr.GetProperty(property_name);
      //printf("Found property '%s' with value '%s'.\n", property_name.c_str(), value.c_str());
    }

    //ASSERT expected property values
    ra::strings::StringVector expected_values;
    expected_values.push_back("0123456789");
    expected_values.push_back("abcdefghijklmnopqrstuvwxyz");
    expected_values.push_back("11");
    expected_values.push_back("George McFly");
    expected_values.push_back("ROADS? WHERE WE'RE GOING, WE DON'T NEED ROADS.");
    expected_values.push_back("roads? where we're going, we don't need roads.");
    expected_values.push_back("0");
    expected_values.push_back("40");
    expected_values.push_back("18446744073709551615");
    expected_values.push_back("13");
    expected_values.push_back("6");
    expected_values.push_back("foobar");
    expected_values.push_back("Error: User 'mcfly.m' not found!");

    //ASSERT expected process values
    ASSERT_EQ(expected_properties.size(), expected_values.size());
    for (size_t i = 0; i < expected_properties.size(); i++)
    {
      const std::string& property_name = expected_properties[i];
      const std::string& actual_value = pmgr.GetProperty(property_name);
      const std::string& expected_value = expected_values[i];
      ASSERT_EQ(expected_value, actual_value) << "Test failure with 'expected_values[" << i << "]'. The property '" << property_name << "' is set to value '" << actual_value << "' but the value '" << expected_value << "' is expected.";
    }

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPlugins, testTime)
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
 
    Configuration * config0 = cmgr.GetConfigurations()[0];

    //ASSERT all plugins were loaded
    for (size_t i = 0; i < config0->GetPlugins().size(); i++)
    {
      const Plugin* plugin = config0->GetPlugins()[i];
      ASSERT_TRUE(plugin->IsLoaded()) << "The plugin '" << plugin->GetPath() << "' is not loaded.";
    }
    
    //Get menus
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ(4, menus.size());
    Menu* menu0 = menus[0];
    Menu* menu1 = menus[1];
    Menu* menu2 = menus[2];
    Menu* menu3 = menus[3];
    ASSERT_TRUE(menu0 != NULL);
    ASSERT_TRUE(menu1 != NULL);
    ASSERT_TRUE(menu2 != NULL);
    ASSERT_TRUE(menu3 != NULL);

    // Force an update to call the plugin
    SelectionContext c;
#ifdef _WIN32
    {
      StringList elements;
      elements.push_back("C:\\Windows\\System32\\cmd.exe");
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    menu0->SetVisible(false);
    menu1->SetVisible(false);
    menu2->SetVisible(false);
    menu3->SetVisible(false);

    config0->Update(c);

    //ASSERT the visibility state
    bool visible0 = menu0->IsVisible();
    bool visible1 = menu1->IsVisible();
    bool visible2 = menu2->IsVisible();
    bool visible3 = menu3->IsVisible();
    ASSERT_TRUE(visible0); //menu0 should always be visible.
    ASSERT_TRUE((visible1 == true && visible2 == false) ||
                (visible1 == false && visible2 == true)); //menu1 and menu2 are mutually exclusive

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPlugins, testPluginActionGetData)
  {
    ConfigManager& cmgr = ConfigManager::GetInstance();
    PropertyManager& pmgr = PropertyManager::GetInstance();

    static const size_t ACTION_COUNT = 4;

    //clear previous counter properties
    for (size_t i = 0; i < ACTION_COUNT; i++)
    {
      std::string name;
      name = "sa_plugin_test_data." + ra::strings::ToString(i + 1) + ".count";
      pmgr.ClearProperty(name);
      name = "sa_plugin_test_data." + ra::strings::ToString(i + 1) + ".address";
      pmgr.ClearProperty(name);
      name = "sa_plugin_test_data." + ra::strings::ToString(i + 1) + ".destroy";
      pmgr.ClearProperty(name);
    }

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
    ASSERT_EQ(1, configs.size());

    Configuration* config0 = cmgr.GetConfigurations()[0];

    //ASSERT all plugins were loaded
    for (size_t i = 0; i < config0->GetPlugins().size(); i++)
    {
      const Plugin* plugin = config0->GetPlugins()[i];
      ASSERT_TRUE(plugin->IsLoaded()) << "The plugin '" << plugin->GetPath() << "' is not loaded.";
    }

    //Get menus
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ(1, menus.size());
    Menu* menu0 = menus[0];
    ASSERT_TRUE(menu0 != NULL);

    //ASSERT all expected content is parsed
    const IAction::ActionPtrList& actions = menu0->GetActions();
    ASSERT_EQ(ACTION_COUNT, actions.size());

    //assert that all actions were "created"
    for (size_t i = 0; i < ACTION_COUNT; i++)
    {
      std::string name = "sa_plugin_test_data." + ra::strings::ToString(i + 1) + ".address";
      ASSERT_TRUE(pmgr.HasProperty(name)) << "Property '" << name << "' not found.";
      std::string value = pmgr.GetProperty(name);
      printf("%s=%s\n", name.c_str(), value.c_str());
    }

    //Force an update to execute the actions
    SelectionContext c;
#ifdef _WIN32
    {
      StringList elements;
      elements.push_back("C:\\Windows\\System32\\cmd.exe");
      c.SetElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    printf("------------------------------------------------\n");

    //execute actions (twice)
    static size_t EXEC_COUNT = 4;
    for (size_t count = 0; count < EXEC_COUNT; count++)
    {
      for (size_t i = 0; i < actions.size(); i++)
      {
        const shellanything::IAction * action = actions[i];
        ASSERT_TRUE(action != NULL);
        ASSERT_TRUE(action->Execute(c)) << "The action[" << i << "] has failed.";
      }
      //print new addresses
      for (size_t i = 0; i < ACTION_COUNT; i++)
      {
        std::string name = "sa_plugin_test_data." + ra::strings::ToString(i + 1) + ".address";
        ASSERT_TRUE(pmgr.HasProperty(name)) << "Property '" << name << "' not found.";
        std::string value = pmgr.GetProperty(name);
        printf("%s=%s\n", name.c_str(), value.c_str());
      }
      printf("------------------------------------------------\n");
    }

    //assert that all actions were "executed" EXEC_COUNT times
    for (size_t i = 0; i < ACTION_COUNT; i++)
    {
      std::string name = "sa_plugin_test_data." + ra::strings::ToString(i + 1) + ".count";
      ASSERT_TRUE(pmgr.HasProperty(name)) << "Property '" << name << "' not found.";
      std::string value = pmgr.GetProperty(name);
      printf("%s=%s\n", name.c_str(), value.c_str());
      ASSERT_TRUE(value == ra::strings::ToString(EXEC_COUNT));
    }

    //force the actions to be destroyed by deleting the configuration file and then refreshing.
    ASSERT_TRUE(ra::filesystem::DeleteFile(config0->GetFilePath().c_str())) << "Failed to delete configuration file '" << config0->GetFilePath() << "'.";
    cmgr.Refresh();

    //assert the configuration is unloaded
    configs = cmgr.GetConfigurations();
    ASSERT_EQ(0, configs.size());

    //assert that all actions were "destroyed"
    for (size_t i = 0; i < ACTION_COUNT; i++)
    {
      std::string destroy_name = "sa_plugin_test_data." + ra::strings::ToString(i + 1) + ".destroy";
      ASSERT_TRUE(pmgr.HasProperty(destroy_name)) << "Property '" << destroy_name << "' not found.";
      std::string destroy_value = pmgr.GetProperty(destroy_name);
      printf("%s=%s\n", destroy_name.c_str(), destroy_value.c_str());

      //assert destroyed address is the same as the my_data adddress
      std::string address_name = "sa_plugin_test_data." + ra::strings::ToString(i + 1) + ".address";
      ASSERT_TRUE(pmgr.HasProperty(address_name)) << "Property '" << address_name << "' not found.";
      std::string address_value = pmgr.GetProperty(address_name);

      ASSERT_TRUE(destroy_value == address_value);
    }

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
 
} //namespace test
} //namespace shellanything
