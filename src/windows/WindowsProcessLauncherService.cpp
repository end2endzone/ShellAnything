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

#include "WindowsProcessLauncherService.h"
#include "rapidassist/process_utf8.h"
#include "rapidassist/unicode.h"

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#include "rapidassist/undef_windows_macros.h"
#include <shellapi.h>

#include <urlmon.h>
#pragma comment(lib, "Urlmon.lib") //for IsValidURL()

namespace shellanything
{
  WindowsProcessLauncherService::WindowsProcessLauncherService()
  {
  }

  WindowsProcessLauncherService::~WindowsProcessLauncherService()
  {
  }

  bool WindowsProcessLauncherService::OpenDocument(const std::string& path, ProcessLaunchResult* result) const
  {
    uint32_t pId = ra::process::OpenDocumentUtf8(path);
    bool success = (pId != ra::process::INVALID_PROCESS_ID);

    if (result)
    {
      result->pId = pId;
    }

    return success;
  }

  bool WindowsProcessLauncherService::OpenPath(const std::string& path, ProcessLaunchResult* result) const
  {
    std::wstring pathW = ra::unicode::Utf8ToUnicode(path);

    SHELLEXECUTEINFOW info = { 0 };

    info.cbSize = sizeof(SHELLEXECUTEINFOW);

    info.fMask |= SEE_MASK_NOCLOSEPROCESS;
    info.fMask |= SEE_MASK_NOASYNC;
    info.fMask |= SEE_MASK_FLAG_DDEWAIT;

    info.hwnd = HWND_DESKTOP;
    info.nShow = SW_SHOWDEFAULT;
    info.lpVerb = L"open";
    info.lpFile = pathW.c_str();
    info.lpParameters = NULL; // arguments
    info.lpDirectory = NULL;  // Default directory

    bool success = (ShellExecuteExW(&info) == TRUE);

    if (result)
    {
      DWORD dwPid = GetProcessId(info.hProcess);
      result->pId = dwPid;
    }
    return success;
  }

  bool WindowsProcessLauncherService::IsValidUrl(const std::string& value) const
  {
    std::wstring valueW = ra::unicode::Utf8ToUnicode(value);
    if (::IsValidURL(NULL, valueW.c_str(), 0) == S_OK)
    {
      return true;
    }
    return false;
  }

  bool WindowsProcessLauncherService::OpenUrl(const std::string& path, ProcessLaunchResult* result) const
  {
    bool success = OpenPath(path, result);
    return success;
  }

} //namespace shellanything
