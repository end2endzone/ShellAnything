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

// https://learn.microsoft.com/en-us/windows/win32/winprog/using-the-windows-headers?redirectedfrom=MSDN

// Windows 8.1:
#define NTDDI_VERSION 0x06030000  // Target Windows 8.1
#define _WIN32_WINNT 0x0603       // Target Windows 8.1
#define WINVER 0x0603             // Target Windows 8.1

// Windows 10
//#define NTDDI_VERSION 0x0A000000  // Target Windows 10 (first version, 1507 "Threshold")
//#define _WIN32_WINNT 0x0A00       // Target Windows 10
//#define WINVER 0x0A00             // Target Windows 10

#include "shellscalingapi.h"
#pragma comment(lib, "Shcore.lib")  // for GetProcessDpiAwareness()


#include "Win32Utils.h"

#include "rapidassist/undef_windows_macros.h"
#include "rapidassist/unicode.h"

#include <string>
#include <assert.h>

#include "shellscalingapi.h"
#pragma comment(lib, "Shcore.lib")  // for GetProcessDpiAwareness()

#pragma pack(push, 1)
// https://en.wikipedia.org/wiki/BMP_file_format
typedef struct tagBITMAPV2INFOHEADER
{
  // V1
  DWORD      biSize;
  LONG       biWidth;
  LONG       biHeight;
  WORD       biPlanes;
  WORD       biBitCount;
  DWORD      biCompression;
  DWORD      biSizeImage;
  LONG       biXPelsPerMeter;
  LONG       biYPelsPerMeter;
  DWORD      biClrUsed;
  DWORD      biClrImportant;

  // V2
  DWORD biRedMask;
  DWORD biGreenMask;
  DWORD biBlueMask;
} BITMAPV2INFOHEADER, FAR* LPBITMAPV2INFOHEADER, * PBITMAPV2INFOHEADER;

typedef struct tagBITMAPV3INFOHEADER
{
  // V1
  DWORD      biSize;
  LONG       biWidth;
  LONG       biHeight;
  WORD       biPlanes;
  WORD       biBitCount;
  DWORD      biCompression;
  DWORD      biSizeImage;
  LONG       biXPelsPerMeter;
  LONG       biYPelsPerMeter;
  DWORD      biClrUsed;
  DWORD      biClrImportant;

  // V2
  DWORD biRedMask;
  DWORD biGreenMask;
  DWORD biBlueMask;

  // V3
  DWORD biAlphaMask;
} BITMAPV3INFOHEADER, FAR* LPBITMAPV3INFOHEADER, * PBITMAPV3INFOHEADER;
#pragma pack(pop)

namespace Win32Utils
{
  static const size_t BITS_PER_PIXEL = 32;
  static const size_t BYTES_PER_PIXEL = BITS_PER_PIXEL / 8;
  static const int DEFAULT_SYSTEM_DPI = 96;

  PROCESS_DPI_AWARENESS GetCurrentProcessDpiAwareness();

  void GetWindowsVersion(int& major, int& minor)
  {
    OSVERSIONINFOEX info;
    ZeroMemory(&info, sizeof(OSVERSIONINFOEX));
    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
# pragma warning(push)
#pragma warning(disable: 4996)
    GetVersionEx((LPOSVERSIONINFO)&info);//info requires typecasting
# pragma warning(pop)
  //VerifyVersionInfo(&info, -1, -1);

    major = info.dwMajorVersion;
    minor = info.dwMinorVersion;
  }

  std::string GetRegistryKeyValue(HKEY hKey, const std::string& subKey, const std::string& value)
  {
    static const std::string EMPTY;

    // Read value size
    DWORD dataSize = 0;
    LONG retCode = ::RegGetValueA(
      hKey,
      subKey.c_str(),
      value.c_str(),
      RRF_RT_REG_SZ,
      nullptr,
      nullptr,
      &dataSize
    );
    if (retCode != ERROR_SUCCESS)
      return EMPTY;

    // Allocate memory
    std::string data;
    data.resize(dataSize / sizeof(data[0]));

    // Read actual value
    retCode = ::RegGetValueA(
      hKey,
      subKey.c_str(),
      value.c_str(),
      RRF_RT_REG_SZ,
      nullptr,
      &data[0],
      &dataSize
    );
    if (retCode != ERROR_SUCCESS)
      return EMPTY;

    DWORD stringLength = dataSize / sizeof(data[0]);
    stringLength--; // Exclude the NUL written by the Win32 API
    data.resize(stringLength);

    return data;
  }

  std::string GetWindowsProductName()
  {
    std::string product_name = GetRegistryKeyValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductName");
    return product_name;
  }

