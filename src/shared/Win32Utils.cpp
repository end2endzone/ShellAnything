//#define WIN32_LEAN_AND_MEAN 1
#include "Win32Utils.h"
#undef GetEnvironmentVariable
#undef DeleteFile
#undef CreateDirectory
#undef CopyFile
#undef CreateFile

#include "rapidassist/unicode.h"

#include <string>
#include <assert.h>

namespace Win32Utils
{
  static const size_t BITS_PER_PIXEL = 32;
  static const size_t BYTES_PER_PIXEL = BITS_PER_PIXEL/8;

  SIZE GetIconSize(HICON hIcon)
  {
    SIZE size = {0};

    ICONINFO IconInfo = {0};
    if(!GetIconInfo(hIcon, &IconInfo))
      return size; //failed getting the icon's size
   
    BITMAP bmpMask={0};
    GetObject(IconInfo.hbmMask, sizeof(BITMAP), &bmpMask);
   
    BITMAP bmpColor={0};
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

  RGBQUAD ToRgbQuad(const DWORD & color)
  {
    RGBQUAD output = {0};
    output.rgbRed   = (BYTE)((color&0x000000FF)    );
    output.rgbGreen = (BYTE)((color&0x0000FF00)>> 8);
    output.rgbBlue  = (BYTE)((color&0x00FF0000)>>16);
    output.rgbReserved = 255;
    return output;
  }

  template <typename T> inline static T InterpolateColor( const T& a, const T& b, const double factor /* from 0.0 to 1.0 */)
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
    for(LONG i=0; i<num_pixels; i++)
    {
      RGBQUAD * first_pixel = (RGBQUAD *)pixel_buffer.data();
      RGBQUAD & pixel = first_pixel[i];
      if (pixel.rgbReserved != full_opacity)
      {
        //blend pixel with the given background color
        double alpha_factor = (double)pixel.rgbReserved / 255.0;
        pixel.rgbRed    = InterpolateColor<BYTE>(BACKGROUND_COLOR.rgbRed,    pixel.rgbRed,   alpha_factor);
        pixel.rgbGreen  = InterpolateColor<BYTE>(BACKGROUND_COLOR.rgbGreen,  pixel.rgbGreen, alpha_factor);
        pixel.rgbBlue   = InterpolateColor<BYTE>(BACKGROUND_COLOR.rgbBlue,   pixel.rgbBlue,  alpha_factor);
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
  void DumpString(const std::string file_path, const std::string & buffer)
  {
    FILE * f = fopen(file_path.c_str(), "wb");
    if (!f)
      return;

    fwrite(buffer.data(), 1, buffer.size(), f);
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
    return CreateDIBSection(hDc, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);;
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
    HBITMAP hbmOld = (HBITMAP)SelectObject(hDcMem, hBitmap);
 
  #if 0
    // Make sure the destination bitmap is fully transparent by default
    LONG numPixelsRead = GetBitmapBits(hBitmap, (LONG)color_pixels.size(), (void*)color_pixels.data());
    assert(numPixelsRead == image_size);
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
      static const RGBQUAD WHITE_PIXEL = {255,255,255,255};
      RGBQUAD * colorPixel = (RGBQUAD *)color_pixels.data();
      RGBQUAD * maskPixel =  (RGBQUAD *)mask_pixels.data();
      for(size_t i=0; i<num_pixels; i++)
      {
        colorPixel->rgbReserved = 255; //make pixel fully opaque by default
 
        //each white pixels in the mask bits needs to be be fully transparent
        if (maskPixel->rgbRed       == WHITE_PIXEL.rgbRed   &&
            maskPixel->rgbGreen     == WHITE_PIXEL.rgbGreen &&
            maskPixel->rgbBlue      == WHITE_PIXEL.rgbBlue )
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
    bool isFullyTransparent = IsFullyTransparent(hBitmap);
    if (isFullyTransparent)
    {
      for(size_t i=0; i<num_pixels && isFullyTransparent == true; i++)
      {
        RGBQUAD * first_pixels = (RGBQUAD*)color_pixels.data();
        RGBQUAD & pixel = first_pixels[i];
        pixel.rgbReserved = 0xFF;
      }
    }

    DumpString("c:\\temp\\blended_bitmap.data", color_pixels);
  #endif
 
    // Clean up.
    SelectObject(hDcMem, hbmOld);
    DeleteDC(hDcMem);
    ReleaseDC(hWndDesktop, hDcDesktop);
 
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
    BITMAP bmp = {0}; 
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

#pragma warning( push )
#pragma warning( disable: 4334 ) //warning C4334: '<<' : result of 32-bit shift implicitly converted to 64 bits (was 64-bit shift intended?)
    if (cClrBits < 24) 
      pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                                      sizeof(BITMAPINFOHEADER) + 
                                      sizeof(RGBQUAD) * (1<< cClrBits)); 

    // There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

    else 
      pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
      sizeof(BITMAPINFOHEADER)); 
#pragma warning( pop )

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
  bool CreateBmpFile(const char * pszFile, HBITMAP hBMP) 
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
    if (!lpBits)
      return false;

    // Retrieve the color table (RGBQUAD array) and the bits  
    // (array of palette indices) from the DIB.  
    GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS);

    // Create the .BMP file.  
    hf = ::CreateFileA(pszFile, 
      GENERIC_READ | GENERIC_WRITE, 
      (DWORD) 0, 
      NULL, 
      CREATE_ALWAYS, 
      FILE_ATTRIBUTE_NORMAL, 
      (HANDLE) NULL);
    if (hf == INVALID_HANDLE_VALUE)
    {
      GlobalFree((HGLOBAL)lpBits);
      return false;
    }

    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
    // Compute the size of the entire file.  
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage); 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 

    // Compute the offset to the array of color indices.  
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof (RGBQUAD); 

    // Copy the BITMAPFILEHEADER into the .BMP file.  
    WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), (LPDWORD) &dwTmp,  NULL); 

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
    WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof (RGBQUAD), (LPDWORD) &dwTmp, (NULL));

    // Copy the array of color indices into the .BMP file.  
    dwTotal = cb = pbih->biSizeImage; 
    hp = lpBits; 
    WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL); 

    // Close the .BMP file.
    CloseHandle(hf); 

    // Free memory.  
    GlobalFree((HGLOBAL)lpBits);

    return true;
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

  BOOL IsFullyTransparent(const std::string & buffer)
  {
    size_t image_size = buffer.size();
    size_t num_pixels = image_size / BYTES_PER_PIXEL;

    BOOL isFullyTransparent = TRUE;
    for(size_t i=0; i<num_pixels && isFullyTransparent == TRUE; i++)
    {
      RGBQUAD * first_pixels = (RGBQUAD*)buffer.data();
      RGBQUAD & pixel = first_pixels[i];
      if (pixel.rgbReserved > 0)
        isFullyTransparent = FALSE;
    }

    return isFullyTransparent;
  }

  std::string GetMenuItemDetails(HMENU hMenu, UINT pos)
  {
    MENUITEMINFOW info = {0};
    info.cbSize = sizeof(MENUITEMINFOW);
    info.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
    BOOL wInfoSuccess = GetMenuItemInfoW(hMenu, pos, TRUE, &info);
    if (!wInfoSuccess)
      return "";
 
    const UINT & id = info.wID; //GetMenuItemID(hMenu, pos);
 
    bool isSeparator  = ((info.fType & MFT_SEPARATOR) != 0);
    bool isDisabled   = ((info.fState & MFS_DISABLED) != 0);
    bool isChecked    = ((info.fState & MFS_CHECKED) != 0);
 
    //compute display name
    static const int BUFFER_SIZE = 1024;
    char title_utf8[BUFFER_SIZE] = {0};
    char tmp[BUFFER_SIZE] = {0};
    wchar_t tmpW[BUFFER_SIZE] = {0};
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
 
    for(int i=0; i<numItems; i++)
    {
      std::string details = GetMenuItemDetails(hMenu, i);
 
      //Detect if this menu is a parent menu
      HMENU hSubMenu = NULL;
      {
        MENUITEMINFOW info = {0};
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
        std::string sub_desc = GetMenuTree(hSubMenu, indent+4);
        output += sub_desc;
 
        //close the opened bracket
        output += std::string(indent, ' ');
        output += "}\n";
      }
    }
 
    return output;
  }

} //namespace Win32Utils
