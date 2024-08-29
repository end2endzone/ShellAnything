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

#include "TestTools.h"
#include "Workspace.h"
#include "PropertyManager.h"
#include "ConfigManager.h"
#include "SelectionContext.h"
#include "ActionManager.h"

#include "rapidassist/testing.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/timing.h"

namespace shellanything
{
  namespace test
  {
    //--------------------------------------------------------------------------------------------------
    void KillArgumentsDebugger()
    {
      system("cmd.exe /c taskkill /IM arguments.debugger.* >NUL 2>NUL");
    }
    //--------------------------------------------------------------------------------------------------
    void TestTools::SetUp()
    {
      //Delete the configurations which source files are deleted
      ConfigManager& cmgr = ConfigManager::GetInstance();
      cmgr.Refresh();

      //Delete the source file of all remaining Configuration instance
      ConfigFile::ConfigFilePtrList configs = cmgr.GetConfigFiles();
      for (size_t i = 0; i < configs.size(); i++)
      {
        ConfigFile* config = configs[i];
        if (config)
        {
          const std::string& file_path = config->GetFilePath();
          ASSERT_TRUE(ra::filesystem::DeleteFile(file_path.c_str())) << "Failed deleting file '" << file_path << "'.";
        }
      }

      //Now that all configuration files are deleted, refresh again
      cmgr.Refresh();

      //ASSERT that no files are loaded
      ASSERT_EQ(0, cmgr.GetConfigFiles().size());
    }
    //--------------------------------------------------------------------------------------------------
    void TestTools::TearDown()
    {
      KillArgumentsDebugger();
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestTools, testArgumentsDebugger) // THIS IS A VISUAL TEST DESIGNED TO SEE THAT ARGUMENTS MUST BE PROPERLY ENCODED. SEE ASSOCIATED XML FOR DETAILS.
    {
      ConfigManager& cmgr = ConfigManager::GetInstance();

      //Creating a temporary workspace for the test execution.
      Workspace workspace;
      ASSERT_FALSE(workspace.GetBaseDirectory().empty());
      ASSERT_TRUE(workspace.IsEmpty());

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
      ConfigFile::ConfigFilePtrList configs = cmgr.GetConfigFiles();
      ASSERT_EQ(1, configs.size());

      //ASSERT at least 1 menu is available
      Menu::MenuPtrList menus = cmgr.GetConfigFiles()[0]->GetMenus();
      ASSERT_GE(1, menus.size());

      // Set a property to allow properly detecting debug executables
      PropertyManager& pmgr = PropertyManager::GetInstance();
#ifdef _DEBUG
      pmgr.SetProperty("debug.postfix", "-d");
#endif

      //Create a valid context
      SelectionContext c;
      StringList elements;
      elements.push_back("C:\\Windows\\System32\\calc.exe");
      c.SetElements(elements);

      c.RegisterProperties();

      // Execute all menus
      for (size_t i = 0; i < menus.size(); i++)
      {
        Menu* menu = menus[i];
        ASSERT_TRUE(menu != NULL);

        bool executed = ActionManager::Execute(menu, c);
        ASSERT_TRUE(executed) << "Failed executing menu[" << i << "] with title '" << menu->GetName() << "'.";
      }

      //Cleanup
      KillArgumentsDebugger();
      ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
    }
    //--------------------------------------------------------------------------------------------------

  } //namespace test
} //namespace shellanything
