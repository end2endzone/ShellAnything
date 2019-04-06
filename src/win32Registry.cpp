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

#include "win32Registry.h"

#include "rapidassist/filesystem.h"
#include "rapidassist/environment.h"
#include "shellanything/Platform.h"

//#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include <assert.h>

namespace win32Registry
{
  REGISTRY_ICON NULL_ICON = {
    std::string(),
    0,
  };

  bool isIconEquals(const REGISTRY_ICON & a, const REGISTRY_ICON & b)
  {
    if (a.path == b.path && a.index == b.index)
    {
      return true;
    }
    return false;
  }

  DWORD convertType(const REGISTRY_TYPE & iValue)
  {
    DWORD type = REG_SZ;

    switch(iValue)
    {
    case REGISTRY_TYPE_STRING:
      type = REG_SZ;
      break;
    case REGISTRY_TYPE_BINARY:
      type = REG_BINARY;
      break;
    case REGISTRY_TYPE_UINT32:
      type = REG_DWORD;
      break;
    case REGISTRY_TYPE_UINT64:
      type = REG_QWORD;
      break;
    default:
      assert( false ); //should not append
    };

    return type;
  }

  REGISTRY_TYPE convertType(DWORD iValue)
  {
    REGISTRY_TYPE type = REGISTRY_TYPE_STRING;
    switch(iValue)
    {
    case REG_SZ:
    case REG_EXPAND_SZ:
    case REG_MULTI_SZ:
      type = REGISTRY_TYPE_STRING;
      break;
    case REG_BINARY:
      type = REGISTRY_TYPE_BINARY;
      break;
    case REG_DWORD:
      type = REGISTRY_TYPE_UINT32;
      break;
    case REG_QWORD:
      type = REGISTRY_TYPE_UINT64;
      break;
    default:
      assert( false ); //should not append
    };
    return type;
  }

  struct HKEY_T
  {
    HKEY key;
    const char* name;
  };
  static HKEY_T supportedKeys[] = {
    {HKEY_CLASSES_ROOT, "HKEY_CLASSES_ROOT"},
    {HKEY_CURRENT_USER, "HKEY_CURRENT_USER"},
    {HKEY_LOCAL_MACHINE, "HKEY_LOCAL_MACHINE"},
    {HKEY_USERS, "HKEY_USERS"},
    {HKEY_CURRENT_CONFIG, "HKEY_CURRENT_CONFIG"},
  };
  static HKEY_T* findKeyInPath(const char* iPath)
  {
    //detecting the key within iPath
    int numSupportedKeys = sizeof(supportedKeys)/sizeof(HKEY_T);
    HKEY_T* rootKey = NULL;
    for(int i=0; i<numSupportedKeys && rootKey == NULL; i++)
    {
      HKEY_T* current = &supportedKeys[i];
      if (_strnicmp(iPath, current->name, strlen(current->name)) == 0)
      {
        rootKey = current;
      }
    }

    return rootKey;
  }
  static const char* getShortKeyPath(const char* iKeyPath)
  {
    HKEY_T* rootKey = findKeyInPath(iKeyPath);
    if (rootKey)
    {
      int keyPathIndex = strlen(rootKey->name)+1;
      return &iKeyPath[keyPathIndex];
    }
    return NULL;
  }
  static bool setRegistryValue(const char* iKeyPath, const char* iValueName, int iType, const void *iValue, const uint32_t & iSize)
  {
    bool result = false;

    HKEY_T* rootKey = findKeyInPath(iKeyPath);
    if (rootKey)
    {
      const char* keyShortPath = getShortKeyPath(iKeyPath);

      HKEY keyHandle;
      if ( !RegCreateKey(rootKey->key, keyShortPath, &keyHandle))
      {
        if ( !RegSetValueEx(keyHandle, iValueName, 0, iType, (LPBYTE)iValue, iSize))
        {
          result = true;
        }
        RegCloseKey(keyHandle);
      }
    }
    return result;
  }


