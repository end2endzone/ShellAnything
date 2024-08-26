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

#ifndef SA_WINDOWS_ICON_RESOLUTION_SERVICE_H
#define SA_WINDOWS_ICON_RESOLUTION_SERVICE_H

#include "sa_windows_export.h"
#include "IIconResolutionService.h"

#include <map>

namespace shellanything
{
  /// <summary>
  /// Windows implementation class of IIconResolutionService that resolves icon based on ExtractAssociatedIconEx().
  /// </summary>
  class SA_WINDOWS_EXPORT WindowsIconResolutionService : public virtual IIconResolutionService
  {
  public:
    WindowsIconResolutionService();
    virtual ~WindowsIconResolutionService();

  private:
    // Disable and copy constructor, dtor and copy operator
    WindowsIconResolutionService(const WindowsIconResolutionService&);
    WindowsIconResolutionService& operator=(const WindowsIconResolutionService&);
  public:

    /// <summary>
    /// Resolve an icon's file extension to the system icon for this file extension.
    /// </summary>
    /// <param name="icon">The icon that have its fileextension's attribute set to resolve.</param>
    /// <returns>Returns true if the operation is successful. Returns false otherwise.</returns>
    virtual bool ResolveFileExtensionIcon(Icon& icon);

    /// <summary>
    /// Check if the given file extension have resolved before.
    /// </summary>
    /// <param name="file_entension">The file extension to check.</param>
    /// <returns>Returns true if the file extension has previously resolve to a valid icon. Returns false otherwise.</returns>
    virtual bool HaveResolvedBefore(const std::string& file_extension) const;

    /// <summary>
    /// Check if the given file extension have previously failed to resolve.
    /// </summary>
    /// <param name="file_extension">The file extension to check.</param>
    /// <returns>Returns true if the file extension has previously failed to resolve. Returns false otherwise.</returns>
    virtual bool HaveFailedBefore(const std::string& file_extension) const;

  private:
    //------------------------
    // Typedef
    //------------------------
    struct ICON_RESOLUTION_INFO
    {
      std::string path;
      int index;
      int id;
    };

    /// <summary>
    /// Resolve an icon's with the given ICON_RESOLUTION_INFO.
    /// </summary>
    /// <param name="icon">The icon that have its fileextension's attribute set to resolve.</param>
    /// <param name="file_extension">The file extension specified by the icon.</param>
    /// <param name="info">The infomation to resolve the icon with.</param>
    void ResolveWith(Icon& icon, const std::string & file_extension, const ICON_RESOLUTION_INFO & info);

    typedef std::map<std::string /*file extension*/, ICON_RESOLUTION_INFO /*info*/> IconResolutionInfoMap;
    typedef std::set<std::string /*file extension*/> FileExtensionSet;
    IconResolutionInfoMap mResolutions;
    FileExtensionSet mFailures;
  };

} //namespace shellanything

#endif //SA_WINDOWS_ICON_RESOLUTION_SERVICE_H
