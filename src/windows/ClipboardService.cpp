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

#include "ClipboardService.h"
#include "Win32Clipboard.h"

namespace shellanything
{
  ClipboardService::ClipboardService()
  {
  }

  ClipboardService::~ClipboardService()
  {
  }

  bool ClipboardService::GetClipboardText(std::string& value)
  {
    //get clipboard handler
    Win32Clipboard::Clipboard& clipboard = Win32Clipboard::Clipboard::GetInstance();

    // WARNING:
    // Do not add logging in this implementation for security concerns.
    // This is mandatory to prevent leaking secret values or passwords into the logs.

    //get clipboard value
    bool result = clipboard.GetAsTextUtf8(value);
    return result;
  }

  bool ClipboardService::SetClipboardText(const std::string& value)
  {
    //get clipboard handler
    Win32Clipboard::Clipboard& clipboard = Win32Clipboard::Clipboard::GetInstance();

    // WARNING:
    // Do not add logging in this implementation for security concerns.
    // This is mandatory to prevent leaking secret values or passwords into the logs.
    
    //set clipboard value
    bool result = clipboard.SetTextUtf8(value);
    return result;
  }

} //namespace shellanything
