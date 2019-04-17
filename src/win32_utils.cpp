//#define WIN32_LEAN_AND_MEAN 1
#include "win32_utils.h"

#include <string>
#include <assert.h>

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
 
  void dumpString(const std::string file_path, const std::string & buffer)
  {
    FILE * f = fopen(file_path.c_str(), "wb");
    if (!f)
      return;

    fwrite(buffer.data(), 1, buffer.size(), f);
    fclose(f);
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

    //for GIMP, if all pixels are invisible, the color information is lost
    bool isFullyTransparent = true;
    for(size_t i=0; i<num_pixels && isFullyTransparent == true; i++)
    {
      RGBQUAD * first_pixels = (RGBQUAD*)color_pixels.data();
      RGBQUAD & pixel = first_pixels[i];
      if (pixel.rgbReserved > 0)
        isFullyTransparent = false;
    }

    //if fully transparent, make it opaque
    if (isFullyTransparent)
    {
      for(size_t i=0; i<num_pixels && isFullyTransparent == true; i++)
      {
        RGBQUAD * first_pixels = (RGBQUAD*)color_pixels.data();
        RGBQUAD & pixel = first_pixels[i];
        pixel.rgbReserved = 0xFF;
      }
    }

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

  PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
  { 
    BITMAP bmp; 
    PBITMAPINFO pbmi; 
    WORD    cClrBits; 

    // Retrieve the bitmap color format, width, and height.  
    assert(GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)); 

    // Convert the color format to a count of bits.  
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
    if (cClrBits == 1) 
      cClrBits = 1; 
    else if (cClrBits <= 4) 
      cClrBits = 4; 
    else if (cClrBits <= 8) 
      cClrBits = 8; 
    else if (cClrBits <= 16) 
      cClrBits = 16; 
    else if (cClrBits <= 24) 
      cClrBits = 24; 
    else cClrBits = 32; 

    // Allocate memory for the BITMAPINFO structure. (This structure  
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
    // data structures.)  

    if (cClrBits < 24) 
      pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                                      sizeof(BITMAPINFOHEADER) + 
                                      sizeof(RGBQUAD) * (1<< cClrBits)); 

    // There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

    else 
      pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
      sizeof(BITMAPINFOHEADER)); 

    // Initialize the fields in the BITMAPINFO structure.  

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
      pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

    // If the bitmap is not compressed, set the BI_RGB flag.  
    pbmi->bmiHeader.biCompression = BI_RGB; 

    // Compute the number of bytes in the array of color  
    // indices and store the result in biSizeImage.  
    // The width must be DWORD aligned unless the bitmap is RLE 
    // compressed. 
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
      * pbmi->bmiHeader.biHeight; 
    // Set biClrImportant to 0, indicating that all of the  
    // device colors are important.  
    pbmi->bmiHeader.biClrImportant = 0; 
    return pbmi; 
  } 

  //https://stackoverflow.com/questions/24720451/save-hbitmap-to-bmp-file-using-only-win32
  void CreateBMPFile(const char * pszFile, HBITMAP hBMP) 
  { 
    HANDLE hf;                 // file handle  
    BITMAPFILEHEADER hdr;       // bitmap file-header  
    PBITMAPINFOHEADER pbih;     // bitmap info-header  
    LPBYTE lpBits;              // memory pointer  
    DWORD dwTotal;              // total count of bytes  
    DWORD cb;                   // incremental count of bytes  
    BYTE *hp;                   // byte pointer  
    DWORD dwTmp;     
    PBITMAPINFO pbi;
    HDC hDC;

    hDC = CreateCompatibleDC(GetWindowDC(GetDesktopWindow()));
    SelectObject(hDC, hBMP);

    pbi = CreateBitmapInfoStruct(hBMP);

    pbih = (PBITMAPINFOHEADER) pbi; 
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    assert(lpBits) ;

    // Retrieve the color table (RGBQUAD array) and the bits  
    // (array of palette indices) from the DIB.  
    assert(GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, 
      DIB_RGB_COLORS));

    // Create the .BMP file.  
    hf = CreateFile(pszFile, 
      GENERIC_READ | GENERIC_WRITE, 
      (DWORD) 0, 
      NULL, 
      CREATE_ALWAYS, 
      FILE_ATTRIBUTE_NORMAL, 
      (HANDLE) NULL); 
    assert(hf != INVALID_HANDLE_VALUE) ;

    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
    // Compute the size of the entire file.  
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
      pbih->biSize + pbih->biClrUsed 
      * sizeof(RGBQUAD) + pbih->biSizeImage); 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 

    // Compute the offset to the array of color indices.  
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
      pbih->biSize + pbih->biClrUsed 
      * sizeof (RGBQUAD); 

    // Copy the BITMAPFILEHEADER into the .BMP file.  
    assert(WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
      (LPDWORD) &dwTmp,  NULL)); 

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
    assert(WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
      + pbih->biClrUsed * sizeof (RGBQUAD), 
      (LPDWORD) &dwTmp, ( NULL)));

    // Copy the array of color indices into the .BMP file.  
    dwTotal = cb = pbih->biSizeImage; 
    hp = lpBits; 
    assert(WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)); 

    // Close the .BMP file.  
    assert(CloseHandle(hf)); 

    // Free memory.  
    GlobalFree((HGLOBAL)lpBits);
  }

} //namespace win32_utils
