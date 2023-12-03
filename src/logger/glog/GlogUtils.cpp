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

#include "GlogUtils.h"
#include "App.h"
#include "SaUtils.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

#include "rapidassist/strings.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/environment.h"
#include "rapidassist/timing.h"
#include "rapidassist/process.h"
#include "rapidassist/user.h"

 //Global declarations
char      g_Path[4096];             // Path to this DLL. 
char* g_argv[] = { g_Path, "" };  // For google::InitGoogleLogging(g_argv[0])

namespace shellanything
{
  extern void ShowErrorMessage(const std::string& title, const std::string& message);

  int DateTimeToSeconds(const GLOG_DATETIME& dt)
  {
    int total_seconds = 0;
    total_seconds += dt.year * 60 * 60 * 24 * 31 * 12;
    total_seconds += dt.month * 60 * 60 * 24 * 31; //rouding to 31 days per month
    total_seconds += dt.day * 60 * 60 * 24;
    total_seconds += dt.hour * 60 * 60;
    total_seconds += dt.minute * 60;
    total_seconds += dt.second * 1;
    return total_seconds;
  }

  int GetDateTimeDiff(const GLOG_DATETIME& dt1, const GLOG_DATETIME& dt2)
  {
    int seconds1 = DateTimeToSeconds(dt1);
    int seconds2 = DateTimeToSeconds(dt2);
    int diff = seconds2 - seconds1;
    return diff;
  }

  int GetLogFileAge(const std::string& path)
  {
    //when did we created this file ?
    const GLOG_DATETIME      file_date = GetFileDateTime(path);
    const GLOG_DATETIME& invalid_date = GetInvalidLogDateTime();
    if (memcmp(&file_date, &invalid_date, sizeof(GLOG_DATETIME)) == 0)
      return 0; //failed getting the file's datetime

    GLOG_DATETIME now;
    tm tmp = ra::timing::GetLocalTime();
    tmp.tm_year += 1900;
    tmp.tm_mon += 1; //from [0,11] range to [1,12]
    now.year = tmp.tm_year;
    now.month = tmp.tm_mon;
    now.day = tmp.tm_mday;
    now.hour = tmp.tm_hour;
    now.minute = tmp.tm_min;
    now.second = tmp.tm_sec;

    int diff = GetDateTimeDiff(file_date, now);
    return diff;
  }

  const GLOG_DATETIME& GetInvalidLogDateTime()
  {
    static GLOG_DATETIME dt = { 0 };
    return dt;
  }

  GLOG_DATETIME GetFileDateTime(const std::string& path)
  {
    GLOG_DATETIME dt = { 0 };

    //sa.shellextension-d.dll.PCNAME.JohnSmith.log.INFO.20190503-180515.14920.log

    std::string filename = ra::filesystem::GetFilename(path.c_str());

    ra::strings::StringVector parts;
    ra::strings::Split(parts, filename, ".");

    //native format is expected to have 9 parts:
    //  0: module filename
    //  1: file extension
    //  2: level (INFO, WARNING, ERROR)
    //  3: date-time
    //  4: process id
    //  5: log

    if (parts.size() < 6)
      return GetInvalidLogDateTime(); //fail

    std::string datetime = parts[parts.size() - 3];
    ra::strings::Replace(datetime, "-", ""); //cleanup

    if (datetime.size() != 14)
      return GetInvalidLogDateTime(); //fail

    //extract fields
    std::string year = datetime.substr(0, 4);
    std::string month = datetime.substr(4, 2);
    std::string day = datetime.substr(6, 2);
    std::string hour = datetime.substr(8, 2);
    std::string minute = datetime.substr(10, 2);
    std::string second = datetime.substr(12, 2);

    //trimming
    year = ra::strings::TrimLeft(year, '0');
    month = ra::strings::TrimLeft(month, '0');
    day = ra::strings::TrimLeft(day, '0');
    hour = ra::strings::TrimLeft(hour, '0');
    minute = ra::strings::TrimLeft(minute, '0');
    second = ra::strings::TrimLeft(second, '0');

    //parsing
    bool parsed = true;
    parsed = parsed && ra::strings::Parse(year, dt.year);
    parsed = parsed && ra::strings::Parse(month, dt.month);
    parsed = parsed && ra::strings::Parse(day, dt.day);
    parsed = parsed && ra::strings::Parse(hour, dt.hour);
    parsed = parsed && ra::strings::Parse(minute, dt.minute);
    parsed = parsed && ra::strings::Parse(second, dt.second);

    if (!parsed)
      return GetInvalidLogDateTime();

    //success
    return dt;
  }

  std::string GetLogDestination(int level)
  {
    shellanything::App& app = shellanything::App::GetInstance();

    //For issue #7 - Change the default filename format for log files
    //NATIVE FORMAT:  sa.shellextension-d.dll.MYCOMPUTERNAME.johnsmith.log.INFO.20180120-124422.8732.log
    //DESIRED FORMAT: sa.shellextension-d.dll.INFO.20180120-124422.8732.log

    // The function google::SetLogDestination() is expecting a full path (including the destination directory)

    std::string module_path = GetCurrentModulePath();
    std::string module_filename = ra::filesystem::GetFilename(module_path.c_str());

    std::string path;

    path += app.GetLogDirectory();
    path += "\\";
    path += module_filename;
    path += ".";
    path += google::GetLogSeverityName(level);
    path += ".";

    return path;
  }

