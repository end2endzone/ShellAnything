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

#include "shellanything/Icon.h"

namespace shellanything
{
  Icon::Icon() :
    mIndex(Icon::INVALID_ICON_INDEX)
  {
  }

  Icon::Icon(const Icon & icon)
  {
    (*this) = icon;
  }

  Icon::~Icon()
  {
  }

  const Icon & Icon::operator =(const Icon & icon)
  {
    if (this != &icon)
    {
      mFileExtension  = icon.mFileExtension ;
      mPath           = icon.mPath ;
      mIndex          = icon.mIndex;
    }
    return (*this);
  }

  bool Icon::isValid() const
  {
    if (!mFileExtension.empty())
      return true;
    if (mPath.empty() || mIndex == Icon::INVALID_ICON_INDEX)
      return false;
    return true;
  }

  const std::string & Icon::getFileExtension() const
  {
    return mFileExtension;
  }

  void Icon::setFileExtension(const std::string & iFileExtension)
  {
    mFileExtension = iFileExtension;
  }

  const std::string & Icon::getPath() const
  {
    return mPath;
  }

  void Icon::setPath(const std::string & iPath)
  {
    mPath = iPath;
  }

  const int & Icon::getIndex() const
  {
    return mIndex;
  }

  void Icon::setIndex(const int & iIndex)
  {
    mIndex = iIndex;
  }

} //namespace shellanything
