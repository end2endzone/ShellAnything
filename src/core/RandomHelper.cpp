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

#include "RandomHelper.h"
#include "App.h"

#include "rapidassist/strings.h"

namespace shellanything
{

  const char RandomHelper::NUMERIC_DIGIT_PATTERN                      = '#';
  const char RandomHelper::NUMERIC_DIGIT_WITH_LEADING_ZEROS_PATTERN   = '0';
  const char RandomHelper::ALPHABETIC_LOWERCASE_LETTER_PATTERN        = 'a';
  const char RandomHelper::ALPHABETIC_UPPERCASE_LETTER_PATTERN        = 'A';
  const char RandomHelper::ALPHANUMERIC_CHARACTER_PATTERN_1           = 'z';
  const char RandomHelper::ALPHANUMERIC_CHARACTER_PATTERN_2           = 'Z';
  const char RandomHelper::ANY_CHARACTER_PATTERN                      = '*';
  const char RandomHelper::INVALID_PATTERN_CHARACTER                  = '.';

  static const std::string LOWERCASE_LETTERS = "abcdefghijklmnopqrstuvwxyz";
  static const std::string UPPERCASE_LETTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  static const std::string ALPHANUMERIC_CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  static const std::string ANY_CHARACTERS = "!#$()+,-.0123456789;=@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]_abcdefghijklmnopqrstuvwxyz{}~";

  static const uint32_t MAX_UINT32 = (uint32_t)-1;
  static const uint32_t MAX_DIGITS_32 = 10; // maximum number of digits in an unsigned 32 bit value (4,294,967,295)

  bool RandomHelper::IsNumericPattern(const std::string& value)
  {
    if (value.empty())
      return false;
    for (size_t i = 0; i < value.size(); i++)
    {
      bool numeric = (value[i] == NUMERIC_DIGIT_PATTERN || value[i] == NUMERIC_DIGIT_WITH_LEADING_ZEROS_PATTERN);
      if (!numeric)
        return false;
    }
    return true;
  }

  uint32_t RandomHelper::GetMaxValue(size_t num_digits)
  {
    if (num_digits >= MAX_DIGITS_32)
      return MAX_UINT32;
    uint32_t max_value = 0;
    while (num_digits > 0)
    {
      if (max_value != 0)
        max_value *= 10;
      max_value += 9;
      num_digits--;
    }
    return max_value;
  }

  std::string RandomHelper::GetRandomMinMaxValue(uint32_t min_value, uint32_t max_value)
  {
    IRandomService* random_service = App::GetInstance().GetRandomService();
    if (random_service == NULL) return "";

    uint32_t value = random_service->GetRandomValue(min_value, max_value);
    std::string output = ra::strings::ToString(value);
    return output;
  }

  void RandomHelper::FormatLeadingZeroes(std::string& value, size_t min_length)
  {
    while (value.size() < min_length)
    {
      value.insert(0, 1, '0');
    }
  }

  inline char GetRandomCharacterFromList(const std::string& chars)
  {
    IRandomService* random_service = App::GetInstance().GetRandomService();
    if (random_service == NULL) return '\0';

    uint32_t index = random_service->GetRandomValue(0, (uint32_t)chars.size());
    char output = chars[index];
    return output;
  }

  inline char GetRandomAlphabeticLowercaseLetter()
  {
    char output = GetRandomCharacterFromList(LOWERCASE_LETTERS);
    return output;
  }

  inline char GetRandomAlphabeticUppercaseLetter()
  {
    char output = GetRandomCharacterFromList(UPPERCASE_LETTERS);
    return output;
  }

  inline char GetRandomAlphanumericCharacter()
  {
    char output = GetRandomCharacterFromList(ALPHANUMERIC_CHARACTERS);
    return output;
  }

  inline char GetRandomAnyCharacter()
  {
    char output = GetRandomCharacterFromList(ANY_CHARACTERS);
    return output;
  }

  size_t RandomHelper::GetNumericPatternLength(const char* value)
  {
    if (value == NULL) return 0;
    char pattern = value[0];
    if (pattern != NUMERIC_DIGIT_PATTERN && pattern != NUMERIC_DIGIT_WITH_LEADING_ZEROS_PATTERN)
      return 0;

    size_t length = 0;
    while (*value == pattern)
    {
      length++;
      value++;
    }
    return length;
  }

  bool RandomHelper::GetRandomFromPattern(const std::string& pattern, std::string& value)
  {
    IRandomService* random_service = App::GetInstance().GetRandomService();
    if (random_service == NULL) return false;

    size_t numeric_digits_series_length = 0;
    size_t num_digits = 0;
    uint32_t min_value = 0;
    uint32_t max_value = 0;
    std::string numeric_value;
    std::string output;

    for (size_t i = 0; i < pattern.size(); i++)
    {
      const char c = pattern[i];
      switch (c)
      {
      case RandomHelper::NUMERIC_DIGIT_PATTERN:
        numeric_digits_series_length = GetNumericPatternLength(&pattern[i]);
        min_value = 0;
        max_value = GetMaxValue(numeric_digits_series_length);
        numeric_value = RandomHelper::GetRandomMinMaxValue(min_value, max_value + 1);
        output.append(numeric_value);
        i += (numeric_digits_series_length - 1); // -1 since the loop will also add 1
        break;
      case RandomHelper::NUMERIC_DIGIT_WITH_LEADING_ZEROS_PATTERN:
        numeric_digits_series_length = GetNumericPatternLength(&pattern[i]);
        min_value = 0;
        max_value = GetMaxValue(numeric_digits_series_length);
        numeric_value = RandomHelper::GetRandomMinMaxValue(min_value, max_value + 1);
        FormatLeadingZeroes(numeric_value, numeric_digits_series_length);
        output.append(numeric_value);
        i += (numeric_digits_series_length - 1); // -1 since the loop will also add 1
        break;
      case RandomHelper::ALPHABETIC_LOWERCASE_LETTER_PATTERN:
        output.append(1, GetRandomAlphabeticLowercaseLetter());
        break;
      case RandomHelper::ALPHABETIC_UPPERCASE_LETTER_PATTERN:
        output.append(1, GetRandomAlphabeticUppercaseLetter());
        break;
      case RandomHelper::ALPHANUMERIC_CHARACTER_PATTERN_1:
      case RandomHelper::ALPHANUMERIC_CHARACTER_PATTERN_2:
        output.append(1, GetRandomAlphanumericCharacter());
        break;
      case RandomHelper::ANY_CHARACTER_PATTERN:
        output.append(1, GetRandomAnyCharacter());
        break;
      default:
        return false;
      };
    }

    value = output;
    return true;
  }

} //namespace shellanything
