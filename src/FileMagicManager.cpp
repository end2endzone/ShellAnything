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

#include <windows.h>

#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/unicode.h"
#include "rapidassist/errors.h"

#include "FileMagicManager.h"
#include "ErrorManager.h"


std::string GetMGCPath()
{
  std::string path;
  wchar_t buffer[MAX_PATH] = {0};
  HMODULE hModule = NULL;
  if (!GetModuleHandleExW( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          (LPCWSTR) __FUNCTIONW__,
                          &hModule))
  {
    uint32_t error_code = ra::errors::GetLastErrorCode();
    std::string desc = ra::errors::GetErrorCodeDescription(error_code);
    std::string message = std::string() +
      "Error in function '" + __FUNCTION__ + "()', file '" + __FILE__ + "', line " + ra::strings::ToString(__LINE__) + ".\n" +
      "\n" +
      "Failed getting the file path of the current module.\n" +
      "The following error code was returned:\n" +
      "\n" +
      ra::strings::Format("0x%08x", error_code) + ": " + desc;

    //display an error on 
    shellanything::ShowErrorMessage("ShellAnything Error", message);

    return std::string();
  }

  /*get the path of this DLL*/
  GetModuleFileNameW(hModule, buffer, sizeof(buffer));
  if (buffer[0] != '\0')
  {
    path = ra::unicode::UnicodeToUtf8(buffer);
  }
  
  std::string mgc_path = ra::filesystem::GetParentPath(path) + "\\magic.mgc";
  return mgc_path;
}

namespace shellanything
{
  FileMagicManager::FileMagicManager()
  {
    magic_cookie = magic_open(MAGIC_NONE);
    std::string path = GetMGCPath();

    if (magic_cookie == NULL) {
      ShowErrorMessage("ShellAnything(libmagic) Error", "ERROR opening MAGIC_MIME_TYPE: out of memory\n");
      return;
    }
    if (magic_load(magic_cookie, path.c_str()) == -1) {
      magic_cookie = NULL;
      ShowErrorMessage("ShellAnything(libmagic) Error", std::string("ERROR loading with NULL file: ") + magic_error(magic_cookie));
      return;
    }
  }

  FileMagicManager::~FileMagicManager()
  {
    magic_close(magic_cookie);
    magic_cookie = NULL;
  }
  
  FileMagicManager & FileMagicManager::GetInstance()
  {
    static FileMagicManager _instance;
    return _instance;
  }

  std::string FileMagicManager::GetMIMEType(const std::string & path) const
  {
    magic_setflags(magic_cookie, MAGIC_MIME_TYPE);
    const char *result = magic_file(magic_cookie, path.c_str());
    if (result == NULL)
    {
      ShowErrorMessage("ShellAnything(libmagic) Error", std::string("ERROR loading file: ") + magic_error(magic_cookie));
      return std::string();
    }
    else
    {
      return std::string(result);
    }
  }

  std::string FileMagicManager::GetDescription(const std::string & path) const
  {
    magic_setflags(magic_cookie, MAGIC_NONE);
    const char *result = magic_file(magic_cookie, path.c_str());
    if (result == NULL)
    {
      ShowErrorMessage("ShellAnything(libmagic) Error", std::string("ERROR loading file: ") + magic_error(magic_cookie));
      return std::string();
    }
    else
    {
      return std::string(result);
    }
  }

  std::string FileMagicManager::GetExtension(const std::string & path) const
  {
    magic_setflags(magic_cookie, MAGIC_EXTENSION);
    const char *result = magic_file(magic_cookie, path.c_str());
    if (result == NULL)
    {
      ShowErrorMessage("ShellAnything(libmagic) Error", std::string("ERROR loading file: ") + magic_error(magic_cookie));
      return std::string();
    }
    else
    {
      return std::string(result);
    }
  }

  std::string FileMagicManager::GetCharset(const std::string & path) const
  {
    magic_setflags(magic_cookie, MAGIC_MIME_ENCODING);
    const char *result = magic_file(magic_cookie, path.c_str());
    if (result == NULL)
    {
      ShowErrorMessage("ShellAnything(libmagic) Error", std::string("ERROR loading file: ") + magic_error(magic_cookie));
      return std::string();
    }
    else
    {
      return std::string(result);
    }
  }

} //shellanything