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

#ifndef SA_ILOGGER_SERVICE_H
#define SA_ILOGGER_SERVICE_H

#include "shellanything/export.h"
#include "shellanything/config.h"

namespace shellanything
{
  /// <summary>
  /// Abstract logger class.
  /// </summary>
  class SHELLANYTHING_EXPORT ILoggerService
  {
  public:
    ILoggerService();
    virtual ~ILoggerService();

  private:
    // Disable and copy constructor, dtor and copy operator
    ILoggerService(const ILoggerService&);
    ILoggerService& operator=(const ILoggerService&);
  public:

    enum LOG_LEVEL
    {
      LOG_LEVEL_DEBUG,
      LOG_LEVEL_INFO,
      LOG_LEVEL_WARNING,
      LOG_LEVEL_ERROR,
      LOG_LEVEL_FATAL,
    };

    /// <summary>
    /// Send a message to this logger.
    /// </summary>
    /// <param name="filename">The originating source code file name.</param>
    /// <param name="line">The line number that producing this message.</param>
    /// <param name="level">The log level of the message.</param>
    /// <param name="message">The actual message.</param>
    virtual void LogMessage(const char* filename, int line, const LOG_LEVEL & level, const char* message) = 0;

    /// <summary>
    /// Send a message to this logger.
    /// </summary>
    /// <param name="level">The log level of the message.</param>
    /// <param name="message">The actual message.</param>
    virtual void LogMessage(const LOG_LEVEL & level, const char* message) = 0;

  };

} //namespace shellanything

#endif //SA_ILOGGER_SERVICE_H
