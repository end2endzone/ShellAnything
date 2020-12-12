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

#ifndef WIN32_REGISTRY_H
#define WIN32_REGISTRY_H

#include <stdint.h>
#include <vector>
#include <string>

namespace Win32Registry
{
  enum REGISTRY_TYPE
  {
    REGISTRY_TYPE_STRING,
    REGISTRY_TYPE_BINARY,
    REGISTRY_TYPE_UINT32,
    REGISTRY_TYPE_UINT64
  };
  
  struct REGISTRY_ICON
  {
    std::string path;
    int index;
  };
  
  struct RGS_ENTRY
  {
    bool isKey; //true if the entry is a key, false if the entry is a value
    bool isNoRemove;
    bool isForceRemove;
    std::string path;
    std::string value;
  };

  typedef std::vector<RGS_ENTRY> RGS_ENTRY_LIST;
  typedef std::string MemoryBuffer;

  static const int & INVALID_ICON_INDEX = -1;
  static const REGISTRY_ICON NULL_ICON = { std::string(), INVALID_ICON_INDEX };
  
  std::string ToString(const RGS_ENTRY & entry);
  bool ParseRgsRegistry(const std::string & rgs, const std::string & module_path, RGS_ENTRY_LIST & entries);
  bool CreateRegistry(const RGS_ENTRY_LIST & entries);
  bool DeleteRegistry(const RGS_ENTRY_LIST & entries);

  bool GetValue( const char * key_path,
                 const char * value_name,
                 REGISTRY_TYPE & type,
                 MemoryBuffer & value);
  bool GetDefaultKeyValue( const char * key_path, REGISTRY_TYPE & type, MemoryBuffer & value);
  
  bool HasKey(const char* key_path);
  bool CreateKey(const char* key_path);
  bool CreateKey(const char* key_path, const char* default_value);
  bool DeleteKey(const char* key_path);
  bool SetValue(const char* key_path, const char* value_name, const uint8_t* buffer, const uint32_t & buffer_size);
  bool SetValue(const char* key_path, const char* value_name, const uint32_t & value);
  bool SetValue(const char* key_path, const char* value_name, const char* value);
  bool DeleteValue(const char* key_path, const char* value_name);

  bool GetAssociatedProgram(const char* file_extention, std::string & command_line);
  bool SetAssociatedProgram(const char* file_extention, const char* file_type, const char* command_line);

  bool RegisterCommandForFile(const char * name, const char * command);
  bool RegisterCommandForFolder(const char * name, const char * command);

  bool IsValid(const REGISTRY_ICON & icon);
  REGISTRY_ICON GetFileTypeIcon(const char * file_extention);
  REGISTRY_ICON GetUnknownFileTypeIcon();

} //namespace Win32Registry

#endif //WIN32_REGISTRY_H
