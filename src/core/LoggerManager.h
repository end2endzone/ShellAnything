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

#ifndef SA_LOGGERMANAGER_H
#define SA_LOGGERMANAGER_H

#include "ILogger.h"

namespace shellanything
{
  /// <summary>
  /// Manages the system's current ILogger instance.
  /// </summary>
  class SHELLANYTHING_EXPORT LoggerManager
  {
  public:
    static LoggerManager& GetInstance();
  private:
    LoggerManager();
    ~LoggerManager();

    // Disable copy constructor and copy operator
    LoggerManager(const LoggerManager&);
    LoggerManager& operator=(const LoggerManager&);

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

  private:
    ILogger * mLogger;
  };


} //namespace shellanything

#endif //SA_LOGGERMANAGER_H
