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

#include "ActionProperty.h"
#include "PropertyManager.h"
#include "libexprtk.h"
#include "ObjectFactory.h"
#include "LoggerHelper.h"
#include "RandomHelper.h"
#include "SaUtils.h"

#include "rapidassist/strings.h"
#include "rapidassist/filesystem_utf8.h"

#include "tinyxml2.h"
using namespace tinyxml2;

namespace shellanything
{
  const std::string ActionProperty::XML_ELEMENT_NAME = "property";
  const size_t ActionProperty::DEFAULT_MAX_FILE_SIZE = 10240;
  const std::string ActionProperty::DEFAULT_FAIL_VALUE = "true";

  class ActionPropertyFactory : public virtual IActionFactory
  {
  public:
    ActionPropertyFactory()
    {
    }
    virtual ~ActionPropertyFactory()
    {
    }

    virtual const std::string& GetName() const
    {
      return ActionProperty::XML_ELEMENT_NAME;
    }

    virtual IAction* ParseFromXml(const std::string& xml, std::string& error) const
    {
      tinyxml2::XMLDocument doc;
      XMLError result = doc.Parse(xml.c_str());
      if (result != XML_SUCCESS)
      {
        if (doc.ErrorStr())
        {
          error = doc.ErrorStr();
          return NULL;
        }
        else
        {
          error = "Unknown error reported by XML library.";
          return NULL;
        }
      }
      XMLElement* element = doc.FirstChildElement(GetName().c_str());

      ActionProperty* action = new ActionProperty();
      std::string tmp_str;

      //parse name
      tmp_str = "";
      if (ObjectFactory::ParseAttribute(element, "name", false, true, tmp_str, error))
      {
        action->SetName(tmp_str);
      }

      //parse value
      tmp_str = "";
      if (ObjectFactory::ParseAttribute(element, "value", true, true, tmp_str, error))
      {
        action->SetValue(tmp_str);
      }

      //parse exprtk
      tmp_str = "";
      if (ObjectFactory::ParseAttribute(element, "exprtk", true, true, tmp_str, error))
      {
        action->SetExprtk(tmp_str);
      }

      //parse file
      tmp_str = "";
      if (ObjectFactory::ParseAttribute(element, "file", true, true, tmp_str, error))
      {
        action->SetFile(tmp_str);
      }

      //parse filesize
      tmp_str = "";
      if (ObjectFactory::ParseAttribute(element, "filesize", true, true, tmp_str, error))
      {
        action->SetFileSize(tmp_str);
      }

      //parse registrykey
      tmp_str = "";
      if (ObjectFactory::ParseAttribute(element, "registrykey", true, true, tmp_str, error))
      {
        action->SetRegistryKey(tmp_str);
      }

      //parse searchpath
      tmp_str = "";
      if (ObjectFactory::ParseAttribute(element, "searchpath", true, true, tmp_str, error))
      {
        action->SetSearchPath(tmp_str);
      }

      //parse random
      tmp_str = "";
      if (ObjectFactory::ParseAttribute(element, "random", true, true, tmp_str, error))
      {
        action->SetRandom(tmp_str);
      }

      //parse randommin
      tmp_str = "";
      if (ObjectFactory::ParseAttribute(element, "randommin", true, true, tmp_str, error))
      {
        action->SetRandomMin(tmp_str);
      }

      //parse randommax
      tmp_str = "";
      if (ObjectFactory::ParseAttribute(element, "randommax", true, true, tmp_str, error))
      {
        action->SetRandomMax(tmp_str);
      }

      //parse fail
      tmp_str = "";
      if (ObjectFactory::ParseAttribute(element, "fail", true, true, tmp_str, error))
      {
        action->SetFail(tmp_str);
      }

      //done parsing
      return action;
    }

  };

  IActionFactory* ActionProperty::NewFactory()
  {
    return new ActionPropertyFactory();
  }

  ActionProperty::ActionProperty() :
    mFail(DEFAULT_FAIL_VALUE)
  {
  }

  ActionProperty::~ActionProperty()
  {
  }

