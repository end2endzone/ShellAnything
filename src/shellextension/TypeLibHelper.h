#pragma once

#include <Windows.h>

/// <summary>
/// Get the GUID of the TypeLib at index lpszIndex within this module.
/// </summary>
/// <param name="hInstTypeLib">A reference to the module that contains the TypeLib.</param>
/// <param name="lpszIndex">The index of the TyleLib within this module.</param>
/// <param name="guid">The output guid value of the typelib at given index.</param>
/// <returns>Returns S_OK if the attributes of the requested typelib is found. Returns an HRESULT matching an error code otherwise.</returns>
HRESULT GetTypeLibGuid(_In_ HINSTANCE hInstTypeLib, _In_opt_z_ LPCOLESTR lpszIndex, GUID& guid);

/// <summary>
/// Get the attributes of the TypeLib at index lpszIndex within this module.
/// </summary>
/// <param name="hInstTypeLib">A reference to the module that contains the TypeLib.</param>
/// <param name="lpszIndex">The index of the queries TypeLib within the instance.</param>
/// <param name="libattr">A pointer to the output library attributes</param>
/// <returns>Returns S_OK if the attributes of the requested typelib is found. Returns an HRESULT matching an error code otherwise.</returns>
HRESULT GetTypeLibAttribute(_In_ HINSTANCE hInstTypeLib, _In_opt_z_ LPCOLESTR lpszIndex, _Out_ LPTLIBATTR pTLibAttr);

/// <summary>
/// Check if the given typelib guid is registered on system.
/// </summary>
/// <remarks>
/// The following registry keys and values are present for a registered typelib:
///   [HKEY_CLASSES_ROOT\TypeLib\{guid}\1.0]
///   @ = "MyClassNameLib"
///   [HKEY_CLASSES_ROOT\TypeLib\{guid}\1.0\0\win64]
///   @ = "Z:\\path\\to\\current\\file.dll"
/// </remarks>
/// <param name="guid">The typelib's guid value.</param>
/// <param name="guid">The typelib version value. Usually "1.0".</param>
/// <returns>Returns S_OK if the typelib is registered on system. Returns S_FALSE if the typelib is not registered on system. Returns an HRESULT matching an error code otherwise.</returns>
HRESULT IsTypeLibRegisteredOnSystem(const GUID& guid, PCWSTR szVersion);

/// <summary>
/// Check if the given typelib is registered on system.
/// </summary>
/// <remarks>
/// The following registry keys and values are present for a registered typelib:
///   [HKEY_CLASSES_ROOT\TypeLib\{guid}\{version}]
///   @ = "MyClassNameLib"
///   [HKEY_CLASSES_ROOT\TypeLib\{guid}\{version}\0\win64]
///   @ = "Z:\\path\\to\\current\\file.dll"
/// </remarks>
/// <param name="pTLibAttr">A pointer to the typelib's attributes.</param>
/// <returns>Returns S_OK if the typelib is registered on system. Returns S_FALSE if the typelib is not registered on system. Returns an HRESULT matching an error code otherwise.</returns>
HRESULT IsTypeLibRegisteredOnSystem(_In_ const LPTLIBATTR pTLibAttr);

/// <summary>
/// Check if the given typelib is registered for the current user.
/// </summary>
/// <remarks>
/// The following registry keys and values are present for a registered typelib:
///   [HKEY_CURRENT_USER\TypeLib\{guid}\{version}]
///   @ = "MyClassNameLib"
///   [HKEY_CURRENT_USER\TypeLib\{guid}\{version}\0\win64]
///   @ = "Z:\\path\\to\\current\\file.dll"
/// </remarks>
/// <param name="pTLibAttr">A pointer to the typelib's attributes.</param>
/// <returns>Returns S_OK if the typelib is registered on system. Returns S_FALSE if the typelib is not registered on system. Returns an HRESULT matching an error code otherwise.</returns>
HRESULT IsTypeLibRegisteredForCurrentUser(_In_ const LPTLIBATTR pTLibAttr);
