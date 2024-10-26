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

#include "SaUtils.h"

#include "rapidassist/errors.h"
#include "rapidassist/unicode.h"
#include "rapidassist/strings.h"
#include "rapidassist/filesystem_utf8.h"

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h> //for MAX_PATH
#include "rapidassist/undef_windows_macros.h"

#include "Win32Registry.h"

 //External declarations
static const std::string  EMPTY_STRING;
static const std::wstring EMPTY_WIDE_STRING;

namespace shellanything
{
  extern void ShowErrorMessage(const std::string& title, const std::string& message);
} //namespace shellanything

std::string GetCurrentModulePath()
{
  std::string path;
  char buffer[MAX_PATH] = { 0 };
  HMODULE hModule = NULL;
  if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
      GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
      (LPCSTR) __FUNCTION__,
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

    return EMPTY_STRING;
  }

  /*get the path of this DLL*/
  GetModuleFileName(hModule, buffer, sizeof(buffer));
  if (buffer[0] != '\0')
  {
    path = buffer;
  }
  return path;
}

std::string GetCurrentModulePathUtf8()
{
  std::string path;
  wchar_t buffer[MAX_PATH] = { 0 };
  HMODULE hModule = NULL;
  if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
      GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
      (LPCWSTR)__FUNCTIONW__,
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

    return EMPTY_STRING;
  }

  /*get the path of this DLL*/
  GetModuleFileNameW(hModule, buffer, sizeof(buffer));
  if (buffer[0] != '\0')
  {
    path = ra::unicode::UnicodeToUtf8(buffer);
  }
  return path;
}

//Test if a directory allow read access to the current user.
//Note: the only way to detect if read access is available is to actually try to list files in the directory
bool HasDirectoryReadAccess(const std::string& path)
{
  //Check if the directory already exists
  if (!ra::filesystem::DirectoryExists(path.c_str()))
    return false; //Directory not found. Denied read access.

  ra::strings::StringVector files;
  bool success = ra::filesystem::FindFiles(files, path.c_str(), 0);
  if (!success)
    return false; // Unable to list files in the given directory

  if (files.empty())
    return true;   // Assume directory is empty

  return true;
}

//Test if a directory allow read access to the current user.
//Note: the only way to detect if read access is available is to actually try to list files in the directory
bool HasDirectoryReadAccessUtf8(const std::string& path)
{
  //Check if the directory already exists
  if (!ra::filesystem::DirectoryExistsUtf8(path.c_str()))
    return false; //Directory not found. Denied read access.

  ra::strings::StringVector files;
  bool success = ra::filesystem::FindFilesUtf8(files, path.c_str(), 0);
  if (!success)
    return false; // Unable to list files in the given directory

  if (files.empty())
    return true;   // Assume directory is empty

  return true;
}

//Test if a directory allow write access to the current user.
//Note: the only way to detect if write access is available is to actually write a file
bool HasDirectoryWriteAccess(const std::string& path)
{
  //Check if the directory already exists
  if (!ra::filesystem::DirectoryExists(path.c_str()))
    return false; //Directory not found. Denied write access.

  //Generate a random filename to use as a "temporary file".
  std::string filename = ra::filesystem::GetTemporaryFileName();

  //Try to create a file. This will validate that we have write access to the directory.
  std::string file_path = path + ra::filesystem::GetPathSeparatorStr() + filename;
  static const std::string data = __FUNCTION__;
  bool file_created = ra::filesystem::WriteFile(file_path, data);
  if (!file_created)
    return false; //Write is denied

  //Write is granted

  //Cleaning up
  bool deleted = ra::filesystem::DeleteFile(file_path.c_str());

  return true;
}

//Test if a directory allow write access to the current user.
//Note: the only way to detect if write access is available is to actually write a file
bool HasDirectoryWriteAccessUtf8(const std::string& path)
{
  //Check if the directory already exists
  if (!ra::filesystem::DirectoryExistsUtf8(path.c_str()))
    return false; //Directory not found. Denied write access.

  //Generate a random filename to use as a "temporary file".
  std::string filename = ra::filesystem::GetTemporaryFileName();

  //Try to create a file. This will validate that we have write access to the directory.
  std::string file_path = path + ra::filesystem::GetPathSeparatorStr() + filename;
  static const std::string data = __FUNCTION__;
  bool file_created = ra::filesystem::WriteFileUtf8(file_path, data);
  if (!file_created)
    return false; //Write is denied

  //Write is granted

  //Cleaning up
  bool deleted = ra::filesystem::DeleteFileUtf8(file_path.c_str());

  return true;
}

