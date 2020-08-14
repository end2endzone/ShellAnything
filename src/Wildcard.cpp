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

#include <string>
#include <vector>

#include "Wildcard.h"

namespace shellanything
{
  static const size_t INVALID_WILDCARD_POSITION = (size_t)-1;

  size_t GetNextWildcardPosition(const size_t * wildcard_positions, size_t num_wildcards, size_t current)
  {
    for(size_t i=0; i<num_wildcards; i++)
    {
      const size_t & pos = wildcard_positions[i];
      if (pos == current)
      {
        if (i+1 < num_wildcards)
        {
          return wildcard_positions[i+1];
        }
        return INVALID_WILDCARD_POSITION; //requested wildcard is the last one
      }
    }
    return INVALID_WILDCARD_POSITION; //wildcard position not found
  }

  bool IsWildcard(char c)
  {
    if (c == '*' || c == '?')
      return true;
    return false;
  }

  size_t FindWildcardCharacters(const char * str, size_t * offsets, size_t offsets_size)
  {
    // Validate str
    if (str == NULL)
      return 0;

    // Validate offsets buffer size
    size_t max_elements = offsets_size/sizeof(size_t);
    if (offsets != NULL && max_elements == 0)
      return 0;

    if (offsets)
      memset(offsets, 0, offsets_size);

    size_t num_wildcard = 0;

    size_t length = strlen(str);
    for(size_t i=0; i<length; i++)
    {
      const char & c = str[i];
      if (IsWildcard(c))
      {
        // Is this a '*' sequence ?
        bool sequence = (c == '*' && i > 0 && str[i-1] == '*');
        if (!sequence)
        {
          if (offsets)
            offsets[num_wildcard] = i;

          num_wildcard++;

          if (offsets != NULL && num_wildcard == max_elements)
          {
            // Buffer is full
            return num_wildcard;
          }
        }
      }
    }

    return num_wildcard;
  }

  void WildcardSimplify(std::string & pattern)
  {
    size_t length = pattern.size();

    // Simplify the pattern if required.
    std::string simplified_pattern;
    simplified_pattern.reserve(length);
    for(size_t i=0; i<length; i++)
    {
      const char & c = pattern[i];
      if (c == '*')
      {
        // Try to advance the pointer as long as the next character is also a '*' character.
        while (i+1 < length && pattern[i+1] == '*')
          i++;
        simplified_pattern.append(1, c); // Add a single '*' character
      }
      else
      {
        simplified_pattern.append(1, c); // No simplification possible
      }
    }

    if (simplified_pattern.size() < pattern.size())
      pattern = simplified_pattern;
  }

  void WildcardSimplify(char * pattern)
  {
    if (pattern == NULL)
      return;

    size_t length = strlen(pattern);

    std::string simplified_pattern = pattern;
    WildcardSimplify(simplified_pattern);

    bool simplified = (simplified_pattern.size() < length);
    if (simplified)
      strcpy(pattern, simplified_pattern.c_str());
  }

  inline bool IsStarSequence(const char * pattern)
  {
    if (pattern == NULL)
      return false;
    while(pattern[0] != '\0')
    {
      if (pattern[0] != '*')
        return false;
      pattern++;
    }
    return true;
  }

