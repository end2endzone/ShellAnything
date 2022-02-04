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
#include "Workspace.h"
#include "ConfigManager.h"
#include "Context.h"
#include "ActionExecute.h"
#include "ActionStop.h"
#include "ActionFile.h"
#include "ActionPrompt.h"
#include "ActionMessage.h"
#include "ActionProperty.h"

#include "rapidassist/testing.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/environment.h"
#include "rapidassist/timing.h"

namespace shellanything { namespace test
{
  static const Configuration * INVALID_CONFIGURATION = NULL;
 
  //--------------------------------------------------------------------------------------------------
  ActionPrompt * GetFirstActionPrompt(Menu * m)
  {
    if (!m)
      return NULL;
 
    Action::ActionPtrList actions = m->GetActions();
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
  ActionProperty * GetFirstActionProperty(Menu * m)
  {
    if (!m)
      return NULL;
 
    Action::ActionPtrList actions = m->GetActions();
    for(size_t i=0; i<actions.size(); i++)
    {
      Action * action = actions[i];
      ActionProperty * action_property = dynamic_cast<ActionProperty *>(action);
      if (action_property)
        return action_property;
    }
 
    return NULL;
  }
  //--------------------------------------------------------------------------------------------------
  ActionMessage * GetFirstActionMessage(Menu * m)
  {
    if (!m)
      return NULL;
 
    Action::ActionPtrList actions = m->GetActions();
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
  ActionExecute * GetFirstActionExecute(Menu * m)
  {
    if (!m)
      return NULL;
 
    Action::ActionPtrList actions = m->GetActions();
    for(size_t i=0; i<actions.size(); i++)
    {
      Action * action = actions[i];
      ActionExecute * action_execute = dynamic_cast<ActionExecute *>(action);
      if (action_execute)
        return action_execute;
    }
 
    return NULL;
  }
  //--------------------------------------------------------------------------------------------------
  ActionStop * GetFirstActionStop(Menu * m)
  {
    if (!m)
      return NULL;
 
    Action::ActionPtrList actions = m->GetActions();
    for(size_t i=0; i<actions.size(); i++)
    {
      Action * action = actions[i];
      ActionStop * action_fail = dynamic_cast<ActionStop*>(action);
      if (action_fail)
        return action_fail;
    }
 
    return NULL;
  }
  //--------------------------------------------------------------------------------------------------
  ActionFile * GetFirstActionFile(Menu * m)
  {
    if (!m)
      return NULL;
 
    Action::ActionPtrList actions = m->GetActions();
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
 
    //ASSERT all menus are available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ( 15, menus.size() );

    //Assert <visibility> tag properly parsed
    static const std::string expected_property = "bar";
    static const std::string expected_file_extension = "com;exe;bat;cmd";
    static const std::string expected_file_exists = "C:\\Users\\Public;C:\\Program Files (x86)";
    static const std::string expected_inverse_empty = "";
    static const std::string expected_inverse_maxfiles = "maxfiles";
    static const std::string expected_inverse_many = "maxfiles;maxfolders";
    static const std::string expected_inverse_unknown = "foo";
    static const std::string expected_class = "file";
    static const std::string expected_pattern = "*IMG_*";
    static const std::string expected_exprtk = "2>1";
    static const std::string expected_istrue = "yes";
    static const std::string expected_isfalse = "no";
    static const std::string expected_isempty = "hello";

    //Assert each menus have a visibility assigned
    for(size_t i=0; i<menus.size(); i++)
    {
      size_t count = menus[i]->GetVisibilityCount();
      ASSERT_GT(count, 0);
    }

    ASSERT_EQ( expected_property,         menus[0]->GetVisibility(0)->GetProperties() );
    ASSERT_EQ( 5,                         menus[1]->GetVisibility(0)->GetMaxFiles() );
    ASSERT_EQ( 6,                         menus[1]->GetVisibility(0)->GetMaxDirectories() );
    ASSERT_EQ( expected_file_extension,   menus[2]->GetVisibility(0)->GetFileExtensions() );
    ASSERT_EQ( expected_file_exists,      menus[3]->GetVisibility(0)->GetFileExists() );
    ASSERT_EQ( expected_inverse_empty,    menus[4]->GetVisibility(0)->GetInserve() );
    ASSERT_EQ( expected_inverse_maxfiles, menus[5]->GetVisibility(0)->GetInserve() );
    ASSERT_EQ( expected_inverse_many,     menus[6]->GetVisibility(0)->GetInserve() );
    ASSERT_EQ( expected_inverse_unknown,  menus[7]->GetVisibility(0)->GetInserve() );
    ASSERT_EQ( expected_class,            menus[8]->GetVisibility(0)->GetClass() );
    ASSERT_EQ( expected_pattern,          menus[9]->GetVisibility(0)->GetPattern() );
    ASSERT_EQ( expected_exprtk,          menus[10]->GetVisibility(0)->GetExprtk() );

    //menu[11] should contains multiple Validators...
    ASSERT_EQ(3, menus[11]->GetVisibilityCount());
    ASSERT_EQ(2, menus[11]->GetValidityCount());

    //Assert first 3 visibility elements
    static const std::string expected_file_extension1 = "txt";
    static const std::string expected_file_extension2 = "doc";
    static const std::string expected_file_extension3 = "ini";
    ASSERT_EQ( expected_file_extension1, menus[11]->GetVisibility(0)->GetFileExtensions() );
    ASSERT_EQ( expected_file_extension2, menus[11]->GetVisibility(1)->GetFileExtensions() );
    ASSERT_EQ( expected_file_extension3, menus[11]->GetVisibility(2)->GetFileExtensions() );

    //Assert first 2 validity elements
    static const std::string expected_properties1 = "bar";
    ASSERT_EQ( expected_properties1, menus[11]->GetValidity(0)->GetProperties() );
    ASSERT_EQ( 1, menus[11]->GetValidity(1)->GetMaxFiles() );
    ASSERT_EQ( 0, menus[11]->GetValidity(1)->GetMaxDirectories() );

    //Assert remaining menus
    ASSERT_EQ( expected_istrue,          menus[12]->GetVisibility(0)->GetIsTrue() );
    ASSERT_EQ( expected_isfalse,         menus[13]->GetVisibility(0)->GetIsFalse() );
    ASSERT_EQ( expected_isempty,         menus[14]->GetVisibility(0)->GetIsEmpty() );

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseIcon)
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
 
    //ASSERT all 3 menus are available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ( 3, menus.size() );

    //Assert all icons are valid
    ASSERT_TRUE( menus[00]->GetIcon().IsValid() );
    ASSERT_TRUE( menus[01]->GetIcon().IsValid() );
    ASSERT_TRUE( menus[02]->GetIcon().IsValid() );

    //Assert <icon> tag properly parsed
    //menu #00
    ASSERT_EQ( std::string("C:\\Windows\\System32\\shell32.dll"), menus[00]->GetIcon().GetPath() );
    ASSERT_EQ( 42,                                                menus[00]->GetIcon().GetIndex() );

    //menu #01
    ASSERT_EQ( std::string("${application.path}"),  menus[01]->GetIcon().GetPath() );
    ASSERT_EQ( 0,                                   menus[01]->GetIcon().GetIndex() );

    //menu #02
    ASSERT_EQ( std::string("txt"),        menus[02]->GetIcon().GetFileExtension() );
    ASSERT_EQ( std::string(""),           menus[02]->GetIcon().GetPath() );
    ASSERT_EQ( Icon::INVALID_ICON_INDEX,  menus[02]->GetIcon().GetIndex() );

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseMenuMaxLength)
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
 
    //ASSERT all 5 menus are available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ( 5, menus.size() );

    //Assert maxlength properly value for each menus
    ASSERT_EQ( Menu::DEFAULT_NAME_MAX_LENGTH, menus[00]->GetNameMaxLength() );  // maxlength attribute not specified.
    ASSERT_EQ( 3,                             menus[01]->GetNameMaxLength() );  // maxlength attribute set to "3".
    ASSERT_EQ( Menu::DEFAULT_NAME_MAX_LENGTH, menus[02]->GetNameMaxLength() );  // maxlength attribute set to "a" which is not numeric (invalid).
    ASSERT_EQ( Menu::DEFAULT_NAME_MAX_LENGTH, menus[03]->GetNameMaxLength() );  // maxlength attribute set to "0" which is out of range (invalid).
    ASSERT_EQ( Menu::DEFAULT_NAME_MAX_LENGTH, menus[04]->GetNameMaxLength() );  // maxlength attribute set to "9999" which is out of range (invalid).

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseActionExecute)
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

