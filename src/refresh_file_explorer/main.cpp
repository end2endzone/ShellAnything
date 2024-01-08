// main.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "main.h"
#include "user.h"
#include "file_explorer.h"

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <tchar.h>

// Global variables

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("File Explorer cycle manager");

// Stored instance handle for use in Win32 API calls such as FindResource
HINSTANCE hInst;

// Default text to show in the window
tstring_t window_text;

// Store the list of initial in a string
Utf8FileList paths_intial;

static const DWORD WM_USER_APP_INITIALIZE = WM_USER + 1;

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
    return 1;
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
    return 1;
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

  return (int)msg.wParam;
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
    {
      // Get user prompt confirmation
      bool confirmed = GetUserConfirmation(hWnd);
      if (!confirmed)
        PostMessage(hWnd, WM_QUIT, 0, 0);

      // Get all the paths from File Explorer windows
      paths_intial.clear();
      bool success = GetFileExplorerWindowPaths(paths_intial);
      if (!success)
      {
        std::wstring message;
        message += L"ERROR!\n";
        message += L"Failed getting paths from File Explorer windows.";
        ShowErrorMessage(message);
        PostMessage(hWnd, WM_QUIT, 0, 0);
      }

      // Print paths to a string
      PrintPathsToString(paths_intial, window_text);

      // Add text to the window.
      SendMessage(hWndEdit, WM_SETTEXT, 0, (LPARAM)window_text.c_str());

      // Continue with next messages
      return DefWindowProc(hWnd, message, wParam, lParam);
    }

  case WM_COMMAND:
    switch ( wParam )
    {
      case IDM_ABOUT:
        DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
        break;
      case IDM_EXIT:
        DestroyWindow(hWnd);
        break;

    /*case IDM_EDUNDO:
      // Send WM_UNDO only if there is something to be undone. 

      if ( SendMessage(hWndEdit, EM_CANUNDO, 0, 0) )
        SendMessage(hWndEdit, WM_UNDO, 0, 0);
      else
      {
        MessageBox(hWndEdit,
                   L"Nothing to undo.",
                   L"Undo notification",
                   MB_OK);
      }
      break;

    case IDM_EDCUT:
      SendMessage(hWndEdit, WM_CUT, 0, 0);
      break;

    case IDM_EDCOPY:
      SendMessage(hWndEdit, WM_COPY, 0, 0);
      break;

    case IDM_EDPASTE:
      SendMessage(hWndEdit, WM_PASTE, 0, 0);
      break;

    case IDM_EDDEL:
      SendMessage(hWndEdit, WM_CLEAR, 0, 0);
      break;
  */

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;

  case WM_SETFOCUS:
    SetFocus(hWndEdit);
    return 0;

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

    return 0;

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
