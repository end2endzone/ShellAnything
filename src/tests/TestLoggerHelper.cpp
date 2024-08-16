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

#include "TestLoggerHelper.h"
#include "LoggerHelper.h"
#include "PropertyManager.h"
#include "Environment.h"
#include "rapidassist/environment_utf8.h"

namespace shellanything
{
  namespace test
  {
    //--------------------------------------------------------------------------------------------------
    void TestLoggerHelper::SetUp()
    {
    }
    //--------------------------------------------------------------------------------------------------
    void TestLoggerHelper::TearDown()
    {
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestLoggerHelper, testIsVerboseLoggingEnabled)
    {
      static const std::string& OPTION_NAME = Environment::SYSTEM_LOGGING_VERBOSE_ENVIRONMENT_VARIABLE_NAME;
      static const std::string& PROPERTY_NAME = PropertyManager::SYSTEM_LOGGING_VERBOSE_PROPERTY_NAME;

      PropertyManager& pmgr = PropertyManager::GetInstance();
      Environment& env = Environment::GetInstance();

      const std::string& true_value = pmgr.GetProperty(PropertyManager::SYSTEM_TRUE_PROPERTY_NAME);
      const std::string& false_value = pmgr.GetProperty(PropertyManager::SYSTEM_FALSE_PROPERTY_NAME);

      // clear all
      ra::environment::SetEnvironmentVariableUtf8(OPTION_NAME.c_str(), NULL);
      pmgr.ClearProperty(PROPERTY_NAME);
      ASSERT_FALSE(LoggerHelper::IsVerboseLoggingEnabled());

      // enable from env var (only)
      ra::environment::SetEnvironmentVariableUtf8(OPTION_NAME.c_str(), true_value.c_str());
      pmgr.ClearProperty(PROPERTY_NAME);
      ASSERT_TRUE(LoggerHelper::IsVerboseLoggingEnabled());

      // enable from property (only)
      ra::environment::SetEnvironmentVariableUtf8(OPTION_NAME.c_str(), NULL);
      pmgr.SetProperty(PROPERTY_NAME, true_value);
      ASSERT_TRUE(LoggerHelper::IsVerboseLoggingEnabled());

      // enable from env var and property
      ra::environment::SetEnvironmentVariableUtf8(OPTION_NAME.c_str(), true_value.c_str());
      pmgr.SetProperty(PROPERTY_NAME, true_value);
      ASSERT_TRUE(LoggerHelper::IsVerboseLoggingEnabled());


      // assert env variable has priority
      // test 1
      ra::environment::SetEnvironmentVariableUtf8(OPTION_NAME.c_str(), true_value.c_str());
      pmgr.SetProperty(PROPERTY_NAME, false_value);
      ASSERT_TRUE(LoggerHelper::IsVerboseLoggingEnabled());
      // test 2
      ra::environment::SetEnvironmentVariableUtf8(OPTION_NAME.c_str(), false_value.c_str());
      pmgr.SetProperty(PROPERTY_NAME, true_value);
      ASSERT_FALSE(LoggerHelper::IsVerboseLoggingEnabled());
    }
    //--------------------------------------------------------------------------------------------------

  } //namespace test
} //namespace shellanything
