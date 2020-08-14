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

namespace shellanything
{
  ///<summary>A wildcard result which allows one to rebuild the matching value from the wildcard pattern.</summary>
  struct WildcardResult
  {
    ///<summary>A validity flag for this results.</summary>
    bool valid;

    ///<summary>The offset in the pattern string where the wildcard character was found.</summary>
    size_t pattern_offset;

    ///<summary>The value in the testing string of the wildcard cahracter.</summary>
    const char * value;

    ///<summary>The length of the value.</summary>
    size_t value_length;
  };

  /// <summary>
  /// Evaluates if the given character is a wildcard character supported by the library.
  /// </summary>
  /// <param name="c">The character to evaluate.</param>
  /// <returns>Returns true if the given character is a wildcard character. Returns false otherwise.</returns>
  bool IsWildcard(char c);

  /// <summary>
  /// Simplify a wildcard pattern. Remove sequences of '*' characters.
  /// </summary>
  /// <param name="pattern">The wildcard pattern to simplify.</param>
  void WildcardSimplify(char * pattern);

  /// <summary>
  /// Finds the position of each wildcard character in the given string.
  /// If the 'findings' array is too small, the functions stops looking for wildcard characters and returns immediately.
  /// </summary>
  /// <param name="str">The string to search.</param>
  /// <param name="offsets">An array of size_t elements which, if provided, contains the offsets where a wildcard character is found. Can be NULL.</param>
  /// <param name="offsets_size">The size of the 'offsets' array in bytes. Mostly sizeof(offsets). Can be 0 if 'offsets' is NULL.</param>
  /// <returns>Returns the number of wildcard characters found in str.</returns>
  size_t FindWildcardCharacters(const char * str, size_t * offsets, size_t offsets_size);

  /// <summary>
  /// Evaluates if the given string contains at least one wildcard character.
  /// </summary>
  /// <param name="str">The string to evaluate.</param>
  /// <returns>Returns true when the given string contains at least one wildcard character. Returns false otherwise.</returns>
  inline bool HasWildcardCharacters(const char * str) { return (FindWildcardCharacters(str, NULL, 0) > 0); }

  /// <summary>
  /// Process a wildcard string by replacing all wildcard characters to match the given string.
  /// </summary>
  /// <param name="pattern">The string with the wildcard pattern.</param>
  /// <param name="value">The value to match.</param>
  /// <param name="results_array">
  /// An output WildcardResult array which contains the expanded values of all wildcard characters.
  /// Valid elements of the array have their 'wildcard' element non NULL.
  /// </param>
  /// <param name="buffer_size">The size of the WildcardResult array buffer in bytes.</param>
  /// <returns>Returns true the given pattern with the wildcard characters can be expanded to match the given value. Returns false otherwise.</returns>
  bool WildcardSolve(const char * pattern, const char * value, WildcardResult * results_array, size_t results_size);

  /// <summary>
  /// Returns true if the given pattern matches the given value.
  /// </summary>
  /// <param name="pattern">The string with the wildcard pattern.</param>
  /// <param name="value">The value to match.</param>
  /// <returns>Returns true if iWildcard can be expanded to match iValue. Returns false otherwise.</returns>
  inline bool WildcardMatch(const char * pattern, const char * value) { return WildcardSolve(pattern, value, NULL, 0); }

} //namespace shellanything

#endif //SA_ACTION_PROPERTY_H
