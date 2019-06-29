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
#include "shellanything/ActionPrompt.h"
#include "shellanything/ActionProperty.h"
#include "Platform.h"

#include "rapidassist/gtesthelp.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/environment.h"
#include "rapidassist/time_.h"

namespace shellanything { namespace test
{
  static const Configuration * INVALID_CONFIGURATION = NULL;
 
  //--------------------------------------------------------------------------------------------------
  ActionPrompt * getFirstActionPrompt(Menu * m)
  {
    if (!m)
      return NULL;
 
    Action::ActionPtrList actions = m->getActions();
    for(size_t i=0; i<actions.size(); i++)
    {
      Action * action = actions[i];
      ActionPrompt * action_prompt = dynamic_cast<ActionPrompt *>(action);
      if (action_prompt)
        return action_prompt;
    }
 
    return NULL;
  }
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

    //assert <visibility> tag properly parsed
    static const std::string expected_property = "bar";
    static const std::string expected_file_extension = "com;exe;bat;cmd";
    static const std::string expected_file_exists = "C:\\Users\\Public;C:\\Program Files (x86)";

    ASSERT_EQ( expected_property,       menus[00]->getVisibility().getProperties() );
    ASSERT_EQ( 5,                       menus[01]->getVisibility().getMaxFiles() );
    ASSERT_EQ( 6,                       menus[01]->getVisibility().getMaxDirectories() );
    ASSERT_EQ( expected_file_extension, menus[02]->getVisibility().getFileExtensions() );
    ASSERT_EQ( expected_file_exists,    menus[03]->getVisibility().getFileExists() );

    //cleanup
    ASSERT_TRUE( ra::filesystem::deleteFile(template_target_path.c_str()) ) << "Failed deleting file '" << template_target_path << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseIcon)
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
 
    //ASSERT a 3 menus are available
    Menu::MenuPtrList menus = cmgr.getConfigurations()[0]->getMenus();
    ASSERT_EQ( 3, menus.size() );

    //assert all icons are valid
    ASSERT_TRUE( menus[00]->getIcon().isValid() );
    ASSERT_TRUE( menus[01]->getIcon().isValid() );
    ASSERT_TRUE( menus[02]->getIcon().isValid() );

    //assert <icon> tag properly parsed
    //menu #00
    ASSERT_EQ( std::string("C:\\Windows\\System32\\shell32.dll"), menus[00]->getIcon().getPath() );
    ASSERT_EQ( 42,                                                menus[00]->getIcon().getIndex() );

    //menu #01
    ASSERT_EQ( std::string("${application.path}"),  menus[01]->getIcon().getPath() );
    ASSERT_EQ( 0,                                   menus[01]->getIcon().getIndex() );

    //menu #02
    ASSERT_EQ( std::string("txt"),        menus[02]->getIcon().getFileExtension() );
    ASSERT_EQ( std::string(""),           menus[02]->getIcon().getPath() );
    ASSERT_EQ( Icon::INVALID_ICON_INDEX,  menus[02]->getIcon().getIndex() );

    //cleanup
    ASSERT_TRUE( ra::filesystem::deleteFile(template_target_path.c_str()) ) << "Failed deleting file '" << template_target_path << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseActionPrompt)
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

    //ASSERT a 3 menus are available
    Menu::MenuPtrList menus = cmgr.getConfigurations()[0]->getMenus();
    ASSERT_EQ( 5, menus.size() );

    //assert all menus have a prompt element as the first action
    ActionPrompt * prompt00 = getFirstActionPrompt(menus[00]);
    ActionPrompt * prompt01 = getFirstActionPrompt(menus[01]);
    ActionPrompt * prompt02 = getFirstActionPrompt(menus[02]);
    ActionPrompt * prompt03 = getFirstActionPrompt(menus[03]);
    ActionPrompt * prompt04 = getFirstActionPrompt(menus[04]);

    ASSERT_TRUE( prompt00 != NULL );
    ASSERT_TRUE( prompt01 != NULL );
    ASSERT_TRUE( prompt02 != NULL );
    ASSERT_TRUE( prompt03 != NULL );
    ASSERT_TRUE( prompt04 != NULL );

    //assert menu #0 have no default values
    static const std::string EMPTY_STRING;
    std::string prompt00_default = prompt00->getDefault();
    ASSERT_EQ( EMPTY_STRING, prompt00_default);

    //assert menu #1 have a default value
    std::string prompt01_default = prompt01->getDefault();
    ASSERT_EQ( std::string("42"), prompt01_default);

    //assert menu #3 is a yesno question
    ASSERT_FALSE( prompt00->isYesNoQuestion() );
    ASSERT_FALSE( prompt01->isYesNoQuestion() );
    ASSERT_FALSE( prompt02->isYesNoQuestion() );
    ASSERT_TRUE ( prompt03->isYesNoQuestion() );
    ASSERT_FALSE( prompt04->isYesNoQuestion() );
 
    //assert menu #3 have a "yes" and a "no" values defined
    ASSERT_TRUE ( prompt00->getValueYes().empty() );
    ASSERT_TRUE ( prompt00->getValueNo ().empty() );
    ASSERT_FALSE( prompt03->getValueYes().empty() );
    ASSERT_FALSE( prompt03->getValueNo ().empty() );
 
    //assert menu #4 is a OK only question
    ASSERT_FALSE( prompt00->isOkQuestion() );
    ASSERT_FALSE( prompt01->isOkQuestion() );
    ASSERT_FALSE( prompt02->isOkQuestion() );
    ASSERT_FALSE( prompt03->isOkQuestion() );
    ASSERT_TRUE ( prompt04->isOkQuestion() );

    //cleanup
    ASSERT_TRUE( ra::filesystem::deleteFile(template_target_path.c_str()) ) << "Failed deleting file '" << template_target_path << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseDefaults)
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
 
    //ASSERT a DefaultSettings section is available
    const DefaultSettings * defaults = cmgr.getConfigurations()[0]->getDefaults();
    ASSERT_TRUE( defaults != NULL );

    //assert 2 properties parsed
    ASSERT_EQ( 2, defaults->getActions().size() );

    const ActionProperty * property1 = dynamic_cast<const ActionProperty *>(defaults->getActions()[0]);
    const ActionProperty * property2 = dynamic_cast<const ActionProperty *>(defaults->getActions()[1]);
    ASSERT_TRUE( property1 != NULL );
    ASSERT_TRUE( property2 != NULL );

    ASSERT_EQ( std::string("services.command.start"), property1->getName());
    ASSERT_EQ( std::string("services.command.stop" ), property2->getName());
    ASSERT_EQ( std::string("runservice /start"), property1->getValue());
    ASSERT_EQ( std::string("runservice /stop" ), property2->getValue());

    //cleanup
    ASSERT_TRUE( ra::filesystem::deleteFile(template_target_path.c_str()) ) << "Failed deleting file '" << template_target_path << "'.";
  }
  //--------------------------------------------------------------------------------------------------
 
} //namespace test
} //namespace shellanything
