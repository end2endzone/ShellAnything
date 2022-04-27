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

#include "rapidassist/testing.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/environment.h"
#include "rapidassist/timing.h"

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
    cmgr.Refresh();

    //ASSERT that no files are loaded
    ASSERT_EQ(0, cmgr.GetConfigurations().size());
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
    ASSERT_EQ(STATUS_STOPPED, pmgr.GetProperty("sa_plugin_services.Fax.status"));
    ASSERT_EQ(STATUS_EMPTY  , pmgr.GetProperty("sa_plugin_services.aaaa.status"));

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
 
} //namespace test
} //namespace shellanything
