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

#include "TestWildcard.h"
#include "Wildcard.h"
#include <sstream>

namespace shellanything { namespace test
{
  std::string WildcardRebuild(const char * pattern, const WildcardList & matches)
  {
    std::string output;
  
    size_t length = strlen(pattern);
    for(size_t i=0; i<length; i++)
    {
      if (IsWildcard(pattern[i]))
      {
        // Find the matching result
        for(size_t j=0; j<matches.size(); j++)
        {
          const WILDCARD & w = matches[j];
          if (w.index == i && w.character == pattern[i])
            output += w.value; //replace wildcard character by string
        }
      }
      else
        output.append(1, pattern[i]);
    }

    return output;
  }

  std::string toString(size_t i)
  {
    std::stringstream out;
    out << i;
    const std::string & s = out.str();
    return s;
  }

  //--------------------------------------------------------------------------------------------------
  void TestWildcard::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestWildcard::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWildcard, testFindWildcardCharacters)
  {
    // Test nothing is found
    {
      const char * str = "foo";
      size_t elements[50];
      size_t count = FindWildcardCharacters(str, elements, sizeof(elements));
      ASSERT_EQ(0, count);

      // Assert first element is zerorized
      ASSERT_EQ(0, elements[0]);
    }

    // Test sample found
    {
      const char * str = "f?o*o";
      size_t elements[50];
      size_t count = FindWildcardCharacters(str, elements, sizeof(elements));
      ASSERT_EQ(2, count);
      ASSERT_EQ(1, elements[0]);
      ASSERT_EQ(3, elements[1]);
      ASSERT_EQ(0, elements[2]);
    }

    // Test sequence of '*' found
    {
      const char * str = "f****o";
      size_t elements[50];
      size_t count = FindWildcardCharacters(str, elements, sizeof(elements));
      ASSERT_EQ(1, count);
      ASSERT_EQ(1, elements[0]);
      ASSERT_EQ(0, elements[1]);
    }

    // Test 2 sequences of '*' found
    {
      const char * str = "f**?**o";
      size_t elements[50];
      size_t count = FindWildcardCharacters(str, elements, sizeof(elements));
      ASSERT_EQ(3, count);
      ASSERT_EQ(1, elements[0]);
      ASSERT_EQ(3, elements[1]);
      ASSERT_EQ(4, elements[2]);
      ASSERT_EQ(0, elements[3]);
    }

    // Test sequence of '?' found
    {
      const char * str = "f???o";
      size_t elements[50];
      size_t count = FindWildcardCharacters(str, elements, sizeof(elements));
      ASSERT_EQ(3, count);
      ASSERT_EQ(1, elements[0]);
      ASSERT_EQ(2, elements[1]);
      ASSERT_EQ(3, elements[2]);
      ASSERT_EQ(0, elements[3]);
    }

    // Test buffer too small
    {
      const char * str = "f????o";
      size_t elements[2];
      size_t count = FindWildcardCharacters(str, elements, sizeof(elements));
      ASSERT_EQ(2, count);
      ASSERT_EQ(1, elements[0]);
      ASSERT_EQ(2, elements[1]);
    }

    // Test buffer overflow
    {
      const char * str = "f????o";
      size_t elements[40] = {0};
      size_t count = FindWildcardCharacters(str, elements, 2*sizeof(size_t));
      ASSERT_EQ(2, count);
      ASSERT_EQ(1, elements[0]);
      ASSERT_EQ(2, elements[1]);
      ASSERT_EQ(0, elements[2]);
    }

    // Test NULL buffer
    {
      const char * str = "f????o";
      size_t count = FindWildcardCharacters(str, NULL, 45);
      ASSERT_EQ(4, count);
      count = FindWildcardCharacters(str, NULL, 0);
      ASSERT_EQ(4, count);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWildcard, testBasic)
  {
    /*
    struct TEST
    {
      const char * pattern;
      const char * value;
      bool expected_result;
    };

    static const TEST tests[] = {
      {"","a",true},
    };
    static const size_t num_tests = sizeof(tests)/sizeof(tests[0]);

    // For each tests
    for(size_t i=0; i<num_tests; i++)
    {
      const TEST & test = tests[i];

      //Build an custom error message
      std::string msg;
      msg += "Unexpected assertion: ";
      msg += "i=" + toString(i) + ", ";
      msg += "pattern \"" + std::string(test.pattern) + "\" and ";
      msg += "value \"" + std::string(test.value) + "\" ";
      if (test.expected_result)
        msg += "should be matching.";
      else
        msg += "should NOT be matching.";

      // Test WildcardSolve()
      WildcardList matches;
      bool success = WildcardSolve(test.pattern, test.value, matches);
      if (test.expected_result)
      {
        ASSERT_TRUE( success ) << msg;

        // Try rebuilding the value from the pattern and the matches
        std::string rebuild = WildcardRebuild(test.pattern, matches);
        ASSERT_EQ( rebuild, test.value ) << msg;
      }
      else
        ASSERT_FALSE( success ) << msg;

      // Test WildcardMatch() as well.
      success = WildcardMatch(test.pattern, test.value);
      if (test.expected_result)
        ASSERT_TRUE( success ) << msg;
      else
        ASSERT_FALSE( success ) << msg;
    }
    */

    // Test no wildcard characters
    {
      WildcardList matches;
      const char * wildcard = "abcd";
      const char * value = "abcd";
      bool success = WildcardSolve(wildcard, value, matches);
      ASSERT_TRUE( success );

      // Try rebuilding the value from the pattern and the matches
      std::string rebuild = WildcardRebuild(wildcard, matches);
      ASSERT_EQ( rebuild, value );

      // Validate WildcardMatch() as well.
      success = WildcardMatch(wildcard, value);
      ASSERT_TRUE( success );
    }

    // Test '?' character in the middle
    {
      WildcardList matches;
      const char * wildcard = "ab?d";
      const char * value = "abcd";
      bool success = WildcardSolve(wildcard, value, matches);
      ASSERT_TRUE( success );

      // Try rebuilding the value from the pattern and the matches
      std::string rebuild = WildcardRebuild(wildcard, matches);
      ASSERT_EQ( rebuild, value );

      // Validate WildcardMatch() as well.
      success = WildcardMatch(wildcard, value);
      ASSERT_TRUE( success );
    }

    // Test '?' character at the beginning
    {
      WildcardList matches;
      const char * wildcard = "?bcd";
      const char * value = "abcd";
      bool success = WildcardSolve(wildcard, value, matches);
      ASSERT_TRUE( success );

      // Try rebuilding the value from the pattern and the matches
      std::string rebuild = WildcardRebuild(wildcard, matches);
      ASSERT_EQ( rebuild, value );

      // Validate WildcardMatch() as well.
      success = WildcardMatch(wildcard, value);
      ASSERT_TRUE( success );
    }

    // Test '?' character at the end
    {
      WildcardList matches;
      const char * wildcard = "abc?";
      const char * value = "abcd";
      bool success = WildcardSolve(wildcard, value, matches);
      ASSERT_TRUE( success );

      // Try rebuilding the value from the pattern and the matches
      std::string rebuild = WildcardRebuild(wildcard, matches);
      ASSERT_EQ( rebuild, value );

      // Validate WildcardMatch() as well.
      success = WildcardMatch(wildcard, value);
      ASSERT_TRUE( success );
    }

    // Test '*' replaces 0 character in the middle
    {
      WildcardList matches;
      const char * wildcard = "ab*cde";
      const char * value = "abcde";
      bool success = WildcardSolve(wildcard, value, matches);
      ASSERT_TRUE( success );

      // Try rebuilding the value from the pattern and the matches
      std::string rebuild = WildcardRebuild(wildcard, matches);
      ASSERT_EQ( rebuild, value );

      // Validate WildcardMatch() as well.
      success = WildcardMatch(wildcard, value);
      ASSERT_TRUE( success );
    }

    // Test '*' replaces 1 character in the middle
    {
      WildcardList matches;
      const char * wildcard = "ab*de";
      const char * value = "abcde";
      bool success = WildcardSolve(wildcard, value, matches);
      ASSERT_TRUE( success );

      // Try rebuilding the value from the pattern and the matches
      std::string rebuild = WildcardRebuild(wildcard, matches);
      ASSERT_EQ( rebuild, value );

      // Validate WildcardMatch() as well.
      success = WildcardMatch(wildcard, value);
      ASSERT_TRUE( success );
    }

    // Test '*' replaces 2 characters in the middle
    {
      WildcardList matches;
      const char * wildcard = "ab*e";
      const char * value = "abcde";
      bool success = WildcardSolve(wildcard, value, matches);
      ASSERT_TRUE( success );

      // Try rebuilding the value from the pattern and the matches
      std::string rebuild = WildcardRebuild(wildcard, matches);
      ASSERT_EQ( rebuild, value );

      // Validate WildcardMatch() as well.
      success = WildcardMatch(wildcard, value);
      ASSERT_TRUE( success );
    }

    // Test '*' replaces 0 character at the beginning
    {
      WildcardList matches;
      const char * wildcard = "*abcde";
      const char * value = "abcde";
      bool success = WildcardSolve(wildcard, value, matches);
      ASSERT_TRUE( success );

      // Try rebuilding the value from the pattern and the matches
      std::string rebuild = WildcardRebuild(wildcard, matches);
      ASSERT_EQ( rebuild, value );

      // Validate WildcardMatch() as well.
      success = WildcardMatch(wildcard, value);
      ASSERT_TRUE( success );
    }

    // Test '*' replaces 1 character at the beginning
    {
      WildcardList matches;
      const char * wildcard = "*bcde";
      const char * value = "abcde";
      bool success = WildcardSolve(wildcard, value, matches);
      ASSERT_TRUE( success );

      // Try rebuilding the value from the pattern and the matches
      std::string rebuild = WildcardRebuild(wildcard, matches);
      ASSERT_EQ( rebuild, value );

      // Validate WildcardMatch() as well.
      success = WildcardMatch(wildcard, value);
      ASSERT_TRUE( success );
    }

    // Test '*' replaces 2 characters at the beginning
    {
      WildcardList matches;
      const char * wildcard = "*cde";
      const char * value = "abcde";
      bool success = WildcardSolve(wildcard, value, matches);
      ASSERT_TRUE( success );

      // Try rebuilding the value from the pattern and the matches
      std::string rebuild = WildcardRebuild(wildcard, matches);
      ASSERT_EQ( rebuild, value );

      // Validate WildcardMatch() as well.
      success = WildcardMatch(wildcard, value);
      ASSERT_TRUE( success );
    }

    // Test '*' replaces 0 character at the end
    {
      WildcardList matches;
      const char * wildcard = "abcde*";
      const char * value = "abcde";
      bool success = WildcardSolve(wildcard, value, matches);
      ASSERT_TRUE( success );

      // Try rebuilding the value from the pattern and the matches
      std::string rebuild = WildcardRebuild(wildcard, matches);
      ASSERT_EQ( rebuild, value );

      // Validate WildcardMatch() as well.
      success = WildcardMatch(wildcard, value);
      ASSERT_TRUE( success );
    }

    // Test '*' replaces 1 character at the end
    {
      WildcardList matches;
      const char * wildcard = "abcd*";
      const char * value = "abcde";
      bool success = WildcardSolve(wildcard, value, matches);
      ASSERT_TRUE( success );

      // Try rebuilding the value from the pattern and the matches
      std::string rebuild = WildcardRebuild(wildcard, matches);
      ASSERT_EQ( rebuild, value );

      // Validate WildcardMatch() as well.
      success = WildcardMatch(wildcard, value);
      ASSERT_TRUE( success );
    }

    // Test '*' replaces 2 characters at the end
    {
      WildcardList matches;
      const char * wildcard = "abc*";
      const char * value = "abcde";
      bool success = WildcardSolve(wildcard, value, matches);
      ASSERT_TRUE( success );

      // Try rebuilding the value from the pattern and the matches
      std::string rebuild = WildcardRebuild(wildcard, matches);
      ASSERT_EQ( rebuild, value );

      // Validate WildcardMatch() as well.
      success = WildcardMatch(wildcard, value);
      ASSERT_TRUE( success );
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWildcard, testComplex)
  {
    // Test All wildcards must be matched. All wildcard characters must be solved.
    // The first '*' cannot be assigned the value "f?h*".
    {
      WildcardList matches;
      const char * wildcard = "abc*f?h*z";
      const char * value = "abcz";
      bool success = WildcardSolve(wildcard, value, matches);
      ASSERT_FALSE( success );
    }

    // Test wildcard '*' can be an empty string
    {
      {
        //1
        WildcardList matches;
        const char * wildcard = "abc*d";
        const char * value = "abcd";
        bool success = WildcardSolve(wildcard, value, matches);
        ASSERT_TRUE( success );

        // Try rebuilding the value from the pattern and the matches
        std::string rebuild = WildcardRebuild(wildcard, matches);
        ASSERT_EQ( rebuild, value );

        // Validate WildcardMatch() as well.
        success = WildcardMatch(wildcard, value);
        ASSERT_TRUE( success );
      }
      {
        //2
        WildcardList matches;
        const char * wildcard = "abcd*";
        const char * value = "abcd";
        bool success = WildcardSolve(wildcard, value, matches);
        ASSERT_TRUE( success );

        // Try rebuilding the value from the pattern and the matches
        std::string rebuild = WildcardRebuild(wildcard, matches);
        ASSERT_EQ( rebuild, value );

        // Validate WildcardMatch() as well.
        success = WildcardMatch(wildcard, value);
        ASSERT_TRUE( success );
      }
      {
        //3
        WildcardList matches;
        const char * wildcard = "*abcd";
        const char * value = "abcd";
        bool success = WildcardSolve(wildcard, value, matches);
        ASSERT_TRUE( success );

        // Try rebuilding the value from the pattern and the matches
        std::string rebuild = WildcardRebuild(wildcard, matches);
        ASSERT_EQ( rebuild, value );

        // Validate WildcardMatch() as well.
        success = WildcardMatch(wildcard, value);
        ASSERT_TRUE( success );
      }
      {
        //4
        WildcardList matches;
        const char * wildcard = "abc*?e";
        const char * value = "abcde";
        bool success = WildcardSolve(wildcard, value, matches);
        ASSERT_TRUE( success );

        // Try rebuilding the value from the pattern and the matches
        std::string rebuild = WildcardRebuild(wildcard, matches);
        ASSERT_EQ( rebuild, value );

        // Validate WildcardMatch() as well.
        success = WildcardMatch(wildcard, value);
        ASSERT_TRUE( success );
      }
    }

    // While checking for * wildcard, the solver must check all possibilities
    // Wildcard * must be "defabc" and NOT de
    {
      WildcardList matches;
      const char * wildcard = "abc*fg";
      const char * value = "abcdefabcfg";
      bool success = WildcardSolve(wildcard, value, matches);
      ASSERT_TRUE( success );

      // Try rebuilding the value from the pattern and the matches
      std::string rebuild = WildcardRebuild(wildcard, matches);
      ASSERT_EQ( rebuild, value );

      // Validate WildcardMatch() as well.
      success = WildcardMatch(wildcard, value);
      ASSERT_TRUE( success );
    }

    // Test '*' followed by '?'
    {
      {
        //1
        WildcardList matches;
        const char * wildcard = "abc*??h";
        const char * value = "abcdefgh";
        bool success = WildcardSolve(wildcard, value, matches);
        ASSERT_TRUE( success );

        // Try rebuilding the value from the pattern and the matches
        std::string rebuild = WildcardRebuild(wildcard, matches);
        ASSERT_EQ( rebuild, value );

        // Validate WildcardMatch() as well.
        success = WildcardMatch(wildcard, value);
        ASSERT_TRUE( success );
      }
      {
        //2
        WildcardList matches;
        const char * wildcard = "abc*??";
        const char * value = "abcdefg";
        bool success = WildcardSolve(wildcard, value, matches);
        ASSERT_TRUE( success );

        // Try rebuilding the value from the pattern and the matches
        std::string rebuild = WildcardRebuild(wildcard, matches);
        ASSERT_EQ( rebuild, value );

        // Validate WildcardMatch() as well.
        success = WildcardMatch(wildcard, value);
        ASSERT_TRUE( success );
      }
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWildcard, testSpecialCases)
  {
    // Test empty string value can only match with '*' pattern
    {
      WildcardList matches;
      const char * wildcard = "*";
      const char * value = "";
      bool success = WildcardSolve(wildcard, value, matches);
      ASSERT_TRUE( success );

      // Try rebuilding the value from the pattern and the matches
      std::string rebuild = WildcardRebuild(wildcard, matches);
      ASSERT_EQ( rebuild, value );

      // Validate WildcardMatch() as well.
      success = WildcardMatch(wildcard, value);
      ASSERT_TRUE( success );
    }
    {
      WildcardList matches;
      const char * wildcard = "******";
      const char * value = "";
      bool success = WildcardSolve(wildcard, value, matches);
      ASSERT_TRUE( success );

      // Try rebuilding the value from the pattern and the matches
      std::string rebuild = WildcardRebuild(wildcard, matches);
      ASSERT_EQ( rebuild, value );

      // Validate WildcardMatch() as well.
      success = WildcardMatch(wildcard, value);
      ASSERT_TRUE( success );
    }

    // Test empty string value can match an empty pattern.
    {
      WildcardList matches;
      const char * wildcard = "";
      const char * value = "";
      bool success = WildcardSolve(wildcard, value, matches);
      ASSERT_TRUE( success );

      // Try rebuilding the value from the pattern and the matches
      std::string rebuild = WildcardRebuild(wildcard, matches);
      ASSERT_EQ( rebuild, value );

      // Validate WildcardMatch() as well.
      success = WildcardMatch(wildcard, value);
      ASSERT_TRUE( success );
    }
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
