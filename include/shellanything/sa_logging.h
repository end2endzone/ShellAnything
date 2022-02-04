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

#ifndef SA_API_LOGGING_H
#define SA_API_LOGGING_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  SA_LOG_LEVEL_INFO = 0,
  SA_LOG_LEVEL_WARNING,
  SA_LOG_LEVEL_ERROR,
  SA_LOG_LEVEL_FATAL,
} sa_log_level_t;

/// <summary>
/// Log a custom message in logs.
/// </summary>
/// <param name="level">The level of the log message.</param>
/// <param name="source_name">The name of the source or module related to the log message. For example, the plugin name.</param>
/// <param name="message">The message to print in the logs.</param>
void sa_logging_print(sa_log_level_t level, const char * source_name, const char * message);

/// <summary>
/// Log a custom message in logs.
/// </summary>
/// <param name="level">The level of the log message.</param>
/// <param name="source_name">The name of the source or module related to the log message. For example, the plugin name.</param>
/// <param name="format">The format of the </param>
/// <param name="...">The arguments in the log to print</param>
void sa_logging_print_format(sa_log_level_t level, const char* source_name, const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif //SA_API_ERROR_MANAGER_H
