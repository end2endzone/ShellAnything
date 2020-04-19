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
#include "shellanything/ActionFile.h"
#include "shellanything/ActionPrompt.h"
#include "shellanything/ActionMessage.h"
#include "shellanything/ActionProperty.h"
#include "Platform.h"

#include "rapidassist/testing.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/environment.h"
#include "rapidassist/timing.h"

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
  ActionMessage * getFirstActionMessage(Menu * m)
  {
    if (!m)
      return NULL;
 
    Action::ActionPtrList actions = m->getActions();
    for(size_t i=0; i<actions.size(); i++)
    {
      Action * action = actions[i];
      ActionMessage * action_message = dynamic_cast<ActionMessage *>(action);
      if (action_message)
        return action_message;
    }
 
    return NULL;
  }
  //--------------------------------------------------------------------------------------------------
  ActionFile * getFirstActionFile(Menu * m)
  {
    if (!m)
      return NULL;
 
    Action::ActionPtrList actions = m->getActions();
    for(size_t i=0; i<actions.size(); i++)
    {
      Action * action = actions[i];
      ActionFile * action_file = dynamic_cast<ActionFile *>(action);
      if (action_file)
        return action_file;
    }
 
    return NULL;
  }
  //--------------------------------------------------------------------------------------------------
  void TestObjectFactory::SetUp()
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
  void TestObjectFactory::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseValidator)
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
 
    //ASSERT a 4 menus are available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
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
    ASSERT_TRUE( ra::filesystem::DeleteFile(template_target_path.c_str()) ) << "Failed deleting file '" << template_target_path << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseIcon)
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
 
    //ASSERT a 3 menus are available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
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
    ASSERT_TRUE( ra::filesystem::DeleteFile(template_target_path.c_str()) ) << "Failed deleting file '" << template_target_path << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseActionFile)
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

    //ASSERT a 3 menus are available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ( 4, menus.size() );

    //assert all menus have a file element as the first action
    ActionFile * file00 = getFirstActionFile(menus[00]);
    ActionFile * file01 = getFirstActionFile(menus[01]);
    ActionFile * file02 = getFirstActionFile(menus[02]);
    ActionFile * file03 = getFirstActionFile(menus[03]);

    ASSERT_TRUE( file00 != NULL );
    ASSERT_TRUE( file01 != NULL );
    ASSERT_TRUE( file02 != NULL );
    ASSERT_TRUE( file03 != NULL );

    //assert menus have text assigned
    static const std::string EMPTY_STRING;
    ASSERT_NE(EMPTY_STRING, file00->GetText());
    ASSERT_NE(EMPTY_STRING, file01->GetText());
    ASSERT_EQ(EMPTY_STRING, file02->GetText());
    ASSERT_NE(EMPTY_STRING, file03->GetText());

    //assert menus have encoding
    ASSERT_EQ(EMPTY_STRING, file00->GetEncoding());
    ASSERT_EQ(EMPTY_STRING, file01->GetEncoding());
    ASSERT_EQ(EMPTY_STRING, file02->GetEncoding());
    ASSERT_NE(EMPTY_STRING, file03->GetEncoding());

    //cleanup
    ASSERT_TRUE( ra::filesystem::DeleteFile(template_target_path.c_str()) ) << "Failed deleting file '" << template_target_path << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseActionPrompt)
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

    //ASSERT a multiple menus are available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
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
    std::string prompt00_default = prompt00->GetDefault();
    ASSERT_EQ( EMPTY_STRING, prompt00_default);

    //assert menu #1 have a default value
    std::string prompt01_default = prompt01->GetDefault();
    ASSERT_EQ( std::string("42"), prompt01_default);

    //assert menu #3 is a yesno question
    ASSERT_FALSE( prompt00->IsYesNoQuestion() );
    ASSERT_FALSE( prompt01->IsYesNoQuestion() );
    ASSERT_FALSE( prompt02->IsYesNoQuestion() );
    ASSERT_TRUE ( prompt03->IsYesNoQuestion() );
    ASSERT_FALSE( prompt04->IsYesNoQuestion() );
 
    //assert menu #3 have a "yes" and a "no" values defined
    ASSERT_TRUE ( prompt00->GetValueYes().empty() );
    ASSERT_TRUE ( prompt00->GetValueNo ().empty() );
    ASSERT_FALSE( prompt03->GetValueYes().empty() );
    ASSERT_FALSE( prompt03->GetValueNo ().empty() );
 
    //assert menu #4 is a OK only question
    ASSERT_FALSE( prompt00->IsOkQuestion() );
    ASSERT_FALSE( prompt01->IsOkQuestion() );
    ASSERT_FALSE( prompt02->IsOkQuestion() );
    ASSERT_FALSE( prompt03->IsOkQuestion() );
    ASSERT_TRUE ( prompt04->IsOkQuestion() );

    //cleanup
    ASSERT_TRUE( ra::filesystem::DeleteFile(template_target_path.c_str()) ) << "Failed deleting file '" << template_target_path << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseActionMessage)
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

    //ASSERT a multiple menus are available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ( 4, menus.size() );

    //assert all menus have a message element as the first action
    ActionMessage * message00 = getFirstActionMessage(menus[00]);
    ActionMessage * message01 = getFirstActionMessage(menus[01]);
    ActionMessage * message02 = getFirstActionMessage(menus[02]);
    ActionMessage * message03 = getFirstActionMessage(menus[03]);

    ASSERT_TRUE( message00 != NULL );
    ASSERT_TRUE( message01 != NULL );
    ASSERT_TRUE( message02 != NULL );
    ASSERT_TRUE( message03 != NULL );

    //assert menu #0 caption and message
    static const std::string EMPTY_STRING;
    ASSERT_EQ( "my_caption",  message00->GetCaption());
    ASSERT_EQ( "my_title",    message00->GetTitle());
    ASSERT_EQ( EMPTY_STRING,  message00->GetIcon());

    //assert menu #1 have a default value
    ASSERT_EQ( "hello",       message01->GetCaption());
    ASSERT_EQ( "world",       message01->GetTitle());
    ASSERT_EQ( EMPTY_STRING,  message01->GetIcon());

    //assert menu #2 have a default value
    ASSERT_EQ( "foo",         message02->GetCaption());
    ASSERT_EQ( "bar",         message02->GetTitle());
    ASSERT_EQ( "exclamation", message02->GetIcon());

    //assert menu #3 have a default value
    ASSERT_EQ( "foo",         message03->GetCaption());
    ASSERT_EQ( "bar",         message03->GetTitle());
    ASSERT_EQ( "warning",     message03->GetIcon());

    //cleanup
    ASSERT_TRUE( ra::filesystem::DeleteFile(template_target_path.c_str()) ) << "Failed deleting file '" << template_target_path << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseDefaults)
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
 
    //ASSERT a DefaultSettings section is available
    const DefaultSettings * defaults = cmgr.GetConfigurations()[0]->GetDefaultSettings();
    ASSERT_TRUE( defaults != NULL );

    //assert 2 properties parsed
    ASSERT_EQ( 2, defaults->getActions().size() );

    const ActionProperty * property1 = dynamic_cast<const ActionProperty *>(defaults->getActions()[0]);
    const ActionProperty * property2 = dynamic_cast<const ActionProperty *>(defaults->getActions()[1]);
    ASSERT_TRUE( property1 != NULL );
    ASSERT_TRUE( property2 != NULL );

    ASSERT_EQ( std::string("services.command.start"), property1->GetName());
    ASSERT_EQ( std::string("services.command.stop" ), property2->GetName());
    ASSERT_EQ( std::string("runservice /start"), property1->GetValue());
    ASSERT_EQ( std::string("runservice /stop" ), property2->GetValue());

    //cleanup
    ASSERT_TRUE( ra::filesystem::DeleteFile(template_target_path.c_str()) ) << "Failed deleting file '" << template_target_path << "'.";
  }
  //--------------------------------------------------------------------------------------------------
 
} //namespace test
} //namespace shellanything
