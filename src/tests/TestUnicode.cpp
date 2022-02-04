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

#include "TestUnicode.h"
#include "Unicode.h"
#include "rapidassist\testing.h"

namespace shellanything { namespace test
{
  const std::string psi = "psi_\xCE\xA8_psi";
  const std::string omega = "omega_\xCE\xA9_omega";
  const std::string copyright = "copyright_\xC2\xA9_copyright";
  const std::string spanish = "espa" "\xC3\xB1" "ol";   //spanish in spanish, encoded in UTF-8, U+00F1
  const std::string school = "\xC3\xA9" "cole";         //school in french, encoded in UTF-8
  const std::string invalid = "invalid_\xC2\xC2_invalid";
  const std::string all = "\xCE\xA8\xCE\xA9\xC2\xA9\xC3\xB1\xC3\xA9"; // psiomegacopyrightne

  //--------------------------------------------------------------------------------------------------
  void TestUnicode::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestUnicode::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestUnicode, testGetCodePointLengthUtf8)
  {
    ASSERT_EQ(1, GetCodePointLengthUtf8(&psi[0])); //p
    ASSERT_EQ(1, GetCodePointLengthUtf8(&psi[1])); //s
    ASSERT_EQ(1, GetCodePointLengthUtf8(&psi[2])); //i
    ASSERT_EQ(1, GetCodePointLengthUtf8(&psi[3])); //_
    ASSERT_EQ(2, GetCodePointLengthUtf8(&psi[4])); //psi code point

    ASSERT_EQ(1, GetCodePointLengthUtf8(psi.c_str(), 0)); //p
    ASSERT_EQ(1, GetCodePointLengthUtf8(psi.c_str(), 1)); //s
    ASSERT_EQ(1, GetCodePointLengthUtf8(psi.c_str(), 2)); //i
    ASSERT_EQ(1, GetCodePointLengthUtf8(psi.c_str(), 3)); //_
    ASSERT_EQ(2, GetCodePointLengthUtf8(psi.c_str(), 4)); //psi code point

    ASSERT_EQ(0, GetCodePointLengthUtf8(invalid.c_str(), 8)); //C2 C2
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestUnicode, testGetLengthUtf8)
  {
    size_t psi_length = GetLengthUtf8(psi.c_str());
    ASSERT_EQ(9, psi_length); //psi_*2 and the psi code point

    size_t omega_length = GetLengthUtf8(omega.c_str());
    ASSERT_EQ(13, omega_length);

    size_t spanish_length = GetLengthUtf8(spanish.c_str());
    ASSERT_EQ(7, spanish_length);

    size_t school_length = GetLengthUtf8(school.c_str());
    ASSERT_EQ(5, school_length);

    size_t invalid_length = GetLengthUtf8(invalid.c_str());
    ASSERT_EQ(0, invalid_length);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestUnicode, testSubstringUtf8)
  {
    // Substring all code points
    {
      std::string sub = SubstringUtf8(psi.c_str(), 0, GetLengthUtf8(psi.c_str()));
      ASSERT_EQ(psi, sub);
    }

    // Test length=0
    {
      std::string sub = SubstringUtf8(psi.c_str(), 0, 0);
      ASSERT_EQ(std::string(), sub);
    }

    // Test length
    {
      static const size_t num_cp = 6;
      std::string sub = SubstringUtf8(psi.c_str(), 0, num_cp);
      ASSERT_EQ(std::string("psi_\xCE\xA8_"), sub);
      ASSERT_GT(sub.size(), num_cp);
    }

    // Test length max
    {
      static const size_t num_cp = 6;
      std::string sub = SubstringUtf8(psi.c_str(), 4, 999999);
      ASSERT_EQ(std::string("\xCE\xA8_psi"), sub);
    }

    // Test start
    {
      static const size_t offset_cp = 4;
      static const size_t num_cp = 2;
      std::string sub = SubstringUtf8(psi.c_str(), offset_cp, num_cp);
      ASSERT_EQ(std::string("\xCE\xA8_"), sub);
    }

    // Test all at once
    {
      static const size_t offset_cp = 2;
      static const size_t num_cp = 3;
      std::string sub = SubstringUtf8(all.c_str(), offset_cp, num_cp);
      ASSERT_EQ(std::string("\xC2\xA9\xC3\xB1\xC3\xA9"), sub);
    }
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
