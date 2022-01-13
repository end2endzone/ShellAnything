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

#include "ActionFile.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/unicode.h"
#include "PropertyManager.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

namespace shellanything
{

  ActionFile::ActionFile()
  {
  }

  ActionFile::~ActionFile()
  {
  }

  bool ActionFile::Execute(const Context & context) const
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
    const std::string path = pmgr.Expand(mPath);
    std::string text = pmgr.Expand(mText);
    const std::string encoding = pmgr.Expand(mEncoding);

    //debug
    LOG(INFO) << "Writing file '" << path << "'.";

    //validate encoding
    const std::string tmp = ra::strings::Uppercase(encoding);
    bool is_ansi = (tmp == "ANSI");
    bool is_unicode = (tmp == "UNICODE");
    bool is_utf8 = (tmp == "UTF-8" || tmp == "UTF8");
    if (!is_ansi && !is_unicode && !is_utf8)
    {
      LOG(ERROR) << "Unknown encoding attribute '" << encoding << "'.";
      return false;
    }

    //convert
    if (is_ansi)
      text = ra::unicode::Utf8ToAnsi(text);
    else if (is_unicode)
    {
      std::wstring textW = ra::unicode::Utf8ToUnicode(text);
      text.clear();
      text.assign((const char *)textW.data(), textW.size()*2);
    }
    //note: utf-8 does not need conversion


    //try to create the file
    bool write_ok = false;
    if (is_ansi || is_utf8)
      write_ok = ra::filesystem::WriteTextFileUtf8(path, text);
    if (is_unicode)
      write_ok = ra::filesystem::WriteFileUtf8(path, text);
    if (!write_ok)
    {
      LOG(ERROR) << "Failed writing content to file '" << path << "'.";
      return false;
    }

    //get write size
    uint32_t write_size = ra::filesystem::GetFileSizeUtf8(path.c_str());
    LOG(INFO) << "Wrote " << write_size << " bytes to file '" << path << "'.";

    return true;
  }

  const std::string & ActionFile::GetPath() const
  {
    return mPath;
  }

  void ActionFile::SetPath(const std::string & path)
  {
    mPath = path;
  }

  const std::string & ActionFile::GetText() const
  {
    return mText;
  }

  void ActionFile::SetText(const std::string & text)
  {
    mText = text;
  }

  const std::string & ActionFile::GetEncoding() const
  {
    return mEncoding;
  }

  void ActionFile::SetEncoding(const std::string & encoding)
  {
    mEncoding = encoding;
  }

} //namespace shellanything
