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

#ifndef SA_CLIPBOARD_SERVICE_H
#define SA_CLIPBOARD_SERVICE_H

#include "sa_windows_export.h"
#include "IClipboardService.h"

namespace shellanything
{
  /// <summary>
  /// Abstract IRegistryService class.
  /// </summary>
  class SA_WINDOWS_EXPORT ClipboardService : public virtual IClipboardService
  {
  public:
    ClipboardService();
    virtual ~ClipboardService();

  private:
    // Disable and copy constructor, dtor and copy operator
    ClipboardService(const ClipboardService&);
    ClipboardService& operator=(const ClipboardService&);
  public:

    /// <summary>
    /// Get the content of the clipboard as a string.
    /// </summary>
    /// <remarks>
    /// WARNING:
    ///   Do not add logging in this implementation for security concerns.
    ///   This is mandatory to prevent leaking secret values or passwords into the logs.
    ///   The caller is responsible for logging an error if false is returned.
    ///   If an error occurs in the implementation, the caller will have to log a generic "operation has failed" kind of log.
    /// </remarks>
    /// <param name="value">The output value to store the result.</param>
    /// <returns>Returns true if the clipboard can be get as a string. Returns false otherwise.</returns>
    virtual bool GetClipboardText(std::string& value);

    /// <summary>
    /// Set the content of the clipboard as a string.
    /// </summary>
    /// <remarks>
    /// WARNING:
    ///   Do not add logging in this implementation for security concerns.
    ///   This is mandatory to prevent leaking secret values or passwords into the logs.
    ///   The caller is responsible for logging an error if false is returned.
    ///   If an error occurs in the implementation, the caller will have to log a generic "operation has failed" kind of log.
    /// </remarks>
    /// <param name="value">The value to store in the clipboard.</param>
    /// <returns>Returns true if the clipboard can be set as a string. Returns false otherwise.</returns>
    virtual bool SetClipboardText(const std::string& value);

  };

} //namespace shellanything

#endif //SA_CLIPBOARD_SERVICE_H
