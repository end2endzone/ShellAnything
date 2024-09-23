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

#ifndef SA_WINDOWS_PROCESS_LAUNCHER_SERVICE_H
#define SA_WINDOWS_PROCESS_LAUNCHER_SERVICE_H

#include "sa_windows_export.h"
#include "IProcessLauncherService.h"

namespace shellanything
{
  /// <summary>
  /// Win32 implementation class of IProcessLauncherService.
  /// </summary>
  class SA_WINDOWS_EXPORT WindowsProcessLauncherService : public virtual IProcessLauncherService
  {
  public:
    WindowsProcessLauncherService();
    virtual ~WindowsProcessLauncherService();

  private:
    // Disable and copy constructor, dtor and copy operator
    WindowsProcessLauncherService(const WindowsProcessLauncherService&);
    WindowsProcessLauncherService& operator=(const WindowsProcessLauncherService&);
  public:

    /// <summary>
    /// Start the given process.
    /// </summary>
    /// <param name="path">The path to the process to start.</param>
    /// <param name="basedir">The base directory for the process to start in.</param>
    /// <param name="arguments">The arguments for the process.</param>
    /// <param name="args">A PropertyStore which contains optional settings for the start process.</param>
    /// <param name="result">The optional result of the process launch.</param>
    /// <returns>Returns true if the process was started. Returns false otherwise.</returns>
    virtual bool StartProcess(const std::string& path, const std::string& basedir, const std::string& arguments, PropertyStore& options, ProcessLaunchResult* result = NULL) const;

    /// <summary>
    /// Open a document with the default system application.
    /// </summary>
    /// <param name="path">The path to the document to open.</param>
    /// <param name="result">The optional result of the process launch.</param>
    /// <returns>Returns true if the document was opened with the system's default application. Returns false otherwise.</returns>
    virtual bool OpenDocument(const std::string& path, ProcessLaunchResult* result = NULL) const;

    /// <summary>
    /// Open a directory with the system file explorer.
    /// </summary>
    /// <param name="path">The path to the directory to open.</param>
    /// <param name="result">The optional result of the process launch.</param>
    /// <returns>Returns true if the given directory was opened with the system's default application. Returns false otherwise.</returns>
    virtual bool OpenPath(const std::string& path, ProcessLaunchResult* result = NULL) const;

    /// <summary>
    /// Check if the given URL is valid.
    /// </summary>
    /// <param name="value">The url link to validate.</param>
    /// <returns>Returns true if the given url is valid. Returns false otherwise.</returns>
    virtual bool IsValidUrl(const std::string& value) const;

    /// <summary>
    /// Open an url with the system default browser.
    /// </summary>
    /// <param name="path">The url path to open.</param>
    /// <param name="result">The optional result of the process launch.</param>
    /// <returns>Returns true if the given url was opened with the system's default application. Returns false otherwise.</returns>
    virtual bool OpenUrl(const std::string& path, ProcessLaunchResult* result = NULL) const;

  private:
    std::string GetErrorMessageUtf8(uint32_t dwMessageId) const;
  };

} //namespace shellanything

#endif //SA_WINDOWS_PROCESS_LAUNCHER_SERVICE_H
