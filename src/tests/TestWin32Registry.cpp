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

#pragma warning( push )
#pragma warning( disable: 4018 ) //googletest\install\include\gtest/gtest.h(1528): warning C4018: '>=' : signed/unsigned mismatch
#include <gtest/gtest.h>
#pragma warning( pop )

#include "TestWin32Registry.h"
#include "Workspace.h"
#include "Win32Registry.h"
#include "Win32Utils.h"
#include <Windows.h>
#undef GetEnvironmentVariable
#undef DeleteFile
#undef CreateDirectory
#undef CopyFile
#undef CreateFile

#include "rapidassist/strings.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/testing.h"

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
    Win32Registry::RGS_ENTRY_LIST entries;
    bool result = Win32Registry::ParseRgsRegistry(test_rgs, "c:\\foo\\bar\\myshellextension.dll", entries);
    ASSERT_TRUE( result );

    for(size_t i=0; i<entries.size(); i++)
    {
      const Win32Registry::RGS_ENTRY & entry = entries[i];
      std::string str = Win32Registry::ToString(entry);
      printf("%s\n", str.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWin32Registry, testGetFileTypeIcon)
  {
    //Creating a temporary workspace for the test execution.
    Workspace workspace;
    ASSERT_FALSE(workspace.GetBaseDirectory().empty());

    static const char * file_extensions[] = {
      "application", "avi", 
      "bat", "bmp", 
      "c", "cab", "chm", "cmd", "cpp", "cs", "csproj", "css", "csv", 
      "def", "dll", "doc", "docx", "dsp", "dsw", "dtd", 
      "exe", 
      "gif", 
      "h", "htm", "html", 
      "inf", "ini", 
      "jar", "java", "jpeg", "jpg", "js", 
      "lib", "log", 
      "mdb", "mid", "mp3", "mpg", "mpeg", "msi", 
      "obj", "ocx", 
      "pdf", "png", "pptx", 
      "rc", "reg", "res", "resx", "rgs", 
      "sln", "svg", "sys", 
      "tga", "tif", "tmp", "ttf", "txt", 
      "udf", "url", "user", 
      "vb", "vbs", "vcproj", "vcxproj", "vdproj", 
      "war", "wav", "website", "wmv", 
      "xaml", "xls", "xlsm", "xlsx", "xml", "xsd", "xslt", 
      "zip", 
    };
    static const size_t num_file_extensions = sizeof(file_extensions)/sizeof(file_extensions[0]);

    //for each file extensions
    for(size_t i=0; i<num_file_extensions; i++)
    {
      const char * file_extension = file_extensions[i];

      //create a fake file with the extensions for visual reference
      static const size_t BUFFER_SIZE = 1024;
      char filename[BUFFER_SIZE];
      sprintf(filename, "%03Iu.sample_file.%s", i, file_extension);
      std::string file_path = workspace.GetFullPathUtf8(filename);
      bool created = ra::testing::CreateFile(file_path.c_str());
      ASSERT_TRUE( created ) << "Failed creating file '" << file_path << "'.";
      
      //get the icon matching this file's extension
      Win32Registry::REGISTRY_ICON icon = Win32Registry::GetFileTypeIcon(file_extension);
      if (icon.path.empty())
      {
        printf("Failed to find icon for file extension. Is this expected? file_extension='%s'. \n", file_extension);

        //use default unknown icon
        icon = Win32Registry::GetUnknownFileTypeIcon();
      }

      //load and save the found icon to a file
      if (!icon.path.empty())
      {
        HICON hIconLarge = NULL;
        HICON hIconSmall = NULL;

        UINT numIconInFile = ExtractIconEx( icon.path.c_str(), -1, NULL, NULL, 1 );
        UINT numIconLoaded = ExtractIconEx( icon.path.c_str(), icon.index, &hIconLarge, &hIconSmall, 1 );
        ASSERT_GE( numIconLoaded, 1) << "Failed to load at least one icon from file '" << icon.path << "' for file extension '" << file_extension << "'."; //at least 1 icon loaded
        
        //select best icon
        HICON hIconBest = NULL;
        if (hIconSmall)
          hIconBest = hIconSmall;
        if (hIconLarge)
          hIconBest = hIconLarge;
        ASSERT_TRUE( hIconBest != NULL ) << "Unable to load large/small icon from file '" << icon.path << "' for file extension '" << file_extension << "'.";

        //Convert the icon to a bitmap (with invisible background)
        SIZE icon_size = Win32Utils::GetIconSize(hIconBest);
        HBITMAP hBitmap = Win32Utils::CopyAsBitmap(hIconBest, icon_size.cx, icon_size.cy);

        DestroyIcon(hIconLarge);
        DestroyIcon(hIconSmall);

        //save to a file
        file_path = ra::testing::GetTestQualifiedName();
        sprintf(filename, "%03Iu.icon_%s.bmp", i, file_extension);
        file_path = workspace.GetFullPathUtf8(filename);
        created = Win32Utils::CreateBmpFile(file_path.c_str(), hBitmap);

        //delete the bitmap
        DeleteObject(hBitmap);

        ASSERT_TRUE( created ) << "Failed creating file '" << file_path << "'.";
      }
    }

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWin32Registry, testGetFileTypeIconSDF)
  {
    //https://github.com/end2endzone/ShellAnything/issues/17

    //The file extension "SDF" is a special case.
    //The file type is registered when you install Visual Studio on the system.
    //The "type" of the file is SQL Server Compact Edition Database File.
    //The icon is defined in HKEY_CLASSES_ROOT\Microsoft SQL Server Compact Edition Database File\DefaultIcon with a default value of C:\Program Files\Microsoft SQL Server Compact Edition\v3.5\sqlceme35.dll,-1.
    //
    //However, the index -1 is an invalid value for ExtractIconEx().
    //See https://docs.microsoft.com/en-us/windows/desktop/api/shellapi/nf-shellapi-extracticonexa
    //
    //When nIconIndex is negative, it means that abs(nIconIndex) is the value of a RESSOURCE ID.
    //If nIconIndex is positive, then nIconIndex refers to a 0-based index inside the file.
    //
    //The value -1 is a special case for nIconIndex and commands ExtractIconEx() to return the total number of icons inside the file instead of loading the icon whose RESSOURCE ID is 1.
    //For more detalis, see https://stackoverflow.com/questions/24786234/correct-usage-of-the-extracticonex-niconindex-parameter
    //
    //The function GetFileTypeIcon() should be bullet proof when dealing with an invalid index value.

    //Creating a temporary workspace for the test execution.
    Workspace workspace;
    ASSERT_FALSE(workspace.GetBaseDirectory().empty());

    static const char * file_extension = "sdf";

    //create a fake file with the extensions for visual reference
    static const size_t BUFFER_SIZE = 1024;
    char filename[BUFFER_SIZE];
    sprintf(filename, ".sample_file.%s", file_extension);
    std::string file_path = workspace.GetFullPathUtf8(filename);
    bool created = ra::testing::CreateFile(file_path.c_str());
    ASSERT_TRUE( created ) << "Failed creating file '" << file_path << "'.";

    //get the icon matching this file's extension
    Win32Registry::REGISTRY_ICON icon = Win32Registry::GetFileTypeIcon(file_extension);

    //For debugging on each system where the test is executed.
    printf("Found icon in file '%s', index '%d'.\n", icon.path.c_str(), icon.index);

    //assert a valid index was returned
    if (!icon.path.empty())
    {
      ASSERT_NE(icon.index, -1);
    }

    //load and save the found icon to a file
    if (!icon.path.empty())
    {
      HICON hIconLarge = NULL;
      HICON hIconSmall = NULL;

      UINT numIconInFile = ExtractIconEx( icon.path.c_str(), -1, NULL, NULL, 1 );

      UINT numIconLoaded = ExtractIconEx( icon.path.c_str(), icon.index, &hIconLarge, &hIconSmall, 1 );
      ASSERT_GE( numIconLoaded, 1) << "Failed to load at least one icon from file '" << icon.path << "' for file extension '" << file_extension << "'."; //at least 1 icon loaded
        
      //select best icon
      HICON hIconBest = NULL;
      if (hIconSmall)
        hIconBest = hIconSmall;
      if (hIconLarge)
        hIconBest = hIconLarge;
      ASSERT_TRUE( hIconBest != NULL ) << "Unable to load large/small icon from file '" << icon.path << "' for file extension '" << file_extension << "'.";

      //Convert the icon to a bitmap (with invisible background)
      SIZE icon_size = Win32Utils::GetIconSize(hIconLarge);
      HBITMAP hBitmap = Win32Utils::CopyAsBitmap(hIconLarge, icon_size.cx, icon_size.cy);

      DestroyIcon(hIconLarge);
      DestroyIcon(hIconSmall);

      //save to a file
      sprintf(filename, ".icon_%s.bmp", file_extension);
      file_path = workspace.GetFullPathUtf8(filename);
      created = Win32Utils::CreateBmpFile(file_path.c_str(), hBitmap);

      //delete the bitmap
      DeleteObject(hBitmap);

      ASSERT_TRUE( created ) << "Failed creating file '" << file_path << "'.";
    }

    //Cleanup
    ASSERT_TRUE(workspace.Cleanup()) << "Failed deleting workspace directory '" << workspace.GetBaseDirectory() << "'.";
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
