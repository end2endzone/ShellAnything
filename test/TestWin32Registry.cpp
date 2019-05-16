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
#include "win32_utils.h"
#include "rapidassist/strings.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/gtesthelp.h"
#include "shellanything/Node.h"

#include <Windows.h>

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
  TEST_F(TestWin32Registry, testGetFileTypeIcon)
  {
    //cleanup
    {
      std::string prefix = ra::gtesthelp::getTestQualifiedName() + ".";
      ra::strings::StringVector files;
      bool found = ra::filesystem::findFiles(files, ".");
      if (found)
      {
        for(size_t i=0; i<files.size(); i++)
        {
          const std::string & path = files[i];
          std::string filename = ra::filesystem::getFilename(path.c_str());
          if (filename.find(prefix) != std::string::npos)
          {
            //This file was generated by a previous execution.
            //It must be deleted.
            ra::filesystem::deleteFile(path.c_str());
          }
        }
      }
    }

    static const char * file_extensions[] = {
      "application", 
      "bat", "bmp", 
      "c", "cab", "chm", "cmd", "cpp", "cs", "csproj", "css", "csv", 
      "def", "dll", "doc", "docx", "dsp", "dsw", "dtd", 
      "exe", 
      "gif", 
      "h", "htm", "html", 
      "inf", "ini", 
      "jar", "java", "jpg", "js", 
      "lib", "log", 
      "mdb", "msi", 
      "obj", "ocx", 
      "pdf", "png", "pptx", 
      "rc", "reg", "res", "resx", "rgs", 
      "sdf", "sln", "svg", "sys", 
      "tga", "tif", "tmp", "ttf", "txt", 
      "udf", "url", "user", 
      "vb", "vbs", "vcproj", "vcxproj", "vdproj", 
      "war", "wav", "website", 
      "xaml", "xls", "xlsm", "xlsx", "xml", "xsd", "xslt", 
      "zip", 
    };
    static const size_t num_file_extensions = sizeof(file_extensions)/sizeof(file_extensions[0]);

    //for each file extensions
    for(size_t i=0; i<num_file_extensions; i++)
    {
      const char * file_extension = file_extensions[i];

      //create a fake file with the extensions for visual reference
      std::string file_path = ra::gtesthelp::getTestQualifiedName();
      static const size_t BUFFER_SIZE = 1024;
      char post_filename[BUFFER_SIZE];
      sprintf(post_filename, ".%03d.sample_file.%s", i, file_extension);
      file_path.append(post_filename);
      bool created = ra::gtesthelp::createFile(file_path.c_str());
      ASSERT_TRUE( created ) << "Failed creating file '" << file_path << "'.";

      //get the icon matching this file's extension
      const win32_registry::REGISTRY_ICON icon = win32_registry::getFileTypeIcon(file_extension);
      if (icon.path.empty())
      {
        printf("Failed to find icon for file extension. Is this expected ? file_extension='%s'. \n", file_extension);
      }

      //load and save the found icon to a file
      if (!icon.path.empty())
      {
        HICON hIconLarge = NULL;
        HICON hIconSmall = NULL;

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
        SIZE icon_size = win32_utils::GetIconSize(hIconLarge);
        HBITMAP hBitmap = win32_utils::CopyAsBitmap(hIconLarge, icon_size.cx, icon_size.cy);

        //Remove the invisible background and replace by red color
        COLORREF background_color = RGB(255,0,255); //pink
        win32_utils::FillTransparentPixels(hBitmap, background_color);

        DestroyIcon(hIconLarge);
        DestroyIcon(hIconSmall);

        //save to a file
        file_path = ra::gtesthelp::getTestQualifiedName();
        sprintf(post_filename, ".%03d.icon_%s.bmp", i, file_extension);
        file_path.append(post_filename);
        win32_utils::CreateBMPFile(file_path.c_str(), hBitmap);

        //delete the bitmap
        DeleteObject(hBitmap);
      }
    }
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
