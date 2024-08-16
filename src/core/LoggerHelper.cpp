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

#include "LoggerHelper.h"
#include <string>

namespace shellanything
{
  LoggerHelper::LoggerHelper(ILoggerService::LOG_LEVEL level) :
    mFilename(NULL),
    mLine(0),
    mLevel(level)
  {
  }

  LoggerHelper::LoggerHelper(const char* filename, int line, ILoggerService::LOG_LEVEL level) :
    mFilename(filename),
    mLine(line),
    mLevel(level)
  {
  }

  LoggerHelper::~LoggerHelper()
  {
    // Log the streamed content to the actual logger
    ILoggerService * logger = App::GetInstance().GetLoggerService();;

    if (logger)
    {
      // Get a copy of the streamed content
      std::string str_copy = mSS.str();

      // Do we have the orignial filename and line number of the logged content?
      if (mFilename)
      {
        logger->LogMessage(mFilename, mLine, mLevel, str_copy.c_str());
      }
      else
      {
        logger->LogMessage(mLevel, str_copy.c_str());
      }
    }
  }

  LoggerHelper& LoggerHelper::operator<<(std::ostream& (*f)(std::ostream&))
  {
    if (f == std::endl)
    {
      mSS << '\n';
    }
    return *this;
  }

} //namespace shellanything
