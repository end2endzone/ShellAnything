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

#ifndef INPUTBOX_H
#define INPUTBOX_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include <Windows.h>
#include <string>

class SHELLANYTHING_EXPORT CInputBox
{
public:
  /// <summary>
  /// Creates a new InputBox.
  /// </summary>
  /// <param name="hParent">The parent windows of the InputBox. Can be NULL.</param>
  CInputBox(HWND hParent);
  virtual ~CInputBox();

  private:
  // Disable copy constructor and copy operator
  CInputBox(const CInputBox&);
  CInputBox& operator=(const CInputBox&);
  public:

  void SetWidth(const int & width);
  const int & GetWidth() const;
  void SetHeight(const int & height);
  const int & GetHeight() const;
  void SetPromptFont(HFONT hFont);
  HFONT GetPromptFont() const;
  void SetWindowFont(HFONT hFont);
  HFONT GetWindowFont() const;
  void SetIcon(HICON hIcon);
  HICON GetIcon() const;
  void SetParentWindow(HWND hParentWindow);
  HWND GetParentWindow() const;
  //void SetWindow(HWND hWindow);
  HWND GetWindow() const;
  //void SetModuleHandle(HINSTANCE hInstance);
  HINSTANCE GetInputBoxModuleHandle() const;
  enum CONTROLS
  {
    TEXTBOX_ANSWER,
    BUTTON_OK,
    BUTTON_CANCEL,
    LABEL_PROMPT,
  };
  void SetCtrl(CONTROLS ctrl, HWND hWnd);
  HWND GetCtrl(CONTROLS ctrl) const;

  /// <summary>
  /// Set the default text from an ansi string.
  /// </summary>
  /// <param name="text">The new text to display</param>
  void SetTextAnsi(const std::string & text);

  /// <summary>
  /// Get the default text as an ansi string.
  /// </summary>
  std::string GetTextAnsi() const;

  /// <summary>
  /// Set the default text from an unicode string.
  /// </summary>
  /// <param name="text">The new text to display</param>
  void SetTextUnicode(const std::wstring & text);

  /// <summary>
  /// Get the default text as an unicode string.
  /// </summary>
  std::wstring GetTextUnicode() const;

  /// <summary>
  /// Display the input box to the user and wait for the user to click the OK or CANCEL buttons.
  /// </summary>
  /// <param name="caption">The caption of the window.</param>
  /// <param name="prompt">The prompt question displayed to the user.</param>
  /// <returns>Returns true if the user have clicked OK button. Returns false otherwise.</returns>
  bool DoModal(const std::string  & caption, const std::string  & prompt);

  /// <summary>
  /// Display the input box to the user and wait for the user to click the OK or CANCEL buttons.
  /// </summary>
  /// <param name="caption">The caption of the window.</param>
  /// <param name="prompt">The prompt question displayed to the user.</param>
  /// <returns>Returns true if the user have clicked OK button. Returns false otherwise.</returns>
  bool DoModal(const std::wstring & caption, const std::wstring & prompt);

private:
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  BOOL UpdateWindowStyle(HWND hWnd, LONG new_styles);

  HINSTANCE m_hInstance;
  HFONT m_hWindowFont; //main font
  HFONT m_hPromptFont;
  HICON m_hIcon;
  HWND m_hParent;
  HWND m_hInputBox;
  HWND m_hTextBoxAnswer;
  HWND m_hButtonOK;
  HWND m_hButtonCancel;
  HWND m_hLabelPrompt;
  int m_Width;
  int m_Height;
  std::wstring m_Text;
};

#endif //INPUTBOX_H
