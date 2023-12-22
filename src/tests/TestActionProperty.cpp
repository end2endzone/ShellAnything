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

#include "TestActionProperty.h"
#include "SelectionContext.h"
#include "ActionProperty.h"
#include "ActionFile.h"
#include "PropertyManager.h"
#include "ConfigManager.h"
#include "ActionManager.h"

#include "rapidassist/strings.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/testing.h"
#include "rapidassist/random.h"
#include "rapidassist/undef_windows_macros.h"

#include "Workspace.h"
#include "QuickLoader.h"

namespace shellanything
{
  namespace test
  {
    bool CreateSampleFile(const char* path, size_t size)
    {
      FILE* f = fopen(path, "wb");
      if (!f)
        return false;
      //size_t total_size = 0;
      size_t write_size = 0;

      static const std::string buffer = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
      const size_t buffer_size = buffer.size();

      // Try to write as much full blocks as possible
      while (write_size + buffer_size < size)
      {
        write_size += fwrite(buffer.c_str(), 1, buffer_size, f);
      }

      // Write 1 byte as a time until file is full.
      while (write_size < size)
      {
        static const char* single_char_buffer = ".";
        write_size += fwrite(single_char_buffer, 1, 1, f);
      }

      fclose(f);
      return true;
    }

    //--------------------------------------------------------------------------------------------------
    void TestActionProperty::SetUp()
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();
      pmgr.Clear();
    }
    //--------------------------------------------------------------------------------------------------
    void TestActionProperty::TearDown()
    {
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestActionProperty, testPropertySet)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();

      const std::string property_name = "foo";
      const std::string property_value = "bar";

      //Create a valid context
      SelectionContext c;
      StringList elements;
      elements.push_back("C:\\Windows");
      c.SetElements(elements);

      c.RegisterProperties();

      //execute the action
      ActionProperty ap;
      ap.SetName(property_name);
      ap.SetValue(property_value);

      bool executed = ap.Execute(c);
      ASSERT_TRUE(executed);

