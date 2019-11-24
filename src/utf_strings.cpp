#include "utf_strings.h"

#include <Windows.h>
#undef GetEnvironmentVariable
#undef DeleteFile
#undef CreateDirectory
#undef CopyFile
#undef CreateFile

namespace encoding
{
namespace utf
{
  static const std::string  EMPTY_STRING;
  static const std::wstring EMPTY_WIDE_STRING;

  // Convert a wide Unicode string to an UTF8 string
  std::string unicode_to_utf8(const std::wstring & wstr)
  {
    if (wstr.empty()) return std::string();
    int num_characters = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    if (num_characters == 0)
      return EMPTY_STRING;
    std::string strTo(num_characters, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], num_characters, NULL, NULL);
    return strTo;
  }

  // Convert an UTF8 string to a wide Unicode String
  std::wstring utf8_to_unicode(const std::string & str)
  {
    if (str.empty()) return std::wstring();
    int num_characters = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    if (num_characters == 0)
      return EMPTY_WIDE_STRING;
    std::wstring wstrTo(num_characters, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], num_characters);
    return wstrTo;
  }

  // Convert an wide Unicode string to ANSI string
  std::string unicode_to_ansi(const std::wstring & wstr)
  {
    if (wstr.empty()) return std::string();
    int num_characters = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    if (num_characters == 0)
      return EMPTY_STRING;
    std::string strTo(num_characters, 0);
    WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &strTo[0], num_characters, NULL, NULL);
    return strTo;
  }

  // Convert an ANSI string to a wide Unicode String
  std::wstring ansi_to_unicode(const std::string & str)
  {
    if (str.empty()) return std::wstring();
    int num_characters = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
    if (num_characters == 0)
      return EMPTY_WIDE_STRING;
    std::wstring wstrTo(num_characters, 0);
    MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], num_characters);
    return wstrTo;
  }

  std::string utf8_to_ansi(const std::string & str)
  {
    std::wstring str_unicode = utf8_to_unicode(str);
    std::string str_ansi = unicode_to_ansi(str_unicode);
    return str_ansi;
  }

  std::string ansi_to_utf8(const std::string & str)
  {
    std::wstring str_unicode = ansi_to_unicode(str);
    std::string str_utf8 = unicode_to_utf8(str_unicode);
    return str_utf8;
  }

} //namespace utf
} //namespace encoding