  bool EnableMonitorDpiAwareness()
  {
    if (SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE) != S_OK)
      return false;
    return true;
  }

  bool IsMonitorDpiAwarenessEnabled()
  {
    PROCESS_DPI_AWARENESS pda = GetCurrentProcessDpiAwareness();
    if (pda == PROCESS_PER_MONITOR_DPI_AWARE)
      return true;
    return false;
  }

  int GetSystemDPI()
  {
    const HDC hDC = ::GetDC(HWND_DESKTOP);
    int dpi = ::GetDeviceCaps(hDC, LOGPIXELSX);
    ::ReleaseDC(HWND_DESKTOP, hDC);
    return dpi;
  }

  float GetSystemScaling()
  {
    float scaling = static_cast<float>(GetSystemDPI()) / static_cast<float>(DEFAULT_SYSTEM_DPI);
    return scaling;
  }

  int GetSystemScalingPercent()
  {
    int scaling = static_cast<int>(GetSystemDPI() * 100) / DEFAULT_SYSTEM_DPI;
    return scaling;
  }

  int GetSystemDefaultDPI()
  {
    return DEFAULT_SYSTEM_DPI;
  }

  int GetMonitorDPI(HMONITOR hMonitor)
  {
    UINT dpi_x = 0;
    UINT dpi_y = 0;
    if (GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpi_x, &dpi_y) == S_OK)
    {
      return dpi_x;
    }
    return 0;
  }

  float GetMonitorScaling(HMONITOR hMonitor)
  {
    float scaling = static_cast<float>(GetMonitorDPI(hMonitor)) / static_cast<float>(DEFAULT_SYSTEM_DPI);
    return scaling;
  }

  int GetMonitorScalingPercent(HMONITOR hMonitor)
  {
    int scaling = static_cast<int>(GetMonitorDPI(hMonitor) * 100) / DEFAULT_SYSTEM_DPI;
    return scaling;
  }

  struct MONITOR_INDEX_FROM_HANDLE_INFO
  {
    bool found;
    int index;
    HMONITOR hMonitor;
  };

  BOOL CALLBACK GetMonitorIndexFromHandleProc(HMONITOR hMonitor, HDC /*hdcMonitor*/, LPRECT /*lprcMonitor*/, LPARAM dwData)
  {
    MONITOR_INDEX_FROM_HANDLE_INFO* info = (MONITOR_INDEX_FROM_HANDLE_INFO*)dwData;
    if (info->hMonitor == hMonitor)
    {
      info->found = true;
      return FALSE;
    }
    info->index++;
    return TRUE;
  }

  int GetMonitorIndexFromHandle(HMONITOR hMonitor)
  {
    MONITOR_INDEX_FROM_HANDLE_INFO info = { 0 };
    info.hMonitor = hMonitor;
    // https://github.com/MicrosoftDocs/feedback/issues/1011
    EnumDisplayMonitors(NULL, NULL, GetMonitorIndexFromHandleProc, (LPARAM)&info); 
    if (!info.found)
      return -1;
    return info.index;
  }

  struct MONITOR_HANDLE_FROM_INDEX_INFO
  {
    bool found;
    int query_index;
    int current_index;
    HMONITOR hMonitor;
  };

  BOOL CALLBACK GetMonitorHandleFromIndexProc(HMONITOR hMonitor, HDC /*hdcMonitor*/, LPRECT /*lprcMonitor*/, LPARAM dwData)
  {
    MONITOR_HANDLE_FROM_INDEX_INFO* info = (MONITOR_HANDLE_FROM_INDEX_INFO*)dwData;
    if (info->query_index == info->current_index)
    {
      info->hMonitor = hMonitor;
      info->found = true;
      return FALSE;
    }
    info->current_index++;
    return TRUE;
  }

  HMONITOR GetMonitorHandleFromIndex(int index)
  {
    MONITOR_HANDLE_FROM_INDEX_INFO info = { 0 };
    info.query_index = index;

    // https://github.com/MicrosoftDocs/feedback/issues/1011
    EnumDisplayMonitors(NULL, NULL, GetMonitorHandleFromIndexProc, (LPARAM)&info);
    if (!info.found)
      return NULL;
    return info.hMonitor;
  }

  struct MONITOR_COUNT_INFO
  {
    int count;
  };

  BOOL CALLBACK GetMonitorCountProc(HMONITOR /*hMonitor*/, HDC /*hdcMonitor*/, LPRECT /*lprcMonitor*/, LPARAM dwData)
  {
    MONITOR_COUNT_INFO* info = (MONITOR_COUNT_INFO*)dwData;
    info->count++;
    return TRUE;
  }

  int GetMonitorCount()
  {
    MONITOR_COUNT_INFO info = { 0 };
    EnumDisplayMonitors(NULL, NULL, GetMonitorCountProc, (LPARAM)&info);
    return info.count;
  }

  bool GetMousePositionInVirtualScreenCoordinates(int* x, int* y)
  {
    if (x == NULL || y == NULL)
      return false;
    POINT p;
    if (!GetCursorPos(&p))
      return false;
    *x = p.x;
    *y = p.y;
    return true;
  }

  bool GetMousePositionInMonitorCoordinates(int* monitor_index, int* x, int* y)
  {
    if (monitor_index == NULL || x == NULL || y == NULL)
      return false;

    POINT p;
    if (!GetCursorPos(&p))
      return false;

    HMONITOR hMonitor = MonitorFromPoint(p, MONITOR_DEFAULTTONULL);
    if (hMonitor == NULL)
      return 0;

    MONITORINFO info = { 0 };
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoA(hMonitor, &info))
      return false;

    // convert from virtual screen coordinates to monitor coordinates
    RECT work_area = info.rcWork;
    p.x -= work_area.left;
    p.y -= work_area.top;

    *monitor_index = GetMonitorIndexFromHandle(hMonitor);
    *x = p.x;
    *y = p.y;

    return true;
  }

  int GetMonitorDPI(int monitor_index)
  {
    HMONITOR hMonitor = GetMonitorHandleFromIndex(monitor_index);
    if (hMonitor == 0)
      return 0;
    int dpi = GetMonitorDPI(hMonitor);
    return dpi;
  }

  float GetMonitorScaling(int monitor_index)
  {
    HMONITOR hMonitor = GetMonitorHandleFromIndex(monitor_index);
    if (hMonitor == 0)
      return 0.0f;
    float scaling = static_cast<float>(GetMonitorDPI(hMonitor)) / static_cast<float>(DEFAULT_SYSTEM_DPI);
    return scaling;
  }

  int GetMonitorScalingPercent(int monitor_index)
  {
    HMONITOR hMonitor = GetMonitorHandleFromIndex(monitor_index);
    if (hMonitor == 0)
      return 0;
    int scaling = static_cast<int>(GetMonitorDPI(hMonitor) * 100) / DEFAULT_SYSTEM_DPI;
    return scaling;
  }

  PROCESS_DPI_AWARENESS GetCurrentProcessDpiAwareness()
  {
    PROCESS_DPI_AWARENESS current_process_awareness;
    HANDLE hProcess = GetCurrentProcess();
    GetProcessDpiAwareness(hProcess, &current_process_awareness);
    CloseHandle(hProcess);
    return current_process_awareness;
  }

  SIZE GetIconSize(HICON hIcon)
  {
    SIZE size = { 0 };

    ICONINFO IconInfo = { 0 };
    if (!GetIconInfo(hIcon, &IconInfo))
      return size; //failed getting the icon's size

    BITMAP bmpMask = { 0 };
    GetObject(IconInfo.hbmMask, sizeof(BITMAP), &bmpMask);

    BITMAP bmpColor = { 0 };
    GetObject(IconInfo.hbmColor, sizeof(BITMAP), &bmpColor);

    size.cx = bmpMask.bmWidth;
    size.cy = bmpMask.bmHeight;

    //cleanup
    DeleteObject(IconInfo.hbmColor);
    DeleteObject(IconInfo.hbmMask);

    return size;
  }

  HICON GetBestIconForMenu(HICON hIconLarge, HICON hIconSmall)
  {
    if (hIconSmall != NULL && hIconLarge == NULL)
      return hIconSmall;
    else if (hIconSmall == NULL && hIconLarge != NULL)
      return hIconLarge;
    else if (hIconSmall == NULL && hIconLarge == NULL)
      return NULL;

    SIZE large_size = GetIconSize(hIconLarge);
    SIZE small_size = GetIconSize(hIconSmall);

    SIZE menu_size;
    menu_size.cx = GetSystemMetrics(SM_CXSMICON);
    menu_size.cy = GetSystemMetrics(SM_CYSMICON);

    //compute total number of pixels for each icons
    const long menu_pixels = menu_size.cx * menu_size.cy;
    const long large_pixels = large_size.cx * large_size.cy;
    const long small_pixels = small_size.cx * small_size.cy;

    //compute distance to menu_pixels
    const long large_diff = abs(large_pixels - menu_pixels);
    const long small_diff = abs(small_pixels - menu_pixels);

    if (small_diff <= large_diff)
      return hIconSmall;
    return hIconLarge;
  }

  RGBQUAD ToRgbQuad(const DWORD& color)
  {
    RGBQUAD output = { 0 };
    output.rgbRed = (BYTE)((color & 0x000000FF));
    output.rgbGreen = (BYTE)((color & 0x0000FF00) >> 8);
    output.rgbBlue = (BYTE)((color & 0x00FF0000) >> 16);
    output.rgbReserved = 255;
    return output;
  }

  template <typename T> inline static T InterpolateColor(const T& a, const T& b, const double factor /* from 0.0 to 1.0 */)
  {
    if (factor <= 0.0)
      return a;
    else if (factor >= 1.0)
      return b;
    else
      return (a + (T)((b - a) * factor));
  }

  SIZE GetBitmapSize(HBITMAP hBitmap)
  {
    SIZE size = { 0 };

    BITMAP b = { 0 };
    GetObject(hBitmap, sizeof(BITMAP), &b);

    size.cx = b.bmWidth;
    size.cy = b.bmHeight;

    return size;
  }

  int GetBitPerPixel(HBITMAP hBitmap)
  {
    BITMAP bmp;

    // Assume the hBitmap was probably not created by CreateDIBSection() but by any other means,
    // so query for info using BITMAP instead of DIBSECTION.
#ifndef NDEBUG
    int object_size =
#endif
    GetObject(hBitmap, sizeof(bmp), &bmp);
#ifndef NDEBUG
    assert((size_t)object_size == sizeof(bmp));
#endif

    return GetBitPerPixel(&bmp);
  }

  int GetBitPerPixel(BITMAP* bmp)
  {
    //HDC hDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
    //wBmpBitCount = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
    //DeleteDC(hDC);
    const WORD wTotalBitCount = (WORD)(bmp->bmPlanes * bmp->bmBitsPixel);
    WORD wBitPerPixels = 0;
    if (wTotalBitCount == 1)
      wBitPerPixels = 1;
    else if (wTotalBitCount <= 4)
      wBitPerPixels = 4;
    else if (wTotalBitCount <= 8)
      wBitPerPixels = 8;
    else if (wTotalBitCount <= 16)
      wBitPerPixels = 16;
    else if (wTotalBitCount <= 24)
      wBitPerPixels = 24;
    else
      wBitPerPixels = 32;

    return wBitPerPixels;
  }

  BOOL FillTransparentPixels(HBITMAP hBitmap, COLORREF background_color)
  {
    //FillTransparentPixels() blends every pixels of the given bitmap with the given background_color.
    //The result is a fully opaque bitmap which makes it usable in most win32 API.
    //It also allows the bitmap to be properly saved to *.bmp file format.
    //Note: The alpha channel is still available but all values are 0xFF (fully opaque).

    const RGBQUAD BACKGROUND_COLOR = ToRgbQuad(background_color);

    SIZE bitmap_size = GetBitmapSize(hBitmap);
    const LONG num_pixels = bitmap_size.cx * bitmap_size.cy;
    const LONG image_size = num_pixels * BYTES_PER_PIXEL;

    //Create a buffer to hold the pixels for updating the bitmap's content
    std::string pixel_buffer;
    pixel_buffer.assign((size_t)image_size, 0);
    if (pixel_buffer.size() != image_size)
      return FALSE;

    //Make a temporary copy of the bitmap pixel buffer
    LONG size_read = GetBitmapBits(hBitmap, (LONG)pixel_buffer.size(), (void*)pixel_buffer.data());
    if (size_read != image_size)
      return FALSE;

    //For each pixels with transparency...
    static const BYTE full_opacity = 0xFF;
    for (LONG i = 0; i < num_pixels; i++)
    {
      RGBQUAD* first_pixel = (RGBQUAD*)pixel_buffer.data();
      RGBQUAD& pixel = first_pixel[i];
      if (pixel.rgbReserved != full_opacity)
      {
        //blend pixel with the given background color
        double alpha_factor = (double)pixel.rgbReserved / 255.0;
        pixel.rgbRed = InterpolateColor<BYTE>(BACKGROUND_COLOR.rgbRed, pixel.rgbRed, alpha_factor);
        pixel.rgbGreen = InterpolateColor<BYTE>(BACKGROUND_COLOR.rgbGreen, pixel.rgbGreen, alpha_factor);
        pixel.rgbBlue = InterpolateColor<BYTE>(BACKGROUND_COLOR.rgbBlue, pixel.rgbBlue, alpha_factor);
        pixel.rgbReserved = full_opacity;
      }
    }

    //Assign our temporary pixel buffer as the new bitmap pixel buffer
    LONG size_write = SetBitmapBits(hBitmap, (DWORD)pixel_buffer.size(), (void*)pixel_buffer.data());
    if (size_write != image_size)
      return FALSE;

    return TRUE;
  }

