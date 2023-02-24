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

#include "stdafx.h"

#include "CContextMenu.h"
#include "CClassFactory.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

#include "ErrorManager.h"
#include "Win32Registry.h"
#include "Win32Utils.h"
#include "GlogUtils.h"

#include "rapidassist/undef_windows_macros.h"
#include "rapidassist/strings.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/process_utf8.h"
#include "rapidassist/errors.h"
#include "rapidassist/user_utf8.h"
#include "rapidassist/unicode.h"
#include "rapidassist/environment.h"

#include "shellanything/version.h"
#include "shellanything/config.h"

#include "ConfigManager.h"
#include "PropertyManager.h"
#include "SaUtils.h"

#include "shellext.h"

//Declarations
UINT        g_cRefDll = 0;            // Reference counter of this DLL
HINSTANCE   g_hmodDll = 0;            // HINSTANCE of the DLL

void DllAddRef(void)
{
  LOG(INFO) << __FUNCTION__ << "(), new";

  InterlockedIncrement(&g_cRefDll);
}

void DllRelease(void)
{
  LOG(INFO) << __FUNCTION__ << "(), delete";

  InterlockedDecrement(&g_cRefDll);
}

STDAPI DllGetClassObject(REFCLSID clsid, REFIID riid, LPVOID* ppv)
{
  std::string clsid_str = GuidToInterfaceName(clsid);
  std::string riid_str = GuidToInterfaceName(riid);
  LOG(INFO) << __FUNCTION__ << "(), clsid=" << clsid_str << ", riid=" << riid_str;

  // Always set out parameter to NULL, validating it first.
  if (!ppv)
    return E_INVALIDARG;
  *ppv = NULL;

  HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;

  if (IsEqualGUID(clsid, CLSID_ShellAnythingMenu))
  {
    hr = E_OUTOFMEMORY;

    CClassFactory* pClassFactory = new CClassFactory();
    if (pClassFactory)
    {
      hr = pClassFactory->QueryInterface(riid, ppv);
      pClassFactory->Release();
    }
  }

  if (hr == CLASS_E_CLASSNOTAVAILABLE)
    LOG(ERROR) << __FUNCTION__ << "(), ClassFactory " << clsid_str << " not found!";
  else if (FAILED(hr))
    LOG(ERROR) << __FUNCTION__ << "(), unknown interface " << riid_str;
  else
    LOG(INFO) << __FUNCTION__ << "(), found interface " << riid_str << ", ppv=" << ToHexString(*ppv);
  return hr;
}

STDAPI DllCanUnloadNow(void)
{
  LOG(INFO) << __FUNCTION__ << "() " << GetProcessContextDesc();

  ULONG ulRefCount = 0;
  ulRefCount = InterlockedIncrement(&g_cRefDll);
  ulRefCount = InterlockedDecrement(&g_cRefDll);

  if (0 == ulRefCount)
  {
    LOG(INFO) << __FUNCTION__ << "() -> Yes";
    return S_OK;
  }
  LOG(INFO) << __FUNCTION__ << "() -> No, " << ulRefCount << " instances are still in use.";
  return S_FALSE;
}

