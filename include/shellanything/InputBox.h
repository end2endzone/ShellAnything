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

#include <Windows.h>
#include <string>

class CInputBox  
{
public:
  CInputBox(HWND hParent);
  virtual ~CInputBox();

  void setWidth(const int & width);
  const int & getWidth() const;
  void setHeight(const int & height);
  const int & getHeight() const;
  void setPromptFont(HFONT hFont);
  HFONT getPromptFont() const;
  void setWindowFont(HFONT hFont);
  HFONT getWindowFont() const;
  void setIcon(HICON hIcon);
  HICON getIcon() const;
  void setParentWindow(HWND hParentWindow);
  HWND getParentWindow() const;
  //void setWindow(HWND hWindow);
  HWND getWindow() const;
  //void setModuleHandle(HINSTANCE hInstance);
  HINSTANCE getModuleHandle() const;
  enum CONTROLS
  {
    TEXTBOX_ANSWER,
    BUTTON_OK,
    BUTTOK_CANCEL,
    LABEL_PROMPT,
  };
  void setCtrl(CONTROLS ctrl, HWND hWnd);
  HWND getCtrl(CONTROLS ctrl) const;
  void setText(const std::string & text);
  const std::string & getText() const;

  bool DoModal(const std::string & caption, const std::string & prompt);

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
  std::string m_Text;
};

#endif //INPUTBOX_H