      //Assert property is set
      ASSERT_TRUE(pmgr.HasProperty(property_name));
      std::string actual_value = pmgr.GetProperty(property_name);
      ASSERT_EQ(property_value, actual_value);
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestActionProperty, testPropertySelfReference)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();

      const std::string property_name = "foo";

      //Create a valid context
      SelectionContext c;
      StringList elements;
      elements.push_back("C:\\Windows");
      c.SetElements(elements);

      c.RegisterProperties();

      // Set a base value for the property
      pmgr.SetProperty(property_name, "bar");

      // Build a value that "doubles" the initial value
      std::string value;
      value += "${";
      value += property_name;
      value += "}${";
      value += property_name;
      value += "}";

      //execute the action
      ActionProperty ap;
      ap.SetName(property_name);
      ap.SetValue(value);

      bool executed = ap.Execute(c);
      ASSERT_TRUE(executed);

      //Assert property is set
      ASSERT_TRUE(pmgr.HasProperty(property_name));
      std::string actual_value = pmgr.GetProperty(property_name);
      ASSERT_EQ(std::string("barbar"), actual_value);
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestActionProperty, testCounter)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();

      const std::string property_name = "foo";

      //Create a valid context
      SelectionContext c;
      StringList elements;
      elements.push_back("C:\\Windows");
      c.SetElements(elements);

      c.RegisterProperties();

      //Test if the property does not exist
      {
        //Clear the property
        pmgr.ClearProperty(property_name);

        //Execute the action multiple times
        for (int i = 1; i <= 10; i++)
        {
          //Execute the action
          ActionProperty ap;
          ap.SetName(property_name);
          ap.SetExprtk(
            "if ('${foo}' == '$'+'{foo}' or '${foo}' == '0') 1; "
            "else if ('${foo}' == '1') 2; "
            "else if ('${foo}' == '2') 3; "
            "else if ('${foo}' == '3') 4; "
            "else if ('${foo}' == '4') 5; "
            "else if ('${foo}' == '5') 6; "
            "else if ('${foo}' == '6') 7; "
            "else if ('${foo}' == '7') 8; "
            "else if ('${foo}' == '8') 9; "
            "else 10.0; "
          );

          bool executed = ap.Execute(c);
          ASSERT_TRUE(executed);

          //Set expected property value
          std::string expected = ra::strings::ToString(i);

          //Assert property is set
          ASSERT_TRUE(pmgr.HasProperty(property_name));
          std::string actual_value = pmgr.GetProperty(property_name);
          ASSERT_EQ(expected, actual_value);
        }
      }

      //Test if the property is set with a default value
      {
        //Simulate default property set to 0.
        pmgr.SetProperty(property_name, "0");

        //Execute the action multiple times
        for (int i = 1; i <= 10; i++)
        {
          //Execute the action
          ActionProperty ap;
          ap.SetName(property_name);
          ap.SetExprtk("${foo}+1.0");

          bool executed = ap.Execute(c);
          ASSERT_TRUE(executed);

          //Set expected property value
          std::string expected = ra::strings::ToString(i);

          //Assert property is set
          ASSERT_TRUE(pmgr.HasProperty(property_name));
          std::string actual_value = pmgr.GetProperty(property_name);
          ASSERT_EQ(expected, actual_value);
        }
      }

      int a = 0;
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestActionProperty, testPropertyGetLength)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();

      const std::string property_name = "foo";

      //Create a valid context
      SelectionContext c;
      StringList elements;
      elements.push_back("C:\\Windows");
      c.SetElements(elements);

      c.RegisterProperties();

      // Set a base value for the property
      pmgr.SetProperty(property_name, "barbaz");

      //execute the action
      ActionProperty ap;
      ap.SetName(property_name);
      ap.SetExprtk("'${foo}'[]");

      bool executed = ap.Execute(c);
      ASSERT_TRUE(executed);

      //Assert property is set
      ASSERT_TRUE(pmgr.HasProperty(property_name));
      std::string actual_value = pmgr.GetProperty(property_name);
      ASSERT_EQ(std::string("6"), actual_value);
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestActionProperty, testPropertyFileReadFullFile)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();

      //Creating a temporary workspace for the test execution.
      Workspace workspace;
      ASSERT_FALSE(workspace.GetBaseDirectory().empty());

      //Define the path for a generated file
      static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
      std::string test_name = ra::testing::GetTestQualifiedName();
      test_name += ".dat";

      //Generate a small filename
      std::string file_path = workspace.GetFullPathUtf8(test_name.c_str());
      bool file_created = CreateSampleFile(file_path.c_str(), 7);
      ASSERT_TRUE(file_created);

      const std::string property_name = "foo";

      //Create a valid context
      SelectionContext c;
      StringList elements;
      elements.push_back("C:\\Windows");
      c.SetElements(elements);

      c.RegisterProperties();

      // Clear pre existing property
      pmgr.ClearProperty(property_name);

      //execute the action
      ActionProperty ap;
      ap.SetName(property_name);
      ap.SetFile(file_path);

      bool executed = ap.Execute(c);
      ASSERT_TRUE(executed);

      //Assert property is set
      ASSERT_TRUE(pmgr.HasProperty(property_name));
      std::string actual_value = pmgr.GetProperty(property_name);
      size_t actual_size = actual_value.size();
      ASSERT_EQ(7, actual_size);
      ASSERT_EQ(std::string("......."), actual_value);

      //Cleanup
      ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestActionProperty, testPropertyFileReadFileUpToDefaultSize)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();

      //Creating a temporary workspace for the test execution.
      Workspace workspace;
      ASSERT_FALSE(workspace.GetBaseDirectory().empty());

      //Define the path for a generated file
      static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
      std::string test_name = ra::testing::GetTestQualifiedName();
      test_name += ".dat";

      //Generate a small filename
      std::string file_path = workspace.GetFullPathUtf8(test_name.c_str());
      bool file_created = CreateSampleFile(file_path.c_str(), 1024*1024);
      ASSERT_TRUE(file_created);

      const std::string property_name = "foo";

      //Create a valid context
      SelectionContext c;
      StringList elements;
      elements.push_back("C:\\Windows");
      c.SetElements(elements);

      c.RegisterProperties();

      // Clear pre existing property
      pmgr.ClearProperty(property_name);

      //execute the action
      ActionProperty ap;
      ap.SetName(property_name);
      ap.SetFile(file_path);

      bool executed = ap.Execute(c);
      ASSERT_TRUE(executed);

      //Assert property is set
      ASSERT_TRUE(pmgr.HasProperty(property_name));
      std::string actual_value = pmgr.GetProperty(property_name);
      size_t actual_size = actual_value.size();
      ASSERT_EQ(ActionProperty::DEFAULT_MAX_FILE_SIZE, actual_size);
      //ASSERT_EQ(std::string("......."), actual_value);

      //Cleanup
      ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestActionProperty, testPropertyFileSize32)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();

      //Creating a temporary workspace for the test execution.
      Workspace workspace;
      ASSERT_FALSE(workspace.GetBaseDirectory().empty());

      //Define the path for a generated file
      static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
      std::string test_name = ra::testing::GetTestQualifiedName();
      test_name += ".dat";

      //Generate a small filename
      std::string file_path = workspace.GetFullPathUtf8(test_name.c_str());
      bool file_created = CreateSampleFile(file_path.c_str(), 1024*1024);
      ASSERT_TRUE(file_created);

      const std::string property_name = "foo";

      //Create a valid context
      SelectionContext c;
      StringList elements;
      elements.push_back("C:\\Windows");
      c.SetElements(elements);

      c.RegisterProperties();

      // Clear pre existing property
      pmgr.ClearProperty(property_name);

      //execute the action
      ActionProperty ap;
      ap.SetName(property_name);
      ap.SetFile(file_path);
      ap.SetFileSize("32");

      bool executed = ap.Execute(c);
      ASSERT_TRUE(executed);

      //Assert property is set
      ASSERT_TRUE(pmgr.HasProperty(property_name));
      std::string actual_value = pmgr.GetProperty(property_name);
      size_t actual_size = actual_value.size();
      ASSERT_EQ(32, actual_size);
      ASSERT_EQ(std::string("0123456789abcdefghijklmnopqrstuv"), actual_value);

      //Cleanup
      ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestActionProperty, testPropertyFileSize0)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();

      //Creating a temporary workspace for the test execution.
      Workspace workspace;
      ASSERT_FALSE(workspace.GetBaseDirectory().empty());

      //Define the path for a generated file
      static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
      std::string test_name = ra::testing::GetTestQualifiedName();
      test_name += ".dat";

      //Generate a small filename
      std::string file_path = workspace.GetFullPathUtf8(test_name.c_str());
      bool file_created = CreateSampleFile(file_path.c_str(), 1024 * 1024);
      ASSERT_TRUE(file_created);

      const std::string property_name = "foo";

      //Create a valid context
      SelectionContext c;
      StringList elements;
      elements.push_back("C:\\Windows");
      c.SetElements(elements);

      c.RegisterProperties();

      // Clear pre existing property
      pmgr.ClearProperty(property_name);

      //execute the action
      ActionProperty ap;
      ap.SetName(property_name);
      ap.SetFile(file_path);
      ap.SetFileSize("0");

      bool executed = ap.Execute(c);
      ASSERT_TRUE(executed);

      //Assert property is set
      ASSERT_TRUE(pmgr.HasProperty(property_name));
      std::string actual_value = pmgr.GetProperty(property_name);
      size_t actual_size = actual_value.size();
      ASSERT_EQ(1024 * 1024, actual_size);
      //ASSERT_EQ(std::string(""), actual_value);

      //Cleanup
      ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestActionProperty, testCopyFile)
    {
      ConfigManager& cmgr = ConfigManager::GetInstance();
      PropertyManager& pmgr = PropertyManager::GetInstance();

      //Creating a temporary workspace for the test execution.
      Workspace workspace;
      ASSERT_FALSE(workspace.GetBaseDirectory().empty());

      //Load the test Configuration File that matches this test name.
      QuickLoader loader;
      loader.SetWorkspace(&workspace);
      ASSERT_TRUE(loader.DeleteConfigurationFilesInWorkspace());
      ASSERT_TRUE(loader.LoadCurrentTestConfigurationFile());

      //Find expected menus.
      Menu* menu_copy = cmgr.FindMenuByName("Copy as text");
      Menu* menu_paste = cmgr.FindMenuByName("Paste");
      ASSERT_TRUE(menu_copy != NULL);
      ASSERT_TRUE(menu_paste != NULL);

      //Generate a small filename in the workspace for the selection context.
      std::string file_name = ra::testing::GetTestQualifiedName() + ".txt";
      std::string file_path = workspace.GetFullPathUtf8(file_name.c_str());
      bool file_created = CreateSampleFile(file_path.c_str(), 75);
      ASSERT_TRUE(file_created);

      //Create a valid context
      SelectionContext c;
      StringList elements;
      elements.push_back(file_path);
      c.SetElements(elements);
      c.RegisterProperties();

      // Execute "copy"
      bool executed = ActionManager::Execute(menu_copy, c);
      ASSERT_TRUE(executed);

      //simulate user has right click a directory (change selection to a directory)
      elements.clear();
      elements.push_back(workspace.GetBaseDirectory());
      c.SetElements(elements);
      c.RegisterProperties();

      // Execute "paste"
      executed = ActionManager::Execute(menu_paste, c);
      ASSERT_TRUE(executed);

      //Assert a new file copy was generated
      std::string expected_file_path = file_path + ".copy.txt";
      ASSERT_TRUE(ra::filesystem::FileExists(expected_file_path.c_str()));
      ASSERT_TRUE(ra::testing::IsFileEquals(expected_file_path.c_str(), file_path.c_str()));

      //Cleanup
      ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestActionProperty, testCaptureOutput)
    {
      ConfigManager& cmgr = ConfigManager::GetInstance();
      PropertyManager& pmgr = PropertyManager::GetInstance();

      //Creating a temporary workspace for the test execution.
      Workspace workspace;
      ASSERT_FALSE(workspace.GetBaseDirectory().empty());

      //Load the test Configuration File that matches this test name.
      QuickLoader loader;
      loader.SetWorkspace(&workspace);
      ASSERT_TRUE(loader.DeleteConfigurationFilesInWorkspace());
      ASSERT_TRUE(loader.LoadCurrentTestConfigurationFile());

      //Generate a random filename for the command output.
      std::string command_output_file_name = "command_output_file." + ra::random::GetRandomString(5) + ".txt";
      std::string command_output_file_path = workspace.GetFullPathUtf8(command_output_file_name.c_str());
      pmgr.SetProperty("command_output_file", command_output_file_path);

      //Find expected menus.
      Menu* menu = cmgr.FindMenuByName("Capture output");
      ASSERT_TRUE(menu != NULL);

      //Create a valid context
      SelectionContext c;
      StringList elements;
      elements.push_back("C:\\Windows");
      c.SetElements(elements);
      c.RegisterProperties();

      // Execute
      bool executed = ActionManager::Execute(menu, c);
      ASSERT_TRUE(executed);

      //Assert a new file copy was generated
      ASSERT_TRUE(ra::filesystem::FileExists(command_output_file_path.c_str()));

      // Read from the file itself
      std::string data;
      bool file_read = ra::filesystem::ReadFile(command_output_file_path, data);
      ASSERT_TRUE(file_read);

      //Assert the property value matches the content of the file
      ASSERT_TRUE(pmgr.HasProperty("files"));
      std::string property_files = pmgr.GetProperty("files");

      ASSERT_EQ(property_files, data);

      //Cleanup
      ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestActionProperty, testRegistryKey)
    {
      ConfigManager& cmgr = ConfigManager::GetInstance();
      PropertyManager& pmgr = PropertyManager::GetInstance();

      //Creating a temporary workspace for the test execution.
      Workspace workspace;
      ASSERT_FALSE(workspace.GetBaseDirectory().empty());

      //Load the test Configuration File that matches this test name.
      QuickLoader loader;
      loader.SetWorkspace(&workspace);
      ASSERT_TRUE(loader.DeleteConfigurationFilesInWorkspace());
      ASSERT_TRUE(loader.LoadCurrentTestConfigurationFile());

      //Get all menus.
      Configuration::ConfigurationPtrList configs = cmgr.GetConfigurations();
      ASSERT_EQ(1, configs.size());

      //ASSERT a single menu is available
      Menu::MenuPtrList menus = cmgr.GetConfigurations()[0]->GetMenus();
      ASSERT_GT(menus.size(), 1);

      //Clear properties
      static const char* properties[] = {
        "test1",
        "test2",
        "test3",
        "test4",
        "test5",
      };
      static const size_t properties_count = sizeof(properties) / sizeof(properties[0]);
      for (size_t i = 0; i < properties_count; i++)
      {
        pmgr.ClearProperty(properties[i]);
      }

      //Create a valid context
      SelectionContext c;
      StringList elements;
      elements.push_back("C:\\Windows");
      c.SetElements(elements);
      c.RegisterProperties();

      // Execute
      for (size_t i = 0; i < menus.size(); i++)
      {
        Menu* menu = menus[i];
        bool executed = ActionManager::Execute(menu, c);
        ASSERT_TRUE(executed) << "Failed to execute actions of menu '" << menu->GetName() << "'.";
      }

      //ASSERT the properties were set
      for (size_t i = 0; i < properties_count; i++)
      {
        const char* property_name = properties[i];
        ASSERT_TRUE(pmgr.HasProperty(property_name)) << "Property not found: '" << property_name << "'.";
      }

      //ASSERT a single value
      std::string test1 = pmgr.GetProperty("test1");
      ASSERT_EQ("C:\\Program Files\\7-Zip\\", test1);

      //Cleanup
      ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
    }

  } //namespace test
} //namespace shellanything
