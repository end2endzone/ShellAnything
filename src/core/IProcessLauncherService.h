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

#ifndef SA_IPROCESSLAUNCHER_SERVICE_H
#define SA_IPROCESSLAUNCHER_SERVICE_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include "Enums.h"

#include <string>

namespace shellanything
{
  /// <summary>
  /// Abstract keyboard service class.
  /// Used to decouple the core from Windows Operating System API.
  /// </summary>
  class SHELLANYTHING_EXPORT IProcessLauncherService
  {
  public:
    IProcessLauncherService(); 
    virtual ~IProcessLauncherService();

  private:
    // Disable and copy constructor, dtor and copy operator
    IProcessLauncherService(const IProcessLauncherService&);
    IProcessLauncherService& operator=(const IProcessLauncherService&);
  public:

    /// <summary>
    /// The value to use to represent an invalid process id (pID).
    /// </summary>
    const uint32_t INVALID_PROCESS_ID = 0;

    struct ProcessLaunchResult
    {
      uint32_t pId; // PROCESS ID
    };

    /// <summary>
    /// Open a document with the default system application.
    /// </summary>
    /// <param name="path">The path to the document to open.</param>
    /// <param name="result">The optional result of the process launch.</param>
    /// <returns>Returns true if the document was opened with the system's default application. Returns false otherwise.</returns>
    virtual bool OpenDocument(const std::string& path, ProcessLaunchResult * result = NULL) const = 0;

    /// <summary>
    /// Open a directory with the system file explorer.
    /// </summary>
    /// <param name="path">The path to the directory to open.</param>
    /// <param name="result">The optional result of the process launch.</param>
    /// <returns>Returns true if the given directory was opened with the system's default application. Returns false otherwise.</returns>
    virtual bool OpenPath(const std::string& path, ProcessLaunchResult* result = NULL) const = 0;

    /// <summary>
    /// Check if the given URL is valid.
    /// </summary>
    /// <param name="value">The url link to validate.</param>
    /// <returns>Returns true if the given url is valid. Returns false otherwise.</returns>
    virtual bool IsValidUrl(const std::string& value) const = 0;

    /// <summary>
    /// Open an url with the system default browser.
    /// </summary>
    /// <param name="path">The url path to open.</param>
    /// <param name="result">The optional result of the process launch.</param>
    /// <returns>Returns true if the given url was opened with the system's default application. Returns false otherwise.</returns>
    virtual bool OpenUrl(const std::string& path, ProcessLaunchResult* result = NULL) const = 0;

  };

} //namespace shellanything

#endif //SA_IPROCESSLAUNCHER_SERVICE_H
