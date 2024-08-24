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
#include "Validator.h"

#include "rapidassist/environment_utf8.h"

namespace shellanything
{
  namespace test
  {
    static const std::string& VERBOSE_OPTION_NAME = Environment::SYSTEM_LOGGING_VERBOSE_ENVIRONMENT_VARIABLE_NAME;
    static const std::string& VERBOSE_PROPERTY_NAME = PropertyManager::SYSTEM_LOGGING_VERBOSE_PROPERTY_NAME;

    static bool gRestoreVerboseState = false;
    static bool gVerboseEnvVarIsSet = false;
    static bool gVerbosePropertyIsSet = false;

    //--------------------------------------------------------------------------------------------------
    void TestLoggerHelper::SetUp()
    {
    }
    //--------------------------------------------------------------------------------------------------
    void TestLoggerHelper::TearDown()
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();
      Environment& env = Environment::GetInstance();
      const std::string& true_value = pmgr.GetProperty(PropertyManager::SYSTEM_TRUE_PROPERTY_NAME);
      const std::string& false_value = pmgr.GetProperty(PropertyManager::SYSTEM_FALSE_PROPERTY_NAME);

      // cleanup test that messes with verbose mode
      if (gRestoreVerboseState)
      {
        gRestoreVerboseState = false;

        // restore env var
        if (gVerboseEnvVarIsSet)
          ra::environment::SetEnvironmentVariableUtf8(VERBOSE_OPTION_NAME.c_str(), true_value.c_str());
        else
          ra::environment::SetEnvironmentVariableUtf8(VERBOSE_OPTION_NAME.c_str(), NULL);

        // restore property
        if (gVerbosePropertyIsSet)
          pmgr.SetProperty(VERBOSE_PROPERTY_NAME, true_value);
        else
          pmgr.ClearProperty(VERBOSE_PROPERTY_NAME);
      }
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestLoggerHelper, testIsVerboseLoggingEnabled)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();
      Environment& env = Environment::GetInstance();
      const std::string& true_value = pmgr.GetProperty(PropertyManager::SYSTEM_TRUE_PROPERTY_NAME);
      const std::string& false_value = pmgr.GetProperty(PropertyManager::SYSTEM_FALSE_PROPERTY_NAME);

      // backup current status
      gVerboseEnvVarIsSet = env.IsOptionTrue(Environment::SYSTEM_LOGGING_VERBOSE_ENVIRONMENT_VARIABLE_NAME);
      gVerbosePropertyIsSet = Validator::IsTrue(pmgr.GetProperty(PropertyManager::SYSTEM_LOGGING_VERBOSE_PROPERTY_NAME));
      gRestoreVerboseState = true; // tell TearDown() to restore this state

      // clear all
      ra::environment::SetEnvironmentVariableUtf8(VERBOSE_OPTION_NAME.c_str(), NULL);
      pmgr.ClearProperty(VERBOSE_PROPERTY_NAME);
      ASSERT_FALSE(LoggerHelper::IsVerboseLoggingEnabled());

      // enable from env var (only)
      ra::environment::SetEnvironmentVariableUtf8(VERBOSE_OPTION_NAME.c_str(), true_value.c_str());
      pmgr.ClearProperty(VERBOSE_PROPERTY_NAME);
      ASSERT_TRUE(LoggerHelper::IsVerboseLoggingEnabled());

      // enable from property (only)
      ra::environment::SetEnvironmentVariableUtf8(VERBOSE_OPTION_NAME.c_str(), NULL);
      pmgr.SetProperty(VERBOSE_PROPERTY_NAME, true_value);
      ASSERT_TRUE(LoggerHelper::IsVerboseLoggingEnabled());

      // enable from env var and property
      ra::environment::SetEnvironmentVariableUtf8(VERBOSE_OPTION_NAME.c_str(), true_value.c_str());
      pmgr.SetProperty(VERBOSE_PROPERTY_NAME, true_value);
      ASSERT_TRUE(LoggerHelper::IsVerboseLoggingEnabled());


      // assert env variable has priority
      // test 1
      ra::environment::SetEnvironmentVariableUtf8(VERBOSE_OPTION_NAME.c_str(), true_value.c_str());
      pmgr.SetProperty(VERBOSE_PROPERTY_NAME, false_value);
      ASSERT_TRUE(LoggerHelper::IsVerboseLoggingEnabled());
      // test 2
      ra::environment::SetEnvironmentVariableUtf8(VERBOSE_OPTION_NAME.c_str(), false_value.c_str());
      pmgr.SetProperty(VERBOSE_PROPERTY_NAME, true_value);
      ASSERT_FALSE(LoggerHelper::IsVerboseLoggingEnabled());
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestLoggerHelper, testScopeLogger)
    {
      SA_LOG(INFO) << __FUNCTION__ "(), line " << __LINE__ << " is the first line of code";

      SA_DECLARE_SCOPE_LOGGER_ARGS(sli);
      sli.verbose = true;
      sli.level = ::shellanything::ILoggerService::LOG_LEVEL::LOG_LEVEL_INFO; // force INFO level to make sure it is always visible
      ScopeLogger logger(&sli);

      SA_LOG(INFO) << __FUNCTION__ "(), line " << __LINE__ << " is the last line of code";
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestLoggerHelper, testMacroWithAllLOG_LEVEL)
    {
      SA_LOG(DEBUG)   << "This is a demo DEBUG   message";
      SA_LOG(INFO)    << "This is a demo INFO    message";
      SA_LOG(WARNING) << "This is a demo WARNING message";
      SA_LOG(ERROR)   << "This is a demo ERROR   message";
      //SA_LOG(FATAL)   << "This is a demo FATAL   message"; // FATAL GLOG MESSAGES actually terminates the application
    }
    //--------------------------------------------------------------------------------------------------
    TEST_F(TestLoggerHelper, testMacroSA_VERBOSE_LOG)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();
      Environment& env = Environment::GetInstance();
      const std::string& true_value = pmgr.GetProperty(PropertyManager::SYSTEM_TRUE_PROPERTY_NAME);
      const std::string& false_value = pmgr.GetProperty(PropertyManager::SYSTEM_FALSE_PROPERTY_NAME);

      // backup current status
      gVerboseEnvVarIsSet = env.IsOptionTrue(Environment::SYSTEM_LOGGING_VERBOSE_ENVIRONMENT_VARIABLE_NAME);
      gVerbosePropertyIsSet = Validator::IsTrue(pmgr.GetProperty(PropertyManager::SYSTEM_LOGGING_VERBOSE_PROPERTY_NAME));
      gRestoreVerboseState = true; // tell TearDown() to restore this state

      // set verbose OFF
      ra::environment::SetEnvironmentVariableUtf8(VERBOSE_OPTION_NAME.c_str(), false_value.c_str());
      SA_VERBOSE_LOG(INFO) << "This VERBOSE message SHOULD NOT be visible since verbose is OFF.";

      // set verbose ON
      ra::environment::SetEnvironmentVariableUtf8(VERBOSE_OPTION_NAME.c_str(), true_value.c_str());
      SA_VERBOSE_LOG(INFO) << "This VERBOSE message is expected to be visible since verbose is ON.";
    }
    //--------------------------------------------------------------------------------------------------

  } //namespace test
} //namespace shellanything
