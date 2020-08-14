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
  typedef std::vector<WildcardResult> WildcardResultList;

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

  bool WildcardSolve(const char * pattern, const char * value, WildcardResultList * presults)
  {
    size_t pattern_length = strlen(pattern);
    size_t value_length = strlen(value);

    // If the pattern is empty, the value must also be empty to match.
    if (pattern_length == 0)
    {
      // If not, then there is not match.
      if (value_length == 0)
      {
        //if (presults)
        //{
        //  WildcardResult r = {0};
        //  r.valid = true;
        //  r.pattern_offset = 0;
        //  r.value = value;
        //  r.value_length = 0;
        //  presults->push_back(r);
        //}
        return true;
      }
      return false;
    }

    // If the value is empty, the pattern must be '*' (or a '*' sequence) to match
    if (value_length == 0)
    {
      if (!IsStarSequence(&pattern[0]))
        return false;

      // We have a match.
      if (presults)
      {
        WildcardResult r = {0};
        r.valid = true;
        r.pattern_offset = 0;
        r.value = value;
        r.value_length = 0;
        presults->push_back(r);
      }
      
      return true;
    }

    // At this point, both pattern and value are not empty

    size_t value_offset = 0;
    for(size_t pattern_offset = 0; pattern_offset < pattern_length; pattern_offset++)
    {
      const char & pattern_char = pattern[pattern_offset];
      const char & value_char = value[value_offset];

      // If we reached a '*' sequence, move forward to the last '*' character of the sequence
      while(pattern[pattern_offset] == '*' && (pattern_offset+1 < pattern_length) && pattern[pattern_offset+1] == '*')
      {
        pattern_offset++;
      }

      size_t remaining_pattern_length = pattern_length - pattern_offset;
      size_t remaining_value_length = value_length - value_offset;

      // If we reached the end of both strings, we are done searching
      if (remaining_pattern_length == 0 && remaining_value_length == 0)
        return true;

      // If the pattern is '*', and the next pattern character is valid, and the value is empty, then we won't be able to match.
      // (because there is a next non-wildcard character in the pattern)
      if (pattern_char == '*' &&
        (pattern_offset+1 < pattern_length) &&  // pattern has more characters
        pattern[pattern_offset+1] != '\0' &&    // next pattern character is valid)
        remaining_value_length == 0)            // value is empty
      {
        return false;
      }

      if (pattern_char == '?')
      {
        // In order to match, the '?' must match the current value character
        if (value_char == '\0')
          return false; // We reached the end of the value string

        // We matched this wildcard character
        // Save this wildcard expansion
        if (presults)
        {
          WildcardResult r = {0};
          r.valid = true;
          r.pattern_offset = pattern_offset;
          r.value = &value[value_offset];
          r.value_length = 1;
          presults->push_back(r);
        }

        // Advance both offsets and keep on looking
        value_offset++;
      }
      else if (pattern_char == value_char)
      {
        // Advance both offsets and keep on looking
        value_offset++;
      }
      else if (pattern_char == '*')
      {
        // Found a '*' character or sequence.
        // From this point, there are 2 possibilities.

        WildcardResultList tmp;
        WildcardResultList * sub_results = NULL;
        if (presults)
          sub_results = &tmp;

        // 1) The '*' replaces the next value character.
        bool match = WildcardSolve(&pattern[pattern_offset], &value[value_offset+1], sub_results);
        if (match)
        {
          // Add the sub results to our results
          if (presults)
          {
            for(size_t i=0; i<sub_results->size(); i++)
            {
              WildcardResult & r = (*sub_results)[i];
              r.pattern_offset += pattern_offset;
              presults->push_back(r);
            }
            sub_results->clear();
          }

          return true;
        }

        // 2) The '*' does not replaces the next value character.
        match = WildcardSolve(&pattern[pattern_offset+1], &value[value_offset], sub_results);
        if (match)
        {
          // Add the sub results to our results
          if (presults)
          {
            for(size_t i=0; i<sub_results->size(); i++)
            {
              WildcardResult & r = (*sub_results)[i];
              r.pattern_offset += pattern_offset+1;
              presults->push_back(r);
            }
            sub_results->clear();
          }

          return true;
        }

        // We recurse on all possibilities. No match found.
        return false;
      }
      else
      {
        // Characters from pattern and value does not match
        return false;
      }
    }

    // We reached the end of the pattern, the value must be empty too to match
    if (value[value_offset] != '\0')
      return false;

    //// The remaining of the value is the expansion of the last wildcard character
    //if (presults)
    //{
    //  WildcardResult r = {0};
    //  r.valid = true;
    //  r.pattern_offset = 0;
    //  r.value = value;
    //  r.value_length = 0;
    //  presults->push_back(r);
    //}

    return true;
  }

  bool WildcardSolve(const char * pattern, const char * value, WildcardResult * results_array, size_t results_size)
  {
    size_t results_array_count = results_size / sizeof(WildcardResult);
    bool has_array = (results_array != NULL && results_array_count > 0);
    if (has_array)
      memset(results_array, 0, results_size);

    if (pattern == NULL || value == NULL)
      return false;

    // Force the pattern to its simplest form
    std::string simplified_pattern = pattern;
    WildcardSimplify(simplified_pattern);

    // Create a results list if required
    WildcardResultList results;
    WildcardResultList * presults = NULL;
    if (has_array)
      presults = &results;

    // Search for a possible match
    bool match = WildcardSolve(simplified_pattern.c_str(), value, presults);
    if (!match)
      return false;

    if (has_array)
    {
      // The pattern and value matches
      // Copy the results list into results_array
      #define MIN(a,b) ((a) < (b) ? (a) : (b))
      size_t count = MIN(results.size(), results_array_count);
      #undef MIN
      for(size_t i=0; i<count; i++)
      {
        const WildcardResult & result = results[i];
        results_array[i] = result;
      }
    }

    return true;
  }

} //namespace shellanything
