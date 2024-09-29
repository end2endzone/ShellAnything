#include "framework.h"
#include "resource.h"
#include "arguments.h"

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <tchar.h>
#include <Richedit.h>


// Debug arguments example:
//   This is "multiple arguments at once" including "utf-8 characters" such as the Greek alphabet Αα Ββ Γγ Δδ Εε Ζζ Ηη Θθ Ιι Κκ Λλ Μμ Νν Ξξ Οο Ππ Ρρ Σσ/ς Ττ Υυ Φφ Χχ Ψψ Ωω.


// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("ShellAything Arguments Debugging Application");

// Stored instance handle for use in Win32 API calls such as FindResource
HINSTANCE hInst;

// Store the list of arguments in a string
tstring_t window_text;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HFONT CreateEditBoxFont(HWND hWnd);


// https://stackoverflow.com/questions/33836706/what-are-tchar-strings-and-the-a-or-w-version-of-win32-api-functions
#ifdef UNICODE
int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
)
#else
int WINAPI WinMain(
   _In_ HINSTANCE hInstance,
   _In_opt_ HINSTANCE hPrevInstance,
   _In_ LPSTR     lpCmdLine,
   _In_ int       nShowCmd
)
#endif
{
  // Get current directory
  TCHAR curdir[MAX_PATH] = { 0 };
  GetCurrentDirectory(MAX_PATH, curdir);
  window_text += _T("Current directory: ");
  window_text += curdir;
  window_text += _T("\r\n");

  tstring_t args_text;
  ReadCommandLineArguments(args_text);

  window_text += args_text;
  window_text += _T("\r\n");

  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

  if ( !RegisterClassEx(&wcex) )
  {
    MessageBox(NULL,
      _T("Call to RegisterClassEx failed!"),
      szTitle,
      NULL);

    return 1;
  }

  // Store instance handle in our global variable
  hInst = hInstance;

  HWND hWnd = CreateWindowEx(
     WS_EX_OVERLAPPEDWINDOW,
     szWindowClass,
     szTitle,
     WS_OVERLAPPEDWINDOW,
     CW_USEDEFAULT, CW_USEDEFAULT,
     500, 500,
     NULL,
     NULL,
     hInstance,
     NULL
  );

  if ( !hWnd )
  {
    MessageBox(NULL,
      _T("Call to CreateWindow failed!"),
      szTitle,
      NULL);

    return 1;
  }

  ShowWindow(hWnd, nShowCmd);
  UpdateWindow(hWnd);

  // Main message loop:
  MSG msg;
  while ( GetMessage(&msg, NULL, 0, 0) )
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return (int)msg.wParam;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  static HWND hWndEdit = NULL;
  static HFONT hEditFont = NULL;

  HDC hDC = NULL;
  HRGN hrgnUpdate = 0;

  switch ( message )
  {
  case WM_CREATE:
    hWndEdit = CreateWindowEx(
                0, _T("EDIT"),// predefined class 
                NULL,         // no window title 
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
                ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
                0, 0, 0, 0,   // set size in WM_SIZE message 
                hWnd,         // parent window 
                (HMENU)ID_EDITCHILD,   // edit control ID 
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL);        // pointer not needed 

    // Set Edit Control window readonly
    SendMessage(hWndEdit, EM_SETREADONLY, TRUE, NULL);

    // Set Edit Control window font
    hEditFont = CreateEditBoxFont(hWndEdit);
    SendMessage(hWndEdit, WM_SETFONT, (WPARAM)hEditFont, 0);

    // Set Edit Control window text. 
    SendMessage(hWndEdit, WM_SETTEXT, 0, (LPARAM)window_text.c_str());

    return DefWindowProc(hWnd, message, wParam, lParam);

  case WM_CTLCOLOREDIT:
  case WM_CTLCOLORSTATIC:
  {
    // Set the foreground and background color for the EDIT control.

    HDC hdcStatic = (HDC)wParam; // handle to display context
    HWND hwndStatic = (HWND)lParam; // handle to control window
    if (hwndStatic != hWndEdit)
      return DefWindowProc(hWnd, message, wParam, lParam);

    SetTextColor(hdcStatic, RGB(0, 0, 0));
    SetBkColor(hdcStatic, RGB(255, 255, 255));

    // https://learn.microsoft.com/en-us/windows/win32/controls/wm-ctlcolorstatic
    // If the application returns a brush that it created (for example, by using the CreateSolidBrush or CreateBrushIndirect function),
    // the application must free the brush. If the application returns a system brush (for example, one that was retrieved by the
    // GetStockObject() or GetSysColorBrush() function), the application does not need to free the brush.
    return (INT_PTR)GetStockObject(WHITE_BRUSH);
  }

  // https://learn.microsoft.com/en-us/windows/win32/controls/use-a-multiline-edit-control
  // User edit commands are sent to the window process via WM_COMMAND notification messages.
  //case WM_COMMAND:
  //  switch ( wParam )
  //  {
  //  case IDM_EDUNDO:
  //    // ...
  //  case IDM_EDCUT:
  //    // ...
  //  case IDM_EDCOPY:
  //    // ...
  //  case IDM_EDPASTE:
  //    // ...
  //  case IDM_EDDEL:
  //    // ...
  //  default:
  //    return DefWindowProc(hwnd, message, wParam, lParam);
  //  }
  //  break;

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

    // Set 2px padding inside the edit box
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
    //destroy edit font
    DeleteObject(hEditFont);

    PostQuitMessage(0);
    break;

  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
    break;
  }

  return 0;
}

HFONT CreateEditBoxFont(HWND hWnd)
{
  //https://stackoverflow.com/questions/221411/how-can-i-specify-a-font-for-a-window-created-through-createwindow

  //List of font to explorer for the prompt and/or the textbox:
  //  Georgia
  //  Microsoft Sans Serif
  //  Segoe UI
  //  Tahoma

  //Get default system font
  //HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

  static const int DotsPerInch = 72;
  HDC hDC = GetDC(hWnd);

  //Manual font creation from LOGFONT struct
  LOGFONT lfont = {0};
  lstrcpy(lfont.lfFaceName, _T("Consolas"));
  lfont.lfHeight = -MulDiv(11, GetDeviceCaps(hDC, LOGPIXELSY), DotsPerInch);
  lfont.lfWeight = FW_NORMAL;
  lfont.lfItalic = FALSE;
  lfont.lfCharSet = DEFAULT_CHARSET;
  lfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
  lfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  lfont.lfQuality = DEFAULT_QUALITY;
  lfont.lfPitchAndFamily = DEFAULT_PITCH;
  HFONT hFont = CreateFontIndirect(&lfont);

  ReleaseDC(hWnd, hDC);

  return hFont;
}
