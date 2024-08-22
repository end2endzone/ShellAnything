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
#include "PropertyManager.h"
#include "Environment.h"
#include "Validator.h"
#include "SaUtils.h"
#include <string>

#include "rapidassist/strings.h"

namespace shellanything
{

  LoggerHelper::LoggerHelper(ILoggerService::LOG_LEVEL level) :
    mFilename(NULL),
    mLine(0),
    mLevel(level),
    mIsVerboseStream(false)
  {
  }

  LoggerHelper::LoggerHelper(ILoggerService::LOG_LEVEL level, bool is_verbose_stream) :
    mFilename(NULL),
    mLine(0),
    mLevel(level),
    mIsVerboseStream(is_verbose_stream)
  {
  }

  LoggerHelper::LoggerHelper(const char* filename, int line, ILoggerService::LOG_LEVEL level) :
    mFilename(filename),
    mLine(line),
    mLevel(level),
    mIsVerboseStream(false)
  {
  }

  LoggerHelper::LoggerHelper(const char* filename, int line, ILoggerService::LOG_LEVEL level, bool is_verbose_stream) :
    mFilename(filename),
    mLine(line),
    mLevel(level),
    mIsVerboseStream(is_verbose_stream)
  {
  }

  LoggerHelper::~LoggerHelper()
  {
    if (mIsVerboseStream && !LoggerHelper::IsVerboseLoggingEnabled())
      return; // nothing to log

    // Log the streamed content to the actual logger
    ILoggerService* logger = App::GetInstance().GetLoggerService();;

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

  bool LoggerHelper::IsVerboseLoggingEnabled()
  {
    PropertyManager& pmgr = PropertyManager::GetInstance();
    Environment& env = Environment::GetInstance();

    // Check environment variable options first.
    static const std::string& VERBOSE_ENV_VAR_NAME = Environment::SYSTEM_LOGGING_VERBOSE_ENVIRONMENT_VARIABLE_NAME;
    if (env.IsOptionSet(VERBOSE_ENV_VAR_NAME))
    {
      if (env.IsOptionTrue(VERBOSE_ENV_VAR_NAME))
        return true;
      return false;
    }

    // Check internal property system.
    static const std::string& VERBOSE_PROPERTY_NAME = PropertyManager::SYSTEM_LOGGING_VERBOSE_PROPERTY_NAME;
    if (!pmgr.HasProperty(VERBOSE_PROPERTY_NAME))
      return false; // no verbose logging

    const std::string& value = pmgr.GetProperty(VERBOSE_PROPERTY_NAME);
    if (value.empty())
      return false; // no verbose logging

    bool has_vebose_logging = Validator::IsTrue(value);
    return has_vebose_logging;
  }

  // ------------------------------------------------------------------------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------------------------------------------------------------------------

  ScopeLogger::ScopeLogger(const char* name, bool is_verbose, ILoggerService::LOG_LEVEL level)
  {
    Reset();
    mLevel = level;
    mName = name;
    mIsVerbose = is_verbose;

    Enter();
  }

  ScopeLogger::ScopeLogger(const char* name, const void* calling_instance, bool is_verbose, ILoggerService::LOG_LEVEL level)
  {
    Reset();
    mLevel = level;
    mName = name;
    mIsVerbose = is_verbose;
    mCallingInstance = calling_instance;

    Enter();
  }

  ScopeLogger::~ScopeLogger()
  {
    Leave();
  }

  void ScopeLogger::Reset()
  {
    mLevel = ILoggerService::LOG_LEVEL::LOG_LEVEL_INFO;
    //mName
    mIsVerbose = false;
    mCallingInstance = NULL;
  }

  void ScopeLogger::Enter()
  {
    if (!mCallingInstance)
      ::shellanything::LoggerHelper(mLevel, mIsVerbose) << mName;
    else
    {
      std::string hex_ptr_value = ToHexString(mCallingInstance);
      ::shellanything::LoggerHelper(mLevel, mIsVerbose) << mName << ",this=" << hex_ptr_value;
    }
  }

  void ScopeLogger::Leave()
  {
    if (!mCallingInstance)
      ::shellanything::LoggerHelper(mLevel, mIsVerbose) << mName << " - leave";
    else
    {
      std::string hex_ptr_value = ToHexString(mCallingInstance);
      ::shellanything::LoggerHelper(mLevel, mIsVerbose) << mName << ",this=" << hex_ptr_value << " - leave";
    }
  }

} //namespace shellanything
