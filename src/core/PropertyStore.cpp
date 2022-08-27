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

#include "PropertyStore.h"

namespace shellanything
{
  PropertyStore::PropertyStore()
  {
  }

  PropertyStore::~PropertyStore()
  {
  }

  PropertyStore::PropertyStore(const PropertyStore& store)
  {
    (*this) = store;
  }

  const PropertyStore& PropertyStore::operator =(const PropertyStore& store)
  {
    if (this != &store)
    {
      properties = store.properties;
    }
    return (*this);
  }

  void PropertyStore::Clear()
  {
    properties.clear();
  }

  void PropertyStore::ClearProperty(const std::string& name)
  {
    PropertyMap::const_iterator propertyIt = properties.find(name);
    bool found = (propertyIt != properties.end());
    if (found)
    {
      properties.erase(propertyIt);
    }
  }

  bool PropertyStore::HasProperty(const std::string& name) const
  {
    PropertyMap::const_iterator propertyIt = properties.find(name);
    bool found = (propertyIt != properties.end());
    return found;
  }

  bool PropertyStore::HasProperties(const StringList& properties) const
  {
    for (size_t i = 0; i < properties.size(); i++)
    {
      const std::string& name = properties[i];
      if (!HasProperty(name))
        return false;
    }
    return true;
  }

  void PropertyStore::SetProperty(const std::string& name, const std::string& value)
  {
    //overwrite previous property
    properties[name] = value;
  }

  const std::string& PropertyStore::GetProperty(const std::string& name) const
  {
    PropertyMap::const_iterator propertyIt = properties.find(name);
    bool found = (propertyIt != properties.end());
    if (found)
    {
      const std::string& value = propertyIt->second;
      return value;
    }

    static std::string EMPTY_VALUE;
    return EMPTY_VALUE;
  }

  size_t PropertyStore::GetPropertyCount() const
  {
    return properties.size();
  }

  bool PropertyStore::IsEmpty() const
  {
    return properties.empty();
  }

  void PropertyStore::GetProperties(StringList& names) const
  {
    names.clear();
    names.reserve(properties.size());
    for (PropertyMap::const_iterator it = properties.begin(); it != properties.end(); ++it)
    {
      const std::string& key = (it->first);
      const std::string& value = (it->second);
      names.push_back(key);
    }
  }

  void PropertyStore::FindMissingProperties(const StringList& input_names, StringList& output_names) const
  {
    output_names.clear();
    for (size_t i = 0; i < input_names.size(); i++)
    {
      const std::string& name = input_names[i];
      if (!HasProperty(name))
        output_names.push_back(name);
    }
  }

} //namespace shellanything