    //ASSERT all menus are available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ( 4, menus.size() );

    //Assert all menus have a file element as the first action
    ActionExecute * exec00 = GetFirstActionExecute(menus[00]);
    ActionExecute * exec01 = GetFirstActionExecute(menus[01]);
    ActionExecute * exec02 = GetFirstActionExecute(menus[02]);
    ActionExecute * exec03 = GetFirstActionExecute(menus[03]);

    ASSERT_TRUE( exec00 != NULL );
    ASSERT_TRUE( exec01 != NULL );
    ASSERT_TRUE( exec02 != NULL );
    ASSERT_TRUE( exec03 != NULL );

    //Assert menu00 attributes
    ASSERT_EQ("C:\\Windows\\System32\\calc.exe", exec00->GetPath());

    //Assert menu01 attributes
    //<exec path="C:\Windows\notepad.exe" basedir="C:\Program Files\7-Zip" arguments="License.txt" />
    ASSERT_EQ("C:\\Windows\\notepad.exe", exec01->GetPath());
    ASSERT_EQ("C:\\Program Files\\7-Zip", exec01->GetBaseDir());
    ASSERT_EQ("License.txt", exec01->GetArguments());

    //Assert menu02 attributes
    //<exec path="C:\Windows\notepad.exe" arguments="C:\Windows\System32\drivers\etc\hosts" verb="runas" />
    ASSERT_EQ("C:\\Windows\\notepad.exe", exec02->GetPath());
    ASSERT_EQ("C:\\Windows\\System32\\drivers\\etc\\hosts", exec02->GetArguments());
    ASSERT_EQ("runas", exec02->GetVerb());

