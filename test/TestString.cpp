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

#include "TestString.h"
#include "shellanything/String.h"
#include <string>

namespace shellanything { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestString::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestString::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testCtor)
  {
    {
      String str;
      ASSERT_TRUE(str.empty());
    }

    {
      String str("Aphrodite");
      ASSERT_EQ(0, strcmp("Aphrodite", str.c_str()));
    }

    {
      String str;
      str = "Apollo";
      ASSERT_EQ(0, strcmp("Apollo", str.c_str()));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testCopy)
  {
    String s1 = "Ares";
    String s2(s1);
    ASSERT_EQ(0, strcmp("Ares", s2.c_str()));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testOperatorEqual)
  {
    String s1 = "Athena";
    String s2;
    
    s2 = s1;
    ASSERT_EQ(0, strcmp("Athena", s2.c_str()));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testOperatorEqualEqual)
  {
    String s1 = "Hades";
    String s2 = "Hades";

    ASSERT_TRUE(s1 == s2);

    s1 = "Hades";
    s2 = "foo";
    ASSERT_FALSE(s1 == s2);

    s1 = "foo";
    s2 = "Hades";
    ASSERT_FALSE(s1 == s2);

    s1 = "Hades";
    s2 = "Hades1";
    ASSERT_FALSE(s1 == s2);

    s1 = "Hades1";
    s2 = "Hades";
    ASSERT_FALSE(s1 == s2);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testOperatorNotEqual)
  {
    String s1 = "Hades";
    String s2 = "Hades";

    ASSERT_FALSE(s1 != s2);

    s1 = "Hades";
    s2 = "foo";
    ASSERT_TRUE(s1 != s2);

    s1 = "foo";
    s2 = "Hades";
    ASSERT_TRUE(s1 != s2);

    s1 = "Hades";
    s2 = "Hades1";
    ASSERT_TRUE(s1 != s2);

    s1 = "Hades1";
    s2 = "Hades";
    ASSERT_TRUE(s1 != s2);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testOperatorBracket)
  {
    String str = "Poseidon";

    ASSERT_EQ('P',  str[0]);
    ASSERT_EQ('o',  str[1]);
    ASSERT_EQ('s',  str[2]);
    ASSERT_EQ('e',  str[3]);
    ASSERT_EQ('i',  str[4]);
    ASSERT_EQ('d',  str[5]);
    ASSERT_EQ('o',  str[6]);
    ASSERT_EQ('n',  str[7]);
    ASSERT_EQ('\0', str[8]);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testOperatorPlusEqual)
  {
    String str;
    str += "hello";
    str += " ";
    str += "world";
    ASSERT_TRUE(str == "hello world");
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything