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
/// Ask for the user's confirmation before proceeding.
/// </summary>
/// <param name="hWnd">The parent window.</param>
/// <returns>Returns true when the user has agreed to proceed. Returns false otherwise.</returns>
bool GetUserConfirmation(HWND hWnd);
