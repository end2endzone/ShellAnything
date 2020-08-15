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

  size_t GetNextWildcardPosition(const size_t * positions, size_t num_wildcards, size_t current)
  {
    for(size_t i=0; i<num_wildcards; i++)
    {
      const size_t & pos = positions[i];
      if (pos == current)
      {
        if (i+1 < num_wildcards)
        {
          return positions[i+1];
        }
        return INVALID_WILDCARD_POSITION; //requested wildcard is the last one
      }
    }
    return INVALID_WILDCARD_POSITION; //wildcard position not found
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
          if (offsets != NULL)
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
                      const size_t * positions,
                      size_t num_wildcards,
                      size_t pattern_offset,
                      size_t value_offset,
                      WildcardList & matches)
  {
    size_t pattern_length = strlen(pattern);
    size_t value_length = strlen(value);
  
    // While pattern and value not fully solved
    while (pattern_offset < pattern_length || value_offset < value_length)
    {
      const char & pattern_char  = pattern[pattern_offset];
      const char & value_char = value[value_offset];
      if ( !IsWildcard(pattern_char) )
      {
        if (pattern_char != value_char)
          return false; // Characters don't match!

        // Next characters
        pattern_offset++;
        value_offset++;
      }
      else
      {
        // pattern_char is wildcard
        if (pattern_char == '?')
        {
          // Save wildcard info
          WILDCARD w;
          w.character = pattern_char;
          w.index = pattern_offset;
          w.value = value_char;
          matches.push_back(w);

          // Next characters
          pattern_offset++;
          value_offset++;
        }
        else if (pattern_char == '*')
        {
          // If wildcard character '*' is the last one
          if (pattern_offset+1 >= pattern_length)
          {
            // Automatically match the end of the string

            // Save wildcard info
            WILDCARD w;
            w.character = pattern_char;
            w.index = pattern_offset;
            w.value = &value_char;
            matches.push_back(w);

            // If we reached a '*' sequence, move forward to the last '*' character of the sequence
            while(pattern[pattern_offset] == '*' && (pattern_offset+1 < pattern_length) && pattern[pattern_offset+1] == '*')
            {
              pattern_offset++;
            }

            // Next characters
            pattern_offset++;
            value_offset += w.value.size();
          }
          else
          {
            // Wildcard character '*' is not the last one

            // Compute all possibles candidates that can fit in '*' and then use recursion to check if it can be resolved.
            // Compute the candidates from the longest to the shortest ("") for optimizing the replacement string.
            // Since all wildcard characters must be mapped, do not compute possibilities/candidates beyond the next wildcard character.

            // Compute next character
            const char & pattern_char_next = (&pattern_char)[1];

            size_t next_wildcard_position = GetNextWildcardPosition(positions, num_wildcards, pattern_offset);

            // Extract non-wildcard (fixed) characters between this wildcard and the next one (or the end of the string)
            // In order to match the current '*' character, these non-wildcard characters in the pattern will also have to match.
            // ie: Extract "fgh" in "abc*fgh?j" or "abc*fgh"
            std::string pattern_fixed_characters = &pattern_char_next;
            if (next_wildcard_position != INVALID_WILDCARD_POSITION)
            {
              // There is at least another wildcard character after this one
              pattern_fixed_characters = &pattern_char_next;
              size_t size = next_wildcard_position - pattern_offset;
              pattern_fixed_characters.resize(size);
            }

            // Compute maximum length of candidate
            size_t remaining_characters_in_value = value_length-value_offset;
            size_t candidate_max_length = remaining_characters_in_value-pattern_fixed_characters.size();

            // Compute all possible candidates that can fit in '*' with a length in [0,candidate_max_length]
            for(size_t length=candidate_max_length; length>=0 && length!=INVALID_WILDCARD_POSITION; length--)
            {
              // Assuming replacement string is the right one

              std::string candidate = &value_char;
              candidate.resize(length);

              WildcardList sub_matches = matches;

              // Save wildcard information
              WILDCARD w;
              w.character = pattern_char;
              w.index = pattern_offset;
              w.value = candidate;
              sub_matches.push_back(w);

              // Next characters
              size_t sub_pattern_offset = pattern_offset + 1;
              size_t sub_value_offset = value_offset + w.value.size();

              // Execute recursive call
              bool solved = WildcardSolve(pattern, value, positions, num_wildcards, sub_pattern_offset, sub_value_offset, sub_matches);
              if (solved)
              {
                // Solved!
                // Keep the sub matches that we found
                matches = sub_matches;
                return true;
              }
            }

            // All possibilities were checked
            // Unable to solve '*' wildcard
            return false;
          }
        }
      }
    }

    if (pattern_offset == pattern_length && value_offset == value_length)
      return true; // Solved
    if ((pattern_offset == pattern_length && value_offset < value_length) ||
        (pattern_offset == pattern_length && value_offset < value_length)     )
      return false; // Reached the end of pattern or the end of value
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
    size_t * positions = new size_t[num_wildcards];
    FindWildcardCharacters(simplified_pattern.c_str(), positions, num_wildcards*sizeof(positions[0]));

    //solve wildcards
    bool solved = WildcardSolve(simplified_pattern.c_str(), value, positions, num_wildcards, 0, 0, matches);

    delete[] positions;
    positions = NULL;

    return solved;
  }

  bool WildcardMatch(const char * pattern, const char * value)
  {
    if (pattern == NULL || value == NULL)
      return false;

    // Move forward in the pattern and the value as long as we have matching characters.
    bool matching_characters = true;
    while(matching_characters)
    {
      // If the pattern is empty, the value must also be empty to match.
      if (pattern[0] == '\0')
      {
        if (value[0] == '\0')
          return true;
        return false;
      }

      // If the value is empty, the pattern must be '*' (or a '*' sequence) to match
      if (value[0] == '\0')
      {
        if (!IsStarSequence(&pattern[0]))
          return false;

        // We have a match.
        return true;
      }

      // At this point, both the pattern and the value are not empty.
      // Both can be moved forward if we have a match.

      // Now check if we have a matching characters
      matching_characters = false;

      // If we reached a '*' sequence, move forward in the pattern to the last '*' character of the sequence
      while(pattern[0] == '*' && pattern[1] == '*')
      {
        pattern++;

        // Even if did not had 'matching characters', set the flag to true so that we run another loop.
        // This will allows us to check for special cases like:
        // 1) pattern="" and value=""
        // 2) pattern="*" and value=""
        matching_characters = true;
      }

      // If the pattern contains '?', if both pattern and value characters are equals, move forward
      if (pattern[0] == '?' || pattern[0] == value[0])
      {
        pattern++;
        value++;

        matching_characters = true;
      }
    }
  
    // If we reached a '*' character in the pattern, there is two possibilities:
    // 1) The '*' replaces the next value character.
    // 2) The '*' does not replaces the next value character.
    // Use recursion to resolve the two possibilities.
    if (pattern[0] == '*')
    {
      // 1) The '*' replaces the next value character.
      bool match = WildcardMatch(pattern, value+1);
      if (match)
        return true;

      // 2) The '*' does not replaces the next value character.
      match = WildcardMatch(pattern+1, value);
      if (match)
        return true;
    }

    // No match possible
    return false;
  }

} //namespace shellanything