    //Assert menu03 attributes
    //<!-- missing path attribute --> <exec arguments="C:\Windows\System32\drivers\etc\hosts" verb="runas" />
    ASSERT_EQ("", exec03->GetPath());

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseActionStop)
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

    //ASSERT a 13 menus are available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ( 13, menus.size() );

    //Assert all menus have a file element as the first action
    ActionStop * stop00 = GetFirstActionStop(menus[ 0]);
    ActionStop * stop01 = GetFirstActionStop(menus[ 1]);
    ActionStop * stop02 = GetFirstActionStop(menus[ 2]);
    ActionStop * stop03 = GetFirstActionStop(menus[ 3]);
    ActionStop * stop04 = GetFirstActionStop(menus[ 4]);
    ActionStop * stop05 = GetFirstActionStop(menus[ 5]);
    ActionStop * stop06 = GetFirstActionStop(menus[ 6]);
    ActionStop * stop07 = GetFirstActionStop(menus[ 7]);
    ActionStop * stop08 = GetFirstActionStop(menus[ 8]);
    ActionStop * stop09 = GetFirstActionStop(menus[ 9]);
    ActionStop * stop10 = GetFirstActionStop(menus[10]);
    ActionStop * stop11 = GetFirstActionStop(menus[11]);
    ActionStop * stop12 = GetFirstActionStop(menus[12]);

    ASSERT_TRUE( stop00 != NULL && stop00->GetValidator() != NULL );
    ASSERT_TRUE( stop01 != NULL && stop01->GetValidator() != NULL );
    ASSERT_TRUE( stop02 != NULL && stop02->GetValidator() != NULL );
    ASSERT_TRUE( stop03 != NULL && stop03->GetValidator() != NULL );
    ASSERT_TRUE( stop04 != NULL && stop04->GetValidator() != NULL );
    ASSERT_TRUE( stop05 != NULL && stop05->GetValidator() != NULL );
    ASSERT_TRUE( stop06 != NULL && stop06->GetValidator() != NULL );
    ASSERT_TRUE( stop07 != NULL && stop07->GetValidator() != NULL );
    ASSERT_TRUE( stop08 != NULL && stop08->GetValidator() != NULL );
    ASSERT_TRUE( stop09 != NULL && stop09->GetValidator() != NULL );
    ASSERT_TRUE( stop10 != NULL && stop10->GetValidator() != NULL );
    ASSERT_TRUE( stop11 != NULL && stop11->GetValidator() != NULL );
    ASSERT_TRUE( stop12 != NULL && stop12->GetValidator() != NULL );