  std::string GetLogFilename(int level, const std::string& date, const std::string& time, uint32_t process_id)
  {
    std::string module_path = GetCurrentModulePath();
    std::string module_filename = ra::filesystem::GetFilename(module_path.c_str());

    std::string filename;

    filename.append(module_filename);
    filename.append(".");
    filename += google::GetLogSeverityName(level);

    filename.append(".");
    filename.append(date);
    filename.append("-");
    filename.append(time);
    filename.append(".");
    filename.append(ra::strings::ToString(process_id));
    filename.append(".log");

    return filename;
  }

  bool IsLogFile(const std::string& path)
  {
    std::string module_path = GetCurrentModulePath();
    std::string module_filename = ra::filesystem::GetFilename(module_path.c_str());

    //validate that 'path' contains the dll filename
    size_t pos = path.find(module_filename);
    if (pos == std::string::npos)
      return false;

    //validate the path's file extension
    std::string extension = ra::filesystem::GetFileExtention(path);
    extension = ra::strings::Uppercase(extension);
    if (extension != "LOG")
      return false;

    return true;
  }

  void DeletePreviousLogs(int max_age_seconds)
  {
    shellanything::App& app = shellanything::App::GetInstance();
    std::string log_dir = app.GetLogDirectory();
    ra::strings::StringVector files;
    bool success = ra::filesystem::FindFiles(files, log_dir.c_str());
    if (!success) return;

    //for each files
    for (size_t i = 0; i < files.size(); i++)
    {
      const std::string& path = files[i];
      if (IsLogFile(path))
      {
        //that's a log file

        //how old is this file ?
        int age = GetLogFileAge(path);
        bool old = (age > max_age_seconds);

        if (old)
          ra::filesystem::DeleteFile(path.c_str());
      }
    }
  }

  void DeletePreviousLogs()
  {
    static const int DAYS_TO_SECONDS = 86400;
    static const int MAX_SECONDS_OLD = 5 * DAYS_TO_SECONDS; //5 days old maximum
    DeletePreviousLogs(MAX_SECONDS_OLD);
  }

  void InitLogger()
  {
    shellanything::App& app = shellanything::App::GetInstance();

    //Create and define the LOG directory
    std::string log_dir = app.GetLogDirectory();

    //Try to create the directory
    if (!ra::filesystem::DirectoryExists(log_dir.c_str()))
    {
      bool created = ra::filesystem::CreateDirectory(log_dir.c_str());
      if (!created)
      {
        //Show an error message
        const std::string title = "ShellAnything init";
        const std::string message = ra::strings::Format("Failed creating log directory '%s'", log_dir.c_str());
        shellanything::ShowErrorMessage(title, message);
      }
    }

    //delete previous logs for easier debugging
    static const int DAYS_TO_SECONDS = 86400;
    if (!app.IsTestingEnvironment())
    {
      static const int MAX_10_DAYS_OLD = 10 * DAYS_TO_SECONDS; //10 days old maximum
      DeletePreviousLogs(MAX_10_DAYS_OLD);
    }
    else
    {
      // Delete all previous logs if we are in a testing environment.
      static const int MAX_1_SECOND_OLD = 1; //1 second old maximum
      DeletePreviousLogs(MAX_1_SECOND_OLD);
    }

    // Prepare Google's logging library.
    fLB::FLAGS_logtostderr = false; //on error, print to stdout instead of stderr
    fLB::FLAGS_log_prefix = 1; //prefix each message in file/console with 'E0405 19:13:07.577863  6652 sa.shellextension.cpp:847]'
    fLI::FLAGS_stderrthreshold = INT_MAX; //disable console output
    fLI::FLAGS_logbufsecs = 0; //flush logs after each LOG() calls
    fLI::FLAGS_minloglevel = google::GLOG_INFO;
    fLI::FLAGS_v = 4; //disables vlog(2), vlog(3), vlog(4)

    //if a log directory is specified, configure it now
    if (!log_dir.empty())
    {
      fLS::FLAGS_log_dir = log_dir;
    }

    //Issue #7 - Change the default filename format for log files
    std::string log_destination_info = GetLogDestination(google::GLOG_INFO);
    std::string log_destination_warning = GetLogDestination(google::GLOG_WARNING);
    std::string log_destination_error = GetLogDestination(google::GLOG_ERROR);
    google::SetLogDestination(google::GLOG_INFO, log_destination_info.c_str());
    google::SetLogDestination(google::GLOG_WARNING, log_destination_warning.c_str());
    google::SetLogDestination(google::GLOG_ERROR, log_destination_error.c_str());

    const std::vector<std::string> dirs = google::GetLoggingDirectories();

    google::SetLogFilenameExtension(".log");

    // Initialize Google's logging library.
    std::string dll_path = GetCurrentModulePath();
    memset(g_Path, 0, sizeof(g_Path));
    strcpy(g_Path, dll_path.c_str());

    //https://codeyarns.com/2017/10/26/how-to-install-and-use-glog/
    google::InitGoogleLogging(g_argv[0]);
  }

} //namespace shellanything
