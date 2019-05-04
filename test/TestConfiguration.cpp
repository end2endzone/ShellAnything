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
#include "shellanything/Configuration.h"
#include "shellanything/Menu.h"
#include "shellanything/ActionExecute.h"

#include "rapidassist/filesystem.h"

namespace shellanything { namespace test
{

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
    config->setFilePath("C:\\Users\\MartyMcfly\\Documents\\ShellAnything\\default.xml");
    
    // <menu name="Command line from here...">
    //   <icon path="C:\Windows\System32\shell32.dll" index="3" />
    //   <validity folder="true" />
    //   <actions>
    //     <exec path="C:\windows\system32\cmd.exe" basedir="${selection.path}" />
    //   </actions>
    // </menu>
    Menu * menu = new Menu();
    menu->setName("Command line from here...");
    Validator validity;
    validity.setMaxFiles(0); //no files accepted
    validity.setMaxDirectories(1); //a single directory is accepted
    Validator visibility; //no requirement, always visible
    ActionExecute * action = new ActionExecute();
    action->setPath("C:\\windows\\system32\\cmd.exe");
    action->setBaseDir("C:\\Users\\MartyMcfly\\AppData\\Local\\Temp");

    //link everything
    config->addChild(menu);
    menu->setValidity(validity);
    menu->setVisibility(visibility);
    menu->addAction(action);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConfiguration, testIsValidConfigFile)
  {
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
      ASSERT_TRUE( shellanything::Configuration::isValidConfigFile(path) );
    }
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
