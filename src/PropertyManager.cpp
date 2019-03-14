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

#include "shellanything/PropertyManager.h"
#include "shellanything/Platform.h"
#include "rapidassist/environment.h"

namespace shellanything
{

  PropertyManager::PropertyManager()
  {
    registerEnvironmentVariables();
  }

  PropertyManager::~PropertyManager()
  {
  }

  PropertyManager & PropertyManager::getInstance()
  {
    static PropertyManager _instance;
    return _instance;
  }

  void PropertyManager::clear()
  {
    properties.clear();
    registerEnvironmentVariables();
  }

  bool PropertyManager::hasProperty(const std::string & name) const
  {
    PropertyMap::const_iterator propertyIt = properties.find(name);
    bool found = (propertyIt != properties.end());
    return found;
  }

  void PropertyManager::setProperty(const std::string & name, const std::string & value)
  {
    //overwrite previous property
    properties[name] = value;
  }

  const std::string & PropertyManager::getProperty(const std::string & name) const
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

  std::string PropertyManager::expand(const std::string & value) const
  {
    std::string output = value;

    //for each properties
    for (PropertyMap::const_iterator propertyIt = properties.begin(); propertyIt != properties.end(); propertyIt++)
    {
      const std::string & name  = propertyIt->first;
      const std::string & value = propertyIt->second;

      //generate the search token
      std::string token;
      token.append("${");
      token.append(name);
      token.append("}");

      //process with search and replace
      ra::strings::replace(output, token, value);
    }

    return output;
  }

  void PropertyManager::registerEnvironmentVariables()
  {
    //register all environment variables
    ra::strings::StringVector vars = getEnvironmentVariables();
    for(size_t i=0; i<vars.size(); i++)
    {
      const std::string & var = vars[i];

      std::string name = "env." + var;
      std::string value = ra::environment::getEnvironmentVariable(var.c_str());
      
      //register the variable as a valid property
      setProperty(name, value);
    }
  }


} //namespace shellanything
