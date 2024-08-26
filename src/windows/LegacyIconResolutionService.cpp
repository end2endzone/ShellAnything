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

#include "LegacyIconResolutionService.h"
#include "PropertyManager.h"
#include "SelectionContext.h"
#include "Win32Registry.h"
#include "LoggerHelper.h"
#include "SaUtils.h"

#include "rapidassist/strings.h"
#include "rapidassist/environment.h"

namespace shellanything
{
  LegacyIconResolutionService::FileExtensionSet LegacyIconResolutionService::mUnresolvedFileExtensions;

  LegacyIconResolutionService::LegacyIconResolutionService()
  {
  }

  LegacyIconResolutionService::~LegacyIconResolutionService()
  {
  }

  bool LegacyIconResolutionService::ResolveFileExtensionIcon(Icon& icon)
  {
    //is this icon have a file extension ?
    std::string original_file_extension = icon.GetFileExtension();
    if (original_file_extension.empty())
      return false; //nothing to resolve

    shellanything::PropertyManager& pmgr = shellanything::PropertyManager::GetInstance();
    std::string file_extension = pmgr.Expand(original_file_extension);
    if (!file_extension.empty())
    {
      //check for multiple values. keep the first value, forget about other selected file extensions.
      const std::string separator = pmgr.GetProperty(SelectionContext::MULTI_SELECTION_SEPARATOR_PROPERTY_NAME);
      if (file_extension.find(separator) != std::string::npos)
      {
        //multiple values detected.
        ra::strings::StringVector extension_list = ra::strings::Split(file_extension, separator.c_str());
        if (!extension_list.empty())
          file_extension = extension_list[0];
      }

      //try to find the path to the icon module for the given file extension.
      Win32Registry::REGISTRY_ICON resolved_icon = Win32Registry::GetFileTypeIcon(file_extension.c_str());

      //An icon with a negative index is valid from the registry.
      //Only the special case index = -1 should be considered invalid (Issue #17).
      //And ShellAnything accept positive (index) and negative index (resource id). (Issue #155, Issue #164).
      //See issue #17, 155, 164.
      if (Win32Registry::IsValid(resolved_icon))
      {
        //found the icon for the file extension
        //replace this menu's icon with the new information
        SA_LOG(INFO) << "Resolving icon for file extension '" << file_extension << "' to file '" << resolved_icon.path << "' with index '" << resolved_icon.index << "'";
        icon.SetPath(resolved_icon.path);
        icon.SetIndex(resolved_icon.index);
        icon.SetFileExtension("");

        return true;
      }
      else
      {
        //failed to find a valid icon.
        //using the default "unknown" icon
        Win32Registry::REGISTRY_ICON unknown_file_icon = Win32Registry::GetUnknownFileTypeIcon();
        icon.SetPath(unknown_file_icon.path);
        icon.SetIndex(unknown_file_icon.index);
        icon.SetFileExtension("");

        const bool is_already_in_log = mUnresolvedFileExtensions.find(file_extension) != mUnresolvedFileExtensions.end();
        if (!is_already_in_log)
        {
          SA_LOG(WARNING) << "Failed to find icon for file extension '" << file_extension << "'. Resolving icon with default icon for unknown file type '" << unknown_file_icon.path << "' with index '" << unknown_file_icon.index << "'";

          //remember this failure.
          mUnresolvedFileExtensions.insert(file_extension);
        }

        return true;
      }
    }

    return false;
  }

} //namespace shellanything
