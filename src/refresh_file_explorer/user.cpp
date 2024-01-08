#include <Windows.h>
#include "user.h"

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
  std::string path = ra::process::GetCurrentProcessPathUtf8();
  std::string filename = ra::filesystem::GetFilenameWithoutExtension(path.c_str());
  std::wstring filename_wide = ra::unicode::Utf8ToUnicode(filename);

  std::wstring title;
  title += filename_wide;
  title += L" ERROR!";

  HWND hWnd = GetForegroundWindow();
  MessageBoxW(hWnd, message.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
}

bool GetUserConfirmation(HWND hWnd)
{
  if (hWnd == NULL)
    hWnd = GetForegroundWindow();

  std::string path = ra::process::GetCurrentProcessPathUtf8();
  std::string filename = ra::filesystem::GetFilenameWithoutExtension(path.c_str());
  std::wstring filename_wide = ra::unicode::Utf8ToUnicode(filename);

  std::wstring title;
  title += filename_wide;

  std::wstring message;
  message +=
    L"The following application restart all File Explorer windows. "
    L"Windows will be closed and then reopened to the same location. "
    L"This restart cycle will release all unused DLL.";
  message += L"\n\n";
  message +=
    L"This is required since File Explorer do not automatically release Shell Extensions DLL that have been unregistered from the system. "
    L"A system reboot is usually required to release unregistered DLL.";
  message += L"\n\n";
  message += L"Do you want to continue?";

  int result = MessageBoxW(hWnd, message.c_str(), title.c_str(), MB_YESNO | MB_ICONQUESTION);
  if (result == IDYES)
    return true;
  return false;
}
