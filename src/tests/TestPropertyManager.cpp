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
  TEST_F(TestPropertyManager, testExpandDefault)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    pmgr.SetProperty("job", "actor");
    pmgr.SetProperty("name", "Brad Pitt");
    pmgr.SetProperty("age", "53");

    std::string expanded = pmgr.Expand("${name} is a ${age} years old ${job}.");

    //Assert all properties was replaced
    ASSERT_EQ("Brad Pitt is a 53 years old actor.", expanded);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testExpandUnknownProperties)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    pmgr.SetProperty("animal", "fox");

    //Properties 'color' and 'characteristics' are unknown.
    std::string expanded = pmgr.Expand("The quick ${color} ${animal} jumps over the ${characteristics} dog.");

    //Assert unknown properties was not replaced. Left as is.
    ASSERT_EQ("The quick ${color} fox jumps over the ${characteristics} dog.", expanded);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testExpandMalformed)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    //Property ${color is malformed
    std::string expanded = pmgr.Expand("The quick ${color fox jumps over the lazy dog.");

    //Assert the text was left unchanged
    ASSERT_EQ("The quick ${color fox jumps over the lazy dog.", expanded);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testExpandPermutations)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    //Assert that expanding ${second} to ${third} will also expand ${third} into "Lambs"
    {
      pmgr.Clear();
      pmgr.SetProperty("first", "Silence");     // A
      pmgr.SetProperty("second", "${third}");   // B
      pmgr.SetProperty("third", "Lambs");       // C

      //The second property should be expanded by a property reference ${third}
      std::string expanded = pmgr.Expand("${first} of the ${second}");

      //Assert the property was expanded recursively
      ASSERT_EQ("Silence of the Lambs", expanded);
    }

    //Assert that property registration order have no impact.
    //Test all permutations of ABC.
    static const char * permutations[] = {"abc", "acb", "bac", "bca", "cba", "cab"};
    static const size_t num_permutations = sizeof(permutations)/sizeof(permutations[0]);
    for(size_t i=0; i<num_permutations; i++)
    {
      pmgr.Clear();

      const char * permutation = permutations[i];
      size_t length = strlen(permutation);

      //register permutations
      for(size_t j=0; j<length; j++)
      {
        char c = permutation[j];
        if (c == 'a')
          pmgr.SetProperty("first", "Silence");     // A
        else if (c == 'b')
          pmgr.SetProperty("second", "${third}");   // B
        else if (c == 'c')
          pmgr.SetProperty("third", "Lambs");       // C
        else
          FAIL() << "Unkown permutation character: " << c << ".";
      }

      //The second property should be expanded by a property reference ${third}
      std::string expanded = pmgr.Expand("${first} of the ${second}");

      //Assert the property was expanded recursively
      ASSERT_EQ("Silence of the Lambs", expanded) << "Failed testing permutation: " << permutation << ".";
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testExpandRecursiveReverseAlphabeticalOrder)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    pmgr.Clear();
    pmgr.SetProperty("text1", "with you");
    pmgr.SetProperty("text2", "the Force");
    pmgr.SetProperty("text3", "${text1}");

    //Try replacement in reverse alphabetical order.
    //If the property replacement is implemented in alphabetical order (or creation order),
    //at the time ${text3} is replaced, property 'text1' should already be processed leaving the text as "${text1}" instead of "with you".
    //If the implementation is working as expected, "${text3}" should be replaced by "with you" skipping "${text1}".
    std::string expanded = pmgr.Expand("May ${text2} be ${text3}");

    //Assert not all place holder was replaced
    ASSERT_EQ("May the Force be with you", expanded);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testExpandDouble)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    pmgr.SetProperty("first", "James");
    pmgr.SetProperty("last", "Bond");

    //Property ${action} should be expanded twice
    std::string expanded = pmgr.Expand("${last}. ${first} ${last}.");

    //Assert the ${action} was left unchanged
    ASSERT_EQ("Bond. James Bond.", expanded);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testExpandHalf)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    pmgr.SetProperty("first", "${sec");
    pmgr.SetProperty("second", "E.T. phone");

    //Property ${first} should be expanded to "${sec" which is followed by "ond}" which makes ${second} which should expand to "E.T. phone"
    std::string expanded = pmgr.Expand("${first}ond} home");

    //Assert the string was properly expanded.
    ASSERT_EQ("E.T. phone home", expanded);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testExpandRecursive1)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    pmgr.SetProperty("text1", "${text2}");
    pmgr.SetProperty("text2", "${text3}");
    pmgr.SetProperty("text3", "You're gonna need a bigger boat.");

    //Property ${text1} should expand up to the expected movie quote.
    std::string expanded = pmgr.Expand("${text1}");

    //Assert the string was properly expanded.
    ASSERT_EQ("You're gonna need a bigger boat.", expanded);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testExpandRecursive2)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    pmgr.SetProperty("varname", "foo");
    pmgr.SetProperty("foo", "${quote}");
    pmgr.SetProperty("quote", "Say 'hello' to my little friend!");

    //Property ${varname} should expand up to the expected movie quote.
    std::string expanded = pmgr.Expand("${${varname}}");

    //Assert the string was properly expanded.
    ASSERT_EQ("Say 'hello' to my little friend!", expanded);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testExpandOnceDefault)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    pmgr.SetProperty("varname", "quote");
    pmgr.SetProperty("quote", "Say 'hello' to my little friend!");

    //Property ${varname} should NOT expand up to the expected movie quote.
    std::string expanded = pmgr.ExpandOnce("${${varname}}");

    //Assert the string was properly expanded.
    ASSERT_EQ("${quote}", expanded);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertyManager, testExpandCircularReference)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    pmgr.SetProperty("first", "${second}");
    pmgr.SetProperty("second", "${first}");

    //Property ${first} expands to "${second}" which expands back to "${first}" creating a circular reference.
    std::string expanded = pmgr.Expand("${first}");

    //Nothing to assert.
    //We only care the Expand() method was not acting like an infinite loop.
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
  TEST_F(TestPropertyManager, testPropertiesAfterClear)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

#ifdef _WIN32
    const std::string env_var_name = "env.TEMP";
#else
    const std::string env_var_name = "env.HOME";
#endif

    // Test for existance of a default property.
    ASSERT_TRUE( pmgr.HasProperty("line.separator") );

    // Test for existance of an environment property.
    ASSERT_TRUE( pmgr.HasProperty(env_var_name) );

    pmgr.Clear();

    // Test for existance of a default property.
    ASSERT_TRUE( pmgr.HasProperty("line.separator") );

    // Test for existance of an environment property.
    ASSERT_TRUE( pmgr.HasProperty(env_var_name) );
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
