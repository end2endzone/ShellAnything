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

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

// Constructeur de l'interface IClassFactory:
CClassFactory::CClassFactory()
{
  LOG(INFO) << __FUNCTION__ << "(), new instance " << ToHexString(this);

  m_refCount = 0; // reference counter must be initialized to 0 even if we are actually creating an instance. A reference to this instance will be added when the instance will be queried by explorer.exe.

  // Increment the dll's reference counter.
  DllAddRef();
}

// Destructeur de l'interface IClassFactory:
CClassFactory::~CClassFactory()
{
  LOG(INFO) << __FUNCTION__ << "(), delete instance " << ToHexString(this);

  // Decrement the dll's reference counter.
  DllRelease();
}

HRESULT STDMETHODCALLTYPE CClassFactory::QueryInterface(REFIID riid, LPVOID FAR* ppv)
{
  std::string riid_str = GuidToInterfaceName(riid);
  LOG(INFO) << __FUNCTION__ << "(), riid=" << riid_str << ", this=" << ToHexString(this);

  //static const QITAB qit[] =
  //{
  //  QITABENT(CClassFactory, IClassFactory),
  //  { 0, 0 }
  //};
  //return QISearch(this, qit, riid, ppvObj);

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
    // Increment the reference count and return the pointer.
    LOG(INFO) << __FUNCTION__ << "(), found interface " << riid_str << ", ppv=" << ToHexString(*ppv);
    AddRef();
    return S_OK;
  }

  LOG(WARNING) << __FUNCTION__ << "(), unknown interface " << riid_str;
  return E_NOINTERFACE;
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
  LOG(INFO) << __FUNCTION__ << "(), pUnkOuter=" << pUnkOuter << ", riid=" << riid_str << " this=" << ToHexString(this);

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
    LOG(ERROR) << __FUNCTION__ << "(), failed creating interface " << riid_str;
    pContextMenu->Release();
    return hr;
  }

  LOG(INFO) << __FUNCTION__ << "(), found interface " << riid_str << ", ppv=" << ToHexString(*ppv);
  return hr;
}

HRESULT STDMETHODCALLTYPE CClassFactory::LockServer(BOOL bLock)
{
  LOG(INFO) << __FUNCTION__ << "(), bLock=" << (int)bLock << " this=" << ToHexString(this);

  //https://docs.microsoft.com/en-us/windows/desktop/api/unknwnbase/nf-unknwnbase-iclassfactory-lockserver
  //https://docs.microsoft.com/en-us/windows/desktop/api/combaseapi/nf-combaseapi-colockobjectexternal

  // Note:
  // Previous implementations was blindly returning S_OK without doing anything else. This is probably a bad idea. Examples: git-for-windows/7-Zip
  // Other implementations resolves on adding a lock on the DLL. This is better but not it does not follow the official microsoft documentation. Examples: SmartRename, https://github.com/microsoft/Windows-classic-samples/
  // Finaly, some implementation just return E_NOTIMPL to let explorer know. Examples: TortoiseGit.

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
