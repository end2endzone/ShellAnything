#include "stdafx.h"
#include "TypeLibHelper.h"

#include <strsafe.h> // for StringCchPrintf()

using namespace ATL;

//
//   FUNCTION: GetHKEYRegistryKeyAndValue
//
//   PURPOSE: The function opens the given HKEY (HKCR, HKCU, etc) registry key and gets the data for the 
//   specified registry value name.
//
//   PARAMETERS:
//   * hRootKey - One of the reserved root key handles. For example:
//     HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, HKEY_USERS or 
//   * pszSubKey - specifies the registry key under HKCR. If the key does not 
//     exist, the function returns an error.
//   * pszValueName - specifies the registry value to be retrieved. If 
//     pszValueName is NULL, the function will get the default value.
//   * pszData - a pointer to a buffer that receives the value's string data.
//   * cbData - specifies the size of the buffer in bytes.
//
//   RETURN VALUE:
//   If the function succeeds, it returns S_OK. Otherwise, it returns an 
//   HRESULT error code. For example, if the specified registry key does not 
//   exist or the data for the specified value name was not set, the function 
//   returns COR_E_FILENOTFOUND (0x80070002).
// 
HRESULT GetHKEYRegistryKeyAndValue(HKEY hRootKey, PCWSTR pszSubKey, PCWSTR pszValueName, PWSTR pszData, DWORD cbData)
{
  HRESULT hr;
  HKEY hKey = NULL;

  // Try to open the specified registry key. 
  hr = HRESULT_FROM_WIN32(RegOpenKeyEx(hRootKey, pszSubKey, 0,
                          KEY_READ, &hKey));

  if (SUCCEEDED(hr))
  {
    // Get the data for the specified value name.
    hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, pszValueName, NULL,
                            NULL, reinterpret_cast<LPBYTE>(pszData), &cbData));

    RegCloseKey(hKey);
  }

  return hr;
}

HRESULT GetTypeLibGuid(_In_ HINSTANCE hInstTypeLib, _In_opt_z_ LPCOLESTR lpszIndex, GUID& guid)
{
  ZeroMemory(&guid, sizeof(GUID));
  TLIBATTR sTLibAttr;
  HRESULT hr = GetTypeLibAttribute(hInstTypeLib, lpszIndex, &sTLibAttr);
  if (SUCCEEDED(hr))
    guid = sTLibAttr.guid;
  return hr;
}

HRESULT GetTypeLibAttribute(_In_ HINSTANCE hInstTypeLib, _In_opt_z_ LPCOLESTR lpszIndex, _Out_ LPTLIBATTR pTLibAttr)
{
  if (NULL == pTLibAttr)
    return E_INVALIDARG;

  ZeroMemory(pTLibAttr, sizeof(TLIBATTR));

  CComBSTR bstrPath;
  CComPtr<ITypeLib> pTypeLib;
  HRESULT hr = AtlLoadTypeLib(hInstTypeLib, lpszIndex, &bstrPath, &pTypeLib);
  if (SUCCEEDED(hr))
  {
    TLIBATTR* ptla;
    hr = pTypeLib->GetLibAttr(&ptla);
    if (SUCCEEDED(hr))
    {
      *pTLibAttr = *ptla;

      pTypeLib->ReleaseTLibAttr(ptla);
    }
  }
  return hr;
}

HRESULT IsTypeLibRegisteredOnSystem(const GUID& guid, PCWSTR szVersion)
{
  TLIBATTR sTLibAttr;
  ZeroMemory(&sTLibAttr, sizeof(TLIBATTR));
  sTLibAttr.guid = guid;
  sTLibAttr.syskind = SYS_WIN64;
  sTLibAttr.wMajorVerNum = 1;
  sTLibAttr.wMinorVerNum = 0;

  HRESULT hr = IsTypeLibRegisteredOnSystem(&sTLibAttr);
  return hr;
}

HRESULT IsTypeLibRegisteredUnderKey(HKEY hRootKey, _In_ const LPTLIBATTR pTLibAttr)
{
  if (NULL == pTLibAttr)
    return E_INVALIDARG;

  HRESULT hr;

  // guid to string
  wchar_t szGUID[64] = { 0 };
  StringFromGUID2(pTLibAttr->guid, szGUID, 64);

  // version to string
  wchar_t szVersion[32] = { 0 };
  hr = StringCchPrintf(szVersion, ARRAYSIZE(szVersion), L"%d.%d", pTLibAttr->wMajorVerNum, pTLibAttr->wMajorVerNum);
  if (FAILED(hr))
    return hr;

  // syskind to string
  const wchar_t* szSysKind = NULL;
  switch (pTLibAttr->syskind)
  {
  case SYS_WIN32:
    szSysKind = L"win32";
    break;
  case SYS_WIN64:
    szSysKind = L"win64";
    break;
  default:
    return HRESULT_FROM_WIN32(ERROR_INVALID_FLAGS);
  };

  // Check for the "HKEY_CLASSES_ROOT\TypeLib\{guid}\{version}" key.
  wchar_t szSubkey[MAX_PATH];
  hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"TypeLib\\%s\\%s", szGUID, szVersion);
  if (FAILED(hr))
    return hr;

  // Get the TypeLib name
  wchar_t szTypeLibName[MAX_PATH];
  hr = GetHKEYRegistryKeyAndValue(hRootKey, szSubkey, nullptr, szTypeLibName, sizeof(szTypeLibName));
  if (FAILED(hr))
  {
    if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
      return S_FALSE;
    return hr; // unexpected error
  }

  // If TypeLib name is empty.
  if (szTypeLibName[0] == L'\0')
    return S_FALSE;

  // Check for the "HKEY_CLASSES_ROOT\TypeLib\{guid}\{version}\0\{syskind}" key.
  hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"TypeLib\\%s\\%s\\0\\%s", szGUID, szVersion, szSysKind);
  if (FAILED(hr))
    return hr;

  // Get the TypeLib dll path
  wchar_t szDllPath[MAX_PATH];
  hr = GetHKEYRegistryKeyAndValue(hRootKey, szSubkey, nullptr, szDllPath, sizeof(szDllPath));
  if (FAILED(hr))
  {
    if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
      return S_FALSE;
    return hr; // unexpected error
  }

  // If TypeLib dll path is empty.
  if (szDllPath[0] == L'\0')
    return S_FALSE;

  //// Get path to current module
  //wchar_t szModule[MAX_PATH];
  //HINSTANCE hInst = GetModuleHandle(NULL);
  //hr = S_OK;
  //if ( GetModuleFileName(hInst, szModule, ARRAYSIZE(szModule)) == 0 )
  //{
  //	hr = HRESULT_FROM_WIN32(GetLastError());
  //	return hr;
  //}

  //// If TypeLib dll path is not this module's path.
  //if ( _wcsicmp(szDllPath, szModule) == 0)
  //	return S_FALSE;

  return S_OK;
}

HRESULT IsTypeLibRegisteredOnSystem(_In_ const LPTLIBATTR pTLibAttr)
{
  HRESULT hr = IsTypeLibRegisteredUnderKey(HKEY_CLASSES_ROOT, pTLibAttr);
  return hr;
}

HRESULT IsTypeLibRegisteredForCurrentUser(_In_ const LPTLIBATTR pTLibAttr)
{
  HRESULT hr = IsTypeLibRegisteredUnderKey(HKEY_CURRENT_USER, pTLibAttr);
  return hr;
}
