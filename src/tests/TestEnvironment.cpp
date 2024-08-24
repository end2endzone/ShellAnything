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

#include "TestEnvironment.h"
#include "PropertyManager.h"
#include "Environment.h"
#include "rapidassist/environment_utf8.h"

namespace shellanything
{
  namespace test
  {
    //--------------------------------------------------------------------------------------------------
    void TestEnvironment::SetUp()
    {
    }
    //--------------------------------------------------------------------------------------------------
    void TestEnvironment::TearDown()
    {
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestEnvironment, testIsOptionSet)
    {
      static const char * OPTION_NAME = "foobar";

      PropertyManager& pmgr = PropertyManager::GetInstance();
      Environment& env = Environment::GetInstance();

      const std::string& true_value = pmgr.GetProperty(PropertyManager::SYSTEM_TRUE_PROPERTY_NAME);
      const std::string& false_value = pmgr.GetProperty(PropertyManager::SYSTEM_FALSE_PROPERTY_NAME);

      // assert when environment variable does not exist
      ASSERT_FALSE(env.IsOptionSet(OPTION_NAME));
      ASSERT_FALSE(env.IsOptionTrue(OPTION_NAME));
      ASSERT_FALSE(env.IsOptionFalse(OPTION_NAME));

      // set as true
      ra::environment::SetEnvironmentVariableUtf8(OPTION_NAME, true_value.c_str());
      ASSERT_TRUE(env.IsOptionSet(OPTION_NAME));
      ASSERT_TRUE(env.IsOptionTrue(OPTION_NAME));
      ASSERT_FALSE(env.IsOptionFalse(OPTION_NAME));

      // set as false
      ra::environment::SetEnvironmentVariableUtf8(OPTION_NAME, false_value.c_str());
      ASSERT_TRUE(env.IsOptionSet(OPTION_NAME));
      ASSERT_FALSE(env.IsOptionTrue(OPTION_NAME));
      ASSERT_TRUE(env.IsOptionFalse(OPTION_NAME));
    }
    //--------------------------------------------------------------------------------------------------

  } //namespace test
} //namespace shellanything
