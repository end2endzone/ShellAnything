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

#include "Platform.h"
#include "rapidassist/environment.h"
#include "rapidassist/filesystem.h"
#include <stdio.h>
#include <direct.h>

#ifdef WIN32
//#   ifndef WIN32_LEAN_AND_MEAN
//#   define WIN32_LEAN_AND_MEAN 1
//#   endif
#   include <windows.h> // for GetModuleHandleEx()
#   include <Shlobj.h>
#   include <ShellAPI.h>
#   include <psapi.h>
#   pragma comment( lib, "psapi.lib" )
#endif

#include <Shlobj.h>

namespace shellanything
{

  std::string expand(const std::string & iValue)
  {
    std::string output = iValue;

    ra::strings::StringVector variables = getEnvironmentVariables();
    for(size_t i=0; i<variables.size(); i++)
    {
      const std::string & name = variables[i];

      std::string pattern = std::string("%") + name + std::string("%");
      std::string value = ra::environment::getEnvironmentVariable(name.c_str());

      //process with search and replace
      ra::strings::replace(output, pattern, value);

      //also look for case insensitive replacement
      std::string pattern_uppercase = ra::strings::uppercase(pattern);
      std::string output_uppercase = ra::strings::uppercase(output);
      size_t pattern_pos = output_uppercase.find(pattern_uppercase);
      while (pattern_pos != std::string::npos)
      {
        //extract the pattern from the value.
        //ie: the value contains %systemdrive% instead of the official %SystemDrive%
        std::string pattern2 = output.substr(pattern_pos, pattern.size());

        //process with search and replace using the unofficial pattern
        ra::strings::replace(output, pattern2, value);

        //search again for next pattern
        output_uppercase = ra::strings::uppercase(output);
        pattern_pos = output_uppercase.find(pattern_uppercase);
      }
    }

    return output;
  }
  ra::strings::StringVector getEnvironmentVariables()
  {
    ra::strings::StringVector vars;

    char *s = *environ;

    int i = 0;
    s = *(environ+i);

    while(s)
    {
      std::string definition = s;
      size_t offset = definition.find('=');
      if (offset != std::string::npos)
      {
        std::string name = definition.substr(0, offset);
        std::string value = definition.substr(offset+1);
        int a = 0;

        vars.push_back(name);
      }

      //next var
      i++; 
      s = *(environ+i);
    }

    return vars;
  }

#ifdef _WIN32
  inline std::string getWin32Directory(int csidl)
  {
    // https://stackoverflow.com/questions/18493484/shgetfolderpath-deprecated-what-is-alternative-to-retrieve-path-for-windows-fol
    // https://superuser.com/questions/150012/what-is-the-difference-between-local-and-roaming-folders
    // CSIDL_PROFILE          matches "C:\Users\JohnSmith"
    // CSIDL_PERSONAL         matches "C:\Users\JohnSmith\Documents"
    // CSIDL_APPDATA          matches "C:\Users\JohnSmith\AppData\Roaming"
    // CSIDL_LOCAL_APPDATA    matches "C:\Users\JohnSmith\AppData\Local"
    // CSIDL_COMMON_APPDATA   matches "C:\ProgramData"
    // CSIDL_COMMON_DOCUMENTS matches "C:\Users\Public\Documents"
    CHAR szPath[MAX_PATH];
    if(SUCCEEDED(SHGetSpecialFolderPath(NULL, szPath, csidl, FALSE)))
    {
      return szPath;
    }
    return "";
  }
#endif

  std::string getHomeDirectory()
  {
#ifdef _WIN32
    std::string dir = getWin32Directory(CSIDL_PROFILE);
    return dir;
#else
    return "/~";
#endif
  }

  std::string getApplicationsDataDirectory()
  {
#ifdef _WIN32
    std::string dir = getWin32Directory(CSIDL_LOCAL_APPDATA);
    return dir;
#endif
  }

  std::string getDocumentsDirectory()
  {
#ifdef _WIN32
    std::string dir = getWin32Directory(CSIDL_PERSONAL);
    return dir;
#endif
  }

  std::string getDesktopDirectory()
  {
#ifdef _WIN32
    std::string dir = getWin32Directory(CSIDL_DESKTOPDIRECTORY);
    return dir;
#endif
  }

  bool peekFile(const std::string & path, size_t size, std::string & content)
  {
    //static const std::string EMPTY;
    content = "";

    //allocate a buffer which can hold the content of the file
    size_t file_size = ra::filesystem::getFileSize(path.c_str());
    size_t max_read_size = (file_size < size ? file_size : size);

    FILE * f = fopen(path.c_str(), "rb");
    if (!f)
      return false;

    char * buffer = new char[max_read_size]; 
    if (!buffer)
    {
      fclose(f);
      return false;
    }
    memset(buffer, 0, max_read_size);

    //read the content
    size_t read_size = fread(buffer, 1, max_read_size, f);
    if (read_size != max_read_size)
    {
      delete[] buffer;
      fclose(f);
      return false;
    }

    fclose(f);

    //copy the content of the buffer to the output string
    content.assign(buffer, max_read_size);

    bool success = (content.size() == max_read_size);
    return success;
  }