  bool getValue(const char * iKeyPath,
                const char * iValueName,
                REGISTRY_TYPE & oType,
                MemoryBuffer & oValue)
  {
    bool success = false;
    HKEY_T* rootKey = findKeyInPath(iKeyPath);
    
    if (rootKey)
    {
      HKEY keyHandle = NULL;
      const char* keyShortPath = getShortKeyPath(iKeyPath);

      if (RegOpenKeyEx(rootKey->key, keyShortPath, 0, KEY_QUERY_VALUE|KEY_WOW64_64KEY, &keyHandle) == ERROR_SUCCESS)
      {
        //Read value's size and type
        DWORD valueType = 0;
        DWORD valueSize = 0;
        RegQueryValueEx( keyHandle, iValueName, NULL, &valueType, NULL, &valueSize);

        //allocate space for value
        if (valueSize > 0 && oValue.assign(valueSize, 0).size())
        {
          //Read the actual data of the value
          valueType = 0;
          valueSize = oValue.size();
          RegQueryValueEx( keyHandle, iValueName, NULL, &valueType, (LPBYTE)oValue.c_str(), &valueSize);
          
          oType = convertType(valueType);
          success = (valueSize == oValue.size());
        }

        RegCloseKey(keyHandle);
        return success;
      }
    }

    return success;
  }

  bool getDefaultKeyValue( const char * iKeyPath, REGISTRY_TYPE & oType, MemoryBuffer & oValue)
  {
    return getValue(iKeyPath, "", oType, oValue);
  }

  bool createKey(const char* iKeyPath)
  {
    bool result = false;

    HKEY_T* rootKey = findKeyInPath(iKeyPath);
    if (rootKey)
    {
      const char* keyShortPath = getShortKeyPath(iKeyPath);

      HKEY keyHandle = NULL;
      printf("RegCreateKeyA(%s, %s, 0x%08x)\n", rootKey->name, keyShortPath, keyHandle);
      if ( !RegCreateKey(rootKey->key, keyShortPath, &keyHandle))
      {
        result = true;
      }
      RegCloseKey(keyHandle);
    }

    return result;
  }

  bool deleteKey(const char* iKeyPath)
  {
    bool result = false;

    HKEY_T* rootKey = findKeyInPath(iKeyPath);
    if (rootKey)
    {
      const char* keyShortPath = getShortKeyPath(iKeyPath);

      //Split the full key path
      std::string parentKeyPath = ra::filesystem::getParentPath(keyShortPath);
      std::string keyName       = ra::filesystem::getFilename(keyShortPath);

      HKEY keyHandle = NULL;
      if ( !RegOpenKeyEx(rootKey->key, parentKeyPath.c_str(), 0, KEY_SET_VALUE|KEY_WOW64_64KEY, &keyHandle))
      {
        if ( !RegDeleteKey(keyHandle, keyName.c_str()))
        {
          result = true;
        }
      }
      RegCloseKey(keyHandle);
    }

    return result;
  }

  bool setValue(const char* iKeyPath, const char* iValueName, const uint8_t* iBuffer, const uint32_t & iBufferSize)
  {
    return setRegistryValue(iKeyPath, iValueName, REG_BINARY, iBuffer, iBufferSize);
  }

  bool setValue(const char* iKeyPath, const char* iValueName, const uint32_t & iValue)
  {
    return setRegistryValue(iKeyPath, iValueName, REG_DWORD, &iValue, sizeof(iValue));
  }

  bool setValue(const char* iKeyPath, const char* iValueName, const char *iValue)
  {
    return setRegistryValue(iKeyPath, iValueName, REG_SZ, iValue, strlen(iValue) + 1);
  }

  bool deleteValue(const char* iKeyPath, const char* iValueName)
  {
    bool result = false;

    HKEY_T* rootKey = findKeyInPath(iKeyPath);
    if (rootKey)
    {
      const char* keyShortPath = getShortKeyPath(iKeyPath);

      HKEY keyHandle = NULL;
      if ( !RegOpenKeyEx(rootKey->key, keyShortPath, 0, KEY_SET_VALUE|KEY_WOW64_64KEY, &keyHandle))
      {
        if ( !RegDeleteValueA(keyHandle, iValueName))
        {
          result = true;
        }
      }
      RegCloseKey(keyHandle);
    }

    return result;
  }

