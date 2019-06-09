//#define WIN32_LEAN_AND_MEAN 1
#include "win32_shell.h"
#include "win32_registry.h"

#include <shlobj.h>

#include "rapidassist/strings.h"
#include "rapidassist/environment.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/process.h"

namespace win32_shell
{

  std::string GuidToString(GUID guid)
  {
    std::string output;
    output.assign(40, 0);
    sprintf_s((char*)output.c_str(), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    return output;
  }

  std::string GuidToInterfaceName(GUID guid)
  {
    if (IsEqualGUID(guid, IID_IUnknown))                  return "IUnknown";                  //{00000000-0000-0000-C000-000000000046}
    if (IsEqualGUID(guid, IID_IClassFactory))             return "IClassFactory";             //{00000001-0000-0000-C000-000000000046}
    if (IsEqualGUID(guid, IID_IShellExtInit))             return "IShellExtInit";             //{000214E8-0000-0000-C000-000000000046}
    if (IsEqualGUID(guid, IID_IContextMenu))              return "IContextMenu";              //{000214E4-0000-0000-C000-000000000046}
    if (IsEqualGUID(guid, IID_IContextMenu2))             return "IContextMenu2";             //{000214F4-0000-0000-C000-000000000046}
    if (IsEqualGUID(guid, IID_IContextMenu3))             return "IContextMenu3";             //{BCFCE0A0-EC17-11D0-8D10-00A0C90F2719}
    if (IsEqualGUID(guid, IID_IObjectWithSite))           return "IObjectWithSite";           //{FC4801A3-2BA9-11CF-A229-00AA003D7352}
    if (IsEqualGUID(guid, IID_IInternetSecurityManager))  return "IInternetSecurityManager";  //{79EAC9EE-BAF9-11CE-8C82-00AA004BA90B}

    //unknown GUID, return the string representation:
    //ie: CLSID_UNDOCUMENTED_01, {924502A7-CC8E-4F60-AE1F-F70C0A2B7A7C}
    return GuidToString(guid);
  }

  HRESULT DllRegisterServer(GUID guid, const char * name, const char * description, const char * module_path)
  {
    const std::string guid_str_tmp = GuidToString(guid).c_str();
    const char * guid_str = guid_str_tmp.c_str();
    const std::string class_name_version1 = std::string(name) + ".1";

    //#define TRACELINE() MessageBox(NULL, (std::string("Line: ") + ra::strings::toString(__LINE__)).c_str(), "DllUnregisterServer() DEBUG", MB_OK);

    //Register version 1 of our class
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\%s", class_name_version1);
      if (!win32_registry::createKey(key.c_str(), description))
        return E_ACCESSDENIED;
    }
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\%s.1\\CLSID", name);
      if (!win32_registry::createKey(key.c_str(), guid_str))
        return E_ACCESSDENIED;
    }

