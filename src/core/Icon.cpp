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

#include "Icon.h"
#include "App.h"
#include "LoggerHelper.h"
#include "PropertyManager.h"
#include "SelectionContext.h"
#include "SaUtils.h"

#include "rapidassist/strings.h"

namespace shellanything
{
  Icon::Icon() :
    mIndex(0) // As per documentation, "If the index is not specified, the value 0 is used." See Issue #17, #150 and #155.
  {
  }

  Icon::Icon(const Icon& icon)
  {
    (*this) = icon;
  }

  Icon::~Icon()
  {
  }

  const Icon& Icon::operator =(const Icon& other)
  {
    if (this != &other)
    {
      mFileExtension = other.mFileExtension;
      mPath = other.mPath;
      mIndex = other.mIndex;
    }
    return (*this);
  }

  bool Icon::operator ==(const Icon& other) const
  {
    if (this == &other)
      return true;
    if ((mFileExtension == other.mFileExtension) &&
        (mPath == other.mPath) &&
        (mIndex == other.mIndex))
        return true;
    return false;
  }

  bool Icon::IsValid() const
  {
    if (!mFileExtension.empty())
      return true;

    //See issue #17, 155, 164.
    //An icon with a negative index is valid from the registry.
    //Only the special case index = -1 should be considered invalid (Issue #17).
    //And ShellAnything accept positive (index) and negative index (resource id). (Issue #155, Issue #164).
    if (!mPath.empty() && mIndex != INVALID_ICON_INDEX)
      return true;

    return false;
}

  void Icon::ResolveFileExtensionIcon()
  {
  }

  const std::string& Icon::GetFileExtension() const
  {
    return mFileExtension;
  }

  void Icon::SetFileExtension(const std::string& file_extension)
  {
    mFileExtension = file_extension;
  }

  const std::string& Icon::GetPath() const
  {
    return mPath;
  }

  void Icon::SetPath(const std::string& path)
  {
    mPath = path;
  }

  const int& Icon::GetIndex() const
  {
    return mIndex;
  }

  void Icon::SetIndex(const int& index)
  {
    mIndex = index;
  }

  Icon Icon::GetDefaultUnknownFileTypeIcon()
  {
    Icon tmp;
    tmp.SetFileExtension("this_file_extension_is_not_registered_on_system");
    tmp.ResolveFileExtensionIcon();
    return tmp;
  }

  std::string Icon::ToShortString() const
  {
    std::string str;
    str += "Icon ";
    str += ToHexString(this);
    if (!mPath.empty())
    {
      str += ", path=";
      str += mPath;
    }
    if (!mFileExtension.empty())
    {
      str += ", ext=";
      str += mFileExtension;
    }
    if (mIndex != INVALID_ICON_INDEX)
    {
      str += ", index=";
      str += ra::strings::ToString(mIndex);
    }
    return str;
  }

  void Icon::ToLongString(std::string& str, int indent) const
  {
    const std::string indent_str = std::string(indent, ' ');

    const std::string short_string = ToShortString();
    str += indent_str + short_string;
  }

} //namespace shellanything
