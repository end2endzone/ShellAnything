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
#include "rapidassist/strings.h"
#include "rapidassist/filesystem.h"
#include "shellanything/Node.h"

namespace shellanything { namespace test
{
  const std::string test_rgs = ""
     "HKCR\n"
     "{\n"
     "	ForceRemove ShellExtension.ShellAnything.1 = s 'ShellAnything Class'\n"
     "	{\n"
     "		CLSID = s '{B0D35103-86A1-471C-A653-E130E3439A3B}'\n"
     "	}\n"
     "	ForceRemove ShellExtension.ShellAnything = s 'ShellAnything Class'\n"
     "	{\n"
     "		CLSID = s '{B0D35103-86A1-471C-A653-E130E3439A3B}'\n"
     "		CurVer = s 'ShellExtension.ShellAnything.1'\n"
     "	}\n"
     "	NoRemove CLSID\n"
     "	{\n"
     "		ForceRemove {B0D35103-86A1-471C-A653-E130E3439A3B} = s 'ShellAnything Class'\n"
     "		{\n"
     "			ProgID = s 'ShellExtension.ShellAnything.1'\n"
     "			VersionIndependentProgID = s 'ShellExtension.ShellAnything'\n"
     "			ForceRemove 'Programmable'\n"
     "			InprocServer32 = s '%MODULE%'\n"
     "			{\n"
     "				val ThreadingModel = s 'Apartment'\n"
     "			}\n"
     "			'TypeLib' = s '{C0F6D726-56D2-4FF2-A0B7-A50BB10F6EC4}'\n"
     "		}\n"
     "	}\n"
     "	NoRemove *\n"
     "	{\n"
     "		NoRemove ShellEx\n"
     "		{\n"
     "			NoRemove ContextMenuHandlers\n"
     "			{\n"
     "				ForceRemove ShellAnything = s '{B0D35103-86A1-471C-A653-E130E3439A3B}'\n"
     "			}\n"
     "		}\n"
     "	}\n"
     "\n"
     "	NoRemove Directory\n"
     "	{\n"
     "		NoRemove ShellEx\n"
     "		{\n"
     "			NoRemove ContextMenuHandlers\n"
     "			{\n"
     "				ForceRemove ShellAnything = s '{B0D35103-86A1-471C-A653-E130E3439A3B}'\n"
     "			}\n"
     "			NoRemove DragDropHandlers\n"
     "			{\n"
     "				ForceRemove ShellAnything = s '{B0D35103-86A1-471C-A653-E130E3439A3B}'\n"
     "			}\n"
     "		}\n"
     "	}\n"
     "\n"
     "	NoRemove Directory\n"
     "	{\n"
     "		NoRemove Background\n"
     "		{\n"
     "			NoRemove ShellEx\n"
     "			{\n"
     "				NoRemove ContextMenuHandlers\n"
     "				{\n"
     "					ForceRemove ShellAnything = s '{B0D35103-86A1-471C-A653-E130E3439A3B}'\n"
     "				}\n"
     "				NoRemove DragDropHandlers\n"
     "				{\n"
     "					ForceRemove ShellAnything = s '{B0D35103-86A1-471C-A653-E130E3439A3B}'\n"
     "				}\n"
     "			}\n"
     "		}\n"
     "	}\n"
     "\n"
     "	NoRemove Drive\n"
     "	{\n"
     "		NoRemove ShellEx\n"
     "		{\n"
     "			NoRemove ContextMenuHandlers\n"
     "			{\n"
     "				ForceRemove ShellAnything = s '{B0D35103-86A1-471C-A653-E130E3439A3B}'\n"
     "			}\n"
     "			NoRemove DragDropHandlers\n"
     "			{\n"
     "				ForceRemove ShellAnything = s '{B0D35103-86A1-471C-A653-E130E3439A3B}'\n"
     "			}\n"
     "		}\n"
     "	}\n"
     "}\n";
  //--------------------------------------------------------------------------------------------------
  void TestWin32Registry::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestWin32Registry::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWin32Registry, testParseRgs)
  {
    win32_registry::RGS_ENTRY_LIST entries;
    bool result = win32_registry::parseRgsRegistry(test_rgs, "c:\\foo\\bar\\myshellextension.dll", entries);
    ASSERT_TRUE( result );

    for(size_t i=0; i<entries.size(); i++)
    {
      const win32_registry::RGS_ENTRY & entry = entries[i];
      std::string str = win32_registry::toString(entry);
      printf("%s\n", str.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