  bool ActionProperty::Execute(const SelectionContext& context) const
  {
    PropertyManager& pmgr = PropertyManager::GetInstance();
    std::string name = pmgr.Expand(mName);
    std::string value = pmgr.Expand(mValue);
    std::string fail = pmgr.Expand(mFail);
    std::string exprtk = pmgr.Expand(mExprtk);
    std::string file = pmgr.Expand(mFile);
    std::string filesize = pmgr.Expand(mFileSize);
    std::string registrykey = pmgr.Expand(mRegistryKey);
    std::string searchpath = pmgr.Expand(mSearchPath);
    std::string random = pmgr.Expand(mRandom);
    std::string random_min = pmgr.Expand(mRandomMin);
    std::string random_max = pmgr.Expand(mRandomMax);

    // Set a flag if an actual value must be set.
    bool has_new_value = false;
    if (
      exprtk.empty() &&
      file.empty() &&
      filesize.empty() &&
      registrykey.empty() &&
      searchpath.empty() &&
      random.empty())
    {
      // If no automatic value has been specified, then value is an actual new value, even if empty.
      has_new_value = true;
    }

    // If random is specified, it has priority over value. This is required to allow setting a property to an empty value (a.k.a. value="").
    if (!random.empty())
    {
      std::string tmp_value;
      bool resolved = GetValueFromRandom(random, random_min, random_max, tmp_value);
      if (Validator::IsTrue(fail) && !resolved)
      {
        SA_LOG(WARNING) << "Reporting an error because fail is set to '" << fail << "'.";
        return false;
      }
      if (resolved)
      {
        // Store the result in 'value' as if user set this specific value (to use the same process as a property that sets a value).
        value = tmp_value;
        has_new_value = true;
      }
    }

    // If searchpath is specified, it has priority over value. This is required to allow setting a property to an empty value (a.k.a. value="").
    if (!searchpath.empty())
    {
      std::string tmp_value;
      bool resolved = GetValueFromSearchPath(searchpath, tmp_value);
      if (Validator::IsTrue(fail) && !resolved)
      {
        SA_LOG(WARNING) << "Reporting an error because fail is set to '" << fail << "'.";
        return false;
      }
      if (resolved)
      {
        // Store the result in 'value' as if user set this specific value (to use the same process as a property that sets a value).
        value = tmp_value;
        has_new_value = true;
      }
    }

    // If exprtk is specified, it has priority over value.
    if (!exprtk.empty())
    {
      std::string tmp_value;
      bool resolved = GetValueFromExprtk(exprtk, tmp_value);
      if (Validator::IsTrue(fail) && !resolved)
      {
        SA_LOG(WARNING) << "Reporting an error because fail is set to '" << fail << "'.";
        return false;
      }
      if (resolved)
      {
        // Store the result in 'value' as if user set this specific value (to use the same process as a property that sets a value).
        value = tmp_value;
        has_new_value = true;
      }
    }

    // If regisrykey is specified, it has priority over value. This is required to allow setting a property to an empty value (a.k.a. value="").
    if (!registrykey.empty())
    {
      std::string tmp_value;
      bool resolved = GetValueFromRegistryKey(registrykey, tmp_value);
      if (Validator::IsTrue(fail) && !resolved)
      {
        SA_LOG(WARNING) << "Reporting an error because fail is set to '" << fail << "'.";
        return false;
      }
      if (resolved)
      {
        // Store the result in 'value' as if user set this specific value (to use the same process as a property that sets a value).
        value = tmp_value;
        has_new_value = true;
      }
    }

    // If file is specified, it has priority over value. This is required to allow setting a property to an empty value (a.k.a. value="").
    if (!file.empty())
    {
      std::string tmp_value;
      bool resolved = GetValueFromFile(file, filesize, tmp_value);
      if (Validator::IsTrue(fail) && !resolved)
      {
        SA_LOG(WARNING) << "Reporting an error because fail is set to '" << fail << "'.";
        return false;
      }
      if (resolved)
      {
        // Store the result in 'value' as if user set this specific value (to use the same process as a property that sets a value).
        value = tmp_value;
        has_new_value = true;
      }
    }

    if (!has_new_value)
    {
      SA_LOG(WARNING) << "Property '" << name << "' has not been updated.";
    }
    else
    {
      //debug
      if (value.size() <= 512 && IsPrintableUtf8(value))
        SA_LOG(INFO) << "Setting property '" << name << "' to value '" << value << "'.";
      else
        SA_LOG(INFO) << "Setting property '" << name << "' to a new unprintable value.";

      // Update the new property.
      pmgr.SetProperty(name, value);
    }

    // If a user has changed property 'selection.multi.separator', the context must rebuild selection-based properties.
    // See issue #52 for details.
    if (name == SelectionContext::MULTI_SELECTION_SEPARATOR_PROPERTY_NAME)
    {
      // Force the context to rebuild selection.* properties.
      context.RegisterProperties();
    }

    return true;
  }

  const std::string& ActionProperty::GetName() const
  {
    return mName;
  }

