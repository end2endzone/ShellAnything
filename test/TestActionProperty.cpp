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

#include "TestActionProperty.h"
#include "Context.h"
#include "ActionProperty.h"
#include "PropertyManager.h"

#include "rapidassist/strings.h"

#include <Windows.h>

namespace shellanything { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestActionProperty::SetUp()
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
    pmgr.Clear();
  }
  //--------------------------------------------------------------------------------------------------
  void TestActionProperty::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestActionProperty, testPropertySet)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    const std::string property_name = "foo";
    const std::string property_value = "bar";

    //Create a valid context
    Context c;
    Context::ElementList elements;
    elements.push_back("C:\\Windows");
    c.SetElements(elements);

    c.RegisterProperties();

    //execute the action
    ActionProperty ap;
    ap.SetName(property_name);
    ap.SetValue(property_value);

    bool executed = ap.Execute(c);
    ASSERT_TRUE( executed );

    //Assert property is set
    ASSERT_TRUE( pmgr.HasProperty(property_name) );
    std::string actual_value = pmgr.GetProperty(property_name);
    ASSERT_EQ( property_value, actual_value );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestActionProperty, testPropertySelfReference)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    const std::string property_name = "foo";

    //Create a valid context
    Context c;
    Context::ElementList elements;
    elements.push_back("C:\\Windows");
    c.SetElements(elements);

    c.RegisterProperties();

    // Set a base value for the property
    pmgr.SetProperty(property_name, "bar");

    // Build a value that "doubles" the initial value
    std::string value;
    value += "${";
    value += property_name;
    value += "}${";
    value += property_name;
    value += "}";

    //execute the action
    ActionProperty ap;
    ap.SetName(property_name);
    ap.SetValue(value);

    bool executed = ap.Execute(c);
    ASSERT_TRUE( executed );

    //Assert property is set
    ASSERT_TRUE( pmgr.HasProperty(property_name) );
    std::string actual_value = pmgr.GetProperty(property_name);
    ASSERT_EQ( std::string("barbar"), actual_value );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestActionProperty, testCounter)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    const std::string property_name = "foo";

    //Create a valid context
    Context c;
    Context::ElementList elements;
    elements.push_back("C:\\Windows");
    c.SetElements(elements);

    c.RegisterProperties();

    //Test if the property does not exist
    {
      //Clear the property
      pmgr.ClearProperty(property_name);

      //Execute the action multiple times
      for(int i=1; i<=10; i++)
      {
        //Execute the action
        ActionProperty ap;
        ap.SetName(property_name);
        ap.SetExprtk(
          "if ('${foo}' == '$'+'{foo}' or '${foo}' == '0') 1; "
          "else if ('${foo}' == '1') 2; "
          "else if ('${foo}' == '2') 3; "
          "else if ('${foo}' == '3') 4; "
          "else if ('${foo}' == '4') 5; "
          "else if ('${foo}' == '5') 6; "
          "else if ('${foo}' == '6') 7; "
          "else if ('${foo}' == '7') 8; "
          "else if ('${foo}' == '8') 9; "
          "else 10.0; "
        );

        bool executed = ap.Execute(c);
        ASSERT_TRUE( executed );

        //Set expected property value
        std::string expected = ra::strings::ToString(i);

        //Assert property is set
        ASSERT_TRUE( pmgr.HasProperty(property_name) );
        std::string actual_value = pmgr.GetProperty(property_name);
        ASSERT_EQ( expected, actual_value );
      }
    }

    //Test if the property is set with a default value
    {
      //Simulate default property set to 0.
      pmgr.SetProperty(property_name, "0");

      //Execute the action multiple times
      for(int i=1; i<=10; i++)
      {
        //Execute the action
        ActionProperty ap;
        ap.SetName(property_name);
        ap.SetExprtk("${foo}+1.0");

        bool executed = ap.Execute(c);
        ASSERT_TRUE( executed );

        //Set expected property value
        std::string expected = ra::strings::ToString(i);

        //Assert property is set
        ASSERT_TRUE( pmgr.HasProperty(property_name) );
        std::string actual_value = pmgr.GetProperty(property_name);
        ASSERT_EQ( expected, actual_value );
      }
    }

    int a = 0;
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestActionProperty, testPropertyGetLength)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    const std::string property_name = "foo";

    //Create a valid context
    Context c;
    Context::ElementList elements;
    elements.push_back("C:\\Windows");
    c.SetElements(elements);

    c.RegisterProperties();

    // Set a base value for the property
    pmgr.SetProperty(property_name, "barbaz");

    //execute the action
    ActionProperty ap;
    ap.SetName(property_name);
    ap.SetExprtk("'${foo}'[]");

    bool executed = ap.Execute(c);
    ASSERT_TRUE( executed );

    //Assert property is set
    ASSERT_TRUE( pmgr.HasProperty(property_name) );
    std::string actual_value = pmgr.GetProperty(property_name);
    ASSERT_EQ( std::string("6"), actual_value );
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
