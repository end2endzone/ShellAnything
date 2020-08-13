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
namespace shellanything { namespace test
{
  std::string testRebuild(const char * iWildcard, const WildcardList & iWildcardValues)
  {
    std::string output;
  
    int len = (int)std::string(iWildcard).size();
    for(int i=0; i<len; i++)
    {
      if ( IsWildcard(iWildcard[i]) )
      {
        //wildcard character found, find the replacement string
        for(size_t j=0; j<iWildcardValues.size(); j++)
        {
          const WILDCARD & w = iWildcardValues[j];
          if (w.index == i && w.character == iWildcard[i])
            output += w.value; //replace wildcard character by string
        }
      }
      else
        output += iWildcard[i]; //replace wildcard character
    }

    return output;
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
  TEST_F(TestWildcard, testValidByDefault)
  {
    //SIMPLE TEST CASES:
    {
      WildcardList values;
      const char * wildcard = "abcd";
      const char * value = "abcd";
      bool success = WildcardSolve(wildcard, value, values);
      ASSERT_TRUE( success );
      std::string rebuild = testRebuild(wildcard, values);
      ASSERT_EQ( rebuild, value );
    }
    {
      WildcardList values;
      const char * wildcard = "ab?d";
      const char * value = "abcd";
      bool success = WildcardSolve(wildcard, value, values);
      ASSERT_TRUE( success );
      std::string rebuild = testRebuild(wildcard, values);
      ASSERT_EQ( rebuild, value );
    }
    {
      WildcardList values;
      const char * wildcard = "?bcd";
      const char * value = "abcd";
      bool success = WildcardSolve(wildcard, value, values);
      ASSERT_TRUE( success );
      std::string rebuild = testRebuild(wildcard, values);
      ASSERT_EQ( rebuild, value );
    }
    {
      WildcardList values;
      const char * wildcard = "abc?";
      const char * value = "abcd";
      bool success = WildcardSolve(wildcard, value, values);
      ASSERT_TRUE( success );
      std::string rebuild = testRebuild(wildcard, values);
      ASSERT_EQ( rebuild, value );
    }
    {
      WildcardList values;
      const char * wildcard = "ab*e";
      const char * value = "abcde";
      bool success = WildcardSolve(wildcard, value, values);
      ASSERT_TRUE( success );
      std::string rebuild = testRebuild(wildcard, values);
      ASSERT_EQ( rebuild, value );
    }
    {
      WildcardList values;
      const char * wildcard = "*cde";
      const char * value = "abcde";
      bool success = WildcardSolve(wildcard, value, values);
      ASSERT_TRUE( success );
      std::string rebuild = testRebuild(wildcard, values);
      ASSERT_EQ( rebuild, value );
    }
    {
      WildcardList values;
      const char * wildcard = "abc*";
      const char * value = "abcde";
      bool success = WildcardSolve(wildcard, value, values);
      ASSERT_TRUE( success );
      std::string rebuild = testRebuild(wildcard, values);
      ASSERT_EQ( rebuild, value );
    }

    //COMPLEX TEST CASES:
    {
      //all wildcards must be matched
      WildcardList values;
      const char * wildcard = "abc*f?h*z";
      const char * value = "abcz";
      bool success = WildcardSolve(wildcard, value, values);
      ASSERT_FALSE( success );
    }
    {
      //wildcard * can be an empty string
      {
        //1
        WildcardList values;
        const char * wildcard = "abc*d";
        const char * value = "abcd";
        bool success = WildcardSolve(wildcard, value, values);
        ASSERT_TRUE( success );
        std::string rebuild = testRebuild(wildcard, values);
        ASSERT_EQ( rebuild, value );
      }
      {
        //2
        WildcardList values;
        const char * wildcard = "abcd*";
        const char * value = "abcd";
        bool success = WildcardSolve(wildcard, value, values);
        ASSERT_TRUE( success );
        std::string rebuild = testRebuild(wildcard, values);
        ASSERT_EQ( rebuild, value );
      }
      {
        //3
        WildcardList values;
        const char * wildcard = "*abcd";
        const char * value = "abcd";
        bool success = WildcardSolve(wildcard, value, values);
        ASSERT_TRUE( success );
        std::string rebuild = testRebuild(wildcard, values);
        ASSERT_EQ( rebuild, value );
      }
      {
        //4
        WildcardList values;
        const char * wildcard = "abc*?e";
        const char * value = "abcde";
        bool success = WildcardSolve(wildcard, value, values);
        ASSERT_TRUE( success );
        std::string rebuild = testRebuild(wildcard, values);
        ASSERT_EQ( rebuild, value );
      }
    }
    {
      //while checking for * wildcard, the solver must check all possibilities
      //wildcard * must be "defabc" and NOT de
      WildcardList values;
      const char * wildcard = "abc*fg";
      const char * value = "abcdefabcfg";
      bool success = WildcardSolve(wildcard, value, values);
      ASSERT_TRUE( success );
      std::string rebuild = testRebuild(wildcard, values);
      ASSERT_EQ( rebuild, value );
    }
    {
      //* followed by ?
      {
        //1
        WildcardList values;
        const char * wildcard = "abc*??h";
        const char * value = "abcdefgh";
        bool success = WildcardSolve(wildcard, value, values);
        ASSERT_TRUE( success );
        std::string rebuild = testRebuild(wildcard, values);
        ASSERT_EQ( rebuild, value );
      }
      {
        //2
        WildcardList values;
        const char * wildcard = "abc*??";
        const char * value = "abcdefg";
        bool success = WildcardSolve(wildcard, value, values);
        ASSERT_TRUE( success );
        std::string rebuild = testRebuild(wildcard, values);
        ASSERT_EQ( rebuild, value );
      }
    }
  }

  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
