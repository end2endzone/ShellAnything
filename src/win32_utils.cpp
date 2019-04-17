//#define WIN32_LEAN_AND_MEAN 1
#include "win32_utils.h"

#include <string>

namespace win32_utils
{
  SIZE GetIconSize(HICON hIcon)
  {
    SIZE size = {0};

    ICONINFO IconInfo = {0};
    if(!GetIconInfo(hIcon, &IconInfo))
      return size; //failed getting the icon's size
   
    BITMAP bmpMask={0};
    GetObject(IconInfo.hbmMask, sizeof(BITMAP), &bmpMask);
   
    size.cx = bmpMask.bmWidth;
    size.cy = bmpMask.bmHeight;
   
    //cleanup
    DeleteObject(IconInfo.hbmColor);
    DeleteObject(IconInfo.hbmMask);

    return size;
  }

  HICON GetBestIconForMenu(HICON hIconLarge, HICON hIconSmall)
  {
    SIZE large_size = GetIconSize(hIconLarge);
    SIZE small_size = GetIconSize(hIconSmall);

    SIZE menu_size;
    menu_size.cx = GetSystemMetrics(SM_CXSMICON);
	  menu_size.cy = GetSystemMetrics(SM_CYSMICON);
  
    //compute total number of pixels for each icons
    const long menu_pixels  = menu_size.cx * menu_size.cy;
    const long large_pixels = large_size.cx * large_size.cy;
    const long small_pixels = small_size.cx * small_size.cy;

    //compute distance to menu_pixels
    const long large_diff = abs(large_pixels - menu_pixels);
    const long small_diff = abs(small_pixels - menu_pixels);

    if (small_diff <= large_diff)
      return hIconSmall;
    return hIconLarge;
  }

  RGBQUAD toRGBQUAD(const DWORD & iColor)
  {
    RGBQUAD output = {0};
    output.rgbRed   = (BYTE)((iColor&0x000000FF)    );
    output.rgbGreen = (BYTE)((iColor&0x0000FF00)>> 8);
    output.rgbBlue  = (BYTE)((iColor&0x00FF0000)>>16);
    output.rgbReserved = 255;
    return output;
  }

  template <typename T> inline static T interpolate_color( const T& a, const T& b, const double factor /* from 0.0 to 1.0 */)
  {
    if (factor <= 0.0)
      return a;
    else if (factor >= 1.0)
      return b;
    else
      return ( a + (T)((b-a)*factor) );
  }
 
  SIZE GetBitmapSize(HBITMAP hBitmap)
  {
    SIZE size = {0};
 
    BITMAP b = {0};
    GetObject(hBitmap, sizeof(BITMAP), &b);
 
    size.cx = b.bmWidth;
    size.cy = b.bmHeight;
 
    return size;
  }
 
  BOOL FillTransparentPixels(HBITMAP hBitmap, COLORREF background_color)
  {
    //FillTransparentPixels() blends every pixels of the given bitmap with the given background_color.
    //The result is a fully opaque bitmap which makes it usable in most win32 API.
    //It also allows the bitmap to be properly saved to *.bmp file format.
    //Note: The alpha channel is still available but all values are 0xFF (fully opaque).
 
    const RGBQUAD BACKGROUND_COLOR = toRGBQUAD(background_color);
 
    static const size_t BITS_PER_PIXEL = 32;
    static const size_t BYTES_PER_PIXEL = BITS_PER_PIXEL/8;
 
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
    for(LONG i=0; i<num_pixels; i++)
    {
      RGBQUAD * first_pixel = (RGBQUAD *)pixel_buffer.data();
      RGBQUAD & pixel = first_pixel[i];
      if (pixel.rgbReserved != full_opacity)
      {
        //blend pixel with the given background color
        double alpha_factor = (double)pixel.rgbReserved / 255.0;
        pixel.rgbRed    = interpolate_color<BYTE>(BACKGROUND_COLOR.rgbRed,    pixel.rgbRed,   alpha_factor);
        pixel.rgbGreen  = interpolate_color<BYTE>(BACKGROUND_COLOR.rgbGreen,  pixel.rgbGreen, alpha_factor);
        pixel.rgbBlue   = interpolate_color<BYTE>(BACKGROUND_COLOR.rgbBlue,   pixel.rgbBlue,  alpha_factor);
        pixel.rgbReserved = full_opacity;
      }
    }
 
    //Assign our temporary pixel buffer as the new bitmap pixel buffer
    LONG size_write = SetBitmapBits(hBitmap, (DWORD)pixel_buffer.size(), (void*)pixel_buffer.data());
    if (size_write != image_size)
      return FALSE;
 
    return TRUE;
  }
 