#if 0
  void DumpString(const std::string file_path, const std::string& buffer)
  {
    FILE* f = fopen(file_path.c_str(), "wb");
    if (!f)
      return;

    fwrite(buffer.data(), 1, buffer.size(), f);
    fclose(f);
  }
  void DumpBuffer(const std::string file_path, const void* buffer, const size_t & buffer_size)
  {
    FILE* f = fopen(file_path.c_str(), "wb");
    if (!f)
      return;

    fwrite(buffer, 1, buffer_size, f);
    fclose(f);
  }

#endif

  HBITMAP CreateBitmapWithAlphaChannel(int width, int height, HDC hDc)
  {
    BITMAPINFO bmi;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = width * height * 4;
    VOID* pvBits;
    return CreateDIBSection(hDc, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
  }

  HBITMAP CopyAsBitmap(HICON hIcon, const int bitmap_width, const int bitmap_height)
  {
    //According to https://devblogs.microsoft.com/oldnewthing/20101021-00/?p=12483, using DrawIconEx()
    //with diFlags=DI_NORMAL is supposed to blend the icon color and transparency with the content of the destination.
    //This function uses a fully transparent bitmap as destination image.
    //The result is a HBITMAP with an 8-bit alpha channel that matches the icon's 8-bit alpha channel.
    //Note: Using the bitmap in other API which does not support transparency may result in alpha related artifacts.
    //Note: Saving the bitmap to a *.bmp file does not support transparency, may also result in alpha related artifacts. Usually, the transparent pixels are displayed as WHITE opaque pixels.
    //See FillTransparentPixels() for converting the bitmap to a fully opaque image.
    //

    const size_t num_pixels = bitmap_width * bitmap_height;
    const size_t image_size = num_pixels * BYTES_PER_PIXEL;

    //Create a buffer to reset everything
    std::string blank_pixels;
    blank_pixels.assign((size_t)image_size, 0);
    if (blank_pixels.size() != image_size)
      return NULL;

    //Create a buffer to hold the color pixels for debugging the bitmap's content
    std::string color_pixels;
    color_pixels.assign((size_t)image_size, 0);
    if (color_pixels.size() != image_size)
      return NULL;

    //Create a buffer to hold the mask pixels for debugging the bitmap's content
    std::string mask_pixels;
    mask_pixels.assign((size_t)image_size, 0);
    if (mask_pixels.size() != image_size)
      return NULL;

    HWND hWndDesktop = GetDesktopWindow();
    HDC hDcDesktop = GetDC(hWndDesktop);
    HDC hDcMem = CreateCompatibleDC(hDcDesktop);

    // Create a 32bbp bitmap and select it.
    HBITMAP hBitmap = CreateBitmapWithAlphaChannel(bitmap_width, bitmap_height, hDcMem);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDcMem, hBitmap);

#if 0
    // Make sure the destination bitmap is fully transparent by default
    LONG numPixelsRead2 = GetBitmapBits(hBitmap, (LONG)color_pixels.size(), (void*)color_pixels.data());
    assert(numPixelsRead2 == image_size);
    DumpString("c:\\temp\\destination_bitmap.data", color_pixels);
#endif

    //Extract the mask first in case something goes wrong with DI_NORMAL
    DrawIconEx(hDcMem, 0, 0, hIcon, bitmap_width, bitmap_height, 0, NULL, DI_MASK);
    LONG numPixelsRead = GetBitmapBits(hBitmap, (LONG)mask_pixels.size(), (void*)mask_pixels.data());

    //Reset the hDcMem to no-color, no-alpha
    LONG numPixelsWrite = SetBitmapBits(hBitmap, (LONG)blank_pixels.size(), (void*)blank_pixels.data());

    //Draw the icon, blending with the fully transparent destination image.
    //The result is a HBITMAP with an 8-bit alpha channel that matches the icon's 8-bit alpha channel.
    DrawIconEx(hDcMem, 0, 0, hIcon, bitmap_width, bitmap_height, 0, NULL, DI_NORMAL);

    //Verify if the DrawIconEx() failed to copy alpha channel properly
    numPixelsRead = GetBitmapBits(hBitmap, (LONG)color_pixels.size(), (void*)color_pixels.data());
    bool isFullyTransparent = (IsFullyTransparent(color_pixels) == TRUE);
    if (isFullyTransparent)
    {
      //yeah, something went wrong with DI_NORMAL.
      //Try to fix the alpha channel as best as we can.
      static const RGBQUAD WHITE_PIXEL = { 255,255,255,255 };
      RGBQUAD* colorPixel = (RGBQUAD*)color_pixels.data();
      RGBQUAD* maskPixel = (RGBQUAD*)mask_pixels.data();
      for (size_t i = 0; i < num_pixels; i++)
      {
        colorPixel->rgbReserved = 255; //make pixel fully opaque by default

        //each white pixels in the mask bits needs to be be fully transparent
        if (maskPixel->rgbRed == WHITE_PIXEL.rgbRed &&
            maskPixel->rgbGreen == WHITE_PIXEL.rgbGreen &&
            maskPixel->rgbBlue == WHITE_PIXEL.rgbBlue)
        {
          colorPixel->rgbReserved = 0; //in case HMENU starts supporting transparency
        }
        colorPixel++;
        maskPixel++;
      }
      SetBitmapBits(hBitmap, (DWORD)color_pixels.size(), (void*)color_pixels.data());
    }

#if 0
    //Output the bitmap pixels to a file in the "DATA image format" for debugging.
    numPixelsRead = GetBitmapBits(hBitmap, (LONG)color_pixels.size(), (void*)color_pixels.data());
    assert(numPixelsRead == image_size);

    //for GIMP, if all pixels are invisible, the color information is lost
    //if fully transparent, make it opaque
    isFullyTransparent = IsFullyTransparent(hBitmap);
    if (isFullyTransparent)
    {
      for (size_t i = 0; i < num_pixels && isFullyTransparent == true; i++)
      {
        RGBQUAD* first_pixels = (RGBQUAD*)color_pixels.data();
        RGBQUAD& pixel = first_pixels[i];
        pixel.rgbReserved = 0xFF;
      }
    }

    DumpString("c:\\temp\\blended_bitmap.data", color_pixels);
#endif

    // Clean up.
    if (hOldBitmap)
      SelectObject(hDcMem, hOldBitmap);
    if (hDcMem)
      DeleteDC(hDcMem);
    if (hDcDesktop)
      ReleaseDC(hWndDesktop, hDcDesktop);

    return hBitmap;
  }

  HBITMAP CopyAsBitmap(HICON hIcon)
  {
    //Get properties related to Windows Menu
    SIZE menu_icon_size = GetIconSize(hIcon);
    const int menu_icon_width = menu_icon_size.cx;
    const int menu_icon_height = menu_icon_size.cy;

    return CopyAsBitmap(hIcon, menu_icon_width, menu_icon_height);
  }

  PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
  {
    BITMAP bmp = { 0 };
    PBITMAPINFO pbmi;
    WORD    cClrBits;

    // Retrieve the bitmap color format, width, and height.  
    GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp);

    // Convert the color format to a count of bits.
    cClrBits = (WORD)GetBitPerPixel(&bmp);

    // Allocate memory for the BITMAPINFO structure. (This structure  
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
    // data structures.)  

