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

#ifndef SA_APP_H
#define SA_APP_H

#include "ILogger.h"

#include <string>

namespace shellanything
{
  /// <summary>
  /// Manages the system's current ILogger instance.
  /// </summary>
  class SHELLANYTHING_EXPORT App
  {
  public:
    static App& GetInstance();
  private:
    App();
    ~App();

    // Disable copy constructor and copy operator
    App(const App&);
    App& operator=(const App&);

  public:

    /// <summary>
    /// Set the current application logger.
    /// </summary>
    /// <remarks>
    /// If a logger instance is already set, the caller must properly destroy the old instance.
    /// </remarks>
    /// <param name="logger">A valid instance of a ILogger.</param>
    void SetLogger(ILogger* logger);

    /// <summary>
    /// Get the current application logger
    /// </summary>
    /// <returns>Returns a pointer to an ILogger instance. Returns NULL if no logger is set.</returns>
    ILogger* GetLogger();

    /// <summary>
    /// Test if application is loaded in a test environment (main's tests executable).
    /// </summary>
    /// <returns>Returns true if application is loaded in a test environment. Returns false otherwise.</returns>
    bool IsTestingEnvironment();

    /// <summary>
    /// Get the application's log directory. The returned directory has write access.
    /// </summary>
    /// <returns>Returns the path of the directory that should be used by the logging framework.</returns>
    std::string GetLogDirectory();

    /// <summary>
    /// Test if the given directory is valid for logging.
    /// </summary>
    /// <returns>Returns true if the directory is valid for logging. Returns false otherwise.</returns>
    bool IsValidLogDirectory(const std::string& path);

  private:
    ILogger * mLogger;
  };


} //namespace shellanything

#endif //SA_APP_H
