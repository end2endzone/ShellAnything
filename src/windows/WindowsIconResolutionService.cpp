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

#include "WindowsIconResolutionService.h"
#include "PropertyManager.h"
#include "SelectionContext.h"
#include "LoggerHelper.h"
#include "App.h"
#include "SaUtils.h"
#include "Win32Registry.h"

#include "rapidassist/strings.h"
#include "rapidassist/environment.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/unicode.h"

#include <Windows.h>

namespace shellanything
{
  WindowsIconResolutionService::WindowsIconResolutionService()
  {
  }

  WindowsIconResolutionService::~WindowsIconResolutionService()
  {
  }

  bool WindowsIconResolutionService::ResolveFileExtensionIcon(Icon& icon)
  {
    // Is this icon have a file extension ?
    std::string original_file_extension = icon.GetFileExtension();
    if (original_file_extension.empty())
      return false; //nothing to resolve

    shellanything::PropertyManager& pmgr = shellanything::PropertyManager::GetInstance();
    std::string file_extension = pmgr.Expand(original_file_extension);
    if (file_extension.empty())
      return false; //nothing to resolve

    // Check for multiple values. Keep the first value, forget about other selected file extensions.
    const std::string separator = pmgr.GetProperty(SelectionContext::MULTI_SELECTION_SEPARATOR_PROPERTY_NAME);
    if (file_extension.find(separator) != std::string::npos)
    {
      //multiple values detected.
      ra::strings::StringVector extension_list = ra::strings::Split(file_extension, separator.c_str());
      if (!extension_list.empty())
        file_extension = extension_list[0];
    }

    // Search within previous resolutions
    IconResolutionInfoMap::const_iterator mapIt = mResolutions.find(file_extension);
    bool found = (mapIt != mResolutions.end());
    if (found)
    {
      const ICON_RESOLUTION_INFO& resolution = mapIt->second;
      icon.SetPath(resolution.path);
      icon.SetIndex(resolution.index);
      icon.SetFileExtension("");
      return true;
    }

    // Generate an empty file with the required file extension
    IRandomService* random_service = App::GetInstance().GetRandomService();
    if (random_service == NULL)
    {
      SA_LOG(ERROR) << "No Random service configured for resolving file extensions to icon.";
      return false;
    }

    SA_LOG(INFO) << "Resolving icon for file extension '" << file_extension << "'.";

    // Generate a random filename in %TEMP% directory
    std::string temp_file_path;
    temp_file_path += ra::filesystem::GetTemporaryDirectoryUtf8();
    temp_file_path += ra::filesystem::GetPathSeparatorStr();
    temp_file_path += "icon_resolution"; // file prefix
    temp_file_path += ra::strings::ToString(random_service->GetRandomValue(10000,99999));
    temp_file_path += "."; // file postfix
    temp_file_path += file_extension;

    // Create a blank file
    bool file_created = ra::filesystem::WriteFileUtf8(temp_file_path, file_extension);
    if (!file_created)
    {
      SA_LOG(ERROR) << "Failed to create sample file '" << temp_file_path << "' to resolve icon for file extension '" << file_extension << "'.";
      mFailures.insert(file_extension);
      return false;
    }

    std::wstring temp_file_path_wide = ra::unicode::Utf8ToUnicode(temp_file_path);

    // Try to get an actual icon
    HINSTANCE hInstance = GetModuleHandle(NULL);
    wchar_t buffer[2 * MAX_PATH];
    wsprintfW(buffer, L"%s", temp_file_path_wide.c_str());
    WORD official_index = 0;
    WORD official_id = 0;
    HICON hIcon = ExtractAssociatedIconExW(hInstance, buffer, &official_index, &official_id);
    std::wstring official_file_path_wide = buffer;

    // Clean up
    ra::filesystem::DeleteFileUtf8(temp_file_path.c_str());
    if (hIcon)
      DestroyIcon(hIcon);

    // Did we failed resolution?
    if (hIcon == NULL || official_file_path_wide == temp_file_path_wide)
    {
      DWORD dwLastError = GetLastError();
      Win32Registry::REGISTRY_ICON unknown_file_icon = Win32Registry::GetUnknownFileTypeIcon();
      SA_LOG(ERROR) <<
        "Failed to find icon for file extension '" << file_extension << "' with ExtractAssociatedIconExW(). GetLastError(): " << ToHexString(dwLastError) << ". "
        "Resolving icon with default icon for unknown file type '" << unknown_file_icon.path << "' with index '" << unknown_file_icon.index << "'";

      // Remember this result
      ICON_RESOLUTION_INFO info;
      info.path = unknown_file_icon.path;
      info.index = unknown_file_icon.index;
      info.id = -1;
      ResolveWith(icon, file_extension, info);

      // But also remember that its not a true resolution
      mFailures.insert(file_extension);

      return true;
    }

    // Remember this result
    ICON_RESOLUTION_INFO info;
    info.path = ra::unicode::UnicodeToUtf8(official_file_path_wide);
    info.index = official_index;
    info.id = official_id;
    ResolveWith(icon, file_extension, info);

    SA_LOG(INFO) << "Resolved icon for file extension '" << file_extension << "' to file '" << info.path << "' with index '" << info.index << "'";

    return true;
  }

  bool WindowsIconResolutionService::HaveResolvedBefore(const std::string& file_extension) const
  {
    IconResolutionInfoMap::const_iterator mapIt = mResolutions.find(file_extension);
    bool found = (mapIt != mResolutions.end());
    if (found)
      return true;

    // or did we resolved as a failure ?
    found = HaveFailedBefore(file_extension);
    return found;
  }

  bool WindowsIconResolutionService::HaveFailedBefore(const std::string& file_extension) const
  {
    bool have_failed_before = mFailures.find(file_extension) != mFailures.end();
    return have_failed_before;
  }

  void WindowsIconResolutionService::ResolveWith(Icon& icon, const std::string& file_extension, const ICON_RESOLUTION_INFO& info)
  {
    icon.SetPath(info.path);
    icon.SetIndex(info.index);
    icon.SetFileExtension("");

    // Remember this result
    mResolutions[file_extension] = info;
  }


} //namespace shellanything
