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

#include <string>
#include <vector>

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
    int index;

    ///<summary>The expanded value of the wildcard to match the given string.</summary>
    std::string value;
  };

  /// <summary>
  /// Defines a list of WILDCARD
  /// </summary>
  typedef std::vector<WILDCARD> WildcardList;

  typedef std::vector<int> IntList;

  struct WILDCARD_GROUP
  {
    char character;
    IntList indexes;
  };
  typedef std::vector<WILDCARD_GROUP> WildcardGroupList;

  /// <summary>
  /// Evaluates if the given character is a wildcard character supported by the library.
  /// </summary>
  /// <param name="c">The character to evaluate.</param>
  /// <returns>Returns true if the given character is a wildcard character. Returns false otherwise.</returns>
  bool IsWildcard(char c);

  /// <summary>
  /// Evaluates if the given string contains at least one wildcard character.
  /// </summary>
  /// <param name="c">The string to evaluate.</param>
  /// <returns>Returns true when the given string contains at least one wildcard character. Returns false otherwise.</returns>
  bool HasWildcardCharacters(const char * iValue);

  /// <summary>
  /// Returns the position of each wildcard character into oIndexes from iString starting at iOffset.
  /// </summary>
  /// <param name="iString">The string to search.</param>
  /// <param name="iOffset">The index in the string where to start the search from.</param>
  /// <param name="oIndexes">The offsets where wildcard characters was found.</param>
  void FindWildcardCharacters(const char * iString, int iOffset, IntList & oIndexes);

  /// <summary>
  /// Processes iWildcard and finds expanding parameters in order to convert iWildcard to iValue.
  /// </summary>
  /// <param name="iWildcard">The pattern string.</param>
  /// <param name="iValue">The value to test.</param>
  /// <param name="oList">The list of WILDCARD findings.</param>
  /// <returns>Returns true if iWildcard can be expanded to match iValue. Returns false otherwise.</returns>
  bool WildcardSolve(const char * iWildcard, const char * iValue, WildcardList & oList);

  /// <summary>
  /// Returns true if iWildcard can be expanded to match iValue.
  /// </summary>
  /// <param name="iWildcard">The pattern string.</param>
  /// <param name="iValue">The value to test.</param>
  /// <returns>Returns true if iWildcard can be expanded to match iValue. Returns false otherwise.</returns>
  bool WildcardMatch(const char * iWildcard, const char * iValue);

  /// <summary>
  /// Returns all wildcard characters and index from iString groupped by wildcard character.
  /// </summary>
  /// <param name="iString">The pattern string.</param>
  /// <returns>Returns all wildcard characters and index from iString groupped by wildcard character</returns>
  WildcardGroupList FindWildcardGroups(const char * iString);

} //namespace shellanything

#endif //SA_ACTION_PROPERTY_H