  bool getAssociatedProgram(const char* iFileExtention, std::string & oCmdLine)
  {
	  const DWORD SIZE = 2048;

    HKEY hKey = NULL;
    if (RegOpenKeyEx(HKEY_CLASSES_ROOT, iFileExtention, 0, KEY_READ|KEY_WOW64_64KEY, &hKey) != ERROR_SUCCESS)
    {
      //Key does not exist
      oCmdLine = "";
      return false;
    }

    //Get key's unnamed value
    char defaultKeyValueName[SIZE];
    DWORD defaultKeyValueSize = SIZE;
    RegQueryValueEx(hKey, "", 0, 0, (LPBYTE)defaultKeyValueName, &defaultKeyValueSize);
    RegCloseKey(hKey);

    //Open Command key of File Association key's Open subkey
    std::string sPath = defaultKeyValueName;
    sPath.append("\\shell\\open\\command");

    HKEY hProgKey = NULL;
    if (RegOpenKeyEx(HKEY_CLASSES_ROOT, sPath.c_str(), 0, KEY_READ|KEY_WOW64_64KEY, &hProgKey) == ERROR_SUCCESS)
    {
      DWORD progNameSize = 0;
      //Determine data type and buffer size of key
      RegQueryValueEx(hProgKey, "", 0, 0, NULL, &progNameSize);

      //Retrieve file association
      char programName[SIZE];
      progNameSize = SIZE;
      DWORD lType = 0;
      RegQueryValueEx(hProgKey, "", 0, &lType, (LPBYTE)programName, &progNameSize);

      RegCloseKey(hProgKey);
    
      oCmdLine = programName;

      return true;
    }

    oCmdLine = "";
    return false;
	}

  bool setAssociatedProgram(const char* iExtention, const char* iFileType, const char* iCmdLine)
  {
    //save current setting;
    //REGISTRY_HANDLER_ROOT current = m_CurrentRoot;
    //CRegistryHandler c;

    //setRootFolder(CLASSES_ROOT);

    //char sExt[100];
    //char sExtAutoFile[100];
    //sprintf(sExt,".%s",ext);
    //sprintf(sExtAutoFile,"%s_auto_file",ext);

    bool success = true;

    //Building the extention (removing the '.' character in iExtention in case)
    std::string extention = iExtention;
    if (iExtention[0] == '.')
    {
      extention = &iExtention[1];
    }

    //Build the extention file type (most of the time the format used is extentionfile. ie: txtfile)
    std::string extentionFileType = extention.c_str();
    extentionFileType.append("file");

    //Create the key for the extention
    std::string extentionKey = "HKEY_CLASSES_ROOT\\.";
    extentionKey.append(extention.c_str());
    success = createKey(extentionKey.c_str());
    if (!success)
    {
      printf("1 Unable to create key: %s\n", extentionKey.c_str());
      return false;
    }

    printf("step #1\n");

    //Create the key for the file type
    std::string typeKey = "HKEY_CLASSES_ROOT\\";
    typeKey.append(extentionFileType.c_str());
    success = createKey(typeKey.c_str());
    if (!success)
    {
      printf("2 Unable to create key: %s\n", typeKey.c_str());
      return false;
    }

    printf("step #2\n");

    //Create the extention key's default value. (extention file type)
    success = setValue(extentionKey.c_str(), "", extentionFileType.c_str());
    if (!success)
    {
      printf("3 Unable to set value. key=%s, name=%s\n", extentionKey.c_str(), extentionFileType.c_str());
      return false;
    }

    printf("step #3\n");

    //Create the filetype key's default value (friendly name) 
    setValue(extentionFileType.c_str(), "", iFileType);

    printf("step #4\n");

    //Create the shell, open and command sub keys
    const char* subKeys[] = {"shell", "open", "command"};
    int numSubKeys = sizeof(subKeys)/sizeof(const char*);
    std::string subKey = typeKey.c_str();
    for(int i=0; i<numSubKeys; i++)
    {
      const char* currentSubKey = subKeys[i];

      subKey.append("\\");
      subKey.append(currentSubKey);

      //create this key
      printf("debug: trying to create key: %s\n", subKey.c_str());
      success = createKey(subKey.c_str());
      if (!success)
      {
        printf("4 Unable to create key: %s\n", subKey.c_str());
        return false;
      }
    }

    printf("step #5\n");

    //Define the default value for the command's key
    success = setValue(subKey.c_str(), "", iCmdLine);
    if (!success)
    {
      printf("5 Unable to set value. key=%s, name=%s\n", subKey.c_str(), iCmdLine);
      return false;
    }

    printf("step #6\n");

    //char sShell[100];
    //sprintf(sShell,"%s_auto_file\\shell",ext);
    //createFolder(sShell);
    //setKeyValue(sShell,"","open");

    //char sOpen[100];
    //sprintf(sOpen,"%s_auto_file\\shell\\open",ext);
    //createFolder(sOpen);

    //char sCommand[100];
    //sprintf(sCommand,"%s_auto_file\\shell\\open\\command",ext);
    //setKeyValue(sCommand,"",execString);

    ////Restore
    //m_CurrentRoot = current;

    return true;
  }