  void ActionProperty::SetName(const std::string& name)
  {
    mName = name;
  }

  const std::string& ActionProperty::GetValue() const
  {
    return mValue;
  }

  void ActionProperty::SetValue(const std::string& value)
  {
    mValue = value;
  }

  const std::string& ActionProperty::GetExprtk() const
  {
    return mExprtk;
  }

  void ActionProperty::SetExprtk(const std::string& exprtk)
  {
    mExprtk = exprtk;
  }

  const std::string& ActionProperty::GetFile() const
  {
    return mFile;
  }

  void ActionProperty::SetFile(const std::string& value)
  {
    mFile = value;
  }

  const std::string& ActionProperty::GetFileSize() const
  {
    return mFileSize;
  }

  void ActionProperty::SetFileSize(const std::string& value)
  {
    mFileSize = value;
  }

  const std::string& ActionProperty::GetRegistryKey() const
  {
    return mRegistryKey;
  }

  void ActionProperty::SetRegistryKey(const std::string& value)
  {
    mRegistryKey = value;
  }

  const std::string& ActionProperty::GetSearchPath() const
  {
    return mSearchPath;
  }

  void ActionProperty::SetSearchPath(const std::string& value)
  {
    mSearchPath = value;
  }

  const std::string& ActionProperty::GetRandom() const
  {
    return mRandom;
  }

  void ActionProperty::SetRandom(const std::string& value)
  {
    mRandom = value;
  }

  const std::string& ActionProperty::GetRandomMin() const
  {
    return mRandomMin;
  }

  void ActionProperty::SetRandomMin(const std::string& value)
  {
    mRandomMin = value;
  }

  const std::string& ActionProperty::GetRandomMax() const
  {
    return mRandomMax;
  }

  void ActionProperty::SetRandomMax(const std::string& value)
  {
    mRandomMax = value;
  }

  const std::string& ActionProperty::GetFail() const
  {
    return mFail;
  }

  void ActionProperty::SetFail(const std::string& value)
  {
    mFail = value;
  }

  bool ActionProperty::GetValueFromExprtk(const std::string& exprtk, std::string& value) const
  {
    static const size_t ERROR_SIZE = 10480;
    char error[ERROR_SIZE];
    error[0] = '\0';

    double result = 0.0;
    int evaluated = EvaluateDouble(exprtk.c_str(), &result, error, ERROR_SIZE);
    if (!evaluated)
    {
      // report a warning.
      SA_LOG(WARNING) << "Failed evaluating exprtk expression '" << exprtk << "'.";
      SA_LOG(WARNING) << "Exprtk error: " << error << "'.";
      return false;
    }

    value = ra::strings::ToString(result);
    return true;
  }

  bool ActionProperty::GetValueFromFile(const std::string& file, const std::string& filesize, std::string& value) const
  {
    PropertyManager& pmgr = PropertyManager::GetInstance();
    std::string name = pmgr.Expand(mName);

    // Validate input file
    if (!ra::filesystem::FileExistsUtf8(file.c_str()))
    {
      SA_LOG(WARNING) << "Failed setting property '" << name << "' from file '" << file << "'. File not found!";
      return false;
    }
    if (!ra::filesystem::HasFileReadAccessUtf8(file.c_str()))
    {
      SA_LOG(WARNING) << "Failed setting property '" << name << "' from file '" << file << "'. File cannot be read!";
      return false;
    }

    // Define the maximum number of bytes to read.
    size_t max_read_size = DEFAULT_MAX_FILE_SIZE;

    // Did user specified a maximum number of bytes to read?
    bool custom_file_size = false;
    if (!filesize.empty())
    {
      size_t tmp_file_size = 0;
      bool parsed = ra::strings::Parse(filesize, tmp_file_size);
      if (!parsed)
      {
        SA_LOG(WARNING) << "Failed parsing filesize value '" << filesize << "'.";
        return false;
      }

      custom_file_size = true;
      max_read_size = tmp_file_size;
    }

    // Custom log entry
    if (custom_file_size && max_read_size != 0)
      SA_LOG(INFO) << "Setting property '" << name << "' from the first " << max_read_size << " bytes of file '" << file << "'.";
    else
      SA_LOG(INFO) << "Setting property '" << name << "' from file '" << file << "'.";

    // Set max_read_size to infinite if set to special value 0.
    if (max_read_size == 0)
      max_read_size = (size_t)-1;

    // Do the actual reading from the file.
    bool file_read_success = ra::filesystem::PeekFileUtf8(file.c_str(), max_read_size, value);
    if (!file_read_success)
    {
      SA_LOG(WARNING) << "Failed setting property '" << name << "' from file '" << file << "'. File cannot be read!";
      return false;
    }

    SA_LOG(INFO) << "Read " << value.size() << " bytes from file '" << file << "'.";
    return true;
  }

