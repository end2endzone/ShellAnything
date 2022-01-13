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

#include "ActionPrompt.h"
#include "PropertyManager.h"
#include "InputBox.h"
#include "rapidassist/strings.h"
#include "rapidassist/unicode.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

namespace shellanything
{

  ActionPrompt::ActionPrompt()
  {
  }

  ActionPrompt::~ActionPrompt()
  {
  }

  bool ActionPrompt::IsYesNoQuestion() const
  {
    bool yes_no_question = (!mType.empty() && ra::strings::Uppercase(mType) == "YESNO");
    return yes_no_question;
  }

  bool ActionPrompt::IsOkQuestion() const
  {
    bool ok_question = (!mType.empty() && ra::strings::Uppercase(mType) == "OK");
    return ok_question;
  }

  bool ActionPrompt::Execute(const Context & context) const
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
    const std::string name = pmgr.Expand(mName);
    const std::string title = pmgr.Expand(mTitle);
    const std::string default_value = pmgr.Expand(mDefault);
    const std::string & type = mType;

    static const char * caption = "Question / Prompt";
    static const HWND parent_window = NULL;

    std::string answer; //the text result of the prompt, in utf-8 format

    //convert to windows unicode...
    std::wstring name_utf16    = ra::unicode::Utf8ToUnicode(name);
    std::wstring title_utf16   = ra::unicode::Utf8ToUnicode(title);
    std::wstring default_utf16 = ra::unicode::Utf8ToUnicode(default_value);
    std::wstring caption_utf16 = ra::unicode::Utf8ToUnicode(caption);

    //debug
    LOG(INFO) << "Prompt: '" << title << "' ?";

    if (IsOkQuestion())
    {
      int result = MessageBoxW(parent_window, title_utf16.c_str(), caption_utf16.c_str(), MB_OK | MB_ICONINFORMATION);
      if (result == IDCANCEL)
      {
        LOG(INFO) << "Prompt: user has cancelled the action.";
        return false;
      }
    }
    else if (IsYesNoQuestion())
    {
      int result = MessageBoxW(parent_window, title_utf16.c_str(), caption_utf16.c_str(), MB_YESNOCANCEL | MB_ICONQUESTION);
      switch(result)
      {
      case IDYES:
        answer = mValueYes;
        break;
      case IDNO:
        answer = mValueNo;
        break;
      default:
      case IDCANCEL:
        LOG(INFO) << "Prompt: user has cancelled the action.";
        return false;
      };
    }
    else
    {
      CInputBox box(parent_window);
      box.SetTextUnicode(default_utf16);
      bool result = box.DoModal(caption_utf16, title_utf16);
      if (!result)
      {
        LOG(INFO) << "Prompt: user has cancelled the action.";
        return false;
      }

      //convert from windows unicode back to utf-8
      std::wstring answer_utf16 = box.GetTextUnicode();
      answer = ra::unicode::UnicodeToUtf8(answer_utf16);
    }

    //update the property
    LOG(INFO) << "Prompt: setting property '" << name << "' to value '" << answer << "'.";
    pmgr.SetProperty(name, answer);

    return true;
  }

  const std::string & ActionPrompt::GetType() const
  {
    return mType;
  }

  void ActionPrompt::SetType(const std::string & type)
  {
    mType = type;
  }

  const std::string & ActionPrompt::GetName() const
  {
    return mName;
  }

  void ActionPrompt::SetName(const std::string & name)
  {
    mName = name;
  }

  const std::string & ActionPrompt::GetTitle() const
  {
    return mTitle;
  }

  void ActionPrompt::SetTitle(const std::string & title)
  {
    mTitle = title;
  }

  const std::string & ActionPrompt::GetDefault() const
  {
    return mDefault;
  }

  void ActionPrompt::SetDefault(const std::string & default_value)
  {
    mDefault = default_value;
  }

  const std::string & ActionPrompt::GetValueYes() const
  {
    return mValueYes;
  }

  void ActionPrompt::SetValueYes(const std::string & value_yes)
  {
    mValueYes = value_yes;
  }

  const std::string & ActionPrompt::GetValueNo() const
  {
    return mValueNo;
  }

  void ActionPrompt::SetValueNo(const std::string & value_no)
  {
    mValueNo = value_no;
  }

} //namespace shellanything