  bool readFile(const std::string & path, std::string & content)
  {
    uint32_t file_size = ra::filesystem::getFileSize(path.c_str());
    bool readed = peekFile(path, file_size, content);
    return readed;
  }

  bool writeFile(const std::string & path, std::string & content)
  {
    FILE * f = fopen(path.c_str(), "wb");
    if (!f)
      return false;
 
    size_t size_write = fwrite(content.c_str(), 1, content.size(), f);
 
    fclose(f);
 
    bool success = (content.size() == size_write);
    return success;
  }

  bool createFolder(const char * iPath)
  {
    if (iPath == NULL)
      return false;

    if (ra::filesystem::folderExists(iPath))
      return true;

    //folder does not already exists and must be created

    //inspired from https://stackoverflow.com/a/675193
    char *pp;
    char *sp;
    int   status;
    char separator = ra::filesystem::getPathSeparator();
#ifdef _WIN32
    char *copypath = _strdup(iPath);
#else
    char *copypath = strdup(iPath);
    static const mode_t mode = 0755;
#endif

    status = 0;
    pp = copypath;
    while (status == 0 && (sp = strchr(pp, separator)) != 0)
    {
      if (sp != pp)
      {
        /* Neither root nor double slash in path */
        *sp = '\0';
#ifdef _WIN32
        status = _mkdir(copypath);
        if (status == -1 && strlen(copypath) == 2 && copypath[1] == ':') //issue #27
          status = 0; //fix for _mkdir("C:") like
        int errno_copy = errno;
        if (status == -1 && errno == EEXIST)
          status = 0; //File already exist
#else
        status = mkdir(copypath, mode);
#endif
        if (status != 0)
        {
          //folder already exists?
          if (ra::filesystem::folderExists(copypath))
          {
            status = 0;
          }
        }
        *sp = separator;
      }
      pp = sp + 1;
    }
    if (status == 0)
    {
#ifdef _WIN32
      status = _mkdir(iPath);
#else
      status = mkdir(iPath, mode);
#endif
    }
    free(copypath);
    return (status == 0);      
  }

  bool copyFileInternal(const std::string & source_path, const std::string & destination_path, IProgressReport * progress_functor, ProgressReportCallback progress_function)
  {
    size_t file_size = ra::filesystem::getFileSize(source_path.c_str());

    FILE* fin = fopen(source_path.c_str(), "rb");
    if (!fin)
      return false;

    FILE* fout = fopen(destination_path.c_str(), "wb");
    if (!fout)
    {
      fclose(fin);
      return false;
    }

    //publish progress
    double progress = 0.0;
    if (progress_functor)
      progress_functor->onProgressReport(progress);
    if (progress_function)
      progress_function(progress);

    const size_t bufferSize = 100*1024; //100k memory buffer
    uint8_t buffer[bufferSize];

    size_t copied_size = 0;

    while( !feof(fin) )
    {
      size_t size_readed = fread(buffer, 1, bufferSize, fin);
      if (size_readed)
      {
        size_t size_writen = fwrite(buffer, 1, size_readed, fout);
        copied_size += size_writen;

        //publish progress
        progress = double(copied_size) / double(file_size);
        if (progress_functor)
          progress_functor->onProgressReport(progress);
        if (progress_function)
          progress_function(progress);
      }
    }

    fclose(fin);
    fclose(fout);

    bool success = (file_size == copied_size);

    if (success && progress < 1.0) //if 100% progress not already sent
    {
      //publish progress
      progress = 1.0;
      if (progress_functor)
        progress_functor->onProgressReport(progress);
      if (progress_function)
        progress_function(progress);
    }

    return success;
  }

  bool copyFile(const std::string & source_path, const std::string & destination_path)
  {
    return copyFileInternal(source_path, destination_path, NULL, NULL);
  }

  bool copyFile(const std::string & source_path, const std::string & destination_path, IProgressReport * progress_functor)
  {
    return copyFileInternal(source_path, destination_path, progress_functor, NULL);
  }

  bool copyFile(const std::string & source_path, const std::string & destination_path, ProgressReportCallback progress_function)
  {
    return copyFileInternal(source_path, destination_path, NULL, progress_function);
  }
  std::string getFilenameWithoutExtension(const char * iPath)
  {
    if (iPath == NULL)
      return "";

    std::string filename = ra::filesystem::getFilename(iPath);
    std::string extension = ra::filesystem::getFileExtention(iPath);
  
    //extract filename without extension
    std::string filenameWE = filename.substr(0, filename.size() - extension.size());
  
    //remove last dot of the filename if required
    filenameWE = ra::strings::trimRight(filenameWE, '.');

    return filenameWE;
  }