bool RenameFile(const std::string& old_path, const std::string& new_path)
{
  if (std::rename(old_path.c_str(), new_path.c_str()) < 0)
  {
    return false;
  }
  return true;
}

bool RenameFileUtf8(const std::string& old_path, const std::string& new_path)
{
#ifndef WIN32
  // Win32 API not available, proceed with a normal ansi rename
  return RenameFile(old_path, new_path);
#else
  std::wstring old_path_w = ra::unicode::Utf8ToUnicode(old_path);
  std::wstring new_path_w = ra::unicode::Utf8ToUnicode(new_path);
  if (_wrename(old_path_w.c_str(), new_path_w.c_str()) < 0)
  {
    return false;
  }
  return true;
#endif
}

bool IsFirstApplicationRun(const std::string& name, const std::string& version)
{
  std::string key = ra::strings::Format("HKEY_CURRENT_USER\\Software\\%s\\%s", name.c_str(), version.c_str());
  if (!Win32Registry::CreateKey(key.c_str(), NULL))
  {
    // unable to get to the application's key.
    // assume it is not the first run.
    return false;
  }

  static const char* FIRST_RUN_VALUE_NAME = "first_run";

  // try to read the value
  Win32Registry::MemoryBuffer value;
  Win32Registry::REGISTRY_TYPE value_type;
  if (!Win32Registry::GetValue(key.c_str(), FIRST_RUN_VALUE_NAME, value_type, value))
  {
    // the registry value is not found.
    // assume the application is run for the first time.

    // update the flag to "false" for the next call
    Win32Registry::SetValue(key.c_str(), FIRST_RUN_VALUE_NAME, "false"); //don't look at the write result

    return true;
  }

  bool first_run = ra::strings::ParseBoolean(value);

  if (first_run)
  {
    //update the flag to "false"
    Win32Registry::SetValue(key.c_str(), FIRST_RUN_VALUE_NAME, "false"); //don't look at the write result
  }

  return first_run;
}

bool IsPrintable(const char c)
{
  if (c >= 32 && c <= 126)
    return true;
  if (c == '\r' ||
      c == '\n' ||
      c == '\t')
    return true;
  return false;
}

bool IsPrintable(const std::string& value)
{
  for (size_t i = 0; i < value.size(); i++)
  {
    if (!IsPrintable(value[i]))
      return false;
  }
  return true;
}

bool IsPrintableUtf8(const std::string& value)
{
  const char* str = value.c_str();

  for (size_t i = 0; i < value.size(); i++)
  {
    const char* substr = &str[i];
    size_t code_point_len = GetFirstCodePointLengthUtf8(substr);
    if (code_point_len == std::string::npos)
      return false;
    if (code_point_len == 1)
    {
      if (!IsPrintable(substr[0]))
        return false;
    }
    else
    {
      // that is a code point
      i += (code_point_len - 1); // -1 because of the i++
    }
  }
  return true;
}

