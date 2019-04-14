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
    PropertyManager & pmgr = PropertyManager::getInstance();
    pmgr.clear();
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
    PropertyManager & pmgr = PropertyManager::getInstance();
    pmgr.setProperty(name, value);
    ASSERT_TRUE( pmgr.hasProperty(name) );
    pmgr.clear();
    ASSERT_FALSE( pmgr.hasProperty(name) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testSetProperty)
  {
    const char * name = "foo";
    PropertyManager & pmgr = PropertyManager::getInstance();
    pmgr.setProperty(name, "bar");
    ASSERT_TRUE( pmgr.hasProperty(name) );
    ASSERT_EQ("bar", pmgr.getProperty(name));

    //overwirte existing
    pmgr.setProperty(name, "baz");
    ASSERT_TRUE( pmgr.hasProperty(name) );
    ASSERT_EQ("baz", pmgr.getProperty(name));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testGetProperty)
  {
    const char * name = "foo";
    PropertyManager & pmgr = PropertyManager::getInstance();
    pmgr.setProperty(name, "bar");
    ASSERT_TRUE( pmgr.hasProperty(name) );
    ASSERT_EQ("bar", pmgr.getProperty(name));

    //test unknown property
    ASSERT_FALSE( pmgr.hasProperty("unknown") );
    ASSERT_EQ("", pmgr.getProperty("unknown") );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testExpand)
  {
    PropertyManager & pmgr = PropertyManager::getInstance();
    pmgr.setProperty("job", "actor");
    pmgr.setProperty("name", "Brad Pitt");
    pmgr.setProperty("age", "53");
    pmgr.setProperty("animal", "fox");

    //default behavior
    {
      std::string expanded = pmgr.expand("${name} is a ${age} years old ${job}.");
      ASSERT_EQ("Brad Pitt is a 53 years old actor.", expanded);
    }

    //unknown properties
    {
      std::string expanded = pmgr.expand("The quick ${color} ${animal} jumps over the ${characteristics} dog.");
      ASSERT_EQ("The quick ${color} fox jumps over the ${characteristics} dog.", expanded);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testEnvironmentVariableProperty)
  {
    PropertyManager & pmgr = PropertyManager::getInstance();

#ifdef _WIN32
    const std::string name = "env.TEMP";
#else
    const std::string name = "env.HOME";
#endif

    ASSERT_TRUE( pmgr.hasProperty(name) ) << "Missing registered property '" << name << "'.";

    const std::string value = pmgr.getProperty(name);
    ASSERT_FALSE( value.empty() );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testClearProperty)
  {
    PropertyManager & pmgr = PropertyManager::getInstance();
    pmgr.setProperty("job", "actor");
    pmgr.setProperty("name", "Brad Pitt");
    pmgr.setProperty("age", "53");
    pmgr.setProperty("animal", "fox");
    
    ASSERT_TRUE( pmgr.hasProperty("job") );
    
    pmgr.clearProperty("job");
    
    ASSERT_FALSE( pmgr.hasProperty("job") );
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