  HBITMAP CopyAsBitmap(HICON hIcon, const int bitmap_width, const int bitmap_height)
  {
    //According to https://devblogs.microsoft.com/oldnewthing/20101021-00/?p=12483, using DrawIconEx()
    //with diFlags=DI_NORMAL is supposed to blend the icon color and transparency with the content of the destination.
    //This function uses a fully transparent bitmap as destination image.
    //The result is a HBITMAP with an 8-bit alpha channel that matches the icon's 8-bit alpha channel.
    //Note: Using the bitmap in other API which does not support transparency may result in alpha related artifacts.
    //Note: Saving the bitmap to a *.bmp file does not support transparency, may also result in alpha related artifacts. Usually, the transparent pixels are displayed as WHITE opaque pixels.
    //See RemoveAlphaChannel() for converting the bitmap to a fully opaque image
    //
 
    static const size_t BITS_PER_PIXEL = 32;
    static const size_t BYTES_PER_PIXEL = BITS_PER_PIXEL/8;
    const size_t num_pixels = bitmap_width * bitmap_height;
    const size_t image_size = num_pixels * BYTES_PER_PIXEL;
 
  #if 0
    //Create a buffer to hold the color pixels for debugging the bitmap's content
    std::string color_pixels;
    color_pixels.assign((size_t)image_size, 0);
    if (color_pixels.size() != image_size)
      return NULL;
  #endif
 
    HWND hWndDesktop = GetDesktopWindow();
    HDC hdcDesktop = GetDC(hWndDesktop);
    HDC hDcMem = CreateCompatibleDC(hdcDesktop);
 
    // Create a 32bbp bitmap and select it.
    HBITMAP hBitmap = CreateBitmap(bitmap_width, bitmap_height, 1, BITS_PER_PIXEL, NULL);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hDcMem, hBitmap);
 
  #if 0
    // Make sure the destination bitmap is fully transparent by default
    LONG numPixelsRead = GetBitmapBits(hBitmap, (LONG)color_pixels.size(), (void*)color_pixels.data());
    assert(numPixelsRead == image_size);
    dumpString("c:\\temp\\destination_bitmap.data", color_pixels);
  #endif
 
    //Draw the icon, blending with the fully transparent destination image.
    //The result is a HBITMAP with an 8-bit alpha channel that matches the icon's 8-bit alpha channel.
    DrawIconEx(hDcMem, 0, 0, hIcon, bitmap_width, bitmap_height, 0, NULL, DI_NORMAL);
 
  #if 0
    //Output the bitmap pixels to a file in the "DATA image format" for debugging.
    numPixelsRead = GetBitmapBits(hBitmap, (LONG)color_pixels.size(), (void*)color_pixels.data());
    assert(numPixelsRead == image_size);
    dumpString("c:\\temp\\blended_bitmap.data", color_pixels);
  #endif
 
    // Clean up.
    SelectObject(hDcMem, hbmOld);
    DeleteDC(hDcMem);
    ReleaseDC(hWndDesktop, hdcDesktop);
 
    return hBitmap;
  }
 
  HBITMAP CopyAsBitmap(HICON hIcon)
  {
    //Get properties related to Windows Menu
    SIZE menu_icon_size = GetIconSize(hIcon);
    const int menu_icon_width  = menu_icon_size.cx;
    const int menu_icon_height = menu_icon_size.cy;
 
    return CopyAsBitmap(hIcon, menu_icon_width, menu_icon_height);
  }
} //namespace win32_utils
