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

#include "shellanything/sa_logging.h"
#include "LoggerHelper.h"
#include <stdarg.h> //for ...
#include <string>

sa_boolean sa_logging_is_verbose()
{
  bool value = shellanything::LoggerHelper::IsVerboseLoggingEnabled();
  if (value)
    return 1;
  return 0;
}

void sa_logging_print(sa_log_level_t level, const char* source_name, const char* message)
{
  if (source_name)
  {
    switch (level)
    {
    case SA_LOG_LEVEL_INFO:
      SA_LOG(INFO) << "[ " << source_name << " ]: " << message;
      break;
    case SA_LOG_LEVEL_WARNING:
      SA_LOG(WARNING) << "[ " << source_name << " ]: " << message;
      break;
    case SA_LOG_LEVEL_ERROR:
      SA_LOG(ERROR) << "[ " << source_name << " ]: " << message;
      break;
    case SA_LOG_LEVEL_FATAL:
      SA_LOG(FATAL) << "[ " << source_name << " ]: " << message;
      break;
    };
  }
  else // source_name == NULL
  {
    switch (level)
    {
    case SA_LOG_LEVEL_INFO:
      SA_LOG(INFO) << message;
      break;
    case SA_LOG_LEVEL_WARNING:
      SA_LOG(WARNING) << message;
      break;
    case SA_LOG_LEVEL_ERROR:
      SA_LOG(ERROR) << message;
      break;
    case SA_LOG_LEVEL_FATAL:
      SA_LOG(FATAL) << message;
      break;
    };
  }
}

void sa_logging_print_format(sa_log_level_t level, const char* source_name, const char* format, ...)
{
  std::string message;

  va_list args;
  va_start(args, format);

  static const int BUFFER_SIZE = 1024 * 32;
  char buffer[BUFFER_SIZE];
  buffer[0] = '\0';
  vsnprintf(buffer, BUFFER_SIZE, format, args);
  message = buffer;

  va_end(args);

  sa_logging_print(level, source_name, message.c_str());
}
