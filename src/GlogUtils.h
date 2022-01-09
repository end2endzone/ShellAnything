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

#ifndef SA_GLOG_UTILS_H
#define SA_GLOG_UTILS_H

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

#include <string>
#include <stdint.h>

namespace shellanything
{
  struct GLOG_DATETIME
  {
    int year;
    int month;  // [1,12]
    int day;    // [1,31]
    int hour;   // [0,23]
    int minute; // [0,59]
    int second; // [0,59]
  };

  const GLOG_DATETIME & GetInvalidLogDateTime();
  int DateTimeToSeconds(const GLOG_DATETIME & dt);
  int GetDateTimeDiff(const GLOG_DATETIME & dt1, const GLOG_DATETIME & dt2);
  int GetLogFileAge(const std::string & path);
  GLOG_DATETIME GetFileDateTime(const std::string & path);
  std::string GetLogDestination(int level);
  std::string GetLogFilename(int level, const std::string & date, const std::string & time, uint32_t process_id);
  bool HasDirectoryWriteAccess(const std::string & path);
  std::string GetLogDirectory();
  bool IsLogFile(const std::string & path);
  bool IsTestingEnvironment();

  void DeletePreviousLogs(int max_age_seconds);
  void DeletePreviousLogs();
  void InitLogger();

} //namespace shellanything

#endif //SA_GLOG_UTILS_H
