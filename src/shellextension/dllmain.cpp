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

#include "resource.h"
#include <initguid.h>
#include <shlobj.h> // for SHChangeNotify(), SHCNE_ASSOCCHANGED

// Generated files
#include "shellext.h"
#include "shellext_i.c"

#include "LoggerHelper.h"

#include "GlogLoggerService.h"
#include "WindowsRegistryService.h"
#include "WindowsClipboardService.h"
#include "WindowsKeyboardService.h"
#include "PcgRandomService.h"
#include "WindowsIconResolutionService.h"
#include "WindowsProcessLauncherService.h"

#include "shellanything/version.h"
#include "shellanything/config.h"

#include "GlogUtils.h"
#include "SaUtils.h"
#include "utils.h"
#include "TypeLibHelper.h"

#include "rapidassist/errors.h"

//Declarations
shellanything::ILoggerService* logger_service = NULL;
shellanything::IRegistryService* registry_service = NULL;
shellanything::IClipboardService* clipboard_service = NULL;
shellanything::IKeyboardService* keyboard_service = NULL;
shellanything::IRandomService* random_service = NULL;
shellanything::IIconResolutionService* icon_resolution_service = NULL;
shellanything::IProcessLauncherService* process_launcher_service = NULL;

class CShellAnythingModule : public ATL::CAtlDllModuleT< CShellAnythingModule >
{
public:
  DECLARE_LIBID(LIBID_SHELLEXTENSIONLib)
  DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SHELLANYTHING, "{EB26EA8E-3B98-4DED-AE59-255C3BA725C3}")
};

class CShellAnythingModule _AtlModule;

// Returns a class factory to create an object of the requested type.
_Use_decl_annotations_
STDAPI DllGetClassObject(_In_ REFCLSID clsid, _In_ REFIID riid, _Outptr_ LPVOID* ppv)
{
  SA_LOG(INFO) << __FUNCTION__ "() " << GetProcessContextDesc();

  std::string clsid_str = GuidToInterfaceName(clsid);
  std::string riid_str = GuidToInterfaceName(riid);

  HRESULT hr = _AtlModule.DllGetClassObject(clsid, riid, ppv);

  if (hr == CLASS_E_CLASSNOTAVAILABLE)
    SA_LOG(ERROR) << __FUNCTION__ << "(), ClassFactory " << clsid_str << " not found!";
  else if (FAILED(hr))
    SA_LOG(ERROR) << __FUNCTION__ << "(), unknown interface " << riid_str;
  else
    SA_LOG(INFO) << __FUNCTION__ << "(), found interface " << riid_str << ", ppv=" << ToHexString(*ppv);
  return hr;
}

// Used to determine whether the DLL can be unloaded by OLE.
_Use_decl_annotations_
STDAPI DllCanUnloadNow(void)
{
  SA_LOG(INFO) << __FUNCTION__ "() " << GetProcessContextDesc();

  HRESULT hr = _AtlModule.DllCanUnloadNow();

  if (hr == S_OK)
  {
    SA_LOG(INFO) << __FUNCTION__ << "() -> Yes";
    return S_OK;
  }
  SA_LOG(INFO) << __FUNCTION__ << "() -> No.";
  return S_FALSE;
}

// DllRegisterServer - Adds entries to the system registry.
_Use_decl_annotations_
STDAPI DllRegisterServer(void)
{
  // registers object, typelib and all interfaces in typelib
  HRESULT hr = _AtlModule.DllRegisterServer();

  if (SUCCEEDED(hr))
  {
    // Notify the Shell to pick the changes:
    // https://docs.microsoft.com/en-us/windows/desktop/shell/reg-shell-exts#predefined-shell-objects
    // Any time you create or change a Shell extension handler, it is important to notify the system that you have made a change.
    // Do so by calling SHChangeNotify, specifying the SHCNE_ASSOCCHANGED event.
    // If you do not call SHChangeNotify, the change might not be recognized until the system is rebooted.
    SHChangeNotify(SHCNE_ASSOCCHANGED, 0, 0, 0);
  }

  return hr;
}

