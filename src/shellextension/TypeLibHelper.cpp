#include "stdafx.h"
#include "TypeLibHelper.h"

#include <strsafe.h> // for StringCchPrintf()

using namespace ATL;
extern HRESULT GetHKCRRegistryKeyAndValue(PCWSTR pszSubKey, PCWSTR pszValueName, PWSTR pszData, DWORD cbData);	// from Reg.cpp

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

HRESULT IsTypeLibRegisteredOnSystem(_In_ LPTLIBATTR pTLibAttr)
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

  // Check for the "HKCR\TypeLib\{guid}\{version}" key.
  wchar_t szSubkey[MAX_PATH];
  hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"TypeLib\\%s\\%s", szGUID, szVersion);
  if (FAILED(hr))
    return hr;

  // Get the TypeLib name
  wchar_t szTypeLibName[MAX_PATH];
  hr = GetHKCRRegistryKeyAndValue(szSubkey, nullptr, szTypeLibName, sizeof(szTypeLibName));
  if (FAILED(hr))
  {
    if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
      return S_FALSE;
    return hr; // unexpected error
  }

  // If TypeLib name is empty.
  if (szTypeLibName[0] == L'\0')
    return S_FALSE;

  // Check for the "HKCR\TypeLib\{guid}\{version}\0\{syskind}" key.
  hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"TypeLib\\%s\\%s\\0\\%s", szGUID, szVersion, szSysKind);
  if (FAILED(hr))
    return hr;

  // Get the TypeLib dll path
  wchar_t szDllPath[MAX_PATH];
  hr = GetHKCRRegistryKeyAndValue(szSubkey, nullptr, szDllPath, sizeof(szDllPath));
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