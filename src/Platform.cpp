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

  bool readFile(const std::string & path, std::string & content)
  {
    //static const std::string EMPTY;
    content = "";

    //allocate a buffer which can hold the content of the file
    uint32_t file_size = ra::filesystem::getFileSize(path.c_str());
    //uint32_t buffer_size = file_size + 1; //+1 for the ending \0 character

    FILE * f = fopen(path.c_str(), "rb");
    if (!f)
      return false;

    char * buffer = new char[file_size]; 
    if (!buffer)
    {
      fclose(f);
      return false;
    }
    memset(buffer, 0, file_size);

    //read the content
    size_t read_size = fread(buffer, 1, file_size, f);
    if (read_size != file_size)
    {
      delete[] buffer;
      fclose(f);
      return false;
    }

    fclose(f);

    //copy the content of the buffer to the output string
    content.assign(buffer, file_size);

    bool success = (content.size() == file_size);
    return success;
  }

} //namespace shellanything