  bool registerCommandForFile(const char * iName, const char * iCommand)
  {
    bool success = false;
    
    std::string key;

    key = ra::strings::format("HKEY_CLASSES_ROOT\\*\\shell\\%s", iName); 
    success = createKey(key.c_str());
    if (!success)
      return false;

    key = ra::strings::format("HKEY_CLASSES_ROOT\\*\\shell\\%s\\command", iName); 
    success = createKey(key.c_str());
    if (!success)
      return false;

    success = setValue(key.c_str(), "", iCommand);
    if (!success)
      return false;

    return true;
  }

  bool registerCommandForFolder(const char * iName, const char * iCommand)
  {
    bool success = false;
    
    std::string key;

    key = ra::strings::format("HKEY_CLASSES_ROOT\\Folder\\shell\\%s", iName); 
    success = createKey(key.c_str());
    if (!success)
      return false;

    key = ra::strings::format("HKEY_CLASSES_ROOT\\Folder\\shell\\%s\\command", iName); 
    success = createKey(key.c_str());
    if (!success)
      return false;

    success = setValue(key.c_str(), "", iCommand);
    if (!success)
      return false;

    return true;
  }

  static REGISTRY_ICON getFileTypeDefaultIcon(const char * iBaseKey)
  {
    //Extract default icon. ie: C:\Windows\Installer\{AC76BA86-7AD7-1036-7B44-A90000000001}\PDFFile_8.ico,0
    std::string defaultIcon;
    {
      std::string key = ra::strings::format("%s\\DefaultIcon", iBaseKey); 
      MemoryBuffer buffer;
      REGISTRY_TYPE type = REGISTRY_TYPE_STRING;
      const char * debugValue = key.c_str();
      bool success = getDefaultKeyValue(key.c_str(), type, buffer);
      if (!success)
        return NULL_ICON;
      if (type != REGISTRY_TYPE_STRING)
        return NULL_ICON;
      defaultIcon = buffer.c_str();
    }
    
    //check format of default icon. File only or file with index
    std::string filePath;
    int index = 0;
    {
      ra::strings::StringVector parts;
      ra::strings::split( parts, defaultIcon, ",");

      if (parts.size() == 1)
      {
        //assumed a file only
        filePath = parts[0];
      }
      else if (parts.size() == 2)
      {
        //assumed a file and index
        filePath = parts[0];
        std::string indexStr = parts[1];
        const char * debugValue = indexStr.c_str();
        ra::strings::replace(indexStr, "-", ""); //some file contains a -102 instead of 102. Don't know why. ie: 
        ra::strings::parse(indexStr, index);
      }
      else
        return NULL_ICON; //unknown format
    }
    
    //Does file exists ?
    {
      const char * debugValue = filePath.c_str();
      if (filePath.size() > 0 && ra::filesystem::fileExists(filePath.c_str()))
      {
        REGISTRY_ICON icon;
        icon.path = filePath;
        icon.index = index;
        return icon;
      }
    }

    //File does not exists. Maybe path contains "expandables" values
    filePath = shellanything::expand(filePath.c_str());
    {
      const char * debugValue = filePath.c_str();
      if (filePath.size() > 0 && ra::filesystem::fileExists(filePath.c_str()))
      {
        REGISTRY_ICON icon;
        icon.path = filePath;
        icon.index = index;
        return icon;
      }
    }

    //Unable to extract icon
    return NULL_ICON;
  }

  REGISTRY_ICON getFileTypeIcon(const char * iFileExtention)
  {
    std::string extention = iFileExtention;
    if (extention[0] != '.')
    {
      extention.insert(0, 1, '.');
    }

    //Extract document short name. ie AcroExch.Document
    std::string documentName;
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\%s", extention.c_str()); 
      MemoryBuffer buffer;
      REGISTRY_TYPE type = REGISTRY_TYPE_STRING;
      bool success = getDefaultKeyValue(key.c_str(), type, buffer);
      if (!success)
        return NULL_ICON;
      if (type != REGISTRY_TYPE_STRING)
        return NULL_ICON;
      documentName = buffer.c_str();
    }

