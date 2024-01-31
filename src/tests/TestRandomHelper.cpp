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

#include "TestRandomHelper.h"
#include "RandomHelper.h"

namespace shellanything
{
  namespace test
  {
    //--------------------------------------------------------------------------------------------------
    void TestRandomHelper::SetUp()
    {
    }
    //--------------------------------------------------------------------------------------------------
    void TestRandomHelper::TearDown()
    {
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestRandomHelper, testIsNumericPattern)
    {
      ASSERT_TRUE(RandomHelper::IsNumericPattern("0000"));
      ASSERT_TRUE(RandomHelper::IsNumericPattern("#######"));

      ASSERT_FALSE(RandomHelper::IsNumericPattern(""));
      ASSERT_FALSE(RandomHelper::IsNumericPattern("aaa"));
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestRandomHelper, testGetMaxValue)
    {
      ASSERT_EQ(9ull, RandomHelper::GetMaxValue(1));
      ASSERT_EQ(99ull, RandomHelper::GetMaxValue(2));
      ASSERT_EQ(999ull, RandomHelper::GetMaxValue(3));
      ASSERT_EQ(9999ull, RandomHelper::GetMaxValue(4));
      ASSERT_EQ(99999ull, RandomHelper::GetMaxValue(5));
      ASSERT_EQ(999999ull, RandomHelper::GetMaxValue(6));
      ASSERT_EQ(9999999ull, RandomHelper::GetMaxValue(7));
      ASSERT_EQ(99999999ull, RandomHelper::GetMaxValue(8));
      ASSERT_EQ(999999999ull, RandomHelper::GetMaxValue(9));

      ASSERT_EQ(4294967295ull, RandomHelper::GetMaxValue(10));
      ASSERT_EQ(4294967295ull, RandomHelper::GetMaxValue(11));
      ASSERT_EQ(4294967295ull, RandomHelper::GetMaxValue(12));
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestRandomHelper, testFormatLeadingZeroes)
    {
      std::string tmp;

      tmp = "foo";
      RandomHelper::FormatLeadingZeroes(tmp, 0);
      ASSERT_EQ(std::string("foo"), tmp);

      tmp = "foo";
      RandomHelper::FormatLeadingZeroes(tmp, 3);
      ASSERT_EQ(std::string("foo"), tmp);

      tmp = "foo";
      RandomHelper::FormatLeadingZeroes(tmp, 5);
      ASSERT_EQ(std::string("00foo"), tmp);
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestRandomHelper, testGetNumericPatternLength)
    {
      ASSERT_EQ(4, RandomHelper::GetNumericPatternLength("0000"));
      ASSERT_EQ(7, RandomHelper::GetNumericPatternLength("#######"));

      ASSERT_EQ(0, RandomHelper::GetNumericPatternLength(NULL));
      ASSERT_EQ(0, RandomHelper::GetNumericPatternLength(""));
      ASSERT_EQ(0, RandomHelper::GetNumericPatternLength("aaa"));
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestRandomHelper, testGetRandomFromPattern)
    {
      std::string r;

      ASSERT_TRUE(RandomHelper::GetRandomFromPattern("", r));
      ASSERT_FALSE(RandomHelper::GetRandomFromPattern("00bbb", r));

      ASSERT_TRUE(RandomHelper::GetRandomFromPattern("#########", r));
      std::cout << "r=" << r << "\n";
      ASSERT_GT(r.size(), 2) << "r=" << r;

      struct SINGLE_TEST
      {
        const char* pattern;
        size_t length;
      };
      static const SINGLE_TEST tests[] = {
        {"0000", 4},
        {"aaa", 3},
        {"AA", 2},
        {"zzZZzzZZ", 8},
        {"*****", 5},
      };
      static const size_t num_tests = sizeof(tests) / sizeof(tests[0]);

      // ASSERT each string has the expected size
      for (size_t i = 0; i < num_tests; i++)
      {
        const SINGLE_TEST& t = tests[i];
        const char* pattern = t.pattern;
        const size_t expected_length = t.length;

        bool success = RandomHelper::GetRandomFromPattern(pattern, r);
        std::cout << "GetRandomFromPattern() with pattern `" << pattern << "` returns `" << r << "`.\n";

        ASSERT_TRUE(success) << "pattern=" << pattern;
        ASSERT_EQ(expected_length, r.size()) << "pattern=" << pattern << " , " << "r=" << r;
      }
    }
    //--------------------------------------------------------------------------------------------------

  } //namespace test
} //namespace shellanything