  bool WildcardSolve( const char * pattern,
                      const char * value,
                      const size_t * wildcard_positions,
                      size_t num_wildcards,
                      const int iIndexWild,
                      const int iIndexValue,
                      WildcardList & matches)
  {
    int indexWild = iIndexWild;
    int indexValue = iIndexValue;
    int wildLen   = (int)std::string(pattern).size();
    int valueLen  = (int)std::string(value).size();
  
    //while wildcard and value not fully solved
    while (indexWild < wildLen || indexValue < valueLen)
    {
      const char & cWild  = pattern[indexWild];
      const char & cValue = value[indexValue];
      if ( !IsWildcard(cWild) )
      {
        if (cWild != cValue)
          return false; //characters don't match!

        //next char
        indexWild++;
        indexValue++;
      }
      else
      {
        //cWild is wildcard
        if (cWild == '?')
        {
          //save wildcard resolve information
          WILDCARD w;
          w.character = cWild;
          w.index = indexWild;
          w.value = cValue;
          matches.push_back(w);

          //next char
          indexWild++;
          indexValue++;
        }
        else if (cWild == '*')
        {
          //if wildcard character * is the last one
          if (indexWild+1 >= wildLen)
          {
            //automatically match the end of the string

            //save wildcard resolve information
            WILDCARD w;
            w.character = cWild;
            w.index = indexWild;
            w.value = &cValue;
            matches.push_back(w);

            // If we reached a '*' sequence, move forward to the last '*' character of the sequence
            while(pattern[indexWild] == '*' && (indexWild+1 < wildLen) && pattern[indexWild+1] == '*')
            {
              indexWild++;
            }

            //next char
            indexWild++;
            indexValue += (int)w.value.size();
          }
          else
          {
            //wildcard character * is not the last one

            //compute all possibilities that can fit in * and then use recursion to check if it can be resolved
            //compute the possibilities in the from the longest to the shortest ("") for optimizing the replacement string
            //since all wildcard caracters must be mapped, do not compute possibilities beyond the next wildcard character

            //compute next character
            const char & nextWild = (&cWild)[1];

            int nextWildcardPosition = (int)GetNextWildcardPosition(wildcard_positions, num_wildcards, indexWild);

            //compute fixed characters between this wildcard and the last one (of the end of the file)
            //ie: fgh in abc*fgh?j or abc*fgh
            std::string fixedCharacters = &nextWild;
            if (nextWildcardPosition != -1)
            {
              //there is at least another wildcard character after this one
              fixedCharacters = &nextWild;
              size_t size = nextWildcardPosition - indexWild;
              fixedCharacters.resize(size);
            }

            //compute maximum length of replacementString
            int remainingCharactersInValue = valueLen-indexValue;
            int replacementStringMaxLength = remainingCharactersInValue-(int)fixedCharacters.size();

            //compute all possibilities that can fit in * with a length in [0,replacementStringMaxLength]
            for(int length=replacementStringMaxLength; length>=0; length--)
            {
              //assuming replacement string is the right one

              std::string replacementString = &cValue;
              replacementString.resize(length);

              WildcardList tmpList = matches;

              //save wildcard resolve information
              WILDCARD w;
              w.character = cWild;
              w.index = indexWild;
              w.value = replacementString;
              tmpList.push_back(w);

              //next char
              int tmpIndexWild = indexWild+1;
              int tmpIndexValue = indexValue + (int)w.value.size();

              //execute recursive call
              bool solved = WildcardSolve(pattern, value, wildcard_positions, num_wildcards, tmpIndexWild, tmpIndexValue, tmpList);
              if (solved)
              {
                //solved!
                //refresh matches
                matches = tmpList;
                return true;
              }
            }

            //all possibilities were checked
            //unable to solve * wildcard
            return false;
          }
        }
      }
    }

    if (indexWild == wildLen && indexValue == valueLen)
      return true; //solved
    if ((indexWild == wildLen && indexValue < valueLen) ||
        (indexWild == wildLen && indexValue < valueLen)     )
      return false; //reached the end of wildcard or the end of value
    return false; //???
  }

  bool WildcardSolve(const char * pattern, const char * value, WildcardList & matches)
  {
    if (pattern == NULL || value == NULL)
      return false;

    matches.clear();

    // Force the pattern to its simplest form
    std::string simplified_pattern = pattern;
    WildcardSimplify(simplified_pattern);

    // Find all wildcard character positions
    size_t num_wildcards = FindWildcardCharacters(simplified_pattern.c_str(), NULL, 0);
    
    // If no wildcard character found, the strings must be equal
    if (num_wildcards == 0)
      return (std::string(pattern) == std::string(value));

    // Allocate memory for all wildcard positions
    size_t * wildcard_positions = new size_t[num_wildcards];
    FindWildcardCharacters(simplified_pattern.c_str(), wildcard_positions, num_wildcards);

    //solve wildcards
    bool solved = WildcardSolve(simplified_pattern.c_str(), value, wildcard_positions, num_wildcards, 0, 0, matches);

    delete[] wildcard_positions;
    wildcard_positions = NULL;

    return solved;
  }

} //namespace shellanything
