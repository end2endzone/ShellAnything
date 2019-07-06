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

#include "shellanything/ActionFile.h"
#include "PropertyManager.h"
#include "Platform.h"

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

  bool ActionFile::execute(const Context & iContext) const
  {
    PropertyManager & pmgr = PropertyManager::getInstance();
    std::string path = pmgr.expand(mPath);
    std::string text = pmgr.expand(mText);

    //debug
    LOG(INFO) << "Writing file '" << path << "'";

    //try to create the file
    FILE * f = fopen(path.c_str(), "w");
    if (!f)
    {
      LOG(ERROR) << "Failed creating file '" << path << "'";
      return false;
    }

    //write to file
    size_t sizeWrite = fputs(text.c_str(), f);
    if (sizeWrite < text.size()) //can't use == operator as \n can be replaced by \r\n
    {
      LOG(ERROR) << "Failed writing content to file '" << path << "'";
      fclose(f);
      return false;
    }

    fclose(f);

    LOG(INFO) << "Wrote " << sizeWrite << " bytes to file '" << path << "'";

    return true;
  }

  const std::string & ActionFile::getPath() const
  {
    return mPath;
  }

  void ActionFile::setPath(const std::string & iPath)
  {
    mPath = iPath;
  }

  const std::string & ActionFile::getText() const
  {
    return mText;
  }

  void ActionFile::setText(const std::string & iText)
  {
    mText = iText;
  }

} //namespace shellanything
