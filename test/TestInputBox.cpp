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

#include "TestInputBox.h"
#include "InputBox.h"

#include "rapidassist/testing.h"
#include "rapidassist/unicode.h"

namespace shellanything { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestInputBox::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestInputBox::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestInputBox, DISABLED_testTextAnsi)
  {
    HWND hWnd = GetDesktopWindow();

    std::string test_name = ra::testing::GetTestQualifiedName();
    std::string hello_text = std::string("hello world from ") + test_name;
    std::string caption = std::string("caption: ") + hello_text;
    std::string prompt = std::string("prompt: ") + hello_text;

    CInputBox box(hWnd);
    bool ok = box.DoModal(caption, prompt);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestInputBox, DISABLED_testTextUnicode)
  {
    HWND hWnd = GetDesktopWindow();

    static const wchar_t COPYRIGHT_SIGN = 0x00A9;
    static const wchar_t CENT_SIGN = 0x00A2;
    static const wchar_t MICRO_SIGN = 0x00B5;
    static const wchar_t REGISTERED_SIGN = 0x00AE;

    std::string test_name = ra::testing::GetTestQualifiedName();
    std::wstring hello_text = std::wstring(L"hello world from ") + ra::unicode::AnsiToUnicode(test_name);

    hello_text.insert(0, 1, COPYRIGHT_SIGN);
    hello_text.append(1, L' ');
    hello_text.append(1, MICRO_SIGN);
    hello_text.append(1, CENT_SIGN);
    hello_text.append(1, REGISTERED_SIGN);

    std::wstring caption = std::wstring(L"caption: ") + hello_text;
    std::wstring prompt = std::wstring(L"prompt: ") + hello_text;

    CInputBox box(hWnd);
    bool ok = box.DoModal(caption, prompt);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestInputBox, DISABLED_testDefaultTextUnicode)
  {
    HWND hWnd = GetDesktopWindow();
 
    std::string test_name = ra::testing::GetTestQualifiedName();
    std::wstring hello_text = std::wstring(L"hello world from ") + ra::unicode::AnsiToUnicode(test_name);
 
    std::wstring caption = std::wstring(L"caption: ") + hello_text;
    std::wstring prompt = std::wstring(L"prompt: ") + hello_text;
 
    CInputBox box(hWnd);
 
    box.SetTextAnsi("my default text");
 
    bool ok = box.DoModal(caption, prompt);
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
