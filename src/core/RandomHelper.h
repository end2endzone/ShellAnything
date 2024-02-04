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

#ifndef SA_RANDOM_HELPER_H
#define SA_RANDOM_HELPER_H

#include "IRandomService.h"
#include <string>

namespace shellanything
{

  /// <summary>
  /// Helper class for the random service
  /// </summary>
  class SHELLANYTHING_EXPORT RandomHelper
  {
  public:

    /// <summary>
    /// Pattern for a numerical digit.
    /// </summary>
    static const char NUMERIC_DIGIT_PATTERN;

    /// <summary>
    /// Pattern for a numerical digit with leading zeroes.
    /// </summary>
    static const char NUMERIC_DIGIT_WITH_LEADING_ZEROS_PATTERN;

    /// <summary>
    /// Pattern for an alphabetic lowercase letter.
    /// </summary>
    static const char ALPHABETIC_LOWERCASE_LETTER_PATTERN;

    /// <summary>
    /// Pattern for an alphabetic uppercase letter.
    /// </summary>
    static const char ALPHABETIC_UPPERCASE_LETTER_PATTERN;

    /// <summary>
    /// Pattern 1 for an alphanumeric character.
    /// </summary>
    static const char ALPHANUMERIC_CHARACTER_PATTERN_1;

    /// <summary>
    /// Pattern 2 for an alphanumeric character.
    /// </summary>
    static const char ALPHANUMERIC_CHARACTER_PATTERN_2;

    /// <summary>
    /// Pattern for any character.
    /// </summary>
    static const char ANY_CHARACTER_PATTERN;

    /// <summary>
    /// Character that represent an unknown pattern character.
    /// </summary>
    static const char INVALID_PATTERN_CHARACTER;

    /// <summary>
    /// Detect if the given random pattern contains only numeric digits.
    /// </summary>
    /// <param name="value">The value to validate.</param>
    /// <returns>Returns true if given random pattern contains only numeric digits. Returns false otherwise.</returns>
    static bool IsNumericPattern(const std::string & value);

    /// <summary>
    /// Get the maximum value of a number with the given number of digits.
    /// </summary>
    /// <param name="num_digits">The number of digits in the value.</param>
    /// <returns>Returns the maximum value of a number with the given number of digits.</returns>
    static uint32_t RandomHelper::GetMaxValue(size_t num_digits);

    /// <summary>
    /// Generate a random value within min_value (inclusive) and max_value (exclusive).
    /// </summary>
    /// <param name="min_value">The minimum possible value to generate.</param>
    /// <param name="max_value">The maximum possible value to generate.</param>
    /// <returns>Returns a random value within min_value (inclusive) and max_value (exclusive).</returns>
    static std::string GetRandomMinMaxValue(uint32_t min_value, uint32_t max_value);

    /// <summary>
    /// Format a value with leading zeroes until the returned value's length matches the given length.
    /// </summary>
    /// <param name="value">The value to add leading zeroes to.</param>
    /// <param name="min_length">The minimum length of the value.</param>
    static void FormatLeadingZeroes(std::string& value, size_t min_length);

    /// <summary>
    /// Get the length of a series of numerical pattern characters (digits).
    /// </summary>
    /// <param name="value">The value where the pattern is starting.</param>
    /// <returns>Returns the length of the consecutive numerical pattern characters. Returns 0 if the given value does not start with a numerical pattern.</returns>
    static size_t RandomHelper::GetNumericPatternLength(const char* value);

    /// <summary>
    /// Generate a random value from the given pattern.
    /// </summary>
    /// <param name="pattern">The pattern use for generating a value.</param>
    /// <param name="value">The output value that contains the random value.</param>
    /// <returns>Returns true if the operation is succesful. Returns false otherwise.</returns>
    static bool GetRandomFromPattern(const std::string& pattern, std::string& value);

  };

} //namespace shellanything

#endif //SA_KEYBOARD_HELPER_H
