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

#ifndef SA_IICON_RESOLUTION_SERVICE_H
#define SA_IICON_RESOLUTION_SERVICE_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include "Icon.h"

#include <stdint.h>

namespace shellanything
{
  /// <summary>
  /// Abstract icon resolution service class.
  /// Used to decouple the core from the operating system.
  /// </summary>
  class SHELLANYTHING_EXPORT IIconResolutionService
  {
  public:
    IIconResolutionService();
    virtual ~IIconResolutionService();

  private:
    // Disable and copy constructor, dtor and copy operator
    IIconResolutionService(const IIconResolutionService&);
    IIconResolutionService& operator=(const IIconResolutionService&);
  public:

    /// <summary>
    /// Resolve an icon's file extension to the system icon for this file extension.
    /// </summary>
    /// <param name="icon">The icon that have its fileextension's attribute set to resolve.</param>
    /// <returns>Returns true if the operation is successful. Returns false otherwise.</returns>
    virtual bool ResolveFileExtensionIcon(Icon & icon) = 0;

    /// <summary>
    /// Check if the given file extension have resolved before.
    /// </summary>
    /// <param name="file_entension">The file extension to check.</param>
    /// <returns>Returns true if the file extension has previously resolve to a valid icon. Returns false otherwise.</returns>
    virtual bool HaveResolvedBefore(const std::string& file_extension) const = 0;

    /// <summary>
    /// Check if the given file extension have previously failed to resolve.
    /// </summary>
    /// <param name="file_entension">The file extension to check.</param>
    /// <returns>Returns true if the file extension has previously failed to resolve. Returns false otherwise.</returns>
    virtual bool HaveFailedBefore(const std::string& file_extension) const = 0;

  };

} //namespace shellanything

#endif //SA_IICON_RESOLUTION_SERVICE_H
