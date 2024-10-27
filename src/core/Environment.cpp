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

#include "Environment.h"
#include "Validator.h"

#include "rapidassist/unicode.h"
#include "rapidassist/environment_utf8.h"

namespace shellanything
{
  static const std::string EMPTY_VALUE;

  const std::string Environment::SYSTEM_LOGGING_VERBOSE_ENVIRONMENT_VARIABLE_NAME = "SA_OPTION_LOGGING_VERBOSE";
  const std::string Environment::SYSTEM_CONFIGURATIONS_DIR_OVERRIDE_ENVIRONMENT_VARIABLE_NAME = "SA_OPTION_CONFIGURATIONS_DIR";
  const std::string Environment::SYSTEM_LOGS_DIR_OVERRIDE_ENVIRONMENT_VARIABLE_NAME = "SA_OPTION_LOGS_DIR";

  Environment::Environment()
  {
  }

  Environment::~Environment()
  {
  }

  Environment& Environment::GetInstance()
  {
    static Environment _instance;
    return _instance;
  }

  bool Environment::IsOptionSet(const std::string& name) const
  {
    std::wstring nameW = ra::unicode::Utf8ToUnicode(name);
    const wchar_t* value = _wgetenv(nameW.c_str());
    if (value == NULL)
      return false;
    return true;
  }

  bool Environment::IsOptionTrue(const std::string& name) const
  {
    std::string value = ra::environment::GetEnvironmentVariableUtf8(name.c_str());
    if (value.empty())
      return false; // does not fit the requirements

    bool is_true = Validator::IsTrue(value);
    return is_true;
  }

  std::string Environment::GetOptionValue(const std::string& name) const
  {
    std::string value = ra::environment::GetEnvironmentVariableUtf8(name.c_str());
    return value;
  }

  bool Environment::IsOptionFalse(const std::string& name) const
  {
    std::string value = ra::environment::GetEnvironmentVariableUtf8(name.c_str());
    if (value.empty())
      return false; // does not fit the requirements

    bool is_false = Validator::IsFalse(value);
    return is_false;
  }


} //namespace shellanything