#pragma warning( push )
#pragma warning( disable: 4334 ) //warning C4334: '<<' : result of 32-bit shift implicitly converted to 64 bits (was 64-bit shift intended?)
    if (cClrBits < 24)
      pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
                                     sizeof(BITMAPINFOHEADER) +
                                     sizeof(RGBQUAD) * (1 << cClrBits));

    // There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

    else
      pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
                                     sizeof(BITMAPINFOHEADER));
#pragma warning( pop )

    // Initialize the fields in the BITMAPINFO structure.  

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = bmp.bmWidth;
    pbmi->bmiHeader.biHeight = bmp.bmHeight;
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
    if (cClrBits < 24)
      pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

    // If the bitmap is not compressed, set the BI_RGB flag.  
    pbmi->bmiHeader.biCompression = BI_RGB;

    // Compute the number of bytes in the array of color  
    // indices and store the result in biSizeImage.  
    // The width must be DWORD aligned unless the bitmap is RLE 
    // compressed. 
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
      * pbmi->bmiHeader.biHeight;
    // Set biClrImportant to 0, indicating that all of the  
    // device colors are important.  
    pbmi->bmiHeader.biClrImportant = 0;
    return pbmi;
  }

  bool SaveAs32BppBitmapFile(const char* path, HBITMAP hBitmap)
  {
    return SaveAs32BppBitmapV3File(path, hBitmap);
  }

  //https://stackoverflow.com/questions/24720451/save-hbitmap-to-bmp-file-using-only-win32
  bool SaveAs32BppBitmapV1File(const char* pszFile, HBITMAP hBitmap)
  {
    HANDLE hFile = NULL;            // file handle  
    BITMAPFILEHEADER bfi;           // bitmap file-header  
    PBITMAPINFOHEADER pbih = NULL;  // bitmap info-header  
    LPBYTE lpBits = NULL;           // memory pointer  
    DWORD dwBytesWritten = 0;       // last amount of bytes written to file
    DWORD dwTotal = 0;              // total count of bytes  
    PBITMAPINFO pbi = NULL;
    bool bReturn = false;           // function return value

    HWND hWndDesktop = GetDesktopWindow();
    HDC hDcDesktop = GetDC(hWndDesktop);
    HDC hDcMem = CreateCompatibleDC(hDcDesktop);

    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDcMem, hBitmap);

    BITMAP bitmap = { 0 };
    GetObject(hBitmap, sizeof(BITMAP), &bitmap);

    pbi = CreateBitmapInfoStruct(hBitmap);

    pbih = (PBITMAPINFOHEADER)pbi;
    lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
    if (!lpBits)
    {
      bReturn = false;
      goto cleanup;
    }

    // Retrieve the color table (RGBQUAD array) and the bits  
    // (array of palette indices) from the DIB.  
    GetDIBits(hDcMem, hBitmap, 0, (WORD)pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS);

#if 0
    std::string pixels;
    size_t pixel_buffer_length = (size_t)pbih->biSizeImage;
    pixels.assign(pixel_buffer_length, 0);
    if (pixels.size() != pixel_buffer_length)
      return false;
    GetDIBits(hDcMem, hBitmap, 0, (WORD)pbih->biHeight, (void*)pixels.data(), pbi, DIB_RGB_COLORS);
    DumpString("c:\\temp\\GetDIBits.data", pixels);
    GetBitmapBits(hBitmap, (LONG)pixels.size(), (void*)pixels.data());
    DumpString("c:\\temp\\GetBitmapBits.data", pixels);
#endif

    // Create the .BMP file.  
    hFile = ::CreateFileA(pszFile,
                       GENERIC_READ | GENERIC_WRITE,
                       (DWORD)0,
                       NULL,
                       CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL,
                       (HANDLE)NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
      bReturn = false;
      goto cleanup;
    }

    bfi.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
    // Compute the size of the entire file.  
    bfi.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage);
    bfi.bfReserved1 = 0;
    bfi.bfReserved2 = 0;

    // Compute the offset to the array of color indices.  
    bfi.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD);

    // Copy the BITMAPFILEHEADER into the .BMP file.
    dwBytesWritten = 0;
    WriteFile(hFile, (LPVOID)&bfi, sizeof(BITMAPFILEHEADER), (LPDWORD)&dwBytesWritten, NULL);
    dwTotal += dwBytesWritten;

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
    dwBytesWritten = 0;
    WriteFile(hFile, (LPVOID)pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof(RGBQUAD), (LPDWORD)&dwBytesWritten, (NULL));
    dwTotal += dwBytesWritten;

    // Copy the array of color indices into the .BMP file.  
    dwBytesWritten = 0;
    WriteFile(hFile, lpBits, (DWORD)pbih->biSizeImage, (LPDWORD)&dwBytesWritten, NULL);
    dwTotal += dwBytesWritten;

