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

#include "shellanything/ActionClipboard.h"
#include "PropertyManager.h"

#include "win32clipboard/win32clipboard.h"
#include "rapidassist/unicode.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

namespace shellanything
{

  ActionClipboard::ActionClipboard()
  {
  }

  ActionClipboard::~ActionClipboard()
  {
  }

  bool ActionClipboard::execute(const Context & iContext) const
  {
    PropertyManager & pmgr = PropertyManager::getInstance();
    std::string value = pmgr.expand(mValue);

    //convert to windows unicode...
    std::wstring value_utf16 = ra::unicode::Utf8ToUnicode(value);

    //get clipboard handler
    win32clipboard::Clipboard & clipboard = win32clipboard::Clipboard::GetInstance();

    //debug
    LOG(INFO) << "Setting clipboard to '" << value << "'";

    //set clipboard value
    bool result = clipboard.SetTextUnicode(value_utf16);

    return result;
  }

  const std::string & ActionClipboard::getValue() const
  {
    return mValue;
  }

  void ActionClipboard::setValue(const std::string & iValue)
  {
    mValue = iValue;
  }

} //namespace shellanything
