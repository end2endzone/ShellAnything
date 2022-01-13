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

#include "Unicode.h"

namespace shellanything
{

  //Based on https://github.com/end2endzone/RapidAssist/blob/master/include/rapidassist/unicode.h#L67
  size_t GetCodePointLengthUtf8(const char * str)
  {
    if (str == NULL)
      return 0;

    const unsigned char * unsigned_str = (const unsigned char *)str;

    //printf("str[%d]=%d\n", offset, (int)unsigned_str[offset]);
    const unsigned char & c1 = unsigned_str[0];
    unsigned char c2 = unsigned_str[1];
    unsigned char c3 = unsigned_str[2];
    unsigned char c4 = unsigned_str[3];
    
    //prevent looking outside of the string
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
      return 0; // invalid UTF-8 sequence

    return n;
  }

  size_t GetLengthUtf8(const char * str)
  {
    if (str == NULL)
      return 0;

    const unsigned char * unsigned_str = (const unsigned char *)str;

    size_t num_code_points = 0;
    size_t offset = 0;
    while (str[offset] != '\0')
    {
      size_t code_point_length_bytes = GetCodePointLengthUtf8(&str[offset]);
      if (code_point_length_bytes == 0)
        return 0; // invalid UTF-8 sequence

      num_code_points++;

      //next code point
      offset += code_point_length_bytes;
    }

    return num_code_points;
  }
  
  std::string SubstringUtf8(const char * str, size_t start, size_t length)
  {
    std::string output;

    if (str == NULL || length == 0)
      return output;

    const unsigned char * unsigned_str = (const unsigned char *)str;

    size_t input_num_code_points = 0;   // number of code points visited in the input string.
    size_t output_num_code_points = 0;  // number of code points in the output string.
    size_t offset = 0;
    while (str[offset] != '\0')
    {
      size_t code_point_length_bytes = GetCodePointLengthUtf8(&str[offset]);
      if (code_point_length_bytes == 0)
        return ""; // invalid UTF-8 sequence

      input_num_code_points++;

      //should we start extracting code points?
      if (input_num_code_points > start)
      {
        output.append(&str[offset], code_point_length_bytes);

        output_num_code_points++;
      }

      //did we get our requested length?
      if (output_num_code_points == length)
        return output;

      //next code point
      offset += code_point_length_bytes;
    }

    return output;
  }

} //namespace shellanything