  std::string makeRelativePath(const std::string & base_path, const std::string & test_path)
  {
    static const std::string path_separator = ra::filesystem::getPathSeparatorStr();
 
    ra::strings::StringVector base_path_parts = ra::strings::split(base_path, path_separator.c_str());
    ra::strings::StringVector test_path_parts = ra::strings::split(test_path, path_separator.c_str());
 
    bool have_common_base = false; //true if base_path and test_path share a common base
 
    //remove beginning of both parts while they match
    while(base_path_parts.size() > 1 && test_path_parts.size() > 1 && base_path_parts[0] == test_path_parts[0])
    {
      //remove first
      base_path_parts.erase(base_path_parts.begin());
      test_path_parts.erase(test_path_parts.begin());
 
      have_common_base = true;
    }
 
    if (have_common_base)
    {
      //resolve:
      //  from base_path,
      //  go up as many element in base_path_parts,
      //  then go through test_path_parts
 
      for(size_t i=0; i<base_path_parts.size(); i++)
      {
        base_path_parts[i] = "..";
      }
 
      std::string new_base_path = ra::strings::join(base_path_parts, path_separator.c_str());
      std::string new_test_path = ra::strings::join(test_path_parts, path_separator.c_str());
 
      std::string relative_path = new_base_path + path_separator + new_test_path;
      return relative_path;
    }
 
    //failed making path relative
    return "";
  }

  uint32_t startProcess(const std::string & iCommand, const std::string & iDefaultDirectory)
  {
    PROCESS_INFORMATION processInfo = {0};
    
    STARTUPINFO startupInfo = {0};
    startupInfo.cb = sizeof(STARTUPINFO);
    startupInfo.dwFlags = STARTF_USESHOWWINDOW;
    startupInfo.wShowWindow = SW_SHOWDEFAULT; //SW_SHOW, SW_SHOWNORMAL

    DWORD creationFlags = 0; //EXTENDED_STARTUPINFO_PRESENT

    bool success = (CreateProcess(NULL, (char*)iCommand.c_str(), NULL, NULL, FALSE, creationFlags, NULL, iDefaultDirectory.c_str(), &startupInfo, &processInfo) != 0);
    if (success)
    {
      //Wait for the application to initialize properly
      WaitForInputIdle(processInfo.hProcess, INFINITE);

      //Extract the program id
      uint32_t pId = processInfo.dwProcessId;
      return pId;
    }
    return 0;
  }

  uint32_t startProcess(const std::string & iExecPath, const std::string & iArguments, const std::string & iDefaultDirectory)
  {
    //merge iExecPath with iArguments
    std::string command;

    //handle iExecPath
    if (!iExecPath.empty())
    {
      if (iExecPath.find(" ") != std::string::npos)
      {
        command += "\"";
        command += iExecPath;
        command += "\"";
      }
      else
        command += iExecPath;
    }

    if (!iArguments.empty())
    {
      command += " ";
      command += iArguments;
    }

    if (command.size() > 0)
    {
      return startProcess(command, iDefaultDirectory);
    }

    return 0;
  }

  uint32_t openDocument(const std::string & iPath)
  {
    SHELLEXECUTEINFO info = {0};

    info.cbSize = sizeof(SHELLEXECUTEINFO);
    
    info.fMask |= SEE_MASK_NOCLOSEPROCESS;
    info.fMask |= SEE_MASK_NOASYNC;
    info.fMask |= SEE_MASK_FLAG_DDEWAIT;

    info.hwnd = HWND_DESKTOP;
    info.nShow = SW_SHOWDEFAULT;
    info.lpVerb = "open";
    info.lpFile = iPath.c_str();
    info.lpParameters = NULL; //arguments
    info.lpDirectory = NULL; // default directory

    BOOL success = ShellExecuteEx(&info);
    if (success)
    {
      HANDLE hProcess = info.hProcess;
      DWORD wPid = GetProcessId(hProcess);
      return wPid;
    }

    return 0;
  }

  uint32_t GetSystemErrorCode()
  {
    DWORD dwError = GetLastError();
    return dwError;
  }

  std::string GetSystemErrorDescription(uint32_t dwError)
  {
    LPSTR buffer = NULL;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                  NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, NULL);

    //Make a copy
    std::string message(buffer, size);

    //Free the buffer.
    LocalFree(buffer);

    return message;
  }

  void ShowErrorMessage(const std::string & title, const std::string & message)
  {
    HWND hWnd = GetDesktopWindow();
    MessageBox(hWnd, message.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
  }

} //namespace shellanything
