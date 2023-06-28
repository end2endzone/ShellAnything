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
#include "ArgumentsHandler.h"

#include "rapidassist/undef_windows_macros.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/testing.h"
#include "rapidassist/cli.h"

namespace shellanything
{
  namespace test
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
      for (int i = 0; i < 305; i++)
      {
        HICON hIconLarge = NULL;
        HICON hIconSmall = NULL;

        UINT numIconLoaded = ExtractIconEx("c:\\windows\\system32\\shell32.dll", i, &hIconLarge, &hIconSmall, 1);
        ASSERT_GE(numIconLoaded, 1); //at least 1 icon loaded
        ASSERT_TRUE(hIconLarge != NULL);

        //Convert the icon to a bitmap (with invisible background)
        SIZE icon_size = Win32Utils::GetIconSize(hIconLarge);
        HBITMAP hBitmap = Win32Utils::CopyAsBitmap(hIconLarge, icon_size.cx, icon_size.cy);

        DestroyIcon(hIconLarge);
        DestroyIcon(hIconSmall);

        //save to a file
        std::string test_name = ra::testing::GetTestQualifiedName();
        static const size_t BUFFER_SIZE = 1024;
        char filename[BUFFER_SIZE];
        sprintf(filename, "%s.shell32.dll.index%03d.v1.bmp", test_name.c_str(), i);
        ASSERT_TRUE(Win32Utils::SaveAs32BppBitmapV1File(filename, hBitmap));
        sprintf(filename, "%s.shell32.dll.index%03d.v3.bmp", test_name.c_str(), i);
        ASSERT_TRUE(Win32Utils::SaveAs32BppBitmapV3File(filename, hBitmap));

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
        const char* path;
        int index;
      };
      static const QUICK_ICON icons[] = {
        {"C:\\Windows\\System32\\imageres.dll", 154},
        {"C:\\Windows\\System32\\shell32.dll",   29},
        {"C:\\Windows\\System32\\imageres.dll", 160},
        {"C:\\Windows\\System32\\mmcndmgr.dll",  69},
      };
      static const size_t num_icons = sizeof(icons) / sizeof(icons[0]);

