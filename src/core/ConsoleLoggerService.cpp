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

#include "ConsoleLoggerService.h"

#include "rapidassist/timing.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/process.h"

#include <Windows.h> // for GetThreadId

#include <iostream>

#if defined(_WIN32)
#include <chrono>
//typedef struct timeval
//{
//  long tv_sec;
//  long tv_usec;
//} timeval;
inline int gettimeofday(struct timeval* tp, struct timezone* tzp)
{
  namespace sc = std::chrono;
  sc::system_clock::duration d = sc::system_clock::now().time_since_epoch();
  sc::seconds s = sc::duration_cast<sc::seconds>(d);
  tp->tv_sec = (long)s.count();
  tp->tv_usec = (long)sc::duration_cast<sc::microseconds>(d - s).count();

  return 0;
}
#endif // _WIN32

namespace shellanything
{

  inline const char * GetLevelString(const ILoggerService::LOG_LEVEL& level)
  {
    switch (level)
    {
    default:
    case ILoggerService::LOG_LEVEL_DEBUG:
      return "DEBUG";
      break;
    case ILoggerService::LOG_LEVEL_INFO:
      return "INFO";
      break;
    case ILoggerService::LOG_LEVEL_WARNING:
      return "WARNING";
      break;
    case ILoggerService::LOG_LEVEL_ERROR:
      return "ERROR";
      break;
    case ILoggerService::LOG_LEVEL_FATAL:
      return "FATAL";
      break;
    };
  }

  inline std::string GetLogTimestamp()
  {
    struct timeval tv;
    time_t nowtime;
    struct tm nowtm;
    char tmbuf[64], buf[64];

    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec;
    localtime_s(&nowtm, &nowtime);
    strftime(tmbuf, sizeof(tmbuf), "%H:%M:%S", &nowtm); // skip the date format string: %Y-%m-%d 
    snprintf(buf, sizeof buf, "%s.%06ld", tmbuf, tv.tv_usec);

    return buf;
  }

  ConsoleLoggerService::ConsoleLoggerService()
  {
  }

  ConsoleLoggerService::~ConsoleLoggerService()
  {
  }

  void ConsoleLoggerService::LogMessage(const char* filename, int line, const ILoggerService::LOG_LEVEL& level, const char* message)
  {
    /*
    W0928 09:06:07.668726 29440 ActionProperty.cpp:233] Reporting an error because fail is set to 'true'.
    E0928 09:06:14.720094 29440 ConfigManager.cpp:139] Failed searching for configuration files in directory 'D:\Projets\Programmation\Cpp\ShellAnything\ShellAnything\build\bin\Debug\test_workspace\TestPlugins.testProcess'.
    */

    const char * level_str = GetLevelString(level);
    std::string log_time = GetLogTimestamp();
    DWORD dwThreadId = GetThreadId(NULL);
    std::string actual_filename = ra::filesystem::GetFilename(filename);

    std::cout << level_str << " " << log_time << " " << dwThreadId << " " << actual_filename << ":" << line << "] " << message << "\n";
  }

  void ConsoleLoggerService::LogMessage(const ILoggerService::LOG_LEVEL& level, const char* message)
  {
    LogMessage(__FILE__, __LINE__, level, message);
  }

} //namespace shellanything
