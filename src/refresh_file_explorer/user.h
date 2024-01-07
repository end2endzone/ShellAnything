#pragma once

#include <string>

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
/// <returns>Returns true when the user has agreed to proceed. Returns false otherwise.</returns>
bool GetUserConfirmation();
