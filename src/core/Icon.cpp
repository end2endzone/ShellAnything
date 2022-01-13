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
#include "Context.h"
#include "PropertyManager.h"
#include "Win32Registry.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

#include "rapidassist/strings.h"

namespace shellanything
{
  Icon::FileExtensionSet Icon::mUnresolvedFileExtensions;

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

  bool Icon::IsValid() const
  {
    if (!mFileExtension.empty())
      return true;
    if (mPath.empty() || mIndex == Icon::INVALID_ICON_INDEX)
      return false;
    return true;
  }

  void Icon::ResolveFileExtensionIcon()
  {
    //is this menu have a file extension ?
    shellanything::PropertyManager & pmgr = shellanything::PropertyManager::GetInstance();
    std::string file_extension = pmgr.Expand(mFileExtension);
    if (!file_extension.empty())
    {
      //check for multiple values. keep the first value, forget about other selected file extensions.
      const std::string separator = pmgr.GetProperty(Context::MULTI_SELECTION_SEPARATOR_PROPERTY_NAME);
      if (file_extension.find(separator) != std::string::npos)
      {
        //multiple values detected.
        ra::strings::StringVector extension_list = ra::strings::Split(file_extension, separator.c_str());
        if (!extension_list.empty())
          file_extension = extension_list[0];
      }

      //try to find the path to the icon module for the given file extension.
      Win32Registry::REGISTRY_ICON resolved_icon = Win32Registry::GetFileTypeIcon(file_extension.c_str());
      if (!resolved_icon.path.empty() && resolved_icon.index != Win32Registry::INVALID_ICON_INDEX)
      {
        //found the icon for the file extension
        //replace this menu's icon with the new information
        LOG(INFO) << "Resolving icon for file extension '" << file_extension << "' to file '" << resolved_icon.path << "' with index '" << resolved_icon.index << "'";
        mPath = resolved_icon.path;
        mIndex = resolved_icon.index;
        mFileExtension = "";
      }
      else
      {
        //failed to find a valid icon.
        //using the default "unknown" icon
        Win32Registry::REGISTRY_ICON unknown_file_icon = Win32Registry::GetUnknownFileTypeIcon();
        mPath = unknown_file_icon.path;
        mIndex = unknown_file_icon.index;
        mFileExtension = "";

        //show the message only once in logs
        const bool is_already_in_log = mUnresolvedFileExtensions.find(file_extension) != mUnresolvedFileExtensions.end();
        if (!is_already_in_log)
        {
          LOG(WARNING) << "Failed to find icon for file extension '" << file_extension << "'. Resolving icon with default icon for unknown file type '" << unknown_file_icon.path << "' with index '" << unknown_file_icon.index << "'";
          
          //remember this failure.
          mUnresolvedFileExtensions.insert(file_extension);
        }
      }
    }
  }

  const std::string & Icon::GetFileExtension() const
  {
    return mFileExtension;
  }

  void Icon::SetFileExtension(const std::string & file_extension)
  {
    mFileExtension = file_extension;
  }

  const std::string & Icon::GetPath() const
  {
    return mPath;
  }

  void Icon::SetPath(const std::string & path)
  {
    mPath = path;
  }

  const int & Icon::GetIndex() const
  {
    return mIndex;
  }

  void Icon::SetIndex(const int & index)
  {
    mIndex = index;
  }

} //namespace shellanything
