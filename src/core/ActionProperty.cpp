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
#include "SaUtils.h"

#include "rapidassist/strings.h"
#include "rapidassist/filesystem_utf8.h"

#include "tinyxml2.h"
using namespace tinyxml2;

namespace shellanything
{
  const std::string ActionProperty::XML_ELEMENT_NAME = "property";
  const size_t ActionProperty::DEFAULT_MAX_FILE_SIZE = 10240;

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

      //done parsing
      return action;
    }

  };

  IActionFactory* ActionProperty::NewFactory()
  {
    return new ActionPropertyFactory();
  }

  ActionProperty::ActionProperty()
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
    std::string exprtk = pmgr.Expand(mExprtk);
    std::string file = pmgr.Expand(mFile);
    std::string filesize = pmgr.Expand(mFileSize);
    std::string regisrykey = pmgr.Expand(mRegistryKey);

    // If exprtk is specified, it has priority over value. This is required to allow setting a property to an empty value (a.k.a. value="").
    if (!exprtk.empty())
    {
      static const size_t ERROR_SIZE = 10480;
      char error[ERROR_SIZE];
      error[0] = '\0';

      double result = 0.0;
      int evaluated = EvaluateDouble(exprtk.c_str(), &result, error, ERROR_SIZE);
      if (!evaluated)
      {
        SA_LOG(WARNING) << "Failed evaluating exprtk expression '" << exprtk << "'.";
        SA_LOG(WARNING) << "Exprtk error: " << error << "'.";
        return false;
      }

      // Store the result in 'value' as if user set this specific value (to use the same process as a property that sets a value).
      value = ra::strings::ToString(result);
    }

    // If regisrykey is specified, it has priority over value. This is required to allow setting a property to an empty value (a.k.a. value="").
    if (!regisrykey.empty())
    {
      IRegistryService* registry = App::GetInstance().GetRegistry();
      if (registry == NULL)
      {
        SA_LOG(ERROR) << "No Registry service configured for evaluating registrykey expression '" << regisrykey << "'.";
        return false;
      }

      // Query for an existing registry key
      std::string key_value;
      if (registry->GetRegistryKeyAsString(regisrykey, key_value))
      {
        // Store the result in 'value' as if user set this specific value (to use the same process as a property that sets a value).
        value = key_value;
      }
      else
      {
        SA_LOG(WARNING) << "Failed evaluating registrykey expression '" << regisrykey << "'.";
        return false;
      }
    }

    // If file is specified, it has priority over value. This is required to allow setting a property to an empty value (a.k.a. value="").
    if (!file.empty())
    {
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
    }

    //debug
    if (value.size() <= 512 && IsPrintableUtf8(value))
      SA_LOG(INFO) << "Setting property '" << name << "' to value '" << value << "'.";
    else
      SA_LOG(INFO) << "Setting property '" << name << "' to a new unprintable value.";

    pmgr.SetProperty(name, value);

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

} //namespace shellanything
