#pragma once

// Stubbing code from supporting `LOG(INFO) << "my log text"` expressions

#include <iostream>
#include <ctime>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <sstream>

#include "rapidassist/strings.h"


enum LOG_LEVELS
{
  LOG_INFO,
  LOG_WARNING,
  LOG_ERROR,
};

enum LOG_FLAGS
{
  LOG_FLAGS_NONE = 0,
  LOG_FLAGS_NEWLINE = 1,
  LOG_FLAGS_LEVEL = 2,
  LOG_FLAGS_TIME = 4,
};

const char* GetLogLevelDesc(LOG_LEVELS level)
{
  switch (level)
  {
  case 0:
    return "INFO";
  case 1:
    return "WARNING";
  case 2:
    return "ERROR";
  default:
    return "UNKNOWN";
  }
}

// https://stackoverflow.com/questions/24686846/get-current-time-in-milliseconds-or-hhmmssmmm-format
std::string get_time_in_HH_MM_SS_MMM()
{
  using namespace std::chrono;

  // get current time
  auto now = system_clock::now();

  // get number of milliseconds for the current second
  // (remainder after division into seconds)
  auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

  // convert to std::time_t in order to convert to std::tm (broken time)
  auto timer = system_clock::to_time_t(now);

  // convert to broken time
  std::tm bt = *std::localtime(&timer);

  std::ostringstream oss;

  oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
  oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

  return oss.str();
}

class LogApp
{
private:
  FILE* file;
  LOG_LEVELS level;

public:
  LogApp()
  {
    level = LOG_INFO;
    file = NULL;
    InitializeLogger();
  }
  ~LogApp()
  {
    TerminateLogger();
  }
  void SetLevel(LOG_LEVELS level)
  {
    this->level = level;
  }
  LOG_LEVELS GetLevel() const
  {
    return level;
  }
  void Log(const char* value)
  {
    fputs(value, file);
    //std::cout << value;
  }
  inline void Log(const std::string& value)
  {
    Log(value.c_str());
  }
  void Log(const char* value, int flags)
  {
    if (file == NULL)
      return;

    if (flags & LOG_FLAGS_LEVEL)
    {
      std::string output;
      output += GetLogLevelDesc(level);
      output += ": ";

      Log(output);
    }
    if (flags & LOG_FLAGS_TIME)
    {
      std::string output;
      output += get_time_in_HH_MM_SS_MMM();
      output += " ";

      Log(output);
    }

    Log(value);

    if (flags & LOG_FLAGS_NEWLINE)
    {
      Log("\n");
    }
  }
  inline void LogLn(const char* value, int flags)
  {
    std::string tmp;
    tmp += value;
    tmp += "\n";
    Log(tmp.c_str(), flags);
  }
  inline void Log(const std::string& value, int flags)
  {
    Log(value.c_str(), flags);
  }
  inline void LogLn(const std::string& value, int flags)
  {
    std::string tmp;
    tmp += value;
    tmp += "\n";
    Log(tmp, flags);
  }

private:
  void InitializeLogger()
  {
    if (file != NULL)
      return;

    DWORD wId = GetCurrentProcessId();
    char name[10240];
    sprintf(name, "C:\\Users\\Antoine\\Documents\\log.%06d.txt", wId);

    file = fopen(name, "w");
  }

  void TerminateLogger()
  {
    if (file == NULL)
      return;
    fclose(file);
    file = NULL;
  }
};

LogApp log_app;

class Logger : public std::stringstream
{
private:
  bool first_print;
public:
  Logger(LOG_LEVELS level)
  {
    log_app.SetLevel(level);
    first_print = true;
  }
  Logger& operator<<(const char* right)
  {
    if (first_print)
    {
      log_app.Log("\n"); // newline from previous LOG(xxx) line
      log_app.Log(right, LOG_FLAGS_LEVEL | LOG_FLAGS_TIME);
    }
    else
    {
      log_app.Log(right, 0);
    }
    first_print = false;

    return *this;
  }
  Logger& operator<<(const std::string& right)
  {
    (*this) << right.c_str();
    return *this;
  }
  Logger& operator<<(const void* right)
  {
    std::string tmp;
    (*this) << tmp;
    return *this;
  }
  Logger& operator<<(const UINT& right)
  {
    std::string tmp;
    (*this) << tmp;
    return *this;
  }
  Logger& operator<<(const ULONG& right)
  {
    std::string tmp;
    (*this) << tmp;
    return *this;
  }
};
#define LOG(expr) (Logger(LOG_##expr))


namespace shellanything
{

  void ShowErrorMessage(const std::string& title, const std::string& message)
  {
  }

} //namespace shellanything
