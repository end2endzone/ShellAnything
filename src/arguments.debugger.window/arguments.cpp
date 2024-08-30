#include "arguments.h"

#include <windows.h>

// Convert an ANSI string to a wide Unicode String
std::wstring AnsiToUnicode(const std::string & str)
{
  if ( str.empty() ) return std::wstring();
  int num_characters = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
  if ( num_characters == 0 )
    return EMPTY_WIDE_STRING;
  std::wstring output(num_characters, 0);
  MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &output[0], num_characters);
  return output;
}

// Convert an wide Unicode string to ANSI string
std::string UnicodeToAnsi(const std::wstring & wstr)
{
  if ( wstr.empty() ) return std::string();
  int num_characters = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
  if ( num_characters == 0 )
    return EMPTY_ANSI_STRING;
  std::string output(num_characters, 0);
  WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &output[0], num_characters, NULL, NULL);
  return output;
}

void tstring_t_Append(tstring_t & str, const int & value)
{
  #ifdef UNICODE
  str += std::to_wstring(value);
  #else  
  str += std::to_string(value);
  #endif  
}

void tstring_t_Append(tstring_t & str, const char * value)
{
  #ifdef UNICODE
  str += AnsiToUnicode(value);
  #else  
  str += value;
  #endif  
}

void tstring_t_Append(tstring_t & str, const WCHAR * value)
{
  #ifdef UNICODE
  str += value;
  #else  
  str += UnicodeToAnsi(value);
  #endif  
}

void ReadCommandLineArguments(tstring_t & str)
{
  str.clear();

  // Get argv/argc
  #ifdef UNICODE
  TCHAR ** argv = __wargv;
  #else  
  TCHAR ** argv = __argv;
  #endif
  int argc = __argc;

  // argc
  str += L"argc=";
  tstring_t_Append(str, argc);
  str += L"\r\n";
  str += L"\r\n";

  // argv
  for ( int i = 0; i < argc; i++ )
  {
    str += L"argv[";
    tstring_t_Append(str, i);
    str += L"]=";

    tstring_t_Append(str, argv[i]);

    str += L"\r\n";
  }
}