STDAPI DllRegisterServer(void)
{
  const std::string guid_str_tmp = GuidToString(CLSID_ShellAnythingMenu).c_str();
  const char* guid_str = guid_str_tmp.c_str();
  const std::string class_name_version1 = std::string(ShellExtensionClassName) + ".1";
  const std::string module_path = GetCurrentModulePath();

  //#define TRACELINE() MessageBox(NULL, (std::string("Line: ") + ra::strings::ToString(__LINE__)).c_str(), "DllUnregisterServer() DEBUG", MB_OK);

  //Register version 1 of our class
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\%s", class_name_version1.c_str());
    if (!Win32Registry::CreateKey(key.c_str(), ShellExtensionDescription))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\%s.1\\CLSID", ShellExtensionClassName);
    if (!Win32Registry::CreateKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  //Register current version of our class
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\%s", ShellExtensionClassName);
    if (!Win32Registry::CreateKey(key.c_str(), ShellExtensionDescription))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\%s\\CLSID", ShellExtensionClassName);
    if (!Win32Registry::CreateKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\%s\\CurVer", ShellExtensionClassName);
    if (!Win32Registry::CreateKey(key.c_str(), class_name_version1.c_str()))
      return E_ACCESSDENIED;
  }

  // Add the CLSID of this DLL to the registry
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\CLSID\\%s", guid_str);
    if (!Win32Registry::CreateKey(key.c_str(), ShellExtensionDescription))
      return E_ACCESSDENIED;
  }

  // Define the path and parameters of our DLL:
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\CLSID\\%s\\ProgID", guid_str);
    if (!Win32Registry::CreateKey(key.c_str(), class_name_version1.c_str()))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\CLSID\\%s\\VersionIndependentProgID", guid_str);
    if (!Win32Registry::CreateKey(key.c_str(), ShellExtensionClassName))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\CLSID\\%s\\Programmable", guid_str);
    if (!Win32Registry::CreateKey(key.c_str()))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\CLSID\\%s\\InprocServer32", guid_str);
    if (!Win32Registry::CreateKey(key.c_str(), module_path.c_str()))
      return E_ACCESSDENIED;
    if (!Win32Registry::SetValue(key.c_str(), "ThreadingModel", "Apartment"))
      return E_ACCESSDENIED;
  }

  // Register the shell extension for all the file types
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\*\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::CreateKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  // Register the shell extension for directories
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\Directory\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::CreateKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  // Register the shell extension for folders
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\Folder\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::CreateKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  // Register the shell extension for the desktop or the file explorer's background
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\Directory\\Background\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::CreateKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  // Register the shell extension for drives
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\Drive\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::CreateKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  // Register the shell extension to the system's approved Shell Extensions
  {
    std::string key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved";
    if (!Win32Registry::CreateKey(key.c_str()))
      return E_ACCESSDENIED;
    if (!Win32Registry::SetValue(key.c_str(), guid_str, ShellExtensionDescription))
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

STDAPI DllUnregisterServer(void)
{
  const std::string guid_str_tmp = GuidToString(CLSID_ShellAnythingMenu).c_str();
  const char* guid_str = guid_str_tmp.c_str();
  const std::string class_name_version1 = std::string(ShellExtensionClassName) + ".1";
  const std::string guid_icontextmenu_tmp = GuidToString(IID_IContextMenu);
  const std::string guid_icontextmenu = guid_icontextmenu_tmp.c_str();

  //#define TRACELINE() MessageBox(NULL, (std::string("Line: ") + ra::strings::ToString(__LINE__)).c_str(), "DllUnregisterServer() DEBUG", MB_OK);

  // Removed the shell extension from the user's cached Shell Extensions
  {
    std::string key = "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Cached";
    std::string value = ra::strings::Format("%s {000214E4-0000-0000-C000-000000000046} 0xFFFF", guid_str); // {B0D35103-86A1-471C-A653-E130E3439A3B} {000214E4-0000-0000-C000-000000000046} 0xFFFF
    if (!Win32Registry::DeleteValue(key.c_str(), value.c_str()))
      return E_ACCESSDENIED;
  }

  // Unregister the shell extension from the system's approved Shell Extensions
  {
    std::string key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved";
    if (!Win32Registry::DeleteValue(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  // Unregister the shell extension for drives
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\Drive\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::DeleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }

  // Unregister the shell extension for the desktop or the file explorer's background
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\Directory\\Background\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::DeleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }

  // Unregister the shell extension for folders
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\Folders\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::DeleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }

  // Unregister the shell extension for directories
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\Directory\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::DeleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }

  // Unregister the shell extension for all the file types
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\*\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::DeleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }

  // Remove the CLSID of this DLL from the registry
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\CLSID\\%s", guid_str);
    if (!Win32Registry::DeleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }

  // Unregister current and version 1 of our extension
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\%s", class_name_version1.c_str());
    if (!Win32Registry::DeleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\%s", ShellExtensionClassName);
    if (!Win32Registry::DeleteKey(key.c_str()))
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

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
  if (dwReason == DLL_PROCESS_ATTACH)
  {
    ATTACH_HOOK_DEBUGGING;

    g_hmodDll = hInstance;

    DisableThreadLibraryCalls((HMODULE)hInstance);

    if (!shellanything::IsTestingEnvironment())
    {
      // Initialize Google's logging library.
      shellanything::InitLogger();
    }

    LogEnvironment();

    // Initialize the configuration manager
    InitConfigManager();
  }
  else if (dwReason == DLL_PROCESS_DETACH)
  {
    if (!shellanything::IsTestingEnvironment())
    {
      // Shutdown Google's logging library.
      google::ShutdownGoogleLogging();
    }
  }
  return 1;
}

//int main(int argc, char* argv[])
//{
//  {
//    HRESULT result = DllRegisterServer();
//    if (result == S_OK)
//      //MessageBox(NULL, "Manual dll registration successfull", ShellExtensionClassName, MB_OK | MB_ICONINFORMATION);
//    else                                              
//      //MessageBox(NULL, "Manual dll registration FAILED !", ShellExtensionClassName, MB_OK | MB_ICONERROR);
//  }
//
//  {
//    HRESULT result = DllUnregisterServer();
//    if (result == S_OK)
//      //MessageBox(NULL, "Manual dll unregistration successfull", ShellExtensionClassName, MB_OK | MB_ICONINFORMATION);
//    else
//      //MessageBox(NULL, "Manual dll unregistration FAILED !", ShellExtensionClassName, MB_OK | MB_ICONERROR);
//  }
//}
