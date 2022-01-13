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

#include "ActionClipboard.h"
#include "PropertyManager.h"
#include "Win32Clipboard.h"

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

  bool ActionClipboard::Execute(const Context & context) const
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
    std::string value = pmgr.Expand(mValue);

    //get clipboard handler
    Win32Clipboard::Clipboard & clipboard = Win32Clipboard::Clipboard::GetInstance();

    //debug
    LOG(INFO) << "Setting clipboard to '" << value << "'.";

    //set clipboard value
    bool result = clipboard.SetTextUtf8(value);

    return result;
  }

  const std::string & ActionClipboard::GetValue() const
  {
    return mValue;
  }

  void ActionClipboard::SetValue(const std::string & value)
  {
    mValue = value;
  }

} //namespace shellanything
