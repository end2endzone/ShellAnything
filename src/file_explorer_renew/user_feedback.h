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