#ifndef NDEBUG // if debuging
    assert(dwTotal == bfi.bfSize);
#endif

    // success
    bReturn = true;

    // Using goto for win32 api is most elegant way of cleaning up:
    // https://github.com/search?q=repo%3Amicrosoft%2FWindows-classic-samples+%22goto+%22&type=code
  cleanup:
    // Close the .BMP file.
    if (hFile)
      CloseHandle(hFile);

    // Free memory.
    if (lpBits)
      GlobalFree((HGLOBAL)lpBits);
    if (pbi)
      LocalFree(pbi);

    // Clean up.
    if (hOldBitmap)
      SelectObject(hDcMem, hOldBitmap);
    if (hDcMem)
      DeleteDC(hDcMem);
    if (hDcDesktop)
      ReleaseDC(hWndDesktop, hDcDesktop);

    return bReturn;
  }

  // https://en.wikipedia.org/wiki/BMP_file_format
  // https://upload.wikimedia.org/wikipedia/commons/7/75/BMPfileFormat.svg
  // https://stackoverflow.com/questions/24720451/save-hbitmap-to-bmp-file-using-only-win32
  bool SaveAs32BppBitmapV3File(const char* path, HBITMAP hBitmap)
  {
    //#define FORCE_DEFAULT_PALETTE
    WORD wBitCount;
    DWORD dwPaletteSize = 0, dwPixelArraySize = 0, dwFileSize = 0, dwBytesWritten = 0, dwDIBSize = 0;
    BITMAP Bitmap0;
    BITMAPFILEHEADER bmfHdr;
    BITMAPV3INFOHEADER bi;
    LPBITMAPV3INFOHEADER lpbi;
    HANDLE hFile = NULL, hDib = NULL, hOldBitmap = NULL;
    bool bReturn = false;
#ifdef FORCE_DEFAULT_PALETTE
    HANDLE hPal = NULL;
    HANDLE hOldPal2 = NULL;
#endif

#ifndef NDEBUG // if debuging
    {
      // Assume hBitmap was created by calling CreateDIBSection().
      DIBSECTION dib = { 0 };
#ifndef NDEBUG
      int object_size =
#endif
        GetObject(hBitmap, sizeof(dib), &dib);
#ifndef NDEBUG
      assert((size_t)object_size == sizeof(dib));
#endif

    }
#endif

    // But also assume the hBitmap was created by any other means.
#ifndef NDEBUG
    int object_size =
#endif
      GetObject(hBitmap, sizeof(Bitmap0), &Bitmap0);
#ifndef NDEBUG
    assert((size_t)object_size == sizeof(Bitmap0));
#endif

    HWND hWndDesktop = GetDesktopWindow();
    HDC hDcDesktop = GetDC(hWndDesktop);
    HDC hDcMem = CreateCompatibleDC(hDcDesktop);

#ifndef NDEBUG // if debuging
    {
      BITMAPINFOHEADER biInternal = { 0 };
      biInternal.biSize = sizeof(biInternal);
      biInternal.biBitCount = 0; // tell GetDIBits to fill in a BITMAPINFOHEADER structure or BITMAPCOREHEADER without the color table.
      // MSDN: The bitmap identified by the hbmp parameter must not be selected into a device context when the application calls this function.
      int result = GetDIBits(hDcMem, hBitmap, 0, (UINT)Bitmap0.bmHeight, NULL, (BITMAPINFO*)&biInternal, DIB_RGB_COLORS);
      assert(result > 0);
    }
#endif

    hOldBitmap = (HBITMAP)SelectObject(hDcMem, hBitmap);

    wBitCount = (WORD)GetBitPerPixel(&Bitmap0);

    // Init our local BITMAP INFO structure. This structure is used in the output file.
    // It is also used as the "requested bitmap format" when calling GetDIBits().
    bi.biSize = sizeof(BITMAPV3INFOHEADER);
    bi.biWidth = Bitmap0.bmWidth;
    bi.biHeight = Bitmap0.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = wBitCount;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 2835;  // 72 DPI
    bi.biYPelsPerMeter = 2835;  // 72 DPI
    bi.biClrImportant = 0;
    bi.biClrUsed = 0; // tell GetDIBits() to not use a color table (a.k.a palette)

    bi.biRedMask =    0x00ff0000;
    bi.biGreenMask =  0x0000ff00;
    bi.biBlueMask =   0x000000ff;
    bi.biAlphaMask =  0xff000000;

    dwPixelArraySize = ((Bitmap0.bmWidth * wBitCount + 31) & ~31) / 8 * Bitmap0.bmHeight;
    bi.biSizeImage = dwPixelArraySize;

    dwDIBSize = sizeof(BITMAPV3INFOHEADER) + dwPaletteSize + dwPixelArraySize;
    hDib = GlobalAlloc(GHND, dwDIBSize);
    lpbi = (LPBITMAPV3INFOHEADER)GlobalLock(hDib);
    *lpbi = bi;

#ifdef FORCE_DEFAULT_PALETTE
    hPal = GetStockObject(DEFAULT_PALETTE);
    if (hPal)
    {
      hDC = GetDC(NULL);
      hOldPal2 = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
      RealizePalette(hDC);
    }
#endif

    void* lpPixelArrayAddress = LPBYTE(lpbi) + sizeof(BITMAPV3INFOHEADER) + dwPaletteSize;
    GetDIBits(hDcMem, hBitmap, 0, (UINT)Bitmap0.bmHeight, (LPSTR)lpPixelArrayAddress, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);

    // Function GetDIBits() usually overrides the BITMAPV3INFOHEADER with an instance of BITMAPINFOHEADER.
    if (lpbi->biSize == sizeof(BITMAPINFOHEADER))
    {
      // Rebuild our BITMAPV3INFOHEADER.
      lpbi->biSize          = bi.biSize         ;
      lpbi->biXPelsPerMeter = bi.biXPelsPerMeter;
      lpbi->biYPelsPerMeter = bi.biYPelsPerMeter;
      lpbi->biClrImportant  = bi.biClrImportant ;
      lpbi->biClrUsed       = bi.biClrUsed      ;
      // V2 and V3
      lpbi->biRedMask       = bi.biRedMask      ;
      lpbi->biGreenMask     = bi.biGreenMask    ;
      lpbi->biBlueMask      = bi.biBlueMask     ;
      lpbi->biAlphaMask     = bi.biAlphaMask    ;
    }

#if 0
    lpbi->biCompression = BI_RGB;
    GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap0.bmHeight, (LPSTR)lpPixelArrayAddress, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);
    DumpBuffer("c:\\temp\\GetDIBits.BI_RGB.data", lpPixelArrayAddress, dwPixelArraySize);
    lpbi->biCompression = BI_BITFIELDS;
    GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap0.bmHeight, (LPSTR)lpPixelArrayAddress, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);
    DumpBuffer("c:\\temp\\GetDIBits.BI_BITFIELDS.data", lpPixelArrayAddress, dwPixelArraySize);
    lpbi->biCompression = BI_RGB;
    GetDIBits(hDC, hBitmap, 0, -Bitmap0.bmHeight, (LPSTR)lpPixelArrayAddress, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);
    DumpBuffer("c:\\temp\\GetDIBits.-BI_RGB.data", lpPixelArrayAddress, dwPixelArraySize);
    lpbi->biCompression = BI_BITFIELDS;
    GetDIBits(hDC, hBitmap, 0, -Bitmap0.bmHeight, (LPSTR)lpPixelArrayAddress, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);
    DumpBuffer("c:\\temp\\GetDIBits.-BI_BITFIELDS.data", lpPixelArrayAddress, dwPixelArraySize);

    lpbi->biCompression = BI_RGB;
    GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap0.bmHeight, (LPSTR)lpPixelArrayAddress, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);

    //GetBitmapBits(hBitmap, (LONG)pixels.size(), (void*)pixels.data());
    //DumpString("c:\\temp\\GetBitmapBits.data", pixels);
