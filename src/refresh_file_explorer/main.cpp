#include <windows.h>
#include "file_explorer.h"
#include "user.h"

#include "rapidassist/process_utf8.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/unicode.h"
#include "rapidassist/errors.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
  // Get prompt confirmation
  bool confirmed = GetUserConfirmation();
  if (!confirmed)
    return 1;

  TestOpenFolderUnicode();
  return 9;

  // Get all the paths from File Explorer windows
  Utf8FileList files;
  bool success = GetFileExplorerWindowPaths(files);
  if (!success)
  {
    std::wstring message;
    message += L"ERROR!\n";
    message += L"Failed getting paths from File Explorer windows.";
    ShowErrorMessage(message);
    return 2;
  }


  return 0;
}