// DllUnregisterServer - Removes entries from the system registry.
_Use_decl_annotations_
STDAPI DllUnregisterServer(void)
{
  ATTACH_HOOK_DEBUGGING;

  // unregisters object, typelib and all interfaces in typelib
  HRESULT hr = _AtlModule.DllUnregisterServer();

  // Issue #148 - Can't uninstall
  if ( hr == TYPE_E_REGISTRYACCESS )
  {
    // Unregistration has failed with error 0x8002801c.
    // Function _AtlModule.DllUnregisterServer() can also return TYPE_E_REGISTRYACCESS if the TypeLib is not registered on system.
    // For example, calling `_AtlModule.DllUnregisterServer()` twice, the second call will return TYPE_E_REGISTRYACCESS.
    // See issue #148 for details.
    // Is this failure because the TypeLib is not registered?

    HRESULT hr2 = S_OK;

    // Get typelib attributes
    TLIBATTR sTLibAttr;
    ZeroMemory(&sTLibAttr, sizeof(TLIBATTR));
    hr2 = GetTypeLibAttribute(_AtlComModule.m_hInstTypeLib, 0, &sTLibAttr);
    if (FAILED(hr2))
      return hr; // return original error

    // Check if typelib is registered on system.
    hr2 = IsTypeLibRegisteredOnSystem(&sTLibAttr);
    if (FAILED(hr2))
      return hr; // We don't know if typelib is registered or not. Return original error.
    if (hr2 == S_OK)
      return hr; // Yes, the typelib is registered, hr is a legit error. 

    // Check if typelib is registered for current user.
    hr2 = IsTypeLibRegisteredForCurrentUser(&sTLibAttr);
    if (FAILED(hr2))
      return hr; // We don't know if typelib is registered or not. Return original error.
    if (hr2 == S_OK)
      return hr; // Yes, the typelib is registered, hr is a legit error. 

    // At this point, assume error 0x8002801c (TYPE_E_REGISTRYACCESS) is because the typelib is not registered.
    // To be certain, we should register the typelib and retry the unregistration:
    //    _AtlModule.DllRegisterServer(); // don't care about the actual result
    //    hr = _AtlModule.DllUnregisterServer();
    // but that could mess up the system if another software is listening for new typelib registrations events.
    // So we just silence the original error.
    return S_OK;
  }

  if (SUCCEEDED(hr))
  {
    // Notify the Shell to pick the changes:
    // https://docs.microsoft.com/en-us/windows/desktop/shell/reg-shell-exts#predefined-shell-objects
    // Any time you create or change a Shell extension handler, it is important to notify the system that you have made a change.
    // Do so by calling SHChangeNotify, specifying the SHCNE_ASSOCCHANGED event.
    // If you do not call SHChangeNotify, the change might not be recognized until the system is rebooted.
    SHChangeNotify(SHCNE_ASSOCCHANGED, 0, 0, 0);
  }

  return hr;
}

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
  //ATTACH_HOOK_DEBUGGING;

  BOOL result = _AtlModule.DllMain(dwReason, lpReserved);
  if (result == FALSE)
  {
    SA_LOG(ERROR) << "Failed initializing module DLLMain().";

    ra::errors::errorcode_t code = ra::errors::GetLastErrorCode();
    std::string desc = ra::errors::GetErrorCodeDescription(code);

    SA_LOG(ERROR) << "ERROR: 0x" << std::hex << code << std::dec << ", " << desc;
    return result;
  }

  // Skip log initializations and default configuration installation if process that is loading this dll is regsvr32.exe.
  // Shell extension un/registration will not call a shell extension entry points and does not require logging support.
  if (IsRegsvr32Process())
    return 1;

  shellanything::App& app = shellanything::App::GetInstance();

  if (dwReason == DLL_PROCESS_ATTACH)
  {
    ATTACH_HOOK_DEBUGGING;

    DisableThreadLibraryCalls((HMODULE)hInstance);

    if (!app.IsTestingEnvironment())
    {
      //Issue #124. Define property 'application.path'.
      std::string dll_path = GetCurrentModulePathUtf8();
      app.SetApplicationPath(dll_path);

      // Initialize Google's logging library.
      shellanything::logging::glog::InitGlog();

      // Setup an active logger in ShellAnything's core.
      logger_service = new shellanything::GlogLoggerService();
      app.SetLoggerService(logger_service);

      // Setup an active registry service in ShellAnything's core.
      registry_service = new shellanything::WindowsRegistryService();
      app.SetRegistryService(registry_service);

      // Setup an active clipboard service in ShellAnything's core.
      clipboard_service = new shellanything::WindowsClipboardService();
      app.SetClipboardService(clipboard_service);

      // Setup an active keyboard service in ShellAnything's core.
      keyboard_service = new shellanything::WindowsKeyboardService();
      app.SetKeyboardService(keyboard_service);

      // Setup an active random service in ShellAnything's core.
      random_service = new shellanything::PcgRandomService();
      app.SetRandomService(random_service);

      // Setup an active icon resolution service in ShellAnything's core.
      icon_resolution_service = new shellanything::WindowsIconResolutionService();
      app.SetIconResolutionService(icon_resolution_service);

      // Setup an active process launcher service in ShellAnything's core.
      process_launcher_service = new shellanything::WindowsProcessLauncherService();
      app.SetProcessLauncherService(process_launcher_service);

      // Setup and starting application
      app.Start();

      LogEnvironment();
    }
  }
  else if (dwReason == DLL_PROCESS_DETACH)
  {
    if (!app.IsTestingEnvironment())
    {
      // Shutdown Google's logging library.
      shellanything::logging::glog::ShutdownGlog();

      // Destroy services
      app.ClearServices();
      delete random_service;
      delete keyboard_service;
      delete clipboard_service;
      delete registry_service;
      delete logger_service;
      delete icon_resolution_service;
      delete process_launcher_service;
      random_service = NULL;
      keyboard_service = NULL;
      clipboard_service = NULL;
      registry_service = NULL;
      logger_service = NULL;
      icon_resolution_service = NULL;
      process_launcher_service = NULL;
    }
  }

  return result;
}

// DllInstall - Adds/Removes entries to the system registry per user per machine.
_Use_decl_annotations_
STDAPI DllInstall(BOOL bInstall, _In_opt_  LPCWSTR pszCmdLine)
{
  ATTACH_HOOK_DEBUGGING;

  HRESULT hr = E_FAIL;
  static const wchar_t szUserSwitch[] = L"user";

  if (pszCmdLine != nullptr)
  {
    if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
    {
      ATL::AtlSetPerUserRegistration(true);
    }
  }

  if (bInstall)
  {
    hr = DllRegisterServer();
    if (FAILED(hr))
    {
      DllUnregisterServer();
    }
  }
  else
  {
    hr = DllUnregisterServer();
  }

  return hr;
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