    ASSERT_EQ("bar",              stop00->GetValidator()->GetProperties());
    ASSERT_EQ(5,                  stop01->GetValidator()->GetMaxFiles());
    ASSERT_EQ(6,                  stop01->GetValidator()->GetMaxDirectories());
    ASSERT_EQ("com;exe;bat;cmd",  stop02->GetValidator()->GetFileExtensions());

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseActionFile)
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

    //ASSERT a 3 menus are available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ( 4, menus.size() );

    //Assert all menus have a file element as the first action
    ActionFile * file00 = GetFirstActionFile(menus[00]);
    ActionFile * file01 = GetFirstActionFile(menus[01]);
    ActionFile * file02 = GetFirstActionFile(menus[02]);
    ActionFile * file03 = GetFirstActionFile(menus[03]);

    ASSERT_TRUE( file00 != NULL );
    ASSERT_TRUE( file01 != NULL );
    ASSERT_TRUE( file02 != NULL );
    ASSERT_TRUE( file03 != NULL );

    //Assert menus have text assigned
    static const std::string EMPTY_STRING;
    ASSERT_NE(EMPTY_STRING, file00->GetText());
    ASSERT_NE(EMPTY_STRING, file01->GetText());
    ASSERT_EQ(EMPTY_STRING, file02->GetText());
    ASSERT_NE(EMPTY_STRING, file03->GetText());

    //Assert menus have encoding
    ASSERT_EQ(EMPTY_STRING, file00->GetEncoding());
    ASSERT_EQ(EMPTY_STRING, file01->GetEncoding());
    ASSERT_EQ(EMPTY_STRING, file02->GetEncoding());
    ASSERT_NE(EMPTY_STRING, file03->GetEncoding());

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseActionPrompt)
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

    //ASSERT a multiple menus are available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ( 5, menus.size() );

    //Assert all menus have a prompt element as the first action
    ActionPrompt * prompt00 = GetFirstActionPrompt(menus[00]);
    ActionPrompt * prompt01 = GetFirstActionPrompt(menus[01]);
    ActionPrompt * prompt02 = GetFirstActionPrompt(menus[02]);
    ActionPrompt * prompt03 = GetFirstActionPrompt(menus[03]);
    ActionPrompt * prompt04 = GetFirstActionPrompt(menus[04]);

    ASSERT_TRUE( prompt00 != NULL );
    ASSERT_TRUE( prompt01 != NULL );
    ASSERT_TRUE( prompt02 != NULL );
    ASSERT_TRUE( prompt03 != NULL );
    ASSERT_TRUE( prompt04 != NULL );

    //Assert menu #0 have no default values
    static const std::string EMPTY_STRING;
    std::string prompt00_default = prompt00->GetDefault();
    ASSERT_EQ( EMPTY_STRING, prompt00_default);

    //Assert menu #1 have a default value
    std::string prompt01_default = prompt01->GetDefault();
    ASSERT_EQ( std::string("42"), prompt01_default);

    //Assert menu #3 is a yesno question
    ASSERT_FALSE( prompt00->IsYesNoQuestion() );
    ASSERT_FALSE( prompt01->IsYesNoQuestion() );
    ASSERT_FALSE( prompt02->IsYesNoQuestion() );
    ASSERT_TRUE ( prompt03->IsYesNoQuestion() );
    ASSERT_FALSE( prompt04->IsYesNoQuestion() );
 
    //Assert menu #3 have a "yes" and a "no" values defined
    ASSERT_TRUE ( prompt00->GetValueYes().empty() );
    ASSERT_TRUE ( prompt00->GetValueNo ().empty() );
    ASSERT_FALSE( prompt03->GetValueYes().empty() );
    ASSERT_FALSE( prompt03->GetValueNo ().empty() );
 
    //Assert menu #4 is a OK only question
    ASSERT_FALSE( prompt00->IsOkQuestion() );
    ASSERT_FALSE( prompt01->IsOkQuestion() );
    ASSERT_FALSE( prompt02->IsOkQuestion() );
    ASSERT_FALSE( prompt03->IsOkQuestion() );
    ASSERT_TRUE ( prompt04->IsOkQuestion() );

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseActionProperty)
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

    //ASSERT a multiple menus are available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ( 3, menus.size() );

    //Assert all menus have a property element as the first action
    ActionProperty * property00 = GetFirstActionProperty(menus[00]);
    ActionProperty * property01 = GetFirstActionProperty(menus[01]);
    ActionProperty * property02 = GetFirstActionProperty(menus[02]);

    ASSERT_TRUE( property00 != NULL );
    ASSERT_TRUE( property01 != NULL );
    ASSERT_TRUE( property02 != NULL );

    //Assert menu #0 have a name and a value parsed
    static const std::string EMPTY_STRING;
    std::string property00_name  = property00->GetName();
    std::string property00_value = property00->GetValue();
    ASSERT_EQ( std::string("foo"), property00_name);
    ASSERT_EQ( std::string("bar"), property00_value);

    //Assert menu #1 have a exprtk attribute parsed
    std::string property01_exprtk = property01->GetExprtk();
    ASSERT_EQ( std::string("1+5"), property01_exprtk);

    //Assert menu #2 have is missing both value and exprtk
    std::string property02_value  = property02->GetValue();
    std::string property02_exprtk = property02->GetExprtk();
    ASSERT_EQ( std::string(""), property02_value);
    ASSERT_EQ( std::string(""), property02_exprtk);

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseActionMessage)
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

    //ASSERT a multiple menus are available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ( 4, menus.size() );

    //Assert all menus have a message element as the first action
    ActionMessage * message00 = GetFirstActionMessage(menus[00]);
    ActionMessage * message01 = GetFirstActionMessage(menus[01]);
    ActionMessage * message02 = GetFirstActionMessage(menus[02]);
    ActionMessage * message03 = GetFirstActionMessage(menus[03]);

    ASSERT_TRUE( message00 != NULL );
    ASSERT_TRUE( message01 != NULL );
    ASSERT_TRUE( message02 != NULL );
    ASSERT_TRUE( message03 != NULL );

    //Assert menu #0 caption and message
    static const std::string EMPTY_STRING;
    ASSERT_EQ( "my_caption",  message00->GetCaption());
    ASSERT_EQ( "my_title",    message00->GetTitle());
    ASSERT_EQ( EMPTY_STRING,  message00->GetIcon());

    //Assert menu #1 have a default value
    ASSERT_EQ( "hello",       message01->GetCaption());
    ASSERT_EQ( "world",       message01->GetTitle());
    ASSERT_EQ( EMPTY_STRING,  message01->GetIcon());

    //Assert menu #2 have a default value
    ASSERT_EQ( "foo",         message02->GetCaption());
    ASSERT_EQ( "bar",         message02->GetTitle());
    ASSERT_EQ( "exclamation", message02->GetIcon());

    //Assert menu #3 have a default value
    ASSERT_EQ( "foo",         message03->GetCaption());
    ASSERT_EQ( "bar",         message03->GetTitle());
    ASSERT_EQ( "warning",     message03->GetIcon());

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseDefaults)
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
 
    //ASSERT a DefaultSettings section is available
    const DefaultSettings * defaults = cmgr.GetConfigurations()[0]->GetDefaultSettings();
    ASSERT_TRUE( defaults != NULL );

    //Assert 2 properties parsed
    ASSERT_EQ( 2, defaults->GetActions().size() );

    const ActionProperty * property1 = dynamic_cast<const ActionProperty *>(defaults->GetActions()[0]);
    const ActionProperty * property2 = dynamic_cast<const ActionProperty *>(defaults->GetActions()[1]);
    ASSERT_TRUE( property1 != NULL );
    ASSERT_TRUE( property2 != NULL );

    ASSERT_EQ( std::string("services.command.start"), property1->GetName());
    ASSERT_EQ( std::string("services.command.stop" ), property2->GetName());
    ASSERT_EQ( std::string("runservice /start"), property1->GetValue());
    ASSERT_EQ( std::string("runservice /stop" ), property2->GetValue());

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestObjectFactory, testParseSeparator)
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
 
    //ASSERT a multiple menus are available
    Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
    ASSERT_EQ(6, menus.size());

    //Assert the following menus are separators
    ASSERT_FALSE(menus[0]->IsSeparator());  // <menu name="menu00" />
    ASSERT_TRUE (menus[1]->IsSeparator());  // <menu separator="true" />
    ASSERT_TRUE (menus[2]->IsSeparator());  // <menu separator="horizontal" />
    ASSERT_FALSE(menus[3]->IsSeparator());  // <menu separator="column" />
    ASSERT_FALSE(menus[4]->IsSeparator());  // <menu separator="vertical" />
    ASSERT_FALSE(menus[5]->IsSeparator());  // <menu name="menu05" />

    //Assert the following menus are column separators
    ASSERT_FALSE(menus[0]->IsColumnSeparator());  // <menu name="menu00" />
    ASSERT_FALSE(menus[1]->IsColumnSeparator());  // <menu separator="true" />
    ASSERT_FALSE(menus[2]->IsColumnSeparator());  // <menu separator="horizontal" />
    ASSERT_TRUE (menus[3]->IsColumnSeparator());  // <menu separator="column" />
    ASSERT_TRUE (menus[4]->IsColumnSeparator());  // <menu separator="vertical" />
    ASSERT_FALSE(menus[5]->IsColumnSeparator());  // <menu name="menu05" />

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
 
} //namespace test
} //namespace shellanything
