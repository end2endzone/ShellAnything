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

#include "TestWin32Utils.h"
#include "win32_utils.h"
#include "rapidassist/gtesthelp.h"

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

      UINT numIconLoaded = ExtractIconEx( "c:\\Windows\\system32\\SHELL32.dll", i, &hIconLarge, &hIconSmall, 1 );
      ASSERT_GE( numIconLoaded, 1); //at least 1 icon loaded
      ASSERT_TRUE( hIconLarge != NULL );

      //Convert the icon to a bitmap (with invisible background)
      SIZE icon_size = win32_utils::GetIconSize(hIconLarge);
      HBITMAP hBitmap = win32_utils::CopyAsBitmap(hIconLarge, icon_size.cx, icon_size.cy);

      //Remove the invisible background and replace by red color
      COLORREF background_color = RGB(255,0,255); //pink
      win32_utils::FillTransparentPixels(hBitmap, background_color);

      DestroyIcon(hIconLarge);
      DestroyIcon(hIconSmall);

      //save to a file
      std::string file_path = ra::gtesthelp::getTestQualifiedName();
      static const size_t BUFFER_SIZE = 1024;
      char post_filename[BUFFER_SIZE];
      sprintf(post_filename, ".shell32.dll.index%03d.bmp", i);
      file_path.append(post_filename);
      win32_utils::CreateBMPFile(file_path.c_str(), hBitmap);

      //delete the bitmap
      DeleteObject(hBitmap);
    }
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