    if (documentName.size() == 0)
    {
      //no document name. Abort
      return NULL_ICON;
    }

    //Check DefautIcon with documentName. ie: HKEY_CLASSES_ROOT\FirefoxHTML\DefaultIcon
    {
      std::string basekey = ra::strings::format("HKEY_CLASSES_ROOT\\%s", documentName.c_str()); 
      REGISTRY_ICON icon = getFileTypeDefaultIcon(basekey.c_str());
      if (!isIconEquals(icon,NULL_ICON))
        return icon;
    }

    //Extract document long name. ie: AcroExch.Document.7
    std::string documentCurrentVersionName;
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\%s\\CurVer", documentName.c_str()); 
      MemoryBuffer buffer;
      REGISTRY_TYPE type = REGISTRY_TYPE_STRING;
      bool success = getDefaultKeyValue(key.c_str(), type, buffer);
      if (success)
        documentCurrentVersionName = buffer.c_str();
    }

    //Does file type has a current version ?
    if (documentCurrentVersionName.size() > 0)
    {
      //Keep looking with current version

      //Check DefautIcon with current version name. ie: HKEY_CLASSES_ROOT\AcroExch.Document.7\DefaultIcon
      {
        std::string basekey = ra::strings::format("HKEY_CLASSES_ROOT\\%s", documentCurrentVersionName.c_str()); 
        REGISTRY_ICON icon = getFileTypeDefaultIcon(basekey.c_str());
        if (!isIconEquals(icon,NULL_ICON))
          return icon;
      }

      //icon is invalid.
    }
    
    //Does the file type has a icon handler ?
    std::string iconHandlerGuid;
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\%s\\ShellEx\\IconHandler", documentName.c_str()); 
      MemoryBuffer buffer;
      REGISTRY_TYPE type = REGISTRY_TYPE_STRING;
      bool success = getDefaultKeyValue(key.c_str(), type, buffer);
      if (success)
        iconHandlerGuid = buffer.c_str();
    }

    if (iconHandlerGuid.size() > 0)
    {
      //Keep looking with iconhandler
                                                
      //check default icon of icon handler. ie: HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{42042206-2D85-11D3-8CFF-005004838597}\Old Icon\htmlfile
      {
        std::string basekey = ra::strings::format("HKEY_CLASSES_ROOT\\SOFTWARE\\Classes\\CLSID\\%s\\Old Icon\\%s", iconHandlerGuid.c_str(), documentName.c_str()); 
        REGISTRY_ICON icon = getFileTypeDefaultIcon(basekey.c_str());
        if (!isIconEquals(icon,NULL_ICON))
          return icon;
      }

      //check default icon of icon handler. ie: KEY_CLASSES_ROOT\Wow6432Node\CLSID\{42042206-2D85-11D3-8CFF-005004838597}\Old Icon\htmlfile\DefaultIcon
      {
        std::string basekey = ra::strings::format("HKEY_CLASSES_ROOT\\Wow6432Node\\CLSID\\%s\\Old Icon\\%s", iconHandlerGuid.c_str(), documentName.c_str()); 
        REGISTRY_ICON icon = getFileTypeDefaultIcon(basekey.c_str());
        if (!isIconEquals(icon,NULL_ICON))
          return icon;
      }
    }

    //Try the associated program
    std::string program;
    if (getAssociatedProgram(extention.c_str(), program))
    {
      if (program.size() > 0)
      {
        //chceck if file exists
        const char * debugValue = program.c_str();
        if (ra::filesystem::fileExists(program.c_str()))
        {
          REGISTRY_ICON icon;
          icon.path = program;
          icon.index = 0;
          return icon;
        }

        //File does not exists. Maybe arguments are included in the path. Try cleaning it
        ra::strings::replace(program, "\"", "");
        size_t pos = 0;
        if (program.find(".exe", pos))
        {
          program = program.substr(pos + 4);

          //matbe there is expandable in the path too
          program = shellanything::expand(program.c_str());

          //try again
          const char * debugValue = program.c_str();
          if (ra::filesystem::fileExists(program.c_str()))
          {
            REGISTRY_ICON icon;
            icon.path = program;
            icon.index = 0;
            return icon;
          }
        }
      }
    }

    //Unable to extract icon
    return NULL_ICON;
  }

} //namespace win32Registry
