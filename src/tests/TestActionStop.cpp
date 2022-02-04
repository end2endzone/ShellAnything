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

#include "TestActionStop.h"
#include "Context.h"
#include "ActionStop.h"
#include "PropertyManager.h"
#include "rapidassist/testing.h"

namespace shellanything { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestActionStop::SetUp()
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
    pmgr.Clear();
  }
  //--------------------------------------------------------------------------------------------------
  void TestActionStop::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestActionStop, testActionStop)
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    //Create a valid context
    Context c;
    Context::ElementList elements;
    elements.push_back("C:\\Windows\\System32\\calc.exe");
    c.SetElements(elements);

    c.RegisterProperties();

    Validator * v = new Validator();

    ActionStop action;
    action.SetValidator(v);

    v->SetMaxFiles(10);
    ASSERT_TRUE( action.Execute(c) );

    v->SetMaxFiles(0);
    ASSERT_FALSE(action.Execute(c));

    v->SetMaxFiles(99999);
    v->SetProperties("foo");
    pmgr.SetProperty("foo", "bar");
    ASSERT_TRUE(action.Execute(c));

    pmgr.ClearProperty("foo");
    ASSERT_FALSE(action.Execute(c));
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
