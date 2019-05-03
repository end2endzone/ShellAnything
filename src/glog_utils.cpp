#include "glog_utils.h"
#include "Platform.h"

#include "rapidassist/strings.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/environment.h"

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
  void DeletePreviousLogs(const std::string & iDirectory)
  {
    std::string module_path = GetCurrentModulePath();
    std::string module_filename = ra::filesystem::getFilename(module_path.c_str());

    ra::strings::StringVector files;
    bool success = ra::filesystem::findFiles(files, iDirectory.c_str());
    if (!success) return;

    std::string pattern_prefix = iDirectory + ra::filesystem::getPathSeparatorStr() + module_filename;

    //for each files
    for(size_t i=0; i<files.size(); i++)
    {
      const std::string & path = files[i];
      if (path.find(pattern_prefix) != std::string::npos)
      {
        //that's a log file
        ra::filesystem::deleteFile(path.c_str());
      }
    }
  }

  std::string CreateLogDirectory()
  {
    //By default, GLOG will output log files in %TEMP% directory.
    //However, I prefer to use %USERPROFILE%\ShellAnything\Logs

    std::string log_dir = shellanything::getHomeDirectory();
    if (log_dir.empty())
      return log_dir; //FAILED getting HOME directory.

    //We got the %USERPROFILE% directory.
    //Now add our custom path to it
    log_dir.append("\\ShellAnything\\Logs");

    //Try to create the directory
    bool created = shellanything::createFolder(log_dir.c_str());
    if (!created)
    {
      //failed creating directory.
      //fallback to %TEMP%
      return EMPTY_STRING;
    }

    return log_dir;
  }

  void InitLogger()
  {
    //Create and define the LOG directory
    std::string log_dir = CreateLogDirectory();

    //delete previous logs for easier debugging
    const std::string temp_dir = ra::environment::getEnvironmentVariable("TEMP");
    DeletePreviousLogs(temp_dir);
    DeletePreviousLogs(log_dir);

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
