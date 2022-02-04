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

#include "TestLibExprtk.h"
#include "libexprtk.h"
#include "PropertyManager.h"

namespace shellanything { namespace test
{
  const double epsilon = 0.000001;

  //--------------------------------------------------------------------------------------------------
  void TestLibExprtk::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestLibExprtk::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestLibExprtk, testSingleValue)
  {
    double result = 0.0;
    int success = EvaluateDoubleEx("5.3", &result);
    ASSERT_EQ(1, success);
    ASSERT_NEAR(result, 5.3, epsilon);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestLibExprtk, testError)
  {
    double result = 0.0;
    static const size_t BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE] = {0};
    int success = EvaluateDouble("foobar;", &result, buffer, BUFFER_SIZE);
    ASSERT_EQ(0, success);
    ASSERT_GT(strlen(buffer), 0);
    printf("Found full error: '%s'\n", buffer);

    // Test for buffer overflow
    static const char RESET_CHARACTER = '\n';
    static const size_t TRUNCATED_BUFFER_SIZE = 13;
    memset(buffer, RESET_CHARACTER, BUFFER_SIZE);
    success = EvaluateDouble("foobar;", &result, buffer, TRUNCATED_BUFFER_SIZE);
    ASSERT_EQ(0, success);
    ASSERT_EQ('\0', buffer[TRUNCATED_BUFFER_SIZE-1]); // assert last character of the buffer is the string end character
    ASSERT_EQ(RESET_CHARACTER, buffer[TRUNCATED_BUFFER_SIZE]); // assert next character outside of given buffer is untouched.
    for(size_t i=TRUNCATED_BUFFER_SIZE; i<BUFFER_SIZE; i++)
      ASSERT_EQ(RESET_CHARACTER, buffer[i]); // assert following characters are untouched.
    printf("Found truncated error: '%s'\n", buffer);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestLibExprtk, testStringPlusScalar) //Adding a string with a scalar is expected to fail evaluation
  {
    double result = 0.0;
    int success = EvaluateDoubleEx("'4'+5", &result);
    ASSERT_EQ(0, success); //
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestLibExprtk, testIfsSingleLine)
  {
    double result = 0.0;
    int success = false;
    
    // Test successful if
    result = 0.0;
    success = EvaluateDoubleEx("if (10 > 3, 2.1, 5.7)", &result);
    ASSERT_EQ(1, success);
    ASSERT_NEAR(result, 2.1, epsilon);

    // Test failing if
    result = 0.0;
    success = EvaluateDoubleEx("if (10 < 3, 2.1, 5.7)", &result);
    ASSERT_EQ(1, success);
    ASSERT_NEAR(result, 5.7, epsilon);
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
