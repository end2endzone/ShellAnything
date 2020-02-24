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

  bool is_ascii(const char * str)
  {
    int offset = 0;
    while (str[offset] != '\0')
    {
      if (str[offset] < 0) //if bit7 is set
        return false;

      //next byte
      offset++;
    }
    return true;
  }

  bool is_cp1252_valid(const char * str)
  {
    int offset = 0;
    while (str[offset] != '\0')
    {
      const char & c = str[offset];
      if (
        c == 0x81 ||
        c == 0x8D ||
        c == 0x8F ||
        c == 0x90 ||
        c == 0x9D )
        return false;

      //next byte
      offset++;
    }
    return true;
  }

  bool is_iso8859_1_valid(const char * str)
  {
    int offset = 0;
    while (str[offset] != '\0')
    {
      const char & c = str[offset];
      if (0x00 <= c && c <= 0x1F)
        return false;
      if (0x7F <= c && c <= 0x9F)
        return false;

      //next byte
      offset++;
    }
    return true;
  }

  bool is_utf8_valid(const char * str)
  {
    int offset = 0;
    while (str[offset] != '\0')
    {
      const char & c1 = str[offset + 0];
      char c2 = str[offset + 1];
      char c3 = str[offset + 2];
      char c4 = str[offset + 3];
    
      //prevent going outside of the string
      if (c1 == '\0')
        c2 = c3 = c4 = '\0';
      else if (c2 == '\0')
        c3 = c4 = '\0';
      else if (c3 == '\0')
        c4 = '\0';

      //size in bytes of the code point
      int n = 1;

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
      else if ( 0xC2 <= c1 && c1 <= 0xDF &&
                0x80 <= c2 && c2 <= 0xBF)  // #2 | U+0080   - U+07FF
        n = 2;
      else if ( 0xE0 == c1 &&
                0xA0 <= c2 && c2 <= 0xBF &&
                0x80 <= c3 && c3 <= 0xBF)  // #3 | U+0800   - U+0FFF
        n = 3;
      else if ( 0xE1 <= c1 && c1 <= 0xEC &&
                0x80 <= c2 && c2 <= 0xBF &&
                0x80 <= c3 && c3 <= 0xBF)  // #4 | U+1000   - U+CFFF
        n = 3;
      else if ( 0xED == c1 &&
                0x80 <= c2 && c2 <= 0x9F &&
                0x80 <= c3 && c3 <= 0xBF)  // #5 | U+D000   - U+D7FF
        n = 3;
      else if ( 0xEE <= c1 && c1 <= 0xEF &&
                0x80 <= c2 && c2 <= 0xBF &&
                0x80 <= c3 && c3 <= 0xBF)  // #6 | U+E000   - U+FFFF
        n = 3;
      else if ( 0xF0 == c1 &&
                0x90 <= c2 && c2 <= 0xBF &&
                0x80 <= c3 && c3 <= 0xBF &&
                0x80 <= c4 && c4 <= 0xBF)  // #7 | U+10000  - U+3FFFF
        n = 4;
      else if ( 0xF1 <= c1 && c1 <= 0xF3 &&
                0x80 <= c2 && c2 <= 0xBF &&
                0x80 <= c3 && c3 <= 0xBF &&
                0x80 <= c4 && c4 <= 0xBF)  // #8 | U+40000  - U+FFFFF
        n = 4;
      else if ( 0xF4 == c1 &&
                0x80 <= c2 && c2 <= 0xBF &&
                0x80 <= c3 && c3 <= 0xBF &&
                0x80 <= c4 && c4 <= 0xBF)  // #7 | U+10000  - U+3FFFF
        n = 4;
      else
        return false; // invalid UTF-8 sequence

      //next code point
      offset += n;
    }
    return true;
  }

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