size_t GetFirstCodePointLengthUtf8(const char* value)
{
  const unsigned char* unsigned_str = (const unsigned char*)value;

  const unsigned char& c1 = unsigned_str[0];
  unsigned char c2 = unsigned_str[1];
  unsigned char c3 = unsigned_str[2];
  unsigned char c4 = unsigned_str[3];

  if (c1 == '\0')
    return 0;

  //prevent going outside of the string
  if (c1 == '\0')
    c2 = c3 = c4 = '\0';
  else if (c2 == '\0')
    c3 = c4 = '\0';
  else if (c3 == '\0')
    c4 = '\0';

  //size in bytes of the code point
  size_t n = 1;

  //See http://www.unicode.org/versions/Unicode6.0.0/ch03.pdf, Table 3-7. Well-Formed UTF-8 Byte Sequences
  // ## | Code Points         | First Byte | Second Byte | Third Byte | Fourth Byte
  // #1 | U+0000   - U+007F   | 00 - 7F    |             |            | 
  // #2 | U+0080   - U+07FF   | C2 - DF    | 80 - BF     |            | 
  // #3 | U+0800   - U+0FFF   | E0         | A0 - BF     | 80 - BF    | 
  // #4 | U+1000   - U+CFFF   | E1 - EC    | 80 - BF     | 80 - BF    | 
  // #5 | U+D000   - U+D7FF   | ED         | 80 - 9F     | 80 - BF    | 
  // #6 | U+E000   - U+FFFF   | EE - EF    | 80 - BF     | 80 - BF    | 
  // #7 | U+10000  - U+3FFFF  | F0         | 90 - BF     | 80 - BF    | 80 - BF
  // #8 | U+40000  - U+FFFFF  | F1 - F3    | 80 - BF     | 80 - BF    | 80 - BF
  // #9 | U+100000 - U+10FFFF | F4         | 80 - 8F     | 80 - BF    | 80 - BF

  if (c1 <= 0x7F) // #1 | U+0000   - U+007F, (ASCII)
    n = 1;
  else if (0xC2 <= c1 && c1 <= 0xDF &&
            0x80 <= c2 && c2 <= 0xBF)  // #2 | U+0080   - U+07FF
    n = 2;
  else if (0xE0 == c1 &&
            0xA0 <= c2 && c2 <= 0xBF &&
            0x80 <= c3 && c3 <= 0xBF)  // #3 | U+0800   - U+0FFF
    n = 3;
  else if (0xE1 <= c1 && c1 <= 0xEC &&
            0x80 <= c2 && c2 <= 0xBF &&
            0x80 <= c3 && c3 <= 0xBF)  // #4 | U+1000   - U+CFFF
    n = 3;
  else if (0xED == c1 &&
            0x80 <= c2 && c2 <= 0x9F &&
            0x80 <= c3 && c3 <= 0xBF)  // #5 | U+D000   - U+D7FF
    n = 3;
  else if (0xEE <= c1 && c1 <= 0xEF &&
            0x80 <= c2 && c2 <= 0xBF &&
            0x80 <= c3 && c3 <= 0xBF)  // #6 | U+E000   - U+FFFF
    n = 3;
  else if (0xF0 == c1 &&
            0x90 <= c2 && c2 <= 0xBF &&
            0x80 <= c3 && c3 <= 0xBF &&
            0x80 <= c4 && c4 <= 0xBF)  // #7 | U+10000  - U+3FFFF
    n = 4;
  else if (0xF1 <= c1 && c1 <= 0xF3 &&
            0x80 <= c2 && c2 <= 0xBF &&
            0x80 <= c3 && c3 <= 0xBF &&
            0x80 <= c4 && c4 <= 0xBF)  // #8 | U+40000  - U+FFFFF
    n = 4;
  else if (0xF4 == c1 &&
            0x80 <= c2 && c2 <= 0xBF &&
            0x80 <= c3 && c3 <= 0xBF &&
            0x80 <= c4 && c4 <= 0xBF)  // #7 | U+10000  - U+3FFFF
    n = 4;
  else
    return std::string::npos; // invalid UTF-8 sequence

  return n;
}

std::string ToHexString(const void* value)
{
  size_t address = reinterpret_cast<size_t>(value);
  char buffer[64];
  static bool is_32bit = (sizeof(address) == 4);
  static bool is_64bit = (sizeof(address) == 8);
#ifdef _WIN32
  if (is_32bit)
    sprintf(buffer, "0x%Ix", address);
  else if (is_64bit)
    sprintf(buffer, "0x%Ix", address);
#else
  if (is_32bit)
    sprintf(buffer, "0x%zx", address);
  else if (is_64bit)
    sprintf(buffer, "0x%zx", address);
#endif
  return buffer;
}

std::string ToHexString(const uint32_t value)
{
  char buffer[64];
  sprintf(buffer, "0x%x", value);
  return buffer;
}
