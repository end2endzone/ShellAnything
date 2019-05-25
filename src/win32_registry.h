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

#include <stdint.h>
#include <vector>
#include <string>

namespace win32_registry
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

  static const REGISTRY_ICON NULL_ICON = { std::string(), 0 };
  static const int & INVALID_ICON_INDEX = -1;
  
  std::string toString(const RGS_ENTRY & entry);
  bool parseRgsRegistry(const std::string & rgs, const std::string & module_path, RGS_ENTRY_LIST & entries);
  bool createRegistry(const RGS_ENTRY_LIST & entries);
  bool deleteRegistry(const RGS_ENTRY_LIST & entries);

  bool getValue( const char * iKeyPath,
                 const char * iValueName,
                 REGISTRY_TYPE & oType,
                 MemoryBuffer & oValue);
  bool getDefaultKeyValue( const char * iKeyPath, REGISTRY_TYPE & oType, MemoryBuffer & oValue);
  
  bool hasKey(const char* iKeyPath);
  bool createKey(const char* iKeyPath);
  bool createKey(const char* iKeyPath, const char* iDefaultValue);
  bool deleteKey(const char* iKeyPath);
  bool setValue(const char* iKeyPath, const char* iValueName, const uint8_t* iBuffer, const uint32_t & iBufferSize);
  bool setValue(const char* iKeyPath, const char* iValueName, const uint32_t & iValue);
  bool setValue(const char* iKeyPath, const char* iValueName, const char *iValue);
  bool deleteValue(const char* iKeyPath, const char* iValueName);

  bool getAssociatedProgram(const char* iFileExtention, std::string & oCmdLine);
  bool setAssociatedProgram(const char* iExtention, const char* iFileType, const char* iCmdLine);

  bool registerCommandForFile(const char * iName, const char * iCommand);
  bool registerCommandForFolder(const char * iName, const char * iCommand);

  bool isValid(const REGISTRY_ICON & icon);
  REGISTRY_ICON getFileTypeIcon(const char * iFileExtention);
  REGISTRY_ICON getUnknownFileTypeIcon();

} //namespace win32_registry
