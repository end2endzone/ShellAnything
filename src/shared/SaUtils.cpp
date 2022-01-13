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

#include "ErrorManager.h"

#include "rapidassist/errors.h"
#include "rapidassist/unicode.h"

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h> //for MAX_PATH
#include "rapidassist/undef_windows_macros.h"

//External declarations
static const std::string  EMPTY_STRING;
static const std::wstring EMPTY_WIDE_STRING;

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
