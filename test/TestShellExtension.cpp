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

#include <windows.h>
#include <comdef.h>

namespace shellanything { namespace test
{
  std::string getShellExtensionDllPath()
  {
    std::string cur_process = ra::process::getCurrentProcessPath();

    std::string path = cur_process;
    ra::strings::replace(path, "shellanything_unittest", "shellext");
    ra::strings::replace(path, ".exe", ".dll");
    return path;
  }

  //--------------------------------------------------------------------------------------------------
  void TestShellExtension::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestShellExtension::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestShellExtension, testDefaults)
  {
    std::string path = getShellExtensionDllPath();
    ASSERT_TRUE( ra::filesystem::fileExists(path.c_str()) ) << "File not found: '" << path << "'.";

 		typedef HRESULT (__stdcall *pfnDllGetClassObject)(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv);
 		typedef HRESULT (__stdcall *pfnDllCanUnloadNow)();

	  // Initialize COM Library
	  HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	  if ( FAILED(hr) )
	  {
		  std::cout << _com_error(hr).ErrorMessage() << std::endl;
		  exit(0);
	  }

	  // Create instance
    HMODULE hModule = ::LoadLibrary(path.c_str());
	  if ( hModule != NULL )
	  {
		  std::cout << "LoadLibrary Success." << std::endl;
 		  pfnDllGetClassObject pfDllGetClassObject = (pfnDllGetClassObject)::GetProcAddress(hModule, "DllGetClassObject");
 		  pfnDllCanUnloadNow pfDllCanUnloadNow = (pfnDllCanUnloadNow)::GetProcAddress(hModule, "DllCanUnloadNow");
 		  if ( pfDllGetClassObject != NULL )
 		  {
 			 // hr = pfn(CLSID_HelloComponent, IID_IHello, (LPVOID *)&pHello);
 			 // if ( SUCCEEDED(hr) )
 			 // {
 				//  if ( pHello != NULL )
 				//  {
 				//	  pHello->SayHello(L"CodeSteps");
					//  pHello->Release();
				 // }
			  //}
		  }
		  else
			  std::cout << "DllGetClassObject failed." << std::endl;

		  ::FreeLibrary(hModule);
	  }
	  else
		  std::cout << "LoadLibrary Failed." << std::endl;		

	  // Uninitialize COM Library
	  // 
	  CoUninitialize();
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
