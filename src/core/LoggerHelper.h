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

#include "ILoggerService.h"
#include "App.h"
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
    LoggerHelper(ILoggerService::LOG_LEVEL level);
    LoggerHelper(ILoggerService::LOG_LEVEL level, bool is_verbose_stream);
    LoggerHelper(const char * filename, int line, ILoggerService::LOG_LEVEL level);
    LoggerHelper(const char * filename, int line, ILoggerService::LOG_LEVEL level, bool is_verbose_stream);
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

    /// <summary>
    /// Define if verbose logging is enabled.
    /// </summary>
    /// <returns>Returns true when verbose logging is enabled. Returns false otherwise.</returns>
    static bool IsVerboseLoggingEnabled();

  private:
    ILoggerService::LOG_LEVEL mLevel;
    bool mIsVerboseStream;
    const char* mFilename;
    int mLine;
    std::stringstream mSS;
  };

  /// <summary>
  /// Helper class for logging the scope of a function or block of code.
  /// </summary>
  /// <example>
  /// <code>
  ///   SA_DECLARE_SCOPE_LOGGER_ARGS(sli);
  ///   sli.verbose = true;
  ///   sli.instance = this;
  ///   ScopeLogger logger(&sli);
  /// </code>
  /// </example>
  class SHELLANYTHING_EXPORT ScopeLogger
  {
  public:
    /// <summary>
    /// Argument struct for a ScopeManager
    /// </summary>
    struct ARGS
    {
      ///<summary>The souce code filename generating the log entries.</summary>
      const char* filename;
      ///<summary>The souce code line number generating the log entries.</summary>
      int line;
      ///<summary>The level to use while producing log entries.</summary>
      ILoggerService::LOG_LEVEL level;
      ///<summary>The name to use while generating logs. Usually the function name.</summary>
      const char* name;
      ///<summary>Is the log entries verbose or not.</summary>
      bool verbose;
      ///<summary>The calling class instance of the scope. Usually 'this'.</summary>
      const void* instance;
    };

  public:
    ScopeLogger(const ScopeLogger::ARGS* info);
    ~ScopeLogger();

  private:
    // Disable copy constructor and copy operator
    ScopeLogger(const ScopeLogger&);
    ScopeLogger& operator=(const ScopeLogger&);

  public:
    const ARGS* args;
  };

  #ifndef SA_DECLARE_SCOPE_LOGGER_ARGS
  #define SA_DECLARE_SCOPE_LOGGER_ARGS(info) \
    ::shellanything::ScopeLogger::ARGS info = {0};\
    info.filename = __FILE__;\
    info.line = __LINE__;\
    info.name = __FUNCTION__ "()";\
    info.level = ::shellanything::ILoggerService::LOG_LEVEL::LOG_LEVEL_DEBUG;
  #endif

  #ifndef SA_LOG
  #define SA_LOG(expr)          (::shellanything::LoggerHelper(__FILE__, __LINE__, ::shellanything::ILoggerService::LOG_LEVEL_##expr, false))
  #endif

  #ifndef SA_VERBOSE_LOG
  #define SA_VERBOSE_LOG(expr)  (::shellanything::LoggerHelper(__FILE__, __LINE__, ::shellanything::ILoggerService::LOG_LEVEL_##expr, true ))
  #endif

} //namespace shellanything

#endif //SA_ACTION_PROMPT_H