    //Register current version of our class
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\%s", name);
      if (!win32_registry::createKey(key.c_str(), description))
        return E_ACCESSDENIED;
    }
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\%s\\CLSID", name);
      if (!win32_registry::createKey(key.c_str(), guid_str))
        return E_ACCESSDENIED;
    }
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\%s\\CurVer", name);
      if (!win32_registry::createKey(key.c_str(), class_name_version1.c_str()))
        return E_ACCESSDENIED;
    }

    // Add the CLSID of this DLL to the registry
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\CLSID\\%s", guid_str);
      if (!win32_registry::createKey(key.c_str(), description))
        return E_ACCESSDENIED;
    }

    // Define the path and parameters of our DLL:
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\CLSID\\%s\\ProgID", guid_str);
      if (!win32_registry::createKey(key.c_str(), class_name_version1.c_str()))
        return E_ACCESSDENIED;
    }
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\CLSID\\%s\\VersionIndependentProgID", guid_str);
      if (!win32_registry::createKey(key.c_str(), name))
        return E_ACCESSDENIED;
    }
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\CLSID\\%s\\Programmable", guid_str);
      if (!win32_registry::createKey(key.c_str()))
        return E_ACCESSDENIED;
    }
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\CLSID\\%s\\InprocServer32", guid_str);
      if (!win32_registry::createKey(key.c_str(), module_path))
        return E_ACCESSDENIED;
      if (!win32_registry::setValue(key.c_str(), "ThreadingModel", "Apartment"))
        return E_ACCESSDENIED;
    }

    // Register the shell extension for all the file types
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\*\\shellex\\ContextMenuHandlers\\%s", name);
      if (!win32_registry::createKey(key.c_str(), guid_str))
        return E_ACCESSDENIED;
    }

    // Register the shell extension for directories
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Directory\\shellex\\ContextMenuHandlers\\%s", name);
      if (!win32_registry::createKey(key.c_str(), guid_str))
        return E_ACCESSDENIED;
    }

    // Register the shell extension for folders
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Folder\\shellex\\ContextMenuHandlers\\%s", name);
      if (!win32_registry::createKey(key.c_str(), guid_str))
        return E_ACCESSDENIED;
    }

    // Register the shell extension for the desktop or the file explorer's background
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Directory\\Background\\ShellEx\\ContextMenuHandlers\\%s", name);
      if (!win32_registry::createKey(key.c_str(), guid_str))
        return E_ACCESSDENIED;
    }

    // Register the shell extension for drives
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Drive\\ShellEx\\ContextMenuHandlers\\%s", name);
      if (!win32_registry::createKey(key.c_str(), guid_str))
        return E_ACCESSDENIED;
    }

    // Register the shell extension to the system's approved Shell Extensions
    {
      std::string key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved";
      if (!win32_registry::createKey(key.c_str()))
        return E_ACCESSDENIED;
      if (!win32_registry::setValue(key.c_str(), guid_str, description))
        return E_ACCESSDENIED;
    }

    // Notify the Shell to pick the changes:
    // https://docs.microsoft.com/en-us/windows/desktop/shell/reg-shell-exts#predefined-shell-objects
    // Any time you create or change a Shell extension handler, it is important to notify the system that you have made a change.
    // Do so by calling SHChangeNotify, specifying the SHCNE_ASSOCCHANGED event.
    // If you do not call SHChangeNotify, the change might not be recognized until the system is rebooted.
    SHChangeNotify(SHCNE_ASSOCCHANGED, 0, 0, 0);

    return S_OK;
  }

  HRESULT DllUnregisterServer(GUID guid, const char * name)
  {
    const std::string guid_str_tmp = GuidToString(guid).c_str();
    const char * guid_str = guid_str_tmp.c_str();
    const std::string class_name_version1 = std::string(name) + ".1";

    //#define TRACELINE() MessageBox(NULL, (std::string("Line: ") + ra::strings::toString(__LINE__)).c_str(), "DllUnregisterServer() DEBUG", MB_OK);

    // Unregister the shell extension from the system's approved Shell Extensions
    {
      std::string key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved";
      if (!win32_registry::deleteValue(key.c_str(), guid_str))
        return E_ACCESSDENIED;
    }

    // Unregister the shell extension for drives
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Drive\\shellex\\ContextMenuHandlers\\%s", name);
      if (!win32_registry::deleteKey(key.c_str()))
        return E_ACCESSDENIED;
    }

    // Unregister the shell extension for the desktop or the file explorer's background
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Directory\\Background\\ShellEx\\ContextMenuHandlers\\%s", name);
      if (!win32_registry::deleteKey(key.c_str()))
        return E_ACCESSDENIED;
    }

    // Unregister the shell extension for folders
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Folders\\shellex\\ContextMenuHandlers\\%s", name);
      if (!win32_registry::deleteKey(key.c_str()))
        return E_ACCESSDENIED;
    }

    // Unregister the shell extension for directories
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Directory\\shellex\\ContextMenuHandlers\\%s", name);
      if (!win32_registry::deleteKey(key.c_str()))
        return E_ACCESSDENIED;
    }

    // Unregister the shell extension for all the file types
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\*\\shellex\\ContextMenuHandlers\\%s", name);
      if (!win32_registry::deleteKey(key.c_str()))
        return E_ACCESSDENIED;
    }

    // Remove the CLSID of this DLL from the registry
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\CLSID\\%s", guid_str);
      if (!win32_registry::deleteKey(key.c_str()))
        return E_ACCESSDENIED;
    }

    // Unregister current and version 1 of our extension
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\%s", class_name_version1.c_str());
      if (!win32_registry::deleteKey(key.c_str()))
        return E_ACCESSDENIED;
    }
    {
      std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\%s", name);
      if (!win32_registry::deleteKey(key.c_str()))
        return E_ACCESSDENIED;
    }

    // Notify the Shell to pick the changes:
    // https://docs.microsoft.com/en-us/windows/desktop/shell/reg-shell-exts#predefined-shell-objects
    // Any time you create or change a Shell extension handler, it is important to notify the system that you have made a change.
    // Do so by calling SHChangeNotify, specifying the SHCNE_ASSOCCHANGED event.
    // If you do not call SHChangeNotify, the change might not be recognized until the system is rebooted.
    SHChangeNotify(SHCNE_ASSOCCHANGED, 0, 0, 0);

    return S_OK;
  }

} //namespace win32_shell
