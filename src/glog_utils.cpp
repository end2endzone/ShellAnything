#include "glog_utils.h"
#include "Platform.h"

#include "rapidassist/strings.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/environment.h"
#include "rapidassist/time_.h"

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h> //for MAX_PATH

//Global declarations
char      g_Path[MAX_PATH];         // Path to this DLL. 
char *    g_argv[] = {g_Path, ""};  // For google::InitGoogleLogging(g_argv[0])

//External declarations
static const std::string  EMPTY_STRING;
static const std::wstring EMPTY_WIDE_STRING;
extern std::string GetCurrentModulePath();

namespace shellanything
{
  int GetDateDiffAbsolute(const GLOG_DATETIME & dt)
  {
    int total_seconds = 0;
    total_seconds += dt.year   *60*60*24*31*12;
    total_seconds += dt.month  *60*60*24*31; //rouding to 31 days per month
    total_seconds += dt.day    *60*60*24;
    total_seconds += dt.hour   *60*60;
    total_seconds += dt.minute *60;
    total_seconds += dt.second *1;
    return total_seconds;
  }

  int GetDateDiff(const GLOG_DATETIME & dt1, const GLOG_DATETIME & dt2)
  {
    int seconds1 = GetDateDiffAbsolute(dt1);
    int seconds2 = GetDateDiffAbsolute(dt2);
    int diff = seconds2 - seconds1;
    return diff;
  }

  int GetLogFileAge(const std::string & path)
  {
    //when did we created this file ?
    const GLOG_DATETIME      file_date = GetLogDateTime(path);
    const GLOG_DATETIME & invalid_date = GetInvalidLogDateTime();
    if (memcmp(&file_date, &invalid_date, sizeof(GLOG_DATETIME)) == 0)
      return 999999999; //failed getting the file's datetime

    GLOG_DATETIME now;
    tm tmp = ra::time::getLocalTime();
    tmp.tm_year += 1900;
    tmp.tm_mon += 1; //from [0,11] range to [1,12]
    now.year   = tmp.tm_year  ;
    now.month  = tmp.tm_mon   ;
    now.day    = tmp.tm_mday  ;
    now.hour   = tmp.tm_hour  ;
    now.minute = tmp.tm_min   ;
    now.second = tmp.tm_sec   ;

    int diff = GetDateDiff(file_date, now);
    return diff;
  }

  const GLOG_DATETIME & GetInvalidLogDateTime()
  {
    static GLOG_DATETIME dt = {0};
    return dt;
  }

  GLOG_DATETIME GetLogDateTime(const std::string & path)
  {
    GLOG_DATETIME dt = {0};

    //shellext-d.dll.PCNAME.JohnSmith.log.INFO.20190503-180515.14920.log
    
    std::string filename = ra::filesystem::getFilename(path.c_str());
    
    ra::strings::StringVector parts;
    ra::strings::split(parts, filename, ".");

    //native format is expected to have 9 parts:
    //  0: module filename
    //  1: file extension
    //  2: %COMPUTERNAME%
    //  3: %USERNAME%
    //  4: log
    //  5: level (INFO, WARNING, ERROR)
    //  6: date-time
    //  7: process id
    //  8: log

    if (parts.size() < 9)
      return GetInvalidLogDateTime(); //fail

    std::string datetime = parts[parts.size()-3];
    ra::strings::replace(datetime, "-", ""); //cleanup

    if (datetime.size() != 14)
      return GetInvalidLogDateTime(); //fail

    //extract fields
    std::string year   = datetime.substr( 0, 4);
    std::string month  = datetime.substr( 4, 2);
    std::string day    = datetime.substr( 6, 2);
    std::string hour   = datetime.substr( 8, 2);
    std::string minute = datetime.substr(10, 2);
    std::string second = datetime.substr(12, 2);

    //trimming
    year   = ra::strings::trimLeft(year  , '0');
    month  = ra::strings::trimLeft(month , '0');
    day    = ra::strings::trimLeft(day   , '0');
    hour   = ra::strings::trimLeft(hour  , '0');
    minute = ra::strings::trimLeft(minute, '0');
    second = ra::strings::trimLeft(second, '0');

    //parsing
    bool parsed = true;
    parsed = parsed && ra::strings::parse(year  , dt.year  );
    parsed = parsed && ra::strings::parse(month , dt.month );
    parsed = parsed && ra::strings::parse(day   , dt.day   );
    parsed = parsed && ra::strings::parse(hour  , dt.hour  );
    parsed = parsed && ra::strings::parse(minute, dt.minute);
    parsed = parsed && ra::strings::parse(second, dt.second);

    if (!parsed)
      return GetInvalidLogDateTime(); 

    //success
    return dt;
  }

