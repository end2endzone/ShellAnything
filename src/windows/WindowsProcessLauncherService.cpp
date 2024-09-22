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
#include "LoggerHelper.h"
#include "SaUtils.h"

#include "rapidassist/unicode.h"

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <Windows.h>
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

  std::string GetErrorMessageUtf8(DWORD dwMessageId)
  {
    LPWSTR lpMessageBuffer = NULL;

    size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, dwMessageId, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&lpMessageBuffer, 0, NULL);

    //Copy the error message into a wide std::string.
    std::wstring message_wide;
    if (size)
      message_wide = std::wstring(lpMessageBuffer, size);

    //Free the Win32's string's buffer.
    if (lpMessageBuffer)
      LocalFree(lpMessageBuffer);

    // Convert from wide characters to utf8
    std::string message = ra::unicode::UnicodeToUtf8(message_wide);

    return message;
  }

  bool WindowsProcessLauncherService::OpenDocument(const std::string& path, ProcessLaunchResult* result) const
  {
    bool success = OpenPath(path, result);
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

    // inform the caller of the result on success
    if (success && result)
    {
      HANDLE hProcess = info.hProcess;
      DWORD dwPid = GetProcessId(hProcess);
      result->pId = dwPid;
    }

    // Log a windows specific error in failure.
    if (!success)
    {
      DWORD dwLastError = ::GetLastError();
      std::string sErrorMessage = GetErrorMessageUtf8(dwLastError);

      SA_LOG(ERROR) << "Failed to call ShellExecuteExW() for value '" << path << "', Error " << ToHexString(dwLastError) << ". Description: " << sErrorMessage << ".";
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
