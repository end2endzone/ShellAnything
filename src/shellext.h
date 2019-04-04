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

#pragma once

#include "extToolsShellExports.h"
#include "ShellMenu.h"

//Shell extension GUID
static const char * CLSID_ShellExtensionStr = "{319D65C7-4D2D-42E9-9EC6-83C1A799CA57}";
static const GUID CLSID_ShellExtension = { 0X319D65C7, 0X4D2D, 0X42E9, { 0X9E, 0XC6, 0X83, 0XC1, 0XA7, 0X99, 0XCA, 0X57 } };
static const char * ShellExtensionName = "extToolsShell";
static const char * ShellExtensionDescription = "extTools Shell extension";

using namespace EXTLIB;

// Prototype de notre interface IClassFactory:
class CClassFactory : public IClassFactory
{
protected:
  ULONG m_cRef;

public:
  // Constructeur et destructeur:
  CClassFactory();
  ~CClassFactory();

  //Méthodes de l'interface IUnknown:
  HRESULT STDMETHODCALLTYPE	QueryInterface(REFIID, LPVOID FAR *);
  ULONG STDMETHODCALLTYPE		AddRef();
  ULONG STDMETHODCALLTYPE		Release();

  // Méthodes de l'interface IClassFactory:
  HRESULT STDMETHODCALLTYPE	CreateInstance(LPUNKNOWN, REFIID, LPVOID FAR *);
  HRESULT STDMETHODCALLTYPE	LockServer(BOOL);
};

// Prototype de notre interface IContextMenu:
class EXTTOOLSSHELL_API CContextMenu : public IContextMenu, IShellExtInit
{
protected:
  ULONG           m_cRef;
  LPDATAOBJECT    m_pDataObj;
  extListString   mSelectedItems;
  bool            mIsBackGround;
  ShellMenu       mMenus;

public:
  // Constructeur et destructeur:
  CContextMenu();
  ~CContextMenu();

  //Méthodes de l'interface IUnknown:
  HRESULT STDMETHODCALLTYPE	QueryInterface(REFIID, LPVOID FAR *);
  ULONG STDMETHODCALLTYPE		AddRef();
  ULONG STDMETHODCALLTYPE		Release();

  // Méthodes de l'interface IContextMenu:
  HRESULT STDMETHODCALLTYPE	QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
  HRESULT STDMETHODCALLTYPE	InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi);
  HRESULT STDMETHODCALLTYPE	GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT FAR *reserved, LPSTR pszName, UINT cchMax);

  // Méthodes de l'interface IShellExtInit:
  HRESULT STDMETHODCALLTYPE	Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hKeyID);
};

#endif //SA_SHELLEXTENSION_H
