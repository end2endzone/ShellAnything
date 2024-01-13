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

#include <Windows.h>
#include "user_feedback.h"

#include "rapidassist/process_utf8.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/unicode.h"
#include "rapidassist/errors.h"

void ShowErrorMessageUtf8(const std::string& message)
{
  std::wstring message_wide = ra::unicode::Utf8ToUnicode(message);
  ShowErrorMessage(message_wide);
}

void ShowErrorMessage(const std::wstring& message)
{
  HWND hWnd = GetForegroundWindow();
  ShowErrorMessage(hWnd, message);
}

void ShowErrorMessage(HWND hWnd, const std::wstring& message)
{
  std::string path = ra::process::GetCurrentProcessPathUtf8();
  std::string filename = ra::filesystem::GetFilenameWithoutExtension(path.c_str());
  std::wstring filename_wide = ra::unicode::Utf8ToUnicode(filename);

  std::wstring title;
  title += filename_wide;
  title += L" ERROR!";

  MessageBoxCentered(hWnd, message.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
}

void ShowSuccessMessage(HWND hWnd, const std::wstring& message)
{
  MessageBoxCentered(hWnd, message.c_str(), L"Operation succesful", MB_OK | MB_ICONINFORMATION);
}

bool GetUserConfirmation(HWND hWnd)
{
  if (hWnd == NULL)
    hWnd = GetForegroundWindow();

  std::wstring title;
  title += L"User confirmation";

  std::wstring message;
  message +=
    L"The following application restart all File Explorer windows. "
    L"Windows will be closed and then reopened to the same location. "
    L"This restart cycle will release all unused DLL.";
  message += L"\n\n";
  message +=
    L"This is required because File Explorer do not automatically release Shell Extensions DLL that have been unregistered from the system. "
    L"A system reboot is usually required to release unregistered DLL. ";
    L"With a restard cycle, shell extension installers will usually skip the reboot step.";
  message += L"\n\n";
  message += L"Do you want to continue?";

  int result = MessageBoxCentered(hWnd, message.c_str(), title.c_str(), MB_YESNO | MB_ICONQUESTION);
  if (result == IDYES)
    return true;
  return false;
}

// https://stackoverflow.com/questions/6299797/c-how-to-center-messagebox/6322616#55700905
int MessageBoxCentered(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
  // Center message box at its parent window
  static HHOOK hHookCBT{};
  hHookCBT = SetWindowsHookEx(WH_CBT,
                              [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT
  {
    if (nCode == HCBT_CREATEWND)
    {
      if (((LPCBT_CREATEWND)lParam)->lpcs->lpszClass == (LPWSTR)(ATOM)32770)  // #32770 = dialog box class
      {
        RECT rcParent{};
        GetWindowRect(((LPCBT_CREATEWND)lParam)->lpcs->hwndParent, &rcParent);
        ((LPCBT_CREATEWND)lParam)->lpcs->x = rcParent.left + ((rcParent.right - rcParent.left) - ((LPCBT_CREATEWND)lParam)->lpcs->cx) / 2;
        ((LPCBT_CREATEWND)lParam)->lpcs->y = rcParent.top + ((rcParent.bottom - rcParent.top) - ((LPCBT_CREATEWND)lParam)->lpcs->cy) / 2;
      }
    }

    return CallNextHookEx(hHookCBT, nCode, wParam, lParam);
  },
                              0, GetCurrentThreadId());

  int iRet{ MessageBox(hWnd, lpText, lpCaption, uType) };

  UnhookWindowsHookEx(hHookCBT);

  return iRet;
}
