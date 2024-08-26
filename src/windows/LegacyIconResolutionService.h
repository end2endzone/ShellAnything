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

#ifndef SA_LEGACY_ICON_RESOLUTION_SERVICE_H
#define SA_LEGACY_ICON_RESOLUTION_SERVICE_H

#include "sa_windows_export.h"
#include "IIconResolutionService.h"

namespace shellanything
{
  /// <summary>
  /// Legacy implementation class of IIconResolutionService that resolved by browsing Windows Registry.
  /// </summary>
  class SA_WINDOWS_EXPORT LegacyIconResolutionService : public virtual IIconResolutionService
  {
  public:
    LegacyIconResolutionService();
    virtual ~LegacyIconResolutionService();

  private:
    // Disable and copy constructor, dtor and copy operator
    LegacyIconResolutionService(const LegacyIconResolutionService&);
    LegacyIconResolutionService& operator=(const LegacyIconResolutionService&);
  public:

    /// <summary>
    /// Resolve an icon's file extension to the system icon for this file extension.
    /// </summary>
    /// <param name="icon">The icon that have its fileextension's attribute set to resolve.</param>
    /// <returns>Returns true if the operation is successful. Returns false otherwise.</returns>
    virtual bool ResolveFileExtensionIcon(Icon& icon);

  private:
    typedef std::set<std::string /*file extension*/> FileExtensionSet;
    static FileExtensionSet mUnresolvedFileExtensions;

  };

} //namespace shellanything

#endif //SA_LEGACY_ICON_RESOLUTION_SERVICE_H