  std::string GetLogFilename(int level, const std::string & date, const std::string & time, uint32_t process_id)
  {
    std::string module_path = GetCurrentModulePath();
    std::string module_filename = ra::filesystem::getFilename(module_path.c_str());
    /*std::string module_extension = ra::filesystem::getFileExtention(module_filename);
    std::string module_filename_we = module_filename;
    if (!module_extension.empty())
    {
      size_t length = module_filename.size() - module_extension.size() - 1;
      module_filename_we = module_filename.substr(0, length);
    };*/

    std::string filename;
    filename.append(module_filename);
    filename.append(".");
    filename.append(ra::environment::getEnvironmentVariable("USERDOMAIN"));
    filename.append(".");
    filename.append(ra::environment::getEnvironmentVariable("USERNAME"));
    filename.append(".log");

    filename.append(".");
    const char * level_str = google::GetLogSeverityName(level);
    filename.append(level_str);

    filename.append(".");
    filename.append(date);
    filename.append("-");
    filename.append(time);
    filename.append(".");
    filename.append(ra::strings::toString(process_id));
    filename.append(".log");

    return filename;
  }

  std::string GetLogDirectory()
  {
    //By default, GLOG will output log files in %TEMP% directory.
    //However, I prefer to use %USERPROFILE%\ShellAnything\Logs

    std::string log_dir = shellanything::getHomeDirectory();
    if (!log_dir.empty())
    {
      //We got the %USERPROFILE% directory.
      //Now add our custom path to it
      log_dir.append("\\ShellAnything\\Logs");
      
      return log_dir;
    }

    //Failed getting HOME directory.
    //Fallback to using %TEMP%.
    log_dir = ra::environment::getEnvironmentVariable("TEMP");
    return log_dir;
  }

  bool IsLogFile(const std::string & path)
  {
    std::string module_path = GetCurrentModulePath();
    std::string module_filename = ra::filesystem::getFilename(module_path.c_str());
    /*std::string module_extension = ra::filesystem::getFileExtention(module_filename);
    std::string module_filename_we = module_filename;
    if (!module_extension.empty())
    {
      size_t length = module_filename.size() - module_extension.size() - 1;
      module_filename_we = module_filename.substr(0, length);
    };*/

    //validate that 'path' contains the dll filename
    size_t pos = path.find(module_filename);
    if (pos == std::string::npos)
      return false;

    //validate the path's file extension
    std::string extension = ra::filesystem::getFileExtention(path);
    extension = ra::strings::uppercase(extension);
    if (extension != "LOG")
      return false;

    return true;
  }

  void DeletePreviousLogs(int max_age_seconds)
  {
    std::string log_dir = GetLogDirectory();
    ra::strings::StringVector files;
    bool success = ra::filesystem::findFiles(files, log_dir.c_str());
    if (!success) return;

    //for each files
    for(size_t i=0; i<files.size(); i++)
    {
      const std::string & path = files[i];
      if (IsLogFile(path))
      {
        //that's a log file

        //how old is this file ?
        int age = GetLogFileAge(path);
        bool old = (age > max_age_seconds);

        if (old)
          ra::filesystem::deleteFile(path.c_str());
      }
    }
  }

  void DeletePreviousLogs()
  {
    static const int DAYS_TO_SECONDS = 86400;
    static const int MAX_5_DAYS_OLD = 10*DAYS_TO_SECONDS; //10 days old maximum
    DeletePreviousLogs(MAX_5_DAYS_OLD);
  }

  void InitLogger()
  {
    //Create and define the LOG directory
    std::string log_dir = GetLogDirectory();

    //Try to create the directory
    if (!ra::filesystem::folderExists(log_dir.c_str()))
    {
      bool created = shellanything::createFolder(log_dir.c_str());
      if (!created)
      {
        //Show an error message
        const std::string title = "ShellAnything init";
        const std::string message = ra::strings::format("Failed creating log directory '%s'", log_dir.c_str());
        ShowErrorMessage(title, message);
      }
    }

    //delete previous logs for easier debugging
    static const int DAYS_TO_SECONDS = 86400;
    static const int MAX_5_DAYS_OLD = 10*DAYS_TO_SECONDS; //10 days old maximum
    DeletePreviousLogs(MAX_5_DAYS_OLD);

    // Prepare Google's logging library.
    fLB::FLAGS_logtostderr = false; //on error, print to stdout instead of stderr
    fLB::FLAGS_log_prefix = 1; //prefix each message in file/console with 'E0405 19:13:07.577863  6652 shellext.cpp:847]'
    fLI::FLAGS_stderrthreshold = INT_MAX; //disable console output
    fLI::FLAGS_logbufsecs = 0; //flush logs after each LOG() calls
    fLI::FLAGS_minloglevel = google::GLOG_INFO;
    fLI::FLAGS_v = 4; //disables vlog(2), vlog(3), vlog(4)

    //if a log directory is specified, configure it now
    if (!log_dir.empty())
    {
      fLS::FLAGS_log_dir = log_dir;
    }
 
    //Do not change the default output files...
    //google::SetLogDestination(google::GLOG_INFO,    "shellext.dll.INFO");
    //google::SetLogDestination(google::GLOG_WARNING, "shellext.dll.WARNING");
    //google::SetLogDestination(google::GLOG_ERROR,   "shellext.dll.ERROR");
 
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
