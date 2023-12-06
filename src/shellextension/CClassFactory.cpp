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

#include "CClassFactory.h"
#include "CContextMenu.h"
#include "shellext.h"
#include "LoggerHelper.h"

// Constructeur de l'interface IClassFactory:
CClassFactory::CClassFactory()
{
  SA_LOG(INFO) << __FUNCTION__ << "(), new instance " << ToHexString(this);

#if SA_QUERYINTERFACE_IMPL == 0
  m_refCount = 0; // reference counter must be initialized to 0 even if we are actually creating an instance. A reference to this instance will be added when the instance will be queried by explorer.exe.
#elif SA_QUERYINTERFACE_IMPL == 1
  m_refCount = 1;
#endif

  // Increment the dll's reference counter.
  DllAddRef();
}

// Destructeur de l'interface IClassFactory:
CClassFactory::~CClassFactory()
{
  SA_LOG(INFO) << __FUNCTION__ << "(), delete instance " << ToHexString(this);

  // Decrement the dll's reference counter.
  DllRelease();
}

HRESULT STDMETHODCALLTYPE CClassFactory::QueryInterface(REFIID riid, LPVOID FAR* ppv)
{
  std::string riid_str = GuidToInterfaceName(riid);
  SA_LOG(INFO) << __FUNCTION__ << "(), riid=" << riid_str << ", this=" << ToHexString(this);

  HRESULT hr = E_NOINTERFACE;

#if SA_QUERYINTERFACE_IMPL == 0
  //https://docs.microsoft.com/en-us/office/client-developer/outlook/mapi/implementing-iunknown-in-c-plus-plus

  // Always set out parameter to NULL, validating it first.
  if (!ppv)
    return E_INVALIDARG;
  *ppv = NULL;

  //https://stackoverflow.com/questions/1742848/why-exactly-do-i-need-an-explicit-upcast-when-implementing-queryinterface-in-a
  if (IsEqualGUID(riid, IID_IUnknown))        *ppv = (LPVOID)this;
  if (IsEqualGUID(riid, IID_IClassFactory))   *ppv = (LPCLASSFACTORY)this;

  if (*ppv)
  {
    AddRef();
    hr = S_OK;
  }
  else
    hr = E_NOINTERFACE;
#elif SA_QUERYINTERFACE_IMPL == 1
  static const QITAB qit[] =
  {
    QITABENT(CClassFactory, IClassFactory),
    { 0, 0 }
  };
  hr = QISearch(this, qit, riid, ppv);
#endif

  if (SUCCEEDED(hr))
    SA_LOG(INFO) << __FUNCTION__ << "(), found interface " << riid_str << ", ppv=" << ToHexString(*ppv);
  else
    SA_LOG(WARNING) << __FUNCTION__ << "(), unknown interface " << riid_str;
  return hr;
}

ULONG STDMETHODCALLTYPE CClassFactory::AddRef()
{
  //https://docs.microsoft.com/en-us/office/client-developer/outlook/mapi/implementing-iunknown-in-c-plus-plus

  // Increment the object's internal counter.
  return InterlockedIncrement(&m_refCount);
}

ULONG STDMETHODCALLTYPE CClassFactory::Release()
{
  //https://docs.microsoft.com/en-us/office/client-developer/outlook/mapi/implementing-iunknown-in-c-plus-plus

  // Decrement the object's internal counter.
  LONG refCount = InterlockedDecrement(&m_refCount);
  if (refCount == 0)
  {
    delete this;
  }
  return refCount;
}

HRESULT STDMETHODCALLTYPE CClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID FAR* ppv)
{
  std::string riid_str = GuidToInterfaceName(riid);
  SA_LOG(INFO) << __FUNCTION__ << "(), pUnkOuter=" << pUnkOuter << ", riid=" << riid_str << " this=" << ToHexString(this);

#if SA_QUERYINTERFACE_IMPL == 0
  // Always set out parameter to NULL, validating it first.
  if (!ppv)
    return E_INVALIDARG;
  *ppv = NULL;

  if (pUnkOuter) return CLASS_E_NOAGGREGATION;
  CContextMenu* pContextMenu = new CContextMenu();
  if (!pContextMenu) return E_OUTOFMEMORY;
  HRESULT hr = pContextMenu->QueryInterface(riid, ppv);
  if (FAILED(hr))
  {
    pContextMenu->Release();
  }
#elif SA_QUERYINTERFACE_IMPL == 1
  HRESULT hr = CLASS_E_NOAGGREGATION;

  // pUnkOuter is used for aggregation. We do not support it in the sample.
  if (pUnkOuter == NULL)
  {
    hr = E_OUTOFMEMORY;

    // Create the COM component.
    CContextMenu* pExt = new (std::nothrow) CContextMenu();
    if (pExt)
    {
      // Query the specified interface.
      hr = pExt->QueryInterface(riid, ppv);
      pExt->Release();
    }
  }
#endif

  if (SUCCEEDED(hr))
    SA_LOG(INFO) << __FUNCTION__ << "(), found interface " << riid_str << ", ppv=" << ToHexString(*ppv);
  else
    SA_LOG(ERROR) << __FUNCTION__ << "(), failed creating interface " << riid_str;
  return hr;
}

HRESULT STDMETHODCALLTYPE CClassFactory::LockServer(BOOL bLock)
{
  SA_LOG(INFO) << __FUNCTION__ << "(), bLock=" << (int)bLock << " this=" << ToHexString(this);

  //https://docs.microsoft.com/en-us/windows/desktop/api/unknwnbase/nf-unknwnbase-iclassfactory-lockserver
  //https://docs.microsoft.com/en-us/windows/desktop/api/combaseapi/nf-combaseapi-colockobjectexternal

  // Note:
  // Previous implementations was blindly returning S_OK without doing anything else. This is probably a bad idea. Examples: git-for-windows/7-Zip
  // Other implementations just return E_NOTIMPL to let explorer know. Examples: TortoiseGit.
  // Finaly, most other implementations resolves on adding a lock on the DLL. This is better but not it does not follow the official microsoft documentation. Examples: chrdavis/SmartRename, owncloud/client, https://github.com/microsoft/Windows-classic-samples/

  if (bLock)
  {
    DllAddRef();
  }
  else
  {
    DllRelease();
  }
  return S_OK;
}
