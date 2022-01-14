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

#include "TestPlatform.h"
#include "Platform.h"
#include "rapidassist/filesystem.h"

namespace shellanything { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestPlatform::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestPlatform::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPlatform, testDefaults)
  {
    std::string    home_dir = getHomeDirectory();
    std::string    apps_dir = getApplicationsDataDirectory();
    std::string    docs_dir = getDocumentsDirectory();
    std::string desktop_dir = getDesktopDirectory();

    ASSERT_FALSE(    home_dir.empty() );
    ASSERT_FALSE(    apps_dir.empty() );
    ASSERT_FALSE(    docs_dir.empty() );
    ASSERT_FALSE( desktop_dir.empty() );

    printf("   home_dir=%s\n",    home_dir.c_str());
    printf("   apps_dir=%s\n",    apps_dir.c_str());
    printf("   docs_dir=%s\n",    docs_dir.c_str());
    printf("desktop_dir=%s\n", desktop_dir.c_str());

    ASSERT_TRUE( ra::filesystem::folderExists(   home_dir.c_str()) ) << "   home_dir='" <<    home_dir << "'";
    ASSERT_TRUE( ra::filesystem::folderExists(   apps_dir.c_str()) ) << "   apps_dir='" <<    apps_dir << "'";
    ASSERT_TRUE( ra::filesystem::folderExists(   docs_dir.c_str()) ) << "   docs_dir='" <<    docs_dir << "'";
    ASSERT_TRUE( ra::filesystem::folderExists(desktop_dir.c_str()) ) << "desktop_dir='" << desktop_dir << "'";
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
