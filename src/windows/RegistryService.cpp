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

#include "RegistryService.h"
#include "Win32Registry.h"

#include "rapidassist/filesystem_utf8.h"

namespace shellanything
{
  void append_optional_null(std::string& value)
  {
    if (value.empty())
      return;
    const char* data = value.data();
    char last = data[value.size()];
    if (last != '\0')
      value.append(1, '\0');
  }


  RegistryService::RegistryService()
  {
  }

  RegistryService::~RegistryService()
  {
  }

  bool RegistryService::GetRegistryKeyAsString(const std::string& path, std::string& value)
  {
    Win32Registry::REGISTRY_TYPE key_type;
    Win32Registry::MemoryBuffer key_value;
    bool key_found = false;

    if (!key_found)
    {
      // Search for a registry key and value that matches regisrykey.
      // Split as a key path and key name.
      std::string key_path = ra::filesystem::GetParentPath(path.c_str());
      std::string key_name = ra::filesystem::GetFilename(path.c_str());
      key_found = Win32Registry::GetValue(key_path.c_str(), key_name.c_str(), key_type, key_value);
    }

    if (!key_found)
    {
      // Search for a registry key default that matches regisrykey.
      key_found = Win32Registry::GetDefaultKeyValue(path.c_str(), key_type, key_value);
    }

    // Store the result in 'value' as if user set this specific value (to use the same process as a property that sets a value).
    if (key_found)
    {
      switch (key_type)
      {
      case Win32Registry::REGISTRY_TYPE_STRING:
        value = key_value;
        break;
      case Win32Registry::REGISTRY_TYPE_BINARY:
        // Properties must end with '\0' to be printable
        append_optional_null(key_value);
        value = key_value;
        break;
      case Win32Registry::REGISTRY_TYPE_UINT32:
      {
        uint32_t* tmp32 = (uint32_t*)key_value.data();
        value = ra::strings::ToString(*tmp32);
      }
      break;
      case Win32Registry::REGISTRY_TYPE_UINT64:
      {
        uint64_t* tmp64 = (uint64_t*)key_value.data();
        value = ra::strings::ToString(*tmp64);
      }
      break;
      default:
        key_found = false;
        break;
      }

      return key_found;
    }

    return false;
  }

} //namespace shellanything
