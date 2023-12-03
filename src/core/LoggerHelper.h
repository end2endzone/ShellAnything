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

#ifndef SA_LOGGER_HELPER_H
#define SA_LOGGER_HELPER_H

#include "ILogger.h"
#include "LoggerManager.h"
#include <ostream>
#include <sstream>

namespace shellanything
{

  /// <summary>
  /// Helper class for logging
  /// </summary>
  class SHELLANYTHING_EXPORT LoggerHelper
  {
  public:
    LoggerHelper(ILogger::LOG_LEVEL level);
    LoggerHelper(const char * filename, int line, ILogger::LOG_LEVEL level);
    virtual ~LoggerHelper();

    /// <summary>
    /// Allow streaming any object/variable that can be streamed to std::stringstream.
    /// </summary>
    template<class Any>
    LoggerHelper& operator<<(const Any& any)
    {
      mSS << any;
      return (*this);
    }

    /// <summary>
    /// Allow streaming specialized functions such as std::endl
    /// </summary>
    LoggerHelper& operator<<(std::ostream& (*f)(std::ostream&));

  private:
    ILogger::LOG_LEVEL mLevel;
    const char* mFilename;
    int mLine;
    std::stringstream mSS;
  };

  #ifndef SA_LOG
  #define SA_LOG(expr) (::shellanything::LoggerHelper(__FILE__, __LINE__, ::shellanything::ILogger::LOG_LEVEL_##expr))
  #endif

} //namespace shellanything

#endif //SA_ACTION_PROMPT_H
