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

#include "TestRandomService.h"
#include "App.h"

namespace shellanything
{
  namespace test
  {
    //--------------------------------------------------------------------------------------------------
    void TestRandomService::SetUp()
    {
    }
    //--------------------------------------------------------------------------------------------------
    void TestRandomService::TearDown()
    {
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestRandomService, testAvailable)
    {
      IRandomService* random_service = App::GetInstance().GetRandomService();
      ASSERT_TRUE(random_service != NULL);
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestRandomService, testRandomness)
    {
      IRandomService* random_service = App::GetInstance().GetRandomService();
      ASSERT_TRUE(random_service != NULL);

      static const size_t count = 10000;

      // Generate multiple random values
      uint32_t values[count] = { 0 };
      for (size_t i = 0; i < count; i++)
      {
        values[i] = random_service->GetRandomValue();
      }

      // ASSERT that values are all different
      for (size_t i = 0; i < count-1; i++)
      {
        const uint32_t& valueA = values[i];

        for (size_t j = i+1; j < count; j++)
        {
          const uint32_t& valueB = values[j];

          ASSERT_NE(valueA, valueB) << "value at index " << i << " is identical to value at index " << j;
        }
      }
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestRandomService, testRandomRange)
    {
      IRandomService* random_service = App::GetInstance().GetRandomService();
      ASSERT_TRUE(random_service != NULL);

      static const size_t count = 100000;
      static const uint32_t min_value = 1000;
      static const uint32_t max_value = 1050;

      // Generate random values until min and max values are picked
      bool min_found = false;
      bool max_found = false;
      for (size_t i = 0; i < count && (!min_found || !max_found); i++)
      {
        uint32_t value = random_service->GetRandomValue(min_value, max_value);
        if (value == min_value)
          min_found = true;
        if (value == max_value - 1)
          max_found = true;
      }

      // ASSERT that both min and max values were generated
      ASSERT_TRUE(min_found);
      ASSERT_TRUE(max_found);
    }
    //--------------------------------------------------------------------------------------------------

  } //namespace test
} //namespace shellanything
