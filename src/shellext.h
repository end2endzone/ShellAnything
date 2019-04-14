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

#ifndef SA_SHELLEXTENSION_H
#define SA_SHELLEXTENSION_H

#include "version.h"
#include "config.h"

#include <vector>

//Shell extension GUID
static const GUID SHELLANYTHING_SHELLEXTENSION_CLSID = { 0xb0d35103, 0x86a1, 0x471c, { 0xa6, 0x53, 0xe1, 0x30, 0xe3, 0x43, 0x9a, 0x3b } }; //this is the CLSID (GUID) or our Shell Extension, {B0D35103-86A1-471C-A653-E130E3439A3B}
static const char * ShellExtensionHandlerName = "ShellAnything";
static const char * ShellExtensionDescription = "ShellAnything Shell Extension";

class CCriticalSection
{
protected:
  CRITICAL_SECTION mCS;

public:
  CCriticalSection();
  virtual ~CCriticalSection();

  void enter();
  void leave();
};

class CCriticalSectionGuard
{
protected:
  CCriticalSection * mCS;

public:
  CCriticalSectionGuard(CCriticalSection * cs);
  ~CCriticalSectionGuard();
};

class CClassFactory : public IClassFactory
{
protected:
  ULONG m_cRef;

public:
  CClassFactory();
  ~CClassFactory();

  //IUnknown interface
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, LPVOID FAR *);
  ULONG   STDMETHODCALLTYPE AddRef();
  ULONG   STDMETHODCALLTYPE Release();

  //IClassFactory interface
  HRESULT STDMETHODCALLTYPE CreateInstance(LPUNKNOWN, REFIID, LPVOID FAR *);
  HRESULT STDMETHODCALLTYPE LockServer(BOOL);
};

// Prototype de notre interface IContextMenu:
class CContextMenu : public IContextMenu, IShellExtInit
{
protected:
  typedef std::vector<std::string> StringVector;
  struct CustomMenu
  {
    std::string title;
    UINT command_id;
    UINT insert_pos;
  };
  typedef std::vector<CustomMenu> CustomMenuVector;
  ULONG             m_cRef;
  CCriticalSection  m_CS; //protects class members
  LPDATAOBJECT      m_pDataObj;
  StringVector      m_SelectedItems;
  CustomMenuVector  m_Menus;
  bool              m_IsBackGround;

public:
  CContextMenu();
  ~CContextMenu();

  //IUnknown interface
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, LPVOID FAR *);
  ULONG   STDMETHODCALLTYPE AddRef();
  ULONG   STDMETHODCALLTYPE Release();

  //IContextMenu interface
  HRESULT STDMETHODCALLTYPE QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
  HRESULT STDMETHODCALLTYPE InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi);
  HRESULT STDMETHODCALLTYPE GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT FAR *reserved, LPSTR pszName, UINT cchMax);

  //IShellExtInit interface
  HRESULT STDMETHODCALLTYPE Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hKeyID);
};

#endif //SA_SHELLEXTENSION_H
