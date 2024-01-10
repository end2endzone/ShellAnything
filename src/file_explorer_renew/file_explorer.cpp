// https://learn.microsoft.com/en-us/cpp/cpp/how-to-create-and-use-ccomptr-and-ccomqiptr-instances?view=msvc-170
// https://forums.codeguru.com/showthread.php?345012-Access-violation-using-CComPtr-lt-gt

#include "file_explorer.h"
#include "user_feedback.h"

#include <objbase.h>
#include <comdef.h>
#include <atlbase.h>
#include <shlobj.h>
#include <exdisp.h>
#include <psapi.h> // For access to GetModuleFileNameEx

#include "rapidassist/undef_windows_macros.h"
#include "rapidassist/unicode.h"
#include "rapidassist/errors.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/environment_utf8.h"
#include "rapidassist/process_utf8.h"
#include "rapidassist/timing.h"

bool FindPath(const Utf8FileList& list, const std::string& path)
{
  for (size_t i = 0; i < list.size(); i++)
  {
    const std::string & element = list[i];
    if (path == element)
      return true;
  }
  return false;
}

bool GetFileExplorerWindowPaths(Utf8FileList& files)
{
  files.clear();

  HRESULT hr;
  hr = CoInitialize(NULL);
  if (FAILED(hr))
  {
    return false;
  }

  static const DWORD BUFFER_SIZE = 32 * 1024;
  static const DWORD BUFFER_LENGTH_IN_CHARACTERS = (BUFFER_SIZE / sizeof(wchar_t) - 1);
  wchar_t* szPath = new wchar_t[BUFFER_LENGTH_IN_CHARACTERS + 1];
  if (szPath == NULL)
  {
    CoUninitialize();
    return false;
  }

  CComPtr<IShellWindows> pShellWindows;
  hr = pShellWindows.CoCreateInstance(
    CLSID_ShellWindows,
    NULL,
    CLSCTX_ALL
  );
  if (FAILED(hr))
  {
    delete[] szPath;
    szPath = NULL;
    CoUninitialize();
    return false;
  }

  // Get Count of Windows...
  long nCount = 0;
  hr = pShellWindows->get_Count(&nCount);
  if (FAILED(hr))
  {
    delete[] szPath;
    szPath = NULL;
    CoUninitialize();
    return false;
  }

  // Browse shell windows one by one
  VARIANT index;
  V_VT(&index) = VT_I4;
  for(V_I4(&index) = 0; V_I4(&index) < nCount; V_I4(&index)++)
  {
    // Check for a shell window at index
    IDispatch* pDisp;
    hr = pShellWindows->Item(index, &pDisp);
    if (hr == S_OK) // Warning, do not check 'hr' with SUCCEEDED() macro since S_FALSE is considered a success.
    {
      // Handle IDispatch* graceful destruction
      CComPtr<IDispatch> pItem = pDisp;

      // Get IWebBrowserApp from Item
      CComPtr<IWebBrowserApp> pWebApp;
      hr = pItem->QueryInterface(IID_PPV_ARGS(&pWebApp));
      if (SUCCEEDED(hr))
      {
        // Get IServiceProvider from IWebBrowserApp
        CComPtr<IServiceProvider> pServiceProvider;
        hr = pWebApp->QueryInterface(IID_PPV_ARGS(&pServiceProvider));
        if (SUCCEEDED(hr))
        {
          // Get IShellBrowser from IServiceProvider
          CComPtr<IShellBrowser> pShellBrowser;
          hr = pServiceProvider->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&pShellBrowser));
          if (SUCCEEDED(hr))
          {
            IShellView* psv;
            hr = pShellBrowser->QueryActiveShellView(&psv);
            if (SUCCEEDED(hr))
            {
              // Handle IShellView* graceful destruction
              CComPtr<IShellView> pShellView = psv;

              // Get IFolderView from IShellView
              CComPtr<IFolderView> pFolderView;
              hr = pShellView->QueryInterface(IID_PPV_ARGS(&pFolderView));
              if (SUCCEEDED(hr))
              {
                IPersistFolder2* ppf2;
                hr = pFolderView->GetFolder(IID_IPersistFolder2, (void**)&ppf2);
                if (SUCCEEDED(hr))
                {
                  // Handle IPersistFolder2* graceful destruction
                  CComPtr<IPersistFolder2> pPersistFolder2 = ppf2;

                  LPITEMIDLIST pidlFolder;
                  hr = ppf2->GetCurFolder(&pidlFolder);
                  if (SUCCEEDED(hr))
                  {
                    szPath[0] = '\0';
                    if (SHGetPathFromIDListEx(pidlFolder, szPath, BUFFER_LENGTH_IN_CHARACTERS, GPFIDL_DEFAULT))
                    {
                      // Convert from WIDE to UTF8
                      std::string str_utf8 = ra::unicode::UnicodeToUtf8(szPath);
                      files.push_back(str_utf8);
                    }
                    else
                    {
                      //not a directory
                    }
                    CoTaskMemFree(pidlFolder);
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  delete[] szPath;
  szPath = NULL;
  CoUninitialize();

  return true;
}

bool OpenFileExplorerWindow(const std::string& path)
{
  // Find the absolute path of explorer.exe
  std::string explorer_path_utf8 = GetFileExplorerExecPath();
  if (explorer_path_utf8.empty())
    return false;
  std::wstring explorer_path_wide = ra::unicode::Utf8ToUnicode(explorer_path_utf8);
  const wchar_t * lpApplicationName = explorer_path_wide.c_str();

  // Convert the given path to wide string
  std::wstring path_wide = ra::unicode::Utf8ToUnicode(path);

  // Build a command line (which is including the path to the executable)
  std::wstring command_line;
  command_line += explorer_path_wide;

  // Add the given path as an argument
  // Wrap the path in double quotes to handle spaces.
  if (!path_wide.empty())
  {
    command_line += L" ";
    command_line += L"\"";
    command_line += path_wide;
    command_line += L"\"";
  }
  // And make a copy of the string for stupid WIN32 api...
  wchar_t * lpCommandLine = new wchar_t[command_line.size() + 1];
  StrCpyW(lpCommandLine, command_line.c_str());

  //launch a new process with the command line
  PROCESS_INFORMATION pi = { 0 };
  STARTUPINFOW si = { 0 };
  si.cb = sizeof(STARTUPINFO);
  si.dwFlags = STARTF_USESHOWWINDOW;
  // Note: Which wShowWindow flags allow creating casding windows ? DO NOT WORK: SW_SHOW, SW_SHOWNOACTIVATE, SW_SHOWNORMAL, SW_RESTORE
  si.wShowWindow = SW_SHOW;
  BOOL created = CreateProcessW(
    lpApplicationName,  // Using module name
    lpCommandLine,      // Command line
    NULL,               // Process handle not inheritable
    NULL,               // Thread handle not inheritable
    FALSE,              // Set handle inheritance to FALSE
    0,                  // No creation flags
    NULL,               // Use parent's environment block
    NULL,               // Use parent's starting directory 
    &si,                // Pointer to STARTUPINFO structure
    &pi                 // Pointer to PROCESS_INFORMATION structure
  );
  if (created != 0)
  {
    //Wait for the application to initialize properly
    WaitForInputIdle(pi.hProcess, INFINITE);

    //Extract the program id
    DWORD process_id = pi.dwProcessId;

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    delete[] lpCommandLine;
    return true;
  }

  delete[] lpCommandLine;
  return false;
}

void TestOpenFolderUnicode()
{
  // Test:
  const std::string test_path = "D:\\" "\xCE" "\xA8"; // psi
  //const std::string test_path = "C:\\temp";
  bool opened = OpenFileExplorerWindow(test_path);
  if (!opened)
  {
    ra::errors::errorcode_t error_code = ra::errors::GetLastErrorCode();
    std::string error_desc = ra::errors::GetErrorCodeDescription(error_code);

    std::wstring message;
    message += L"Failed to open directory:\n";
    message += ra::unicode::Utf8ToUnicode(test_path);
    message += L"\n";
    message += L"\n";
    message += ra::unicode::Utf8ToUnicode(error_desc);

    ShowErrorMessage(message);
  }
}

void PrintPathsToString(const Utf8FileList& paths, tstring_t& str)
{
  str.clear();

  for (size_t i = 0; i < paths.size(); i++)
  {
    const std::string& path_utf8 = paths[i];

    #ifdef UNICODE
    str += ra::unicode::Utf8ToUnicode(path_utf8);
    str += L"\r\n";
    #else
    str += ra::unicode::Utf8ToAnsi(path_utf8);
    str += "\r\n";
    #endif
  }
}

std::string GetFileExplorerExecPath()
{
  // Find the absolute path of explorer.exe
  std::string explorer_path_utf8 = ra::filesystem::FindFileFromPathsUtf8("explorer.exe");
  return explorer_path_utf8;
}

std::string GetProcessExecPathFromProcessId(DWORD pid)
{
  HANDLE hProcess = NULL;

  std::string output;

  static const DWORD BUFFER_SIZE = 32 * 1024;
  static const DWORD BUFFER_LENGTH_IN_CHARACTERS = (BUFFER_SIZE / sizeof(TCHAR) - 1);
  TCHAR* szPath = new TCHAR[BUFFER_LENGTH_IN_CHARACTERS + 1];
  if (szPath == NULL)
  {
    return false;
  }

  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
  if (hProcess != NULL)
  {
    if (GetModuleFileNameEx(hProcess, NULL, szPath, BUFFER_LENGTH_IN_CHARACTERS) != 0)
    {
      #ifdef UNICODE
      output = ra::unicode::UnicodeToUtf8(szPath);
      #else
      output = ra::unicode::AnsiToUtf8(szPath);
      #endif
    }

    CloseHandle(hProcess);
  }

  delete[] szPath;
  szPath = NULL;
  return output;
}

ra::process::ProcessIdList GetFileExplorerProcessIds()
{
  ra::process::ProcessIdList pids;

  std::string explorer_path_utf8 = GetFileExplorerExecPath();
  if (explorer_path_utf8.empty())
    return pids;  // error.

  ra::process::ProcessIdList system_process_ids = ra::process::GetProcesses();
  if (system_process_ids.empty())
    return pids;  // error.

  // For each process
  for (size_t i = 0; i < system_process_ids.size(); i++)
  {
    const ra::process::processid_t system_pid = system_process_ids[i];

    // Does this pid match File Explorer?
    std::string exec_path = GetProcessExecPathFromProcessId(system_pid);
    if (explorer_path_utf8 == exec_path)
    {
      // Match
      pids.push_back(system_pid);
    }
  }

  return pids;
}

bool KillFileExplorerProcesses()
{
  std::string explorer_path_utf8 = GetFileExplorerExecPath();
  if (explorer_path_utf8.empty())
    return false;

  ra::process::ProcessIdList process_ids = GetFileExplorerProcessIds();
  if (process_ids.empty())
    return true; // nothing to do

  // For each process
  for (size_t i = 0; i < process_ids.size(); i++)
  {
    const ra::process::processid_t pid = process_ids[i];
    bool killed = ra::process::Kill(pid);
    if (!killed)
      return false;
  }

  // Confirm that all expected processes were killed.
  // Allow time to each killed process to disaprear from the process list.
  static const uint64_t TIMEOUT_TIME_MS = 30000;
  uint64_t timer_start = ra::timing::GetMillisecondsCounterU64();
  uint64_t timer_elapsed_ms = 0;
  bool success = false;
  while (!success && timer_elapsed_ms < TIMEOUT_TIME_MS)
  {
    process_ids = GetFileExplorerProcessIds();
    success = (process_ids.empty());

    if (!success)
    {
      // Wait a little more and then check again...
      Sleep(1000);
      uint64_t timer_now = ra::timing::GetMillisecondsCounterU64();
      timer_elapsed_ms = timer_now - timer_start;
    }
  }
  if (success == false)
  {
    int a = 0;
  }
  return success;
}

bool FindMissingElements(const Utf8FileList& previous, Utf8FileList& output)
{
  output.clear();

  // Check again the path of each File Explorer windows
  Utf8FileList current;
  bool success = GetFileExplorerWindowPaths(current);
  if (!success)
    return false;

  // Remove from the missing list every path that we succesfully restored.
  for (size_t i = 0; i < previous.size(); i++)
  {
    const std::string previous_path = previous[i];

    // Is t still missing ?
    bool found = FindPath(previous, previous_path);

    if (!found)
    {
      // Still missing...
      output.push_back(previous_path);
    }
  }

  // Debugging and testing...
  // Force simulate a last missing path.
  //output.clear();
  //output.push_back("foobar");

  return true;
}