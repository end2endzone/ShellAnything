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

#include "TestConfiguration.h"
#include "Workspace.h"
#include "ConfigManager.h"
#include "Configuration.h"
#include "Menu.h"
#include "ActionExecute.h"

#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/testing.h"
#include "rapidassist/timing.h"

#include "PropertyManager.h"

namespace shellanything { namespace test
{
  static const Configuration * INVALID_CONFIGURATION = NULL;

  std::string BuildConfigurationFileWithEncoding(const std::string & encoding)
  {
    std::string file;
    file += "<?xml version=\"1.0\" encoding=\"" + encoding + "\"?>\n";
    file += "<root>\n";
    file += "  <shell>\n";
    file += "    <menu name=\"test\" />\n";
    file += "  </shell>\n";
    file += "</root>\n";
    return file;
  }

  //--------------------------------------------------------------------------------------------------
  void TestConfiguration::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestConfiguration::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfiguration, testDefault)
  {
    //fake the implementation
    Configuration * config = new Configuration();
    config->SetFilePath("C:\\Users\\MartyMcfly\\Documents\\ShellAnything\\default.xml");
    
    // <menu name="Command line from here...">
    //   <icon path="C:\Windows\System32\shell32.dll" index="3" />
    //   <validity folder="true" />
    //   <actions>
    //     <exec path="C:\windows\system32\cmd.exe" basedir="${selection.path}" />
    //   </actions>
    // </menu>
    Menu * menu = new Menu();
    menu->SetName("Command line from here...");
    Validator * validity = new Validator();
    validity->SetMaxFiles(0); //no files accepted
    validity->SetMaxDirectories(1); //a single directory is accepted
    
    ActionExecute * action = new ActionExecute();
    action->SetPath("C:\\windows\\system32\\cmd.exe");
    action->SetBaseDir("C:\\Users\\MartyMcfly\\AppData\\Local\\Temp");

    //link everything
    config->AddMenu(menu);
    menu->AddValidity(validity);
    menu->AddAction(action);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfiguration, testIsValidConfigFile)
  {
    static const char * files[] = {
      //default configuration files
      "configurations\\default.xml",
      "configurations\\Microsoft Office 2003.xml",
      "configurations\\Microsoft Office 2007.xml",
      "configurations\\Microsoft Office 2010.xml",
      "configurations\\Microsoft Office 2013.xml",
      "configurations\\Microsoft Office 2016.xml",
      "configurations\\shellanything.xml",
      "configurations\\WinDirStat.xml",

      //test configuration files
      "test_files\\samples.xml",
      "test_files\\TestConfigManager.testAssignCommandId.1.xml",
      "test_files\\TestConfigManager.testAssignCommandId.2.xml",
      "test_files\\TestConfigManager.testAssignCommandIdsInvalid.xml",
      "test_files\\TestConfigManager.testClear.xml",
      "test_files\\TestConfigManager.testDetectNewFile.xml",
      "test_files\\TestConfigManager.testFileModifications.xml",
      "test_files\\TestConfigManager.testParentWithoutChildren.xml",
      "test_files\\TestConfiguration.testLoadProperties.xml",
      "test_files\\TestObjectFactory.testParseActionExecute.xml",
      "test_files\\TestObjectFactory.testParseActionFile.xml",
      "test_files\\TestObjectFactory.testParseActionPrompt.xml",
      "test_files\\TestObjectFactory.testParseActionProperty.xml",
      "test_files\\TestObjectFactory.testParseActionStop.xml",
      "test_files\\TestObjectFactory.testParseDefaults.xml",
      "test_files\\TestObjectFactory.testParseIcon.xml",
      "test_files\\TestObjectFactory.testParseMenuMaxLength.xml",
      "test_files\\TestObjectFactory.testParseSeparator.xml",
      "test_files\\TestObjectFactory.testParseValidator.xml",
      "test_files\\tests.xml",
    };
    const size_t num_files = sizeof(files)/sizeof(files[0]);
    
    //for each test files
    for(size_t i=0; i<num_files; i++)
    {
      const std::string path = files[i];
      ASSERT_TRUE( ra::filesystem::FileExists(path.c_str()) ) << "File '" << path.c_str() << "' is not found.";
      ASSERT_TRUE( shellanything::Configuration::IsValidConfigFile(path) ) << "The file '" << path.c_str() << "' is not a valid configuration file.";
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfiguration, testIsValidConfigFileUtf8)
  {
    static const std::string separator = ra::filesystem::GetPathSeparatorStr();
    const std::string source_path = "configurations/default.xml";
    std::string target_path = ra::filesystem::GetTemporaryDirectory() + separator + ra::testing::GetTestQualifiedName() + ".psi_\xCE\xA8_psi.xml";

    //copy default config to the new utf-8 path
    bool copied = ra::filesystem::CopyFileUtf8(source_path, target_path);
    ASSERT_TRUE(copied) << "Failed to copy file '" << source_path << "' to '" << target_path << "'.";

    ASSERT_TRUE( shellanything::Configuration::IsValidConfigFile(target_path) ) << "The file '" << target_path.c_str() << "' is not a valid configuration file.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfiguration, testLoadFile)
  {
    const std::string path = "configurations/default.xml";
    std::string error_message = ra::testing::GetTestQualifiedName(); //init error message to an unexpected string
    Configuration * config = Configuration::LoadFile(path, error_message);

    ASSERT_TRUE( error_message.empty() ) << "error_message=" << error_message;
    ASSERT_NE( INVALID_CONFIGURATION, config );

    //cleanup
    delete config;
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfiguration, testLoadFileUtf8)
  {
    //This test validates that Configuration::LoadFile() supports filename with utf-8 characters.

    static const std::string separator = ra::filesystem::GetPathSeparatorStr();
    const std::string source_path = "configurations/default.xml";
    std::string target_path = ra::filesystem::GetTemporaryDirectory() + separator + ra::testing::GetTestQualifiedName() + ".psi_\xCE\xA8_psi.xml";

    //copy default config to the new utf-8 path
    bool copied = ra::filesystem::CopyFileUtf8(source_path, target_path);
    ASSERT_TRUE(copied) << "Failed to copy file '" << source_path << "' to '" << target_path << "'.";

    std::string error_message = ra::testing::GetTestQualifiedName(); //init error message to an unexpected string
    Configuration * config = Configuration::LoadFile(target_path, error_message);

    ASSERT_TRUE( error_message.empty() ) << "error_message=" << error_message;
    ASSERT_NE( INVALID_CONFIGURATION, config );

    //cleanup
    delete config;
    ra::filesystem::DeleteFileUtf8(target_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfiguration, testLoadFileEncoding)
  {
    static const std::string separator = ra::filesystem::GetPathSeparatorStr();
    std::string path = ra::filesystem::GetTemporaryDirectory() + separator + ra::testing::GetTestQualifiedName() + ".xml";

    //-----------------------------------------
    // supported encoding
    //-----------------------------------------
    {
      static const char * encodings[] = {
        "utf-8",
        "UTF-8",
      };
      const size_t num_encodings = sizeof(encodings)/sizeof(encodings[0]);

      //for each params
      for(size_t i=0; i<num_encodings; i++)
      {
        const char * encoding = encodings[i];

        std::string content = BuildConfigurationFileWithEncoding(encoding);
        bool created = ra::filesystem::WriteTextFile(path, content);
        ASSERT_TRUE( created ) << "Failed to create text file: " << path;

        //load this config
        std::string error_message;
        Configuration * config = Configuration::LoadFile(path, error_message);

        //build error description message, if required
        std::string failure_desc;
        failure_desc << "Failed to load configuration file. Encoding=" << encoding << ". Error_message=" << error_message << ".";

        //assert success
        ASSERT_TRUE( error_message.empty() ) << failure_desc;
        ASSERT_NE( INVALID_CONFIGURATION, config ) << failure_desc;

        //cleanup
        delete config;
      }
    } //supported encoding



    //-----------------------------------------
    // unsupported encoding
    //-----------------------------------------
    {
      static const char * encodings[] = {
        "",
        "windows-1252",
        "iso-8859-1",
      };
      const size_t num_encodings = sizeof(encodings)/sizeof(encodings[0]);

      //for each params
      for(size_t i=0; i<num_encodings; i++)
      {
        const char * encoding = encodings[i];

        std::string content = BuildConfigurationFileWithEncoding(encoding);
        bool created = ra::filesystem::WriteTextFile(path, content);
        ASSERT_TRUE( created ) << "Failed to create text file: " << path;

        //load this config
        std::string error_message;
        Configuration * config = Configuration::LoadFile(path, error_message);

        //build error description message, if required
        std::string failure_desc;
        failure_desc << "Expecting a load failure but the configuration load was succesful. Encoding=" << encoding << ". Error_message=" << error_message << ".";

        //assert success
        ASSERT_FALSE( error_message.empty() ) << failure_desc;
        ASSERT_EQ( INVALID_CONFIGURATION, config ) << failure_desc;
      }
    } //unsupported encoding

  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfiguration, testLoadProperties)
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

    //cleanup properties
    PropertyManager & pmgr = PropertyManager::GetInstance();
    pmgr.Clear();
 
    //Setup ConfigManager to read files from workspace
    cmgr.ClearSearchPath();
    cmgr.AddSearchPath(workspace.GetBaseDirectory());
    cmgr.Refresh();
 
    //ASSERT the file is loaded
    Configuration::ConfigurationPtrList configs = cmgr.GetConfigurations();
    ASSERT_EQ( 1, configs.size() );
 
    //ASSERT that properties was applied
    static const std::string EMPTY_STRING;
    std::string services_command_start = pmgr.GetProperty("services.command.start");
    std::string services_command_stop  = pmgr.GetProperty("services.command.stop");
    ASSERT_NE( EMPTY_STRING, services_command_start );
    ASSERT_NE( EMPTY_STRING, services_command_stop  );

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
