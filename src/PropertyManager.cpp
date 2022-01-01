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

#include <map>

namespace shellanything
{
  typedef std::map<String /*name*/, String /*value*/> PropertyMap;
  struct PropertyManager::Properties
  {
    PropertyMap properties;
  };

  static const int EXPANDING_MAX_ITERATIONS = 20;

  const String PropertyManager::SYSTEM_TRUE_PROPERTY_NAME = "system.true";
  const String PropertyManager::SYSTEM_TRUE_DEFAULT_VALUE = "true";
  const String PropertyManager::SYSTEM_FALSE_PROPERTY_NAME = "system.false";
  const String PropertyManager::SYSTEM_FALSE_DEFAULT_VALUE = "false";

  PropertyManager::PropertyManager()
  {
    this->p = new Properties();
    RegisterEnvironmentVariables();
    RegisterDefaultProperties();
  }

  PropertyManager::~PropertyManager()
  {
    delete this->p;
    this->p = NULL;
  }

  PropertyManager & PropertyManager::GetInstance()
  {
    static PropertyManager _instance;
    return _instance;
  }

  void PropertyManager::Clear()
  {
    p->properties.clear();
    RegisterEnvironmentVariables();
    RegisterDefaultProperties();
  }

  void PropertyManager::ClearProperty(const String & name)
  {
    PropertyMap::const_iterator propertyIt = p->properties.find(name);
    bool found = (propertyIt != p->properties.end());
    if (found)
    {
      p->properties.erase(propertyIt);
    }
  }

  bool PropertyManager::HasProperty(const String & name) const
  {
    PropertyMap::const_iterator propertyIt = p->properties.find(name);
    bool found = (propertyIt != p->properties.end());
    return found;
  }

  void PropertyManager::SetProperty(const String & name, const String & value)
  {
    //overwrite previous property
    p->properties[name] = value;
  }

  const String & PropertyManager::GetProperty(const String & name) const
  {
    PropertyMap::const_iterator propertyIt = p->properties.find(name);
    bool found = (propertyIt != p->properties.end());
    if (found)
    {
      const String & value = propertyIt->second;
      return value;
    }

    static String EMPTY_VALUE;
    return EMPTY_VALUE;
  }

  inline bool IsPropertyReference(const String & token_open, const String & token_close, const String & value, size_t offset, String & name)
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
    String temp_name(&value[name_start_pos], length);

    //Validate if name
    PropertyManager & pmgr = PropertyManager::GetInstance();
    bool exists = pmgr.HasProperty(temp_name);
    if (exists)
      name = temp_name;
    return exists;
  }

  String PropertyManager::Expand(const String & value) const
  {
    int count = 1;
    String previous = value;
    String output = ExpandOnce(value);

    //Prevent circular reference by expanding at most 20 times.
    while(output != previous && count <= EXPANDING_MAX_ITERATIONS)
    {
      previous = output;
      output = ExpandOnce(output);
      count++;
    }

    return output;
  }

  String PropertyManager::ExpandOnce(const String & value) const
  {
    //Process expansion in-place
    std::string output;
    output.reserve(value.size()*2);
    output = value.c_str();

    static const String token_open = "${";
    static const String token_close = "}";

    //Prevent circular reference by dfining a counter which counts how many time a character position was expanded.
    int count = 1;
    size_t previous_pos = std::string::npos;

    for(size_t i=0; i<output.size(); i++)
    {
      //Count how many time we tried to expand this position
      if (i == previous_pos)
        count++; //same character as previous loop
      else
        count = 0; //that is a new character
      previous_pos = i;

      //If we find a property reference token at this location...
      String name;
      if (strncmp(&output[i], token_open.c_str(), token_open.size()) == 0 && IsPropertyReference(token_open, token_close, output.c_str(), i, name))
      {
        //Found a property reference at output[i]
        const std::string property_value = this->GetProperty(name).c_str();

        //Replace the property reference by the property's value
        size_t token_length = token_open.size() + name.size() + token_close.size();
        output.replace(output.begin() + i, output.begin() + i + token_length, property_value);

        //Prevent circular reference by expanding 20 times maximum.
        if (count < EXPANDING_MAX_ITERATIONS)
        {
          //Keep i at the same value for the next loop to process the same character position.
          //This is required if the property value also contains property references.
          i--; //-1 since the next for loop will increase i by 1.
        }
        else
        {
          //Reached the maximum of 20 loops.
          //Stop looking for property reference at this position.
          int a = 0;
        }
      }
    }

    return output.c_str();
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
      SetProperty(name.c_str(), value.c_str());
    }
  }

  void PropertyManager::RegisterDefaultProperties()
  {
    //define global properties
    String prop_path_separator         = ra::filesystem::GetPathSeparatorStr();
    String prop_line_separator         = ra::environment::GetLineSeparator();

    SetProperty("path.separator"       , prop_path_separator       );
    SetProperty("line.separator"       , prop_line_separator       );
    SetProperty("newline"              , prop_line_separator       );

    SetProperty(PropertyManager::SYSTEM_TRUE_PROPERTY_NAME    , PropertyManager::SYSTEM_TRUE_DEFAULT_VALUE   );
    SetProperty(PropertyManager::SYSTEM_FALSE_PROPERTY_NAME   , PropertyManager::SYSTEM_FALSE_DEFAULT_VALUE  );

    // Set default property for multi selection. Issue #52.
    SetProperty(Context::MULTI_SELECTION_SEPARATOR_PROPERTY_NAME, Context::DEFAULT_MULTI_SELECTION_SEPARATOR);
  }

} //namespace shellanything
