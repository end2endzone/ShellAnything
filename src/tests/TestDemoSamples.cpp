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

#include "TestDemoSamples.h"
#include "PropertyManager.h"
#include "ActionPrompt.h"

namespace shellanything { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestDemoSamples::SetUp()
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
    pmgr.Clear();
  }
  //--------------------------------------------------------------------------------------------------
  void TestDemoSamples::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestDemoSamples, DISABLED_demoActionPromptQuestion)
  {
    ActionPrompt prompt;
    prompt.SetName("myproperty");
    prompt.SetTitle("What is your name?");
    prompt.SetDefault("Isabelle Ringing");

    Context c;

    bool result = prompt.Execute(c);
    
    ASSERT_TRUE(result);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestDemoSamples, DISABLED_demoActionPromptYesNo)
  {

    ActionPrompt prompt;
    prompt.SetType("yesno");
    prompt.SetValueYes("yes");
    prompt.SetValueNo("no");
    prompt.SetName("myproperty");
    prompt.SetTitle("Are you afraid of the dark?");
    
    Context c;

    bool result = prompt.Execute(c);
    
    ASSERT_TRUE(result);

    //assert user have answered "yes"
    PropertyManager & pmgr = PropertyManager::GetInstance();
    std::string myproperty = pmgr.GetProperty("myproperty");
    ASSERT_TRUE(myproperty == prompt.GetValueYes() || myproperty == prompt.GetValueNo());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestDemoSamples, DISABLED_demoActionPromptOk)
  {
 
    ActionPrompt prompt;
    prompt.SetType("ok");
    prompt.SetName("myproperty");
    prompt.SetTitle("Wait for the system to load and press OK button.");
   
    Context c;
 
    bool result = prompt.Execute(c);
   
    ASSERT_TRUE(result);
    ASSERT_TRUE(prompt.IsOkQuestion());
 
    //assert property is unchanged
    PropertyManager & pmgr = PropertyManager::GetInstance();
    pmgr.SetProperty("myproperty", "test");
    std::string myproperty = pmgr.GetProperty("myproperty");
    ASSERT_EQ( std::string("test"), myproperty );
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