#endif

#ifdef FORCE_DEFAULT_PALETTE
    if (hOldPal2)
    {
      SelectPalette(hDC, (HPALETTE)hOldPal2, TRUE);
      RealizePalette(hDC);
      ReleaseDC(NULL, hDC);
    }
#endif

    hFile = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
      bReturn = false;
      goto cleanup;
    }

    bmfHdr.bfType = 0x4D42; // "BM"
    dwFileSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPV3INFOHEADER) + dwPaletteSize + dwPixelArraySize;
    bmfHdr.bfSize = dwFileSize;
    bmfHdr.bfReserved1 = 0;
    bmfHdr.bfReserved2 = 0;
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPV3INFOHEADER) + dwPaletteSize;

    WriteFile(hFile, (LPSTR)&bmfHdr, sizeof(bmfHdr), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbi, dwDIBSize, &dwBytesWritten, NULL);

    // success
    bReturn = true;

    // Using goto for win32 api is most elegant way of cleaning up:
    // https://github.com/search?q=repo%3Amicrosoft%2FWindows-classic-samples+%22goto+%22&type=code
  cleanup:
    // Close the .BMP file.
    if (hFile)
      CloseHandle(hFile);

    // Free memory.
    if (hDib)
      GlobalUnlock(hDib);
    if (hDib)
      GlobalFree(hDib);

    // Clean up.
    if (hOldBitmap)
      SelectObject(hDcMem, hOldBitmap);
    if (hDcMem)
      DeleteDC(hDcMem);
    if (hDcDesktop)
      ReleaseDC(hWndDesktop, hDcDesktop);

    return bReturn;
  }

  // https://en.wikipedia.org/wiki/BMP_file_format
  // https://upload.wikimedia.org/wikipedia/commons/7/75/BMPfileFormat.svg
  // https://stackoverflow.com/questions/24720451/save-hbitmap-to-bmp-file-using-only-win32
  // https://stackoverflow.com/questions/69574015/retrieving-the-palette-of-a-bitmap-image
  bool SaveBitmapFile(const char* path, HBITMAP hBitmap)
  {
    WORD wBitCount = 0;
    DWORD dwPaletteSize = 0;
    DWORD dwPaletteColorCount = 0;
    DWORD dwPixelArraySize = 0;
    DWORD dwFileSize = 0;
    DWORD dwBitmapInfoHeaderSize = 0;
    DWORD dwBitmapInfoAndPaletteSize = 0;
    DWORD dwBytesWritten = 0;
    DWORD dwTotalBytesWritten = 0;
    BITMAP bmp;
    BITMAPFILEHEADER bmpFileHdr;
    BITMAPINFO* lpBmpInfo = NULL;
    BITMAPV3INFOHEADER bmpInfo3;
    BYTE* lpPixelArray = NULL;
    HANDLE hFile = NULL;
    bool bReturn = false;

    // Get properties of the bitmap
#ifndef NDEBUG
    int object_size =
#endif
      GetObject(hBitmap, sizeof(bmp), &bmp);
#ifndef NDEBUG
    assert((size_t)object_size == sizeof(bmp));
#endif

    HWND hWndDesktop = GetDesktopWindow();
    HDC hDcDesktop = GetDC(hWndDesktop);
    HDC hDcMem = CreateCompatibleDC(hDcDesktop);

    // Check bit per pixels
    wBitCount = (WORD)GetBitPerPixel(&bmp);

    // Get the internal bitmap format, palette and pixels.
    // If the bitmap is 24bpp or more, there is no color table.
    // If the bitmap has a color table, we need to allocate space for the color table and the BITMAPINFOHEADER
    dwPaletteColorCount = (wBitCount < 24) ? (1 << wBitCount) : 0;
    dwPaletteSize = (wBitCount < 24) ? sizeof(RGBQUAD) * dwPaletteColorCount : 0;
    dwPixelArraySize = ((bmp.bmWidth * wBitCount + 31) & ~31) / 8 * bmp.bmHeight;
    dwBitmapInfoAndPaletteSize = sizeof(BITMAPINFO) + dwPaletteSize - 1 * sizeof(RGBQUAD); // -1*RGBQUAD (because of BITMAPINFO::bmiColors[1])
    lpBmpInfo = (BITMAPINFO*)LocalAlloc(LPTR, dwBitmapInfoAndPaletteSize);
    if (lpBmpInfo == NULL) goto cleanup;
    RGBQUAD * lpPaletteColorArray = (wBitCount < 24) ? lpBmpInfo->bmiColors : NULL;
    lpPixelArray = (BYTE*)LocalAlloc(LPTR, dwPixelArraySize);
    if (lpPixelArray == NULL) goto cleanup;
#ifndef NDEBUG
    // DEBUG: fill memory to check for overflows
    memset(lpBmpInfo, 0x77, dwBitmapInfoAndPaletteSize);
    if (lpPaletteColorArray)
      memset(lpPaletteColorArray, 0x88, dwPaletteSize);
    memset(lpPixelArray, 0x99, dwPixelArraySize);
#endif
    memset(lpBmpInfo, 0x00, dwBitmapInfoAndPaletteSize);
    lpBmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    lpBmpInfo->bmiHeader.biWidth = bmp.bmWidth;
    lpBmpInfo->bmiHeader.biHeight = bmp.bmHeight;
    lpBmpInfo->bmiHeader.biPlanes = bmp.bmPlanes;
    lpBmpInfo->bmiHeader.biBitCount = bmp.bmBitsPixel;
    lpBmpInfo->bmiHeader.biCompression = BI_RGB;
    lpBmpInfo->bmiHeader.biSizeImage = dwPixelArraySize;
    lpBmpInfo->bmiHeader.biClrUsed = dwPaletteColorCount;
    lpBmpInfo->bmiHeader.biClrImportant = dwPaletteColorCount;
#ifndef NDEBUG
    int result =
#endif
      GetDIBits(hDcMem, hBitmap, 0, (UINT)bmp.bmHeight, lpPixelArray, lpBmpInfo, DIB_RGB_COLORS);
#ifndef NDEBUG
    assert(result > 0);
    //NOT ALWAYS TRUE. assert(dwPaletteColorCount == lpBmpInfo->bmiHeader.biClrUsed);
#endif

    // Note, GetDIBits() is cleaning biClrUsed and biClrImportant.
    // According to the documentation:
    //   If biClrUsed is zero, the array contains the maximum number of colors for the given bitdepth; that is, 2^biBitCount colors.
    //   If biClrImportant is zero, all colors are important.

    // Double check what we got
#ifndef NDEBUG
    assert(dwPixelArraySize == lpBmpInfo->bmiHeader.biSizeImage);
#endif

    // Set default properties
    lpBmpInfo->bmiHeader.biXPelsPerMeter = 0;  // Windows 10 Paint does not set any DPI.
    lpBmpInfo->bmiHeader.biYPelsPerMeter = 0;  // Windows 10 Paint does not set any DPI.
    if (wBitCount == 32)
    {
      // 32 bit per pixels are not supported in Paint.
      // Use GIMP default of 76 dpi.
      lpBmpInfo->bmiHeader.biXPelsPerMeter = 2835;  // 72 DPI
      lpBmpInfo->bmiHeader.biYPelsPerMeter = 2835;  // 72 DPI
    }

    // Compute the expected output file size.
    dwBitmapInfoHeaderSize = sizeof(BITMAPINFOHEADER);
    if (wBitCount == 32)
    {
      // For 32bpp bitmap, it is best to use BITMAPV3INFOHEADER instead of the normal BITMAPINFOHEADER.
      // Transparency (alpha channel) seems to be only supported from BITMAPV3INFOHEADER.
      dwBitmapInfoHeaderSize = sizeof(BITMAPV3INFOHEADER);
    }
    dwFileSize = sizeof(BITMAPFILEHEADER) + dwBitmapInfoHeaderSize + dwPaletteSize + dwPixelArraySize;

    // Build the file header
    bmpFileHdr.bfType = 0x4D42; // "BM"
    bmpFileHdr.bfSize = dwFileSize;
    bmpFileHdr.bfReserved1 = 0;
    bmpFileHdr.bfReserved2 = 0;
    bmpFileHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + dwBitmapInfoHeaderSize + dwPaletteSize;

    // Create the output file
    hFile = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE) goto cleanup;

    // Write file header
    dwBytesWritten = 0;
    WriteFile(hFile, &bmpFileHdr, sizeof(bmpFileHdr), &dwBytesWritten, NULL);
    dwTotalBytesWritten += dwBytesWritten;

    // Write BITMAPFILEHEADER and palette
    if (wBitCount < 32)
    {
      dwBytesWritten = 0;
      WriteFile(hFile, lpBmpInfo, dwBitmapInfoAndPaletteSize, &dwBytesWritten, NULL);
      dwTotalBytesWritten += dwBytesWritten;
    }
    else
    {
      // 32 bit per pixel bitmaps...

      // Use the original BITMAPINFOHEADER as a base.
      memcpy(&bmpInfo3, &lpBmpInfo->bmiHeader, sizeof(BITMAPINFOHEADER));

      // Fix the actual size
      bmpInfo3.biSize = sizeof(BITMAPV3INFOHEADER);

      // Fix biCompression because we use BITMAPV3INFOHEADER.
      // BI_BITFIELDS: Uncompressed RGB with color masks. Valid for 16-bpp and 32-bpp bitmaps.
      bmpInfo3.biCompression = BI_BITFIELDS;

      // Fill remaining fields
      bmpInfo3.biRedMask =    0x00ff0000;
      bmpInfo3.biGreenMask =  0x0000ff00;
      bmpInfo3.biBlueMask =   0x000000ff;
      bmpInfo3.biAlphaMask =  0xff000000;

      // BITMAPV3INFOHEADER
      dwBytesWritten = 0;
      WriteFile(hFile, &bmpInfo3, sizeof(bmpInfo3), &dwBytesWritten, NULL);
      dwTotalBytesWritten += dwBytesWritten;

      // No palette for 32 bit bitmaps.
    }

    // Pixels data
    dwBytesWritten = 0;
    WriteFile(hFile, lpPixelArray, dwPixelArraySize, &dwBytesWritten, NULL);
    dwTotalBytesWritten += dwBytesWritten;
    