  bool ActionProperty::GetValueFromRegistryKey(const std::string& registrykey, std::string& value) const
  {
    IRegistryService* registry = App::GetInstance().GetRegistryService();
    if (registry == NULL)
    {
      SA_LOG(ERROR) << "No Registry service configured for evaluating registrykey expression '" << registrykey << "'.";
      return false;
    }

    // Query for an existing registry key
    std::string key_value;
    bool success = (registry->GetRegistryKeyAsString(registrykey, key_value));

    if (!success)
    {
      SA_LOG(WARNING) << "Failed evaluating registrykey expression '" << registrykey << "'.";
      return false;
    }

    value = key_value;
    return true;
  }

  bool ActionProperty::GetValueFromSearchPath(const std::string& searchpath, std::string& value) const
  {
    // Search for a file in PATH environment variable.
    std::string abs_path = ra::filesystem::FindFileFromPathsUtf8(searchpath);
    bool success = !abs_path.empty();

    if (!success)
    {
      // Only report a warning.
      SA_LOG(WARNING) << "File not found in PATH environment variable: '" << searchpath << "'.";
      return false;
    }
    value = abs_path;
    return true;
  }

  bool ActionProperty::GetValueFromRandom(const std::string& random, const std::string& random_min, std::string& random_max, std::string& value) const
  {
    IRandomService* random_service = App::GetInstance().GetRandomService();
    if (random_service == NULL)
    {
      SA_LOG(ERROR) << "No Random service configured for evaluating random expression '" << random << "'.";
      return false;
    }

    if (random.empty())
    {
      SA_LOG(WARNING) << "Failed evaluating random expression. The value is empty.";
      return false;
    }

    // Check if min/max are specified
    bool min_max_mode = false;
    uint32_t min_value = 0;
    uint32_t max_value = uint32_t(-1);

    if (!random_min.empty())
    {
      uint32_t tmp = 0;
      bool parsed = ra::strings::Parse(random_min, tmp);
      if (!parsed)
      {
        SA_LOG(WARNING) << "Failed parsing randommin value '" << random_min << "'.";
        return false;
      }

      min_max_mode = true;
      min_value = tmp;
    }
    if (!random_max.empty())
    {
      uint32_t tmp = 0;
      bool parsed = ra::strings::Parse(random_max, tmp);
      if (!parsed)
      {
        SA_LOG(WARNING) << "Failed parsing randommax value '" << random_max << "'.";
        return false;
      }

      min_max_mode = true;
      max_value = tmp;
    }

    // Validate min/max
    if (min_max_mode && min_value > max_value)
    {
      SA_LOG(WARNING) << "Failed validating random min/max: randommin (" << min_value << ") must be smaller than randommax (" << max_value << ").";
      return false;
    }

    // Check which mode we are generating
    if (!min_max_mode)
    {
      // Generate from the pattern
      std::string random_string;
      bool success = RandomHelper::GetRandomFromPattern(random, random_string);

      if (!success)
      {
        SA_LOG(WARNING) << "Failed evaluating random expression '" << random << "'.";
        return false;
      }

      value = random_string;
      return true;
    }
    else
    {
      // Generate from the pattern using custom min/max values
      if (!RandomHelper::IsNumericPattern(random))
      {
        SA_LOG(WARNING) << "Failed validating random expression '" << random << "'. The expression must only contain a numerical pattern (digits).";
        return false;
      }

      // Limit maximum value based on pattern length if not specified.
      if (random_max.empty())
      {
        max_value = RandomHelper::GetMaxValue(random.size());
      }
      
      // Validate pattern length against
      size_t min_length = RandomHelper::GetNumericPatternLength(&random[0]);
      std::string random_string = RandomHelper::GetRandomMinMaxValue(min_value, max_value + 1); // +1 to include max_value as a possible outcome.

      // Add leading zeroes if pattern is made of '0' characters and generated value is smaller than pattern.
      bool must_have_leading_zeroes = (random[0] == RandomHelper::NUMERIC_DIGIT_WITH_LEADING_ZEROS_PATTERN);
      if (must_have_leading_zeroes)
      {
        RandomHelper::FormatLeadingZeroes(random_string, min_length);
      }

      value = random_string;
      return true;
    }
  }

} //namespace shellanything
