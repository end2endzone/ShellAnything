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

#include "TestShellExtension.h"

#include "rapidassist/filesystem.h"
#include "rapidassist/environment.h"
#include "rapidassist/process.h"
#include "rapidassist/timing.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

#include <windows.h>
#include <comdef.h>
#include <Shobjidl.h>

namespace shellanything { namespace test
{
  static const GUID SHELLANYTHING_SHELLEXTENSION_CLSID = { 0xb0d35103, 0x86a1, 0x471c, { 0xa6, 0x53, 0xe1, 0x30, 0xe3, 0x43, 0x9a, 0x3b } };
  static const GUID CLSID_INVALID = { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

  std::string GetShellExtensionDllPath()
  {
    std::string cur_process = ra::process::GetCurrentProcessPath();

    std::string path = cur_process;
    ra::strings::Replace(path, "sa.tests", "sa.shellextension");
    ra::strings::Replace(path, ".exe", ".dll");
    return path;
  }

  class ComHandler
  {
  public:
    ComHandler() :
      mFailed(false)
    {
	    // Initialize COM Library
	    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	    if ( FAILED(hr) )
	    {
        mFailed = true;
        mErrorMessage = _com_error(hr).ErrorMessage();
	    }
    }

    ~ComHandler()
    {
      if (!HasFailed())
      {
	      // Uninitialize COM Library
	      CoUninitialize();
      }
    }

    bool HasFailed()
    {
      return mFailed;
    }

    const std::string & GetErrorMessage()
    {
      return mErrorMessage;
    }

  private:
    bool mFailed;
    std::string mErrorMessage;
  };

  class ShellExtensionLibraryHandler
  {
  public:
 		typedef HRESULT (__stdcall *DllGetClassObjectFuncPtr)(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv);
 		typedef HRESULT (__stdcall *DllCanUnloadNowFuncPtr)();

    ShellExtensionLibraryHandler() :
      mFailed(true),
      mModule(NULL)
    {
      std::string path = GetShellExtensionDllPath();

	    // Load the library
      mModule = ::LoadLibrary(path.c_str());
      if (mModule)
      {
 		    pfnDllGetClassObject = (DllGetClassObjectFuncPtr)::GetProcAddress(mModule, "DllGetClassObject");
 		    pfnDllCanUnloadNow = (DllCanUnloadNowFuncPtr)::GetProcAddress(mModule, "DllCanUnloadNow");

        if (pfnDllGetClassObject == NULL) return;
        if (pfnDllCanUnloadNow == NULL) return;

        mFailed = false;
      }
    }

    ~ShellExtensionLibraryHandler()
    {
      if (!HasFailed())
      {
	      // Unload the library
		    ::FreeLibrary(mModule);
      }
    }

    HRESULT DllCanUnloadNow()
    {
      HRESULT result = pfnDllCanUnloadNow();
      return result;
    }

    HRESULT DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
    {
      HRESULT result = pfnDllGetClassObject(rclsid, riid, ppvOut);
      return result;
    }

    bool HasFailed()
    {
      return mFailed;
    }

    const std::string & GetErrorMessage()
    {
      return mErrorMessage;
    }

  private:
    bool mFailed;
    std::string mErrorMessage;
    HMODULE mModule;
    DllGetClassObjectFuncPtr pfnDllGetClassObject;
    DllCanUnloadNowFuncPtr pfnDllCanUnloadNow;
  };



  //--------------------------------------------------------------------------------------------------
  void TestShellExtension::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestShellExtension::TearDown()
  {
    //Sleep betwwen tests to allow GLOG to generate different log filenames.
    ra::timing::Millisleep(1500);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestShellExtension, testDefaultDllCanUnloadNow)
  {
    LOG(INFO) << __FUNCTION__ << "() - BEGIN";

    std::string path = GetShellExtensionDllPath();
    ASSERT_TRUE( ra::filesystem::FileExists(path.c_str()) ) << "File not found: '" << path << "'.";

    ComHandler com_handler;
    ASSERT_FALSE( com_handler.HasFailed() ) << "Failed initializing COM: '" << com_handler.GetErrorMessage() << "'.";

    ShellExtensionLibraryHandler lib_handler;
    ASSERT_FALSE( lib_handler.HasFailed() ) << "Failed loading the library: '" << lib_handler.GetErrorMessage() << "'.";

    //assert ok to unload the dll of no classes was instanciated
    ASSERT_EQ( S_OK, lib_handler.DllCanUnloadNow() );

    LOG(INFO) << __FUNCTION__ << "() - END";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestShellExtension, testGetClassFactory)
  {
    LOG(INFO) << __FUNCTION__ << "() - BEGIN";

    std::string path = GetShellExtensionDllPath();
    ASSERT_TRUE( ra::filesystem::FileExists(path.c_str()) ) << "File not found: '" << path << "'.";

    ComHandler com_handler;
    ASSERT_FALSE( com_handler.HasFailed() ) << "Failed initializing COM: '" << com_handler.GetErrorMessage() << "'.";

    ShellExtensionLibraryHandler lib_handler;
    ASSERT_FALSE( lib_handler.HasFailed() ) << "Failed loading the library: '" << lib_handler.GetErrorMessage() << "'.";

    IClassFactory * pClassFactory = NULL;

    //test with an invalid CLSID query
    HRESULT hr = lib_handler.DllGetClassObject(CLSID_INVALID, IID_IClassFactory, (LPVOID *)&pClassFactory);
    ASSERT_EQ( CLASS_E_CLASSNOTAVAILABLE, hr );
    ASSERT_EQ( S_OK, lib_handler.DllCanUnloadNow() );

    //test with an invalid IID query
    hr = lib_handler.DllGetClassObject(SHELLANYTHING_SHELLEXTENSION_CLSID, CLSID_INVALID, (LPVOID *)&pClassFactory);
    ASSERT_EQ( E_NOINTERFACE, hr );
    ASSERT_EQ( S_OK, lib_handler.DllCanUnloadNow() );

    //get valid class factory
    hr = lib_handler.DllGetClassObject(SHELLANYTHING_SHELLEXTENSION_CLSID, IID_IClassFactory, (LPVOID *)&pClassFactory);
    ASSERT_EQ( NOERROR, hr );
    ASSERT_TRUE( pClassFactory != NULL );

    //we now have a valid class
    ASSERT_EQ( S_FALSE, lib_handler.DllCanUnloadNow() );
    pClassFactory->Release(); //this should automatically destroy the CClassFactory::~CClassFactory()
    ASSERT_EQ( S_OK, lib_handler.DllCanUnloadNow() );
    int a = 0;

    LOG(INFO) << __FUNCTION__ << "() - END";
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestShellExtension, testCreateInstance)
  {
    LOG(INFO) << __FUNCTION__ << "() - BEGIN";

    std::string path = GetShellExtensionDllPath();
    ASSERT_TRUE( ra::filesystem::FileExists(path.c_str()) ) << "File not found: '" << path << "'.";

    ComHandler com_handler;
    ASSERT_FALSE( com_handler.HasFailed() ) << "Failed initializing COM: '" << com_handler.GetErrorMessage() << "'.";

    ShellExtensionLibraryHandler lib_handler;
    ASSERT_FALSE( lib_handler.HasFailed() ) << "Failed loading the library: '" << lib_handler.GetErrorMessage() << "'.";

    //get IClassFactory interface pointer
    IClassFactory * pClassFactory = NULL;
    HRESULT hr = lib_handler.DllGetClassObject(SHELLANYTHING_SHELLEXTENSION_CLSID, IID_IClassFactory, (LPVOID *)&pClassFactory);
    ASSERT_EQ( NOERROR, hr );
    ASSERT_TRUE( pClassFactory != NULL );

    //get IShellExtInit interface pointer
    IShellExtInit * pShellExtInit = NULL;
    hr = pClassFactory->CreateInstance(NULL, IID_IShellExtInit, (void**)&pShellExtInit);
    ASSERT_EQ( NOERROR, hr );
    ASSERT_TRUE( pShellExtInit != NULL );

    //get IContextMenu interface pointer
    IContextMenu * pContextMenu = NULL;
    hr = pClassFactory->CreateInstance(NULL, IID_IContextMenu, (void**)&pContextMenu);
    ASSERT_EQ( NOERROR, hr );
    ASSERT_TRUE( pContextMenu != NULL );

    //release everything
    pContextMenu->Release();
    pShellExtInit->Release();
    pClassFactory->Release();

    ASSERT_EQ( S_OK, lib_handler.DllCanUnloadNow() );
    int a = 0;

    LOG(INFO) << __FUNCTION__ << "() - END";
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
