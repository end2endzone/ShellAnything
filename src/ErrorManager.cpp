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

#include "ErrorManager.h"
#include <stdio.h>
#include <direct.h>

#include "rapidassist/unicode.h"

#ifdef WIN32
//#   ifndef WIN32_LEAN_AND_MEAN
//#   define WIN32_LEAN_AND_MEAN 1
//#   endif
#   include <windows.h> // for GetModuleHandleEx()
#   include <Shlobj.h>
#   include <ShellAPI.h>
#   include <psapi.h>
#   pragma comment( lib, "psapi.lib" )
#endif

namespace shellanything
{

  void ShowErrorMessage(const std::string & title, const std::string & message)
  {
    HWND hWnd = GetDesktopWindow();
    MessageBoxA(hWnd, message.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
  }

  void ShowErrorMessageUtf8(const std::string & title, const std::string & message)
  {
    HWND hWnd = GetDesktopWindow();
    std::wstring   title_unicode = ra::unicode::Utf8ToUnicode(title);
    std::wstring message_unicode = ra::unicode::Utf8ToUnicode(message);
    MessageBoxW(hWnd, message_unicode.c_str(), title_unicode.c_str(), MB_OK | MB_ICONERROR);
  }

} //namespace shellanything
