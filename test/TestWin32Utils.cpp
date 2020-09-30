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

#include "TestWin32Utils.h"
#include "Win32Utils.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/testing.h"

namespace shellanything { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestWin32Utils::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestWin32Utils::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWin32Utils, testIconToBitmapConversion)
  {
    for(int i=0; i<305; i++)
    {
      HICON hIconLarge = NULL;
      HICON hIconSmall = NULL;

      UINT numIconLoaded = ExtractIconEx( "c:\\windows\\system32\\shell32.dll", i, &hIconLarge, &hIconSmall, 1 );
      ASSERT_GE( numIconLoaded, 1); //at least 1 icon loaded
      ASSERT_TRUE( hIconLarge != NULL );

      //Convert the icon to a bitmap (with invisible background)
      SIZE icon_size = Win32Utils::GetIconSize(hIconLarge);
      HBITMAP hBitmap = Win32Utils::CopyAsBitmap(hIconLarge, icon_size.cx, icon_size.cy);

      DestroyIcon(hIconLarge);
      DestroyIcon(hIconSmall);

      //save to a file
      std::string file_path = ra::testing::GetTestQualifiedName();
      static const size_t BUFFER_SIZE = 1024;
      char post_filename[BUFFER_SIZE];
      sprintf(post_filename, ".shell32.dll.index%03d.bmp", i);
      file_path.append(post_filename);
      Win32Utils::CreateBmpFile(file_path.c_str(), hBitmap);

      //delete the bitmap
      DeleteObject(hBitmap);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWin32Utils, testSymbolicLinksIconTransparencyIssue12)
  {
    //https://github.com/end2endzone/ShellAnything/issues/12

    struct QUICK_ICON
    {
      const char * path;
      int index;
    };
    static const QUICK_ICON icons[] = {
      {"C:\\Windows\\System32\\imageres.dll", 154},
      {"C:\\Windows\\System32\\shell32.dll",   29},
      {"C:\\Windows\\System32\\imageres.dll", 160},
      {"C:\\Windows\\System32\\mmcndmgr.dll",  69},
    };
    static const size_t num_icons = sizeof(icons)/sizeof(icons[0]);

    for(int i=0; i<num_icons; i++)
    {
      const char * icon_path = icons[i].path;
      const int & index = icons[i].index;

      HICON hIconLarge = NULL;
      HICON hIconSmall = NULL;

      UINT numIconLoaded = ExtractIconEx( icon_path, index, &hIconLarge, &hIconSmall, 1 );
      ASSERT_GE( numIconLoaded, 1); //at least 1 icon loaded
      ASSERT_TRUE( hIconLarge != NULL );

      //Convert the icon to a bitmap (with invisible background)
      SIZE icon_size = Win32Utils::GetIconSize(hIconLarge);
      HBITMAP hBitmap = Win32Utils::CopyAsBitmap(hIconLarge, icon_size.cx, icon_size.cy);

      DestroyIcon(hIconLarge);
      DestroyIcon(hIconSmall);

      //save to a file
      std::string file_path = ra::testing::GetTestQualifiedName();
      std::string dll_filename = ra::filesystem::GetFilename(icon_path);
      static const size_t BUFFER_SIZE = 1024;
      char post_filename[BUFFER_SIZE];
      sprintf(post_filename, ".%s.index%03d.bmp", dll_filename.c_str(), index);
      file_path.append(post_filename);
      Win32Utils::CreateBmpFile(file_path.c_str(), hBitmap);

      //delete the bitmap
      DeleteObject(hBitmap);
    }
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
