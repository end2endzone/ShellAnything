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

#include "TestContext.h"
#include "shellanything/Context.h"

namespace shellanything { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestContext::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestContext::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestContext, testDefaults)
  {
    Context c;
    ASSERT_EQ( 0, c.getNumFiles() );
    ASSERT_EQ( 0, c.getNumDirectories() );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestContext, testFileDirectoryAutoDetect)
  {
    Context c;
#ifdef _WIN32
    {
      Context::ElementList elements;
      elements.push_back("C:\\Windows\\System32\\kernel32.dll");
      elements.push_back("C:\\Windows\\System32\\cmd.exe"     );
      elements.push_back("C:\\Windows\\System32\\notepad.exe" );
      elements.push_back("C:\\Windows\\System32\\services.msc");
      elements.push_back("C:\\Program Files");
      elements.push_back("C:\\Users"        );
      elements.push_back("C:\\Windows"      );
      c.setElements(elements);
    }
#else
    //TODO: complete with known path to files
#endif

    ASSERT_EQ( 4, c.getNumFiles() );
    ASSERT_EQ( 3, c.getNumDirectories() );
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