#ifndef NDEBUG
    assert(dwTotalBytesWritten == bmpFileHdr.bfSize);
#endif

    // success
    bReturn = true;

    // Using goto for win32 api is most elegant way of cleaning up:
    // https://github.com/search?q=repo%3Amicrosoft%2FWindows-classic-samples+%22goto+%22&type=code
  cleanup:
    // Close the .BMP file.
    if (hFile)
      CloseHandle(hFile);

    // Free memory.
    if (lpBmpInfo)
      LocalFree(lpBmpInfo);
    if (lpPixelArray)
      LocalFree(lpPixelArray);

    // Clean up.
    if (hDcMem)
      DeleteDC(hDcMem);
    if (hDcDesktop)
      ReleaseDC(hWndDesktop, hDcDesktop);

    return bReturn;
  }

  HBITMAP LoadBitmapFromFile(const char* path)
  {
    // LoadImage() function does not support 32bpp bitmap. We must load the bitmap ourself.

    DWORD dwReadSize = 0;
    DWORD dwBitmapInfoReadSize = 0;  // how much we have read the BITMAPINFOHEADER
    DWORD dwPixelArraySize = 0;
    //LONG lPixelWriteSize = 0;
    BYTE* lpPixelArray = NULL;
    BITMAPFILEHEADER bmpFileHdr;
    BITMAPV3INFOHEADER bi3;
    BITMAPINFO bmi;
    VOID* pvBits;
    HBITMAP hBitmap = NULL;
    HBITMAP hReturn = NULL;

    FILE* f = fopen(path, "rb");
    if (!f) return NULL;

    HWND hWndDesktop = GetDesktopWindow();
    HDC hDcDesktop = GetDC(hWndDesktop);
    HDC hDcMem = CreateCompatibleDC(hDcDesktop);

    memset(&bmpFileHdr, 0x00, sizeof(bmpFileHdr));
    dwReadSize = (DWORD)fread(&bmpFileHdr, 1, sizeof(bmpFileHdr), f);
    if (dwReadSize != sizeof(bmpFileHdr)) goto cleanup;

    // Check integrity
    if (bmpFileHdr.bfType != 0x4D42) goto cleanup; // "BM"

    // Read the BITMAPINFOHEADER
    memset(&bi3, 0x00, sizeof(bi3));
    dwReadSize = (DWORD)fread(&bi3, 1, sizeof(BITMAPINFOHEADER), f);
    if (dwReadSize != sizeof(BITMAPINFOHEADER)) goto cleanup;
    dwBitmapInfoReadSize += dwReadSize;

    // Check integrity
    if (bi3.biSize < sizeof(BITMAPINFOHEADER)) goto cleanup;  
    if (bi3.biBitCount == 0) goto cleanup;
    if (bi3.biSizeImage == 0) goto cleanup;
    if (bi3.biClrUsed != 0) goto cleanup;
    if (bi3.biClrImportant != 0) goto cleanup;

    // Could we fallback to LoadImage() ?
    if (bi3.biBitCount < 32)
    {
      // Yes fallback to LoadImage()
      hBitmap = (HBITMAP)LoadImageA(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
      hReturn = hBitmap;
      goto cleanup;
    }

    // Read remaining fields of the info header
    if (bi3.biSize >= sizeof(BITMAPV2INFOHEADER))
    {
      // Read red, greeen and blue masks.
      dwReadSize = (DWORD)fread(&bi3.biRedMask, 1, 3 * sizeof(RGBQUAD), f);
      if (dwReadSize != 3 * sizeof(RGBQUAD)) goto cleanup;
      dwBitmapInfoReadSize += dwReadSize;

      // Check integrity
      if (bi3.biRedMask   != 0x00ff0000) goto cleanup;
      if (bi3.biGreenMask != 0x0000ff00) goto cleanup;
      if (bi3.biBlueMask  != 0x000000ff) goto cleanup;
    }
    if (bi3.biSize >= sizeof(BITMAPV3INFOHEADER))
    {
      // Read red, greeen and blue masks.
      dwReadSize = (DWORD)fread(&bi3.biAlphaMask, 1, 1 * sizeof(RGBQUAD), f);
      if (dwReadSize != 1 * sizeof(RGBQUAD)) goto cleanup;
      dwBitmapInfoReadSize += dwReadSize;

      // Check integrity
      if (bi3.biAlphaMask != 0xff000000) goto cleanup;
    }

    // Read remaining part of the BITMAPINFOHEADER which we do not support
    if (dwBitmapInfoReadSize < bi3.biSize)
    {
      size_t remainder = bi3.biSize - dwBitmapInfoReadSize;
      if (fseek(f, (long)remainder, SEEK_CUR) == 0) goto cleanup;
    }

    // Create a 32bpp bitmap with a DIB section
    memcpy(&bmi.bmiHeader, &bi3, sizeof(bmi.bmiHeader));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biCompression = BI_RGB;
    pvBits = NULL;
    hBitmap = CreateDIBSection(hDcMem, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
    if (hBitmap == NULL) goto cleanup;

    // Read the pixel data directly into the created hBitmap's DIB section
#ifndef NDEBUG
    assert((DWORD)ftell(f) == bmpFileHdr.bfOffBits);
#endif
    dwPixelArraySize = bi3.biSizeImage;
    dwReadSize = (DWORD)fread(pvBits, 1, dwPixelArraySize, f);
    if (dwReadSize != dwPixelArraySize) goto cleanup;

    //// Allocate temporary local memory to read the pixels.
    //lpPixelArray = (BYTE*)LocalAlloc(LPTR, dwPixelArraySize);
    //if (lpPixelArray == NULL) goto cleanup;
    //dwReadSize = (DWORD)fread(lpPixelArray, 1, dwPixelArraySize, f);
    //if (dwReadSize != dwPixelArraySize) goto cleanup;

    //Assign our temporary pixel buffer as the new bitmap pixel buffer
    //lPixelWriteSize = SetBitmapBits(hBitmap, dwPixelArraySize, lpPixelArray);
    //if ((DWORD)lPixelWriteSize != dwPixelArraySize) goto cleanup;
    //memcpy(pvBits, lpPixelArray, dwPixelArraySize);

    // success
    hReturn = hBitmap;

    // Using goto for win32 api is most elegant way of cleaning up:
    // https://github.com/search?q=repo%3Amicrosoft%2FWindows-classic-samples+%22goto+%22&type=code
  cleanup:
    // Close the .BMP file.
    if (f)
      fclose(f);

    // Free memory.
    if (lpPixelArray)
      LocalFree(lpPixelArray);

    // Clean up.
    if (hDcMem)
      DeleteDC(hDcMem);
    if (hDcDesktop)
      ReleaseDC(hWndDesktop, hDcDesktop);

    return hReturn;
  }

  BOOL IsFullyTransparent(HBITMAP hBitmap)
  {
    SIZE bitmap_size = GetBitmapSize(hBitmap);
    LONG num_pixels = bitmap_size.cx * bitmap_size.cy;
    LONG image_size = num_pixels * BYTES_PER_PIXEL;

    //Create a buffer to hold the color pixels for debugging the bitmap's content
    std::string color_pixels;
    color_pixels.assign((size_t)image_size, 0);
    if (color_pixels.size() != image_size)
      return FALSE;

    UINT numPixelsRead = GetBitmapBits(hBitmap, (LONG)color_pixels.size(), (void*)color_pixels.data());

    BOOL transparent = IsFullyTransparent(color_pixels);
    return transparent;
  }

  BOOL IsFullyTransparent(const std::string& buffer)
  {
    size_t image_size = buffer.size();
    size_t num_pixels = image_size / BYTES_PER_PIXEL;

    BOOL isFullyTransparent = TRUE;
    for (size_t i = 0; i < num_pixels && isFullyTransparent == TRUE; i++)
    {
      RGBQUAD* first_pixels = (RGBQUAD*)buffer.data();
      RGBQUAD& pixel = first_pixels[i];
      if (pixel.rgbReserved > 0)
        isFullyTransparent = FALSE;
    }

    return isFullyTransparent;
  }

  std::string GetMenuItemDetails(HMENU hMenu, UINT pos)
  {
    MENUITEMINFOW info = { 0 };
    info.cbSize = sizeof(MENUITEMINFOW);
    info.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
    BOOL wInfoSuccess = GetMenuItemInfoW(hMenu, pos, TRUE, &info);
    if (!wInfoSuccess)
      return "";

    const UINT& id = info.wID; //GetMenuItemID(hMenu, pos);

    bool isSeparator = ((info.fType & MFT_SEPARATOR) != 0);
    bool isDisabled = ((info.fState & MFS_DISABLED) != 0);
    bool isChecked = ((info.fState & MFS_CHECKED) != 0);

    //compute display name
    static const int BUFFER_SIZE = 1024;
    char title_utf8[BUFFER_SIZE] = { 0 };
    char tmp[BUFFER_SIZE] = { 0 };
    wchar_t tmpW[BUFFER_SIZE] = { 0 };
    if (isSeparator)
    {
      strcpy(title_utf8, "------------------------");
    }
    //try with ansi text
    else if (GetMenuStringW(hMenu, id, tmpW, BUFFER_SIZE, 0))
    {
      //Can't log unicode characters, convert to utf-8.
      std::string atext = ra::unicode::UnicodeToUtf8(tmpW);
      sprintf(title_utf8, "%s", atext.c_str());
    }

    //build full menu description string
    std::string description_utf8;
    description_utf8.append(title_utf8);
    description_utf8.append(" (");
    sprintf(tmp, "pos=%lu, id=%lu", pos, id);
    description_utf8.append(tmp);
    if (isChecked)
      description_utf8.append(", checked");
    if (isDisabled && !isSeparator)
      description_utf8.append(", disabled");
    description_utf8.append(")");

    return description_utf8;
  }

  std::string GetMenuTree(HMENU hMenu, int indent)
  {
    std::string output;

    int numItems = GetMenuItemCount(hMenu);

    for (int i = 0; i < numItems; i++)
    {
      std::string details = GetMenuItemDetails(hMenu, i);

      //Detect if this menu is a parent menu
      HMENU hSubMenu = NULL;
      {
        MENUITEMINFOW info = { 0 };
        info.cbSize = sizeof(MENUITEMINFOW);
        info.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
        BOOL wInfoSuccess = GetMenuItemInfoW(hMenu, i, TRUE, &info);
        if (wInfoSuccess)
        {
          if (info.hSubMenu)
          {
            hSubMenu = info.hSubMenu;
          }
        }
      }

      if (hSubMenu == NULL)
      {
        //no child
        output += std::string(indent, ' ');
        output += details + "\n";
      }
      else
      {
        //output this menu and open a bracket
        output += std::string(indent, ' ');
        output += details + " {\n";

        //query for submenu description.
        std::string sub_desc = GetMenuTree(hSubMenu, indent + 4);
        output += sub_desc;

        //close the opened bracket
        output += std::string(indent, ' ');
        output += "}\n";
      }
    }

    return output;
  }

} //namespace Win32Utils
