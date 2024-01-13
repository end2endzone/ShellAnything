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

#include "framework.h"
#include "main.h"
#include "user_feedback.h"
#include "file_explorer.h"

#include "rapidassist/unicode.h"

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <tchar.h>

// Global variables

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("File Explorer renew");

// Stored instance handle for use in Win32 API calls such as FindResource
HINSTANCE hInst;

// Default text to show in the window
tstring_t window_text;

// Store the list of initial in a string
Utf8FileList paths_backup;
Utf8FileList paths_missing;

// Custom messages
static const DWORD WM_USER_APP_INITIALIZE           = (WM_USER + 0x001);
static const DWORD WM_USER_KILL_FILE_EXPLORER       = (WM_USER + 0x002);
static const DWORD WM_USER_RESTORE_FILE_EXPLORER    = (WM_USER + 0x003);

enum APP_EXIT_CODES
{
  EXIT_SUCCESS_NO_ERROR,
  EXIT_USER_STOP_REQUEST,
  EXIT_REGISTER_CLASS_FAILED,
  EXIT_WINDOW_CREATE_FAILED,
  EXIT_GET_FILE_EXPLORER_PATHS_FAILED,
  EXIT_PROCESS_KILL_FAILED,
};

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK   About(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
   _In_ HINSTANCE hInstance,
   _In_opt_ HINSTANCE hPrevInstance,
   _In_ LPSTR     lpCmdLine,
   _In_ int       nCmdShow
)
{
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYAPP_ICON));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MYAPP);
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYAPP_ICON));

  if ( !RegisterClassEx(&wcex) )
  {
    ShowErrorMessage(L"Window registration has failed!");
    return EXIT_REGISTER_CLASS_FAILED;
  }

  // Store instance handle in our global variable
  hInst = hInstance;

  // The parameters to CreateWindowEx explained:
  // WS_EX_OVERLAPPEDWINDOW : An optional extended window style.
  // szWindowClass: the name of the application
  // szTitle: the text that appears in the title bar
  // WS_OVERLAPPEDWINDOW: the type of window to create
  // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
  // 800, 600: initial size (width, length)
  // NULL: the parent of this window
  // NULL: this application does not have a menu bar
  // hInstance: the first parameter from WinMain
  // NULL: not used in this application
  HWND hWnd = CreateWindowEx(
     WS_EX_OVERLAPPEDWINDOW,
     szWindowClass,
     szTitle,
     WS_OVERLAPPEDWINDOW,
     CW_USEDEFAULT, CW_USEDEFAULT,
     800, 600,
     NULL,
     NULL,
     hInstance,
     NULL
  );

  if ( !hWnd )
  {
    ShowErrorMessage(L"Window creation has failed!");
    return EXIT_WINDOW_CREATE_FAILED;
  }

  // The parameters to ShowWindow explained:
  // hWnd: the value returned from CreateWindow
  // nCmdShow: the fourth parameter from WinMain
  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  // Tell our application it has to initilize
  SendMessage(hWnd, WM_USER_APP_INITIALIZE, 0, 0);

  // Main message loop:
  HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYAPP));
  MSG msg;
  // Main message loop:
  while (GetMessage(&msg, nullptr, 0, 0))
  {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  int exit_code = (int)msg.wParam;
  return exit_code;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  static HWND hWndEdit = NULL;
  static HFONT hEditFont = NULL;
  static UINT_PTR uCleanupIDEvent = NULL;

  bool killed = false;
  bool confirmed = false;
  bool success = false;
  bool opened = false;

  switch ( message )
  {
  case WM_CREATE:
    hWndEdit = CreateWindowEx(
                0, L"EDIT",   // predefined class 
                NULL,         // no window title 
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
                ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
                0, 0, 0, 0,   // set size in WM_SIZE message 
                hWnd,         // parent window 
                (HMENU)ID_EDITCHILD,   // edit control ID 
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL);        // pointer not needed 

    // Add text to the window.
    SendMessage(hWndEdit, WM_SETTEXT, 0, (LPARAM)window_text.c_str());
    // And force EDIT window to redraw
    SendMessage(hWndEdit, WM_PAINT, 0, 0);
    InvalidateRect(hWndEdit, NULL, true);
    UpdateWindow(hWndEdit);

    // Create font for edit label
    {
      //https://stackoverflow.com/questions/221411/how-can-i-specify-a-font-for-a-window-created-through-createwindow
      LOGFONT lf = { 0 };
      lstrcpy(lf.lfFaceName, L"Consolas"); // same as notepad
      lf.lfHeight = 18;
      lf.lfWeight = FW_REGULAR;
      lf.lfItalic = FALSE;
      lf.lfCharSet = DEFAULT_CHARSET;
      lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
      lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
      lf.lfQuality = CLEARTYPE_QUALITY; //ANTIALIASED_QUALITY; //DEFAULT_QUALITY;
      lf.lfPitchAndFamily = DEFAULT_PITCH;
      hEditFont = CreateFontIndirect(&lf);

      //hEditFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

      SendMessage(hWndEdit, WM_SETFONT, (WPARAM)hEditFont, 0);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);

  //case WM_ACTIVATE:
  //  return DefWindowProc(hWnd, message, wParam, lParam);
  //case WM_ACTIVATEAPP:
  //  return DefWindowProc(hWnd, message, wParam, lParam);

  case WM_USER_APP_INITIALIZE:
    // Get user prompt confirmation
    confirmed = GetUserConfirmation(hWnd);
    if (!confirmed)
    {
      PostQuitMessage(EXIT_USER_STOP_REQUEST);
      return 0;
    }

    // Backup the path of each File Explorer windows
    paths_backup.clear();
    success = GetFileExplorerWindowPaths(paths_backup);
    if (!success)
    {
      std::wstring str;
      str += L"Failed getting paths from File Explorer windows.";
      ShowErrorMessage(hWnd, str);
      PostQuitMessage(EXIT_GET_FILE_EXPLORER_PATHS_FAILED);
      return 0;
    }

    // Print paths to a string
    PrintPathsToString(paths_backup, window_text);

    // Add text to the window.
    SendMessage(hWndEdit, WM_SETTEXT, 0, (LPARAM)window_text.c_str());
    // And force EDIT window to redraw
    SendMessage(hWndEdit, WM_PAINT, 0, 0);
    InvalidateRect(hWndEdit, NULL, true);
    UpdateWindow(hWndEdit);

    if (paths_backup.empty())
    {
      std::wstring str;
      str += L"There is no File Explorer windows opened.";
      ShowErrorMessage(hWnd, str);
      PostQuitMessage(EXIT_SUCCESS_NO_ERROR);
      return 0;
    }

    // Continue with next step
    SendMessage(hWnd, WM_USER_KILL_FILE_EXPLORER, 0, 0);
    break;
  case WM_USER_KILL_FILE_EXPLORER:
    killed = KillFileExplorerProcesses();
    if (!killed)
    {
      std::wstring str;
      str += L"Failed to kill all File Explorer processes.";
      ShowErrorMessage(hWnd, str);
      PostQuitMessage(EXIT_PROCESS_KILL_FAILED);
      return 0;
    }

    // Start a default File Explorer instance (for desktop and taskbar handling)
    opened = OpenFileExplorerWindow("");
    if (!opened)
    {
      std::wstring str;
      str += L"Failed to a default File Explorer instance.";
      ShowErrorMessage(hWnd, str);
      ShowErrorMessage(hWnd, str);
      PostQuitMessage(EXIT_PROCESS_KILL_FAILED);
      return 0;
    }
    Sleep(500);

    // Continue with next step
    SendMessage(hWnd, WM_USER_RESTORE_FILE_EXPLORER, 0, 0);
    break;
  case WM_USER_RESTORE_FILE_EXPLORER:
    for (size_t i = 0; i < paths_backup.size(); i++)
    {
      const std::string& path_utf8 = paths_backup[i];
      opened = OpenFileExplorerWindow(path_utf8);
      if (!opened)
      {
        std::wstring str;
        str += L"Failed to open directory '";
        str += ra::unicode::Utf8ToUnicode(path_utf8);
        str += L"'. Press OK to continue.";
        ShowErrorMessage(hWnd, str);
      }
    }

    // Remember which paths we need to restore.
    paths_missing = paths_backup;

    // Start a timer to check every second
    uCleanupIDEvent = SetTimer(hWnd, IDT_TIMER1, 1000, (TIMERPROC)NULL);
    break;
  case WM_TIMER:
    switch (wParam)
    {
    case IDT_TIMER1:
      // Prevent triggering the timer while handling the time event.
      KillTimer(hWnd, uCleanupIDEvent);

      // Check again the path of each File Explorer windows
      Utf8FileList tmp;
      success = FindMissingElements(paths_missing, tmp);
      if (!success)
      {
        std::wstring str;
        str += L"Failed getting paths from File Explorer windows.";
        ShowErrorMessage(hWnd, str);
        PostQuitMessage(EXIT_GET_FILE_EXPLORER_PATHS_FAILED);
        return 0;
      }

      bool has_changed = (paths_missing != tmp);
      paths_missing = tmp;

      if (has_changed)
      {
        // Print paths to a string
        PrintPathsToString(paths_missing, window_text);

        // Add text to the window.
        SendMessage(hWndEdit, WM_SETTEXT, 0, (LPARAM)window_text.c_str());
        // And force EDIT window to redraw
        SendMessage(hWndEdit, WM_PAINT, 0, 0);
        InvalidateRect(hWndEdit, NULL, true);
        UpdateWindow(hWndEdit);
      }

      // Did we restored all previous windows ?
      if (paths_missing.empty())
      {
        std::wstring str;
        str += L"All File Explorer windows has been restored.\n"
          L"Press OK to close the application.";
        ShowSuccessMessage(hWnd, str);
        PostQuitMessage(EXIT_SUCCESS_NO_ERROR);
        return 0;
      }

      // No? Wait then to look again...
      uCleanupIDEvent = SetTimer(hWnd, IDT_TIMER1, 1000, (TIMERPROC)NULL);
      break;
    };
    break;
  case WM_COMMAND:
    switch ( wParam )
    {
      case IDM_ABOUT:
        DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
        break;
      case IDM_EXIT:
        DestroyWindow(hWnd);
        break;
      default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;
  case WM_SETFOCUS:
    SetFocus(hWndEdit);
    break;

  case WM_SIZE:
    // Make the edit control the size of the window's client area.
    MoveWindow(hWndEdit,
               0, 0,                  // starting x- and y-coordinates 
               LOWORD(lParam),        // width of client area 
               HIWORD(lParam),        // height of client area 
               TRUE);                 // repaint window

    // Set 2px padding
    RECT rc;
    GetClientRect(hWnd, &rc);
    rc.left += 10; // increase the left margin
    rc.top += 10; // increase the top margin
    rc.right -=   GetSystemMetrics(SM_CXVSCROLL); // decrease the right margin
    rc.bottom -=  GetSystemMetrics(SM_CYHSCROLL); // decrease the bottom margin (rectangle)
    // Set the rectangle
    SendMessage(hWndEdit, EM_SETRECT, 0, (LPARAM)&rc);

    break;
  case WM_DESTROY:
    DeleteObject(hEditFont);
    hEditFont = NULL;

    PostQuitMessage(0);
    break;

  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
    break;
  }

  return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  UNREFERENCED_PARAMETER(lParam);
  switch (message)
  {
  case WM_INITDIALOG:
    return (INT_PTR)TRUE;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
    {
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }
    break;
  }
  return (INT_PTR)FALSE;
}
