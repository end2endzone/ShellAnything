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

#include "shellanything/Platform.h"
#include "rapidassist/environment.h"
#include "rapidassist/filesystem.h"
#include <stdio.h>

#ifdef WIN32
#   ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN 1
#   endif
#   include <windows.h> // for GetModuleHandleEx()
#   include <Shlobj.h>
#endif

namespace shellanything
{

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

  std::string getHomeDirectory()
  {
#ifdef _WIN32
    CHAR szPath[MAX_PATH];
    if(SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, szPath)))
    {
      return szPath;
    }
    return "";
#else
    return "/~";
#endif
  }

  std::string getApplicationsDataDirectory()
  {
#ifdef _WIN32
    // https://superuser.com/questions/150012/what-is-the-difference-between-local-and-roaming-folders
    // CSIDL_APPDATA matches "C:\Users\beauchamp.a3\AppData\Roaming"
    // CSIDL_COMMON_APPDATA matches "C:\ProgramData"
    // CSIDL_LOCAL_APPDATA matches "C:\Users\beauchamp.a3\AppData\Local"
    CHAR szPath[MAX_PATH];
    if(SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA , NULL, 0, szPath)))
    {
      return szPath;
    }
    return "";
#endif
  }

  std::string getDocumentsDirectory()
  {
#ifdef _WIN32
    CHAR szPath[MAX_PATH];
    if(SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_COMMON_DOCUMENTS , NULL, 0, szPath)))
    {
      return szPath;
    }
    return "";
#endif
  }

  std::string getDesktopDirectory()
  {
#ifdef _WIN32
    std::string userprofile = ra::environment::getEnvironmentVariable("USERPROFILE");
    std::string dektop_dir = userprofile + "\\Desktop";
    return dektop_dir;
#endif
  }

  bool peekFile(const std::string & path, size_t size, std::string & content)
  {
    //static const std::string EMPTY;
    content = "";

    //allocate a buffer which can hold the content of the file
    uint32_t file_size = ra::filesystem::getFileSize(path.c_str());
    uint32_t max_read_size = (file_size < size ? file_size : size);

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

} //namespace shellanything
