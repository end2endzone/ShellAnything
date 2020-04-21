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

#include "TestPropertyManager.h"
#include "PropertyManager.h"

namespace shellanything { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestPropertyManager::SetUp()
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
    pmgr.Clear();
  }
  //--------------------------------------------------------------------------------------------------
  void TestPropertyManager::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testClear)
  {
    const char * name = "foo";
    const char * value = "bar";
    PropertyManager & pmgr = PropertyManager::GetInstance();
    pmgr.SetProperty(name, value);
    ASSERT_TRUE( pmgr.HasProperty(name) );
    pmgr.Clear();
    ASSERT_FALSE( pmgr.HasProperty(name) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testSetProperty)
  {
    const char * name = "foo";
    PropertyManager & pmgr = PropertyManager::GetInstance();
    pmgr.SetProperty(name, "bar");
    ASSERT_TRUE( pmgr.HasProperty(name) );
    ASSERT_EQ("bar", pmgr.GetProperty(name));

    //overwirte existing
    pmgr.SetProperty(name, "baz");
    ASSERT_TRUE( pmgr.HasProperty(name) );
    ASSERT_EQ("baz", pmgr.GetProperty(name));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testGetProperty)
  {
    const char * name = "foo";
    PropertyManager & pmgr = PropertyManager::GetInstance();
    pmgr.SetProperty(name, "bar");
    ASSERT_TRUE( pmgr.HasProperty(name) );
    ASSERT_EQ("bar", pmgr.GetProperty(name));

    //test unknown property
    ASSERT_FALSE( pmgr.HasProperty("unknown") );
    ASSERT_EQ("", pmgr.GetProperty("unknown") );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testExpand)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
    pmgr.SetProperty("job", "actor");
    pmgr.SetProperty("name", "Brad Pitt");
    pmgr.SetProperty("age", "53");
    pmgr.SetProperty("animal", "fox");

    //default behavior
    {
      std::string expanded = pmgr.Expand("${name} is a ${age} years old ${job}.");
      ASSERT_EQ("Brad Pitt is a 53 years old actor.", expanded);
    }

    //unknown properties
    {
      std::string expanded = pmgr.Expand("The quick ${color} ${animal} jumps over the ${characteristics} dog.");
      ASSERT_EQ("The quick ${color} fox jumps over the ${characteristics} dog.", expanded);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testEnvironmentVariableProperty)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

#ifdef _WIN32
    const std::string name = "env.TEMP";
#else
    const std::string name = "env.HOME";
#endif

    ASSERT_TRUE( pmgr.HasProperty(name) ) << "Missing registered property '" << name << "'.";

    const std::string value = pmgr.GetProperty(name);
    ASSERT_FALSE( value.empty() );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testClearProperty)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
    pmgr.SetProperty("job", "actor");
    pmgr.SetProperty("name", "Brad Pitt");
    pmgr.SetProperty("age", "53");
    pmgr.SetProperty("animal", "fox");
    
    ASSERT_TRUE( pmgr.HasProperty("job") );
    
    pmgr.ClearProperty("job");
    
    ASSERT_FALSE( pmgr.HasProperty("job") );
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
