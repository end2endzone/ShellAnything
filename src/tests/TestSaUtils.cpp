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

#pragma warning( push )
#pragma warning( disable: 4018 ) //googletest\install\include\gtest/gtest.h(1528): warning C4018: '>=' : signed/unsigned mismatch
#include <gtest/gtest.h>
#pragma warning( pop )

#include "TestSaUtils.h"
#include "SaUtils.h"

namespace shellanything
{
  namespace test
  {
    //--------------------------------------------------------------------------------------------------
    void TestSaUtils::SetUp()
    {
    }
    //--------------------------------------------------------------------------------------------------
    void TestSaUtils::TearDown()
    {
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSaUtils, testGetFirstCodePointLengthUtf8)
    {
      ASSERT_EQ(0, GetFirstCodePointLengthUtf8(""));
      ASSERT_EQ(1, GetFirstCodePointLengthUtf8("test"));
      ASSERT_EQ(2, GetFirstCodePointLengthUtf8("\xc3" "\xa9" "cole")); // école
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSaUtils, testIsPrintable)
    {
      ASSERT_TRUE(IsPrintable("hello world!"));
      ASSERT_TRUE(IsPrintable("hello\nworld!"));
      ASSERT_FALSE(IsPrintable("Espa" "\xc3" "\xb1" "ol")); // Español 
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestSaUtils, testIsPrintableUtf8)
    {
      ASSERT_TRUE(IsPrintableUtf8("hello world!"));
      ASSERT_TRUE(IsPrintableUtf8("hello\nworld!"));
      ASSERT_TRUE(IsPrintableUtf8("Espa" "\xc3" "\xb1" "ol")); // Español 
    }
    //--------------------------------------------------------------------------------------------------

  } //namespace test
} //namespace shellanything
