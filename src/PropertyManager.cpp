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

#include "PropertyManager.h"

#include "shellanything/Context.h"

#include "rapidassist/environment_utf8.h"
#include "rapidassist/filesystem_utf8.h"

namespace shellanything
{

  PropertyManager::PropertyManager()
  {
    RegisterEnvironmentVariables();
    RegisterDefaultProperties();
  }

  PropertyManager::~PropertyManager()
  {
  }

  PropertyManager & PropertyManager::GetInstance()
  {
    static PropertyManager _instance;
    return _instance;
  }

  void PropertyManager::Clear()
  {
    properties.clear();
    RegisterEnvironmentVariables();
    RegisterDefaultProperties();
  }

  void PropertyManager::ClearProperty(const std::string & name)
  {
    PropertyMap::const_iterator propertyIt = properties.find(name);
    bool found = (propertyIt != properties.end());
    if (found)
    {
      properties.erase(propertyIt);
    }
  }

  bool PropertyManager::HasProperty(const std::string & name) const
  {
    PropertyMap::const_iterator propertyIt = properties.find(name);
    bool found = (propertyIt != properties.end());
    return found;
  }

  void PropertyManager::SetProperty(const std::string & name, const std::string & value)
  {
    //overwrite previous property
    properties[name] = value;
  }

  const std::string & PropertyManager::GetProperty(const std::string & name) const
  {
    PropertyMap::const_iterator propertyIt = properties.find(name);
    bool found = (propertyIt != properties.end());
    if (found)
    {
      const std::string & value = propertyIt->second;
      return value;
    }

    static std::string EMPTY_VALUE;
    return EMPTY_VALUE;
  }

  bool IsPropertyReference(const std::string & token_open, const std::string & token_close, const std::string & value, size_t offset, std::string & name)
  {
    size_t value_length = value.size();
    name.clear();
    if (offset >= value_length)
      return false;

    //Validate a token_open prefix at value[offset]
    for(size_t i=0; i<token_open.size(); i++)
    {
      if (value[offset+i] != token_open[i])
        return false;
    }

    //Found a token_open position at value[offset]
    //Search for the token_close position.
    size_t name_start_pos = offset + token_open.size();
    size_t token_close_pos = value.find(token_close, name_start_pos);
    if (token_close_pos == std::string::npos)
      return false; // token_close not found

    //Found a token_close position.
    //Extract the name of the property.
    size_t length = (token_close_pos) - name_start_pos;
    if (length == 0)
      return false;
    std::string temp_name(&value[name_start_pos], length);

    //Validate if name
    PropertyManager & pmgr = PropertyManager::GetInstance();
    bool exists = pmgr.HasProperty(temp_name);
    if (exists)
      name = temp_name;
    return exists;
  }

  std::string PropertyManager::Expand(const std::string & value) const
  {
    std::string output;
    output.reserve(value.size()*2);

    static const std::string token_open = "${";
    static const std::string token_close = "}";

    for(size_t i=0; i<value.size(); i++)
    {
      std::string name;
      if (strncmp(&value[i], token_open.c_str(), token_open.size()) == 0 && IsPropertyReference(token_open, token_close, value, i, name))
      {
        //Found a property reference at value[i]
        const std::string & property_value = this->GetProperty(name);

        //Also expands property_value
        std::string expanded = this->Expand(property_value);

        //Proceed with the string replacement
        output.append(expanded);

        //Update i to skip this property reference
        size_t length = token_open.size() + name.size() + token_close.size();
        i += length-1; //-1 since the next for loop will increase i by 1.
      }
      else
        output.append(1, value[i]);
    }

    return output;
  }

  void PropertyManager::RegisterEnvironmentVariables()
  {
    //Work around for https://github.com/end2endzone/RapidAssist/issues/54
    ra::environment::GetEnvironmentVariableUtf8("foo");

    //register all environment variables
    ra::strings::StringVector vars = ra::environment::GetEnvironmentVariablesUtf8();
    for(size_t i=0; i<vars.size(); i++)
    {
      const std::string & var = vars[i];

      std::string name = "env." + var;
      std::string value = ra::environment::GetEnvironmentVariableUtf8(var.c_str());
      
      //register the variable as a valid property
      SetProperty(name, value);
    }
  }

  void PropertyManager::RegisterDefaultProperties()
  {
    //define global properties
    std::string prop_path_separator         = ra::filesystem::GetPathSeparatorStr();
    std::string prop_line_separator         = ra::environment::GetLineSeparator();

    SetProperty("path.separator"       , prop_path_separator       );
    SetProperty("line.separator"       , prop_line_separator       );
    SetProperty("newline"              , prop_line_separator       );

    // Set default property for multi selection. Issue #52.
    SetProperty(Context::MULTI_SELECTION_SEPARATOR_PROPERTY_NAME, Context::DEFAULT_MULTI_SELECTION_SEPARATOR);
  }

} //namespace shellanything
