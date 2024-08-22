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
  /// // For static functions:
  /// ScopeLogger scope_logger(__FUNCTION__ "()");
  /// ScopeLogger verbose_scope_logger(__FUNCTION__ "()", true);
  /// ScopeLogger info_verbose_scope_logger(__FUNCTION__ "()", true, ILoggerService::LOG_LEVEL::LOG_LEVEL_INFO);
  ///
  /// // For class methods:
  /// ScopeLogger scope_logger(__FUNCTION__ "()", this);
  /// ScopeLogger verbose_scope_logger(__FUNCTION__ "()", this, true);
  /// ScopeLogger info_verbose_scope_logger(__FUNCTION__ "()", this, true, ILoggerService::LOG_LEVEL::LOG_LEVEL_INFO);
  /// </code>
  /// </example>
  class SHELLANYTHING_EXPORT ScopeLogger
  {
  public:
    ScopeLogger(const char* name, bool is_verbose = false, ILoggerService::LOG_LEVEL level = ILoggerService::LOG_LEVEL::LOG_LEVEL_DEBUG);
    ScopeLogger(const char* name, const void * calling_instance, bool is_verbose = false, ILoggerService::LOG_LEVEL level = ILoggerService::LOG_LEVEL::LOG_LEVEL_DEBUG);
    ~ScopeLogger();

  private:
    // Disable copy constructor and copy operator
    ScopeLogger(const ScopeLogger&);
    ScopeLogger& operator=(const ScopeLogger&);

  private:
    void Reset();
    void Enter();
    void Leave();

  public:
    ILoggerService::LOG_LEVEL mLevel;
    std::string mName;
    bool mIsVerbose;
    const void* mCallingInstance;
  };

  #ifndef SA_LOG
  #define SA_LOG(expr)          (::shellanything::LoggerHelper(__FILE__, __LINE__, ::shellanything::ILoggerService::LOG_LEVEL_##expr, false))
  #endif

  #ifndef SA_VERBOSE_LOG
  #define SA_VERBOSE_LOG(expr)  (::shellanything::LoggerHelper(__FILE__, __LINE__, ::shellanything::ILoggerService::LOG_LEVEL_##expr, true ))
  #endif

} //namespace shellanything

#endif //SA_ACTION_PROMPT_H
