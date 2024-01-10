#pragma once

#include <Windows.h>
#include <string>

#ifdef UNICODE
typedef std::wstring tstring_t;
#else
typedef std::string tstring_t;
#endif

/// <summary>
/// Show an error message to the user.
/// </summary>
void ShowErrorMessageUtf8(const std::string& message);

/// <summary>
/// Show an error message to the user.
/// </summary>
void ShowErrorMessage(const std::wstring& message);

/// <summary>
/// Show an error message to the user.
/// </summary>
/// <param name="hWnd">The parent window.</param>
void ShowErrorMessage(HWND hWnd, const std::wstring& message);

/// <summary>
/// Show an generic operation successful message to the user.
/// </summary>
/// <param name="hWnd">The parent window.</param>
void ShowSuccessMessage(HWND hWnd, const std::wstring& message);

/// <summary>
/// Ask for the user's confirmation before proceeding.
/// </summary>
/// <param name="hWnd">The parent window.</param>
/// <returns>Returns true when the user has agreed to proceed. Returns false otherwise.</returns>
bool GetUserConfirmation(HWND hWnd);

/// <summary>
/// Show a MessageBox centered on parent window.
/// </summary>
int MessageBoxCentered(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
