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

#ifndef SA_WILDCARD_H
#define SA_WILDCARD_H

#include "shellanything/export.h"
#include "shellanything/config.h"

namespace shellanything
{
  /// <summary>
  /// Defines a wildcard expansion solution to match a given string.
  /// </summary>
  struct WILDCARD
  {
    ///<summary>The wildcard character found in the given string.</summary>
    char character;

    ///<summary>The index at which the wildcard character was found.</summary>
    size_t index;

    ///<summary>The expanded value of the wildcard to match the given string.</summary>
    std::string value;
  };

  /// <summary>
  /// Defines a list of WILDCARD
  /// </summary>
  typedef std::vector<WILDCARD> WildcardList;

  /// <summary>
  /// Evaluates if the given character is a wildcard character supported by the library.
  /// </summary>
  /// <param name="c">The character to evaluate.</param>
  /// <returns>Returns true if the given character is a wildcard character. Returns false otherwise.</returns>
  inline bool IsWildcard(char c)
  {
    if (c == '*' || c == '?')
      return true;
    return false;
  }

  /// <summary>
  /// Simplify a wildcard pattern. Remove sequences of '*' characters.
  /// </summary>
  /// <param name="pattern">The wildcard pattern to simplify.</param>
  SHELLANYTHING_EXPORT void WildcardSimplify(char * pattern);

  /// <summary>
  /// Finds the position of each wildcard character in the given string.
  /// If the 'findings' array is too small, the functions stops looking for wildcard characters and returns immediately.
  /// </summary>
  /// <param name="str">The string to search.</param>
  /// <param name="offsets">An array of size_t elements which, if provided, contains the offsets where a wildcard character is found. Can be NULL.</param>
  /// <param name="offsets_size">The size of the 'offsets' array in bytes. Mostly sizeof(offsets). Can be 0 if 'offsets' is NULL.</param>
  /// <returns>Returns the number of wildcard characters found in str.</returns>
  SHELLANYTHING_EXPORT size_t FindWildcardCharacters(const char * str, size_t * offsets, size_t offsets_size);

  /// <summary>
  /// Evaluates if the given string contains at least one wildcard character.
  /// </summary>
  /// <param name="str">The string to evaluate.</param>
  /// <returns>Returns true when the given string contains at least one wildcard character. Returns false otherwise.</returns>
  inline bool HasWildcardCharacters(const char * str) { return (FindWildcardCharacters(str, NULL, 0) > 0); }

  /// <summary>
  /// Returns true if the given pattern with wildcard characters can be expanded to match the given value.
  /// The function also returns the expanded value for each wildcard character match.
  /// If you do not need to know the value of the wildcard characters, use the function <see cref="WildcardMatch()"/> which is faster.
  /// </summary>
  /// <param name="pattern">The string with the wildcard pattern.</param>
  /// <param name="value">The value to match.</param>
  /// <param name="matches">The list of matches which allows the pattern to expand to the given value.</param>
  /// <returns>Returns true if the given pattern with wildcard characters can be expanded to match the given value. Returns false otherwise.</returns>
  SHELLANYTHING_EXPORT bool WildcardSolve(const char * pattern, const char * value, WildcardList & matches);

  /// <summary>
  /// Returns true if the given pattern with wildcard characters matches the given value.
  /// If you need to know the value of the wildcard characters, use the function <see cref="WildcardSolve()"/>.
  /// </summary>
  /// <param name="pattern">The string with the wildcard pattern.</param>
  /// <param name="value">The value to match.</param>
  /// <returns>Returns true if the given pattern with wildcard characters matches the given value. Returns false otherwise.</returns>
  SHELLANYTHING_EXPORT bool WildcardMatch(const char * pattern, const char * value);

} //namespace shellanything

#endif //SA_WILDCARD_H
