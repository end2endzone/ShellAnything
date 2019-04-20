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

#include "TestWin32Registry.h"
#include "win32_registry.h"
#include "shellext_registry_script.h"

#include "rapidassist/strings.h"
#include "rapidassist/filesystem.h"

#include "shellanything/Node.h"

namespace shellanything { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestWin32Registry::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestWin32Registry::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWin32Registry, testRgsParse)
  {
    //get the registry script for the shell extension
    const std::string registry_script = bin2cpp::getRegistryScriptFile().getBuffer();

    win32_registry::RGS_ENTRY_LIST entries;
    bool result = win32_registry::parseRgsRegistry(registry_script, "c:\\foo\\bar\\myshellextension.dll", entries);
    ASSERT_TRUE( result );

    for(size_t i=0; i<entries.size(); i++)
    {
      const win32_registry::RGS_ENTRY & entry = entries[i];
      std::string str = win32_registry::toString(entry);
      printf("%s\n", str.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWin32Registry, testRgsCreate)
  {
    //get the registry script for the shell extension
    const std::string registry_script = bin2cpp::getRegistryScriptFile().getBuffer();

    win32_registry::RGS_ENTRY_LIST entries;
    bool result = win32_registry::parseRgsRegistry(registry_script, "c:\\foo\\bar\\myshellextension.dll", entries);
    ASSERT_TRUE( result );

    //result = win32_registry::createRegistry(entries);
    //ASSERT_TRUE( result );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWin32Registry, testRgsDelete)
  {
    //get the registry script for the shell extension
    const std::string registry_script = bin2cpp::getRegistryScriptFile().getBuffer();

    win32_registry::RGS_ENTRY_LIST entries;
    bool result = win32_registry::parseRgsRegistry(registry_script, "c:\\foo\\bar\\myshellextension.dll", entries);
    ASSERT_TRUE( result );

    //result = win32_registry::deleteRegistry(entries);
    //ASSERT_TRUE( result );
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