      for (int i = 0; i < num_icons; i++)
      {
        const char* icon_path = icons[i].path;
        const int& index = icons[i].index;

        HICON hIconLarge = NULL;
        HICON hIconSmall = NULL;

        UINT numIconLoaded = ExtractIconEx(icon_path, index, &hIconLarge, &hIconSmall, 1);
        ASSERT_GE(numIconLoaded, 1); //at least 1 icon loaded
        ASSERT_TRUE(hIconLarge != NULL);

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
        ASSERT_TRUE(Win32Utils::SaveAs32BppBitmapFile(file_path.c_str(), hBitmap));

        //delete the bitmap
        DeleteObject(hBitmap);
      }
    }
    //--------------------------------------------------------------------------------------------------
    int PrintMonitorInformation(int argc, char** argv)
    {
      // Set current process as monitor dpi aware...
      Win32Utils::EnableMonitorDpiAwareness();
      bool monitor_dpi_aware = Win32Utils::IsMonitorDpiAwarenessEnabled();
      printf("monitor_dpi_aware=%d\n", monitor_dpi_aware);
      if (!monitor_dpi_aware)
        return 1;

      // Print system metrics
      std::cout << "System metrics:\n";
      std::cout << "SM_CXSCREEN    : " << GetSystemMetrics(SM_CXSCREEN) << "\n";
      std::cout << "SM_CYSCREEN    : " << GetSystemMetrics(SM_CYSCREEN) << "\n";
      std::cout << "SM_CXSMICON    : " << GetSystemMetrics(SM_CXSMICON) << "\n";
      std::cout << "SM_CYSMICON    : " << GetSystemMetrics(SM_CYSMICON) << "\n";
      std::cout << "SM_CXICON      : " << GetSystemMetrics(SM_CXICON) << "\n";
      std::cout << "SM_CYICON      : " << GetSystemMetrics(SM_CYICON) << "\n";
      std::cout << "System DPI     : " << Win32Utils::GetSystemDPI() << "\n";
      std::cout << "System Scaling : " << Win32Utils::GetSystemScalingPercent() << "%" << "\n";

      // Print information about monitor and their scaling
      int count = Win32Utils::GetMonitorCount();
      printf("Found %d monitors.\n", count);
      for (int i = 0; i < count; i++)
      {
        int dpi = Win32Utils::GetMonitorDPI(i);
        int scaling = Win32Utils::GetMonitorScalingPercent(i);
        std::cout << "Monitor " << i << " is scaled to " << scaling << "% (dpi " << dpi << ")\n";
      }

      return 0; // success
    }
    COMMAND_LINE_ENTRY_POINT * PrintMonitorInformationEntryPoint = shellanything::RegisterCommandLineEntryPoint("PrintMonitorInformation", PrintMonitorInformation);
    TEST_F(TestWin32Utils, testMonitorInfo)
    {
      int exit_code = shellanything::InvokeCommandLineEntryPoint("PrintMonitorInformation", 0, NULL);
      ASSERT_EQ(0, exit_code);
    }
    //--------------------------------------------------------------------------------------------------
    std::string ToString(SIZE s)
    {
      return ra::strings::ToString(s.cx) + "x" + ra::strings::ToString(s.cy);
    }
    int ProcessIconAsBitmap(HICON hIcon, const std::string& outout_file_path, SIZE & icon_size)
    {
      if (hIcon == NULL)
        return 16;

      //Convert the icon to a bitmap (with invisible background)
      icon_size = Win32Utils::GetIconSize(hIcon);
      HBITMAP hBitmap = Win32Utils::CopyAsBitmap(hIcon, icon_size.cx, icon_size.cy);

      //save to a file
      bool created = Win32Utils::SaveAs32BppBitmapFile(outout_file_path.c_str(), hBitmap);
      if (!created)
      {
        printf("Failed to create bitmap: '%s'\n", outout_file_path.c_str());
        return 15;
      }

      //delete the bitmap
      DeleteObject(hBitmap);

      return 0;
    }
    int ExtractLargeAndSmallIcons(int argc, char** argv)
    {
      //MessageBox(NULL, "ATTACH NOW!", "ATTACH NOW!", MB_OK);

      // Set current process as monitor dpi aware...
      Win32Utils::EnableMonitorDpiAwareness();
      bool monitor_dpi_aware = Win32Utils::IsMonitorDpiAwarenessEnabled();
      if (!monitor_dpi_aware)
        return 5;

      const char* argument_name;

      argument_name = "icon_path";
      std::string icon_path;
      bool has_icon_path = ra::cli::ParseArgument(argument_name, icon_path, argc, argv);
      if (!has_icon_path)
      {
        printf("Missing argument: '%s'\n", argument_name);
        return 10;
      }

      argument_name = "output_large_path";
      std::string output_large_path;
      bool has_output_large_path = ra::cli::ParseArgument(argument_name, output_large_path, argc, argv);
      if (!has_output_large_path)
      {
        printf("Missing argument: '%s'\n", argument_name);
        return 11;
      }

      argument_name = "output_small_path";
      std::string output_small_path;
      bool has_output_small_path = ra::cli::ParseArgument(argument_name, output_small_path, argc, argv);
      if (!has_output_small_path)
      {
        printf("Missing argument: '%s'\n", argument_name);
        return 12;
      }

      if (!ra::filesystem::FileExists(icon_path.c_str()))
      {
        printf("File not found: '%s'\n", icon_path.c_str());
        return 13;
      }

      static const HICON INVALID_ICON_HANDLE_VALUE = NULL;

      HICON hIconLarge = INVALID_ICON_HANDLE_VALUE;
      HICON hIconSmall = INVALID_ICON_HANDLE_VALUE;

      static const int index = 0;
      UINT numIconLoaded = ExtractIconEx(icon_path.c_str(), index, &hIconLarge, &hIconSmall, 1);
      if (numIconLoaded != 1)
      {
        printf("No icons in file: '%s'\n", icon_path.c_str());
        return 14;
      }

      SIZE icon_size_large;
      SIZE icon_size_small;

      if (hIconLarge != INVALID_ICON_HANDLE_VALUE)
      {
        int exit_code = ProcessIconAsBitmap(hIconLarge, output_large_path, icon_size_large);
        DestroyIcon(hIconLarge);
        if (exit_code != 0)
          return 100 + exit_code;
      }
      if (hIconSmall != INVALID_ICON_HANDLE_VALUE)
      {
        int exit_code = ProcessIconAsBitmap(hIconSmall, output_small_path, icon_size_small);
        DestroyIcon(hIconSmall);
        if (exit_code != 0)
          return 1000 + exit_code;
      }

      ////Convert the icon to a bitmap (with invisible background)
      //SIZE icon_size_large = Win32Utils::GetIconSize(hIconLarge);
      //HBITMAP hBitmapLarge = Win32Utils::CopyAsBitmap(hIconLarge, icon_size_large.cx, icon_size_large.cy);
      //SIZE icon_size_small = Win32Utils::GetIconSize(hIconSmall);
      //HBITMAP hBitmapSmall = Win32Utils::CopyAsBitmap(hIconSmall, icon_size_small.cx, icon_size_small.cy);

      //DestroyIcon(hIconLarge);
      //DestroyIcon(hIconSmall);

      ////save to a file
      //bool large_created = Win32Utils::SaveAs32BppBitmapFile(output_large_path.c_str(), hBitmapLarge);
      //if (!large_created)
      //{
      //  printf("Failed to create bitmap: '%s'\n", output_large_path.c_str());
      //  return 15;
      //}

      //bool small_created = Win32Utils::SaveAs32BppBitmapFile(output_small_path.c_str(), hBitmapSmall);
      //if (!small_created)
      //{
      //  printf("Failed to create bitmap: '%s'\n", output_small_path.c_str());
      //  return 16;
      //}

      ////delete the bitmap
      //DeleteObject(hBitmapLarge);
      //DeleteObject(hBitmapSmall);

      // print stats about icons
      std::string icon_file_name = ra::filesystem::GetFilename(icon_path.c_str());
      std::cout << "icon_path=" << icon_file_name << "  -->  ";
      if (ra::filesystem::FileExists(output_large_path.c_str()))
        std::cout << " large:" << ToString(icon_size_large);
      if (ra::filesystem::FileExists(output_small_path.c_str()))
        std::cout << " small:" << ToString(icon_size_small);
      std::cout << "\n";

      return 0;
    }
    COMMAND_LINE_ENTRY_POINT* ExtractLargeAndSmallIconsEntryPoint = shellanything::RegisterCommandLineEntryPoint("ExtractLargeAndSmallIcons", ExtractLargeAndSmallIcons);
    TEST_F(TestWin32Utils, testExtractIconResolutionsWithScalingIssue117)
    {
      //https://github.com/end2endzone/ShellAnything/issues/117

      static const char * icons[] = {
        "test_files\\test-res-16-20-24-32-40-48-60-64-72-80-96-120-128-144-160-192-256.ico",
        "test_files\\test-res-16-32-48-64-96-128-256.ico",
        "test_files\\issue117\\alarm_clock-256-32bpp-8ba-compressed.ico",
        "test_files\\issue117\\alarm_clock-16-32bpp-8ba-uncompressed.ico",
        "test_files\\issue117\\alarm_clock-20-32bpp-8ba-uncompressed.ico",
        "test_files\\issue117\\alarm_clock-24-32bpp-8ba-uncompressed.ico",
        "test_files\\issue117\\alarm_clock-32-32bpp-8ba-uncompressed.ico",
        "test_files\\issue117\\alarm_clock-40-32bpp-8ba-uncompressed.ico",
        "test_files\\issue117\\alarm_clock-48-32bpp-8ba-uncompressed.ico",
        "test_files\\issue117\\alarm_clock-64-32bpp-8ba-uncompressed.ico",
        "test_files\\issue117\\alarm_clock-96-32bpp-8ba-uncompressed.ico",
        "test_files\\issue117\\alarm_clock-128-32bpp-8ba-uncompressed.ico",
        "test_files\\issue117\\alarm_clock-all-compressed.ico",
      };
      static const size_t num_icons = sizeof(icons) / sizeof(icons[0]);

      for (int i = 0; i < num_icons; i++)
      {
        const std::string icon_path = ra::filesystem::GetPathBasedOnCurrentProcess(icons[i]);
        const int index = 0;

        ASSERT_TRUE(ra::filesystem::FileExists(icon_path.c_str())) << "File does not exists: " << icon_path;

        // build output files
        std::string temp_dir = ra::filesystem::GetTemporaryDirectory() + "\\" + ra::testing::GetTestQualifiedName();
        std::string icon_filename = ra::filesystem::GetFilename(icon_path.c_str());
        ASSERT_TRUE(ra::filesystem::CreateDirectory(temp_dir.c_str()));

        std::string large_bmp_path = temp_dir + "\\" + icon_filename + ".large.bmp";
        std::string small_bmp_path = temp_dir + "\\" + icon_filename + ".small.bmp";

        // remove data from previous runs
        ra::filesystem::DeleteFile(large_bmp_path.c_str());
        ra::filesystem::DeleteFile(small_bmp_path.c_str());

        // invoke extraction function
        int argc = 3;
        static const std::string DOUBLE_QUOTES = "\"";
        std::string arg0 = std::string("--icon_path=") + DOUBLE_QUOTES + icon_path + DOUBLE_QUOTES;
        std::string arg1 = std::string("--output_large_path=") + DOUBLE_QUOTES + large_bmp_path + DOUBLE_QUOTES;
        std::string arg2 = std::string("--output_small_path=") + DOUBLE_QUOTES + small_bmp_path + DOUBLE_QUOTES;
        char* argv[3] = {
          (char*)arg0.c_str(),
          (char*)arg1.c_str(),
          (char*)arg2.c_str(),
        };
        int exit_code = shellanything::InvokeCommandLineEntryPoint("ExtractLargeAndSmallIcons", argc, argv);
        ASSERT_EQ(0, exit_code);
      }
    }
    //--------------------------------------------------------------------------------------------------

  } //namespace test
} //namespace shellanything
