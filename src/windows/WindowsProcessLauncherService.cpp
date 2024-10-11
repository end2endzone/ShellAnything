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
#include "Validator.h"
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

  std::string WindowsProcessLauncherService::GetErrorMessageUtf8(uint32_t dwMessageId) const
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

  bool WindowsProcessLauncherService::StartProcess(const std::string& path, const std::string& basedir, const std::string& arguments, PropertyStore& options, ProcessLaunchResult* result) const
  {
    PROCESS_START_CONTEXT context;

    // options
    std::wstring verbW = ra::unicode::Utf8ToUnicode(options.GetProperty("verb"));
    context.hide_console = Validator::IsFalse(options.GetProperty("console"));

    context.pathW = ra::unicode::Utf8ToUnicode(path);
    context.basedirW = ra::unicode::Utf8ToUnicode(basedir);
    context.argumentsW = ra::unicode::Utf8ToUnicode(arguments);

    HANDLE hProcess = NULL;
    DWORD pId = 0;

    const char* launch_api_function = "";

    // If a verb was specified...
    if (!verbW.empty())
    {
      // We must use ShellExecuteEx api
      launch_api_function = "ShellExecuteExW";

      // Start process with ShellExecute
      hProcess = StartProcessFromShellExecute(context, options);
    }
    else
    {
      // We use CreateProcess api
      launch_api_function = "CreateProcessW";

      // Start process with ShellExecute
      hProcess = StartProcessFromCreateProcess(context, options);
    }

    // inform the caller of the result on success
    bool success = (hProcess != NULL);
    if (success && result)
    {
      pId = GetProcessId(hProcess);
      result->pId = pId;
    }

    // Log a windows specific error in case of failure.
    if (!success)
    {
      DWORD dwLastError = ::GetLastError();
      std::string sErrorMessage = GetErrorMessageUtf8((uint32_t)dwLastError);

      SA_LOG(ERROR) << "Failed to call " << launch_api_function << "() for value '" << path << "', Error " << ToHexString(dwLastError) << ".Description: " << sErrorMessage << ".";
    }

    return success;
  }

  HANDLE WindowsProcessLauncherService::StartProcessFromShellExecute(const PROCESS_START_CONTEXT& context, PropertyStore& options) const
  {
    // options
    std::wstring verbW = ra::unicode::Utf8ToUnicode(options.GetProperty("verb"));

    HANDLE hProcess = NULL;

    SHELLEXECUTEINFOW info = { 0 };

    info.cbSize = sizeof(SHELLEXECUTEINFOW);

    info.fMask |= SEE_MASK_NOCLOSEPROCESS;
    info.fMask |= SEE_MASK_NOASYNC;
    info.fMask |= SEE_MASK_FLAG_DDEWAIT;
    //info.fMask |= SEE_MASK_NO_CONSOLE; Force child process to have its own console

    info.hwnd = HWND_DESKTOP;
    info.nShow = SW_SHOW;
    if (context.hide_console)
      info.nShow = SW_HIDE;

    info.lpFile = context.pathW.c_str();

    if (!verbW.empty())
      info.lpVerb = verbW.c_str(); // Verb
    if (!context.argumentsW.empty())
      info.lpParameters = context.argumentsW.c_str(); // Arguments
    if (!context.basedirW.empty())
      info.lpDirectory = context.basedirW.c_str(); // Default directory

    bool success = (ShellExecuteExW(&info) == TRUE);
    if (success)
      hProcess = info.hProcess;

    return hProcess;
  }

  HANDLE WindowsProcessLauncherService::StartProcessFromCreateProcess(const PROCESS_START_CONTEXT& context, PropertyStore& options) const
  {
    HANDLE hProcess = NULL;

    //build the full command line
    std::wstring commandW = context.pathW;
    if (context.pathW.find(L" ") != std::string::npos)
    {
      commandW.insert(0, 1, '\"');
      commandW.append(1, '\"');
    }

    if (!context.argumentsW.empty())
    {
      commandW.append(1, ' ');
      commandW += context.argumentsW;
    }

    //launch a new process with the command line
    PROCESS_INFORMATION pi = { 0 };
    STARTUPINFOW si = { 0 };
    si.cb = sizeof(STARTUPINFOW);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOW;
    DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP;
    if (context.hide_console)
    {
      si.wShowWindow = SW_HIDE;
      dwCreationFlags &= ~CREATE_NEW_CONSOLE;
      dwCreationFlags |= CREATE_NO_WINDOW;
    }
    bool success = (CreateProcessW(NULL, (wchar_t*)commandW.c_str(), NULL, NULL, FALSE, dwCreationFlags, NULL, context.basedirW.c_str(), &si, &pi) != 0);
    if (success)
    {
      hProcess = pi.hProcess;

      //Wait for the application to initialize properly
      WaitForInputIdle(hProcess, INFINITE);
    }

    return hProcess;
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
    info.nShow = SW_SHOW;
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

    // Log a windows specific error in case of failure.
    if (!success)
    {
      DWORD dwLastError = ::GetLastError();
      std::string sErrorMessage = GetErrorMessageUtf8((uint32_t)dwLastError);

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
