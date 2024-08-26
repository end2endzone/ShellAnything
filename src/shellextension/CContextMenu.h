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

#pragma once

#include "stdafx.h"

#include <shlobj.h>
#include <shlguid.h>
#include <comdef.h>
#include <atlconv.h>	// for ATL string conversion macros
#include "resource.h"   // main symbols

// Generated files
#include "shellext.h"
#include "shellext_i.c"

#include "CCriticalSection.h"
#include "BitmapCache.h"
#include "SelectionContext.h"
#include "Menu.h"
#include "Icon.h"

#include <vector>
#include <map>

class ATL_NO_VTABLE CContextMenu :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CContextMenu, &CLSID_ShellAnything>,
	public IDispatchImpl<IShellAnything, &IID_IShellAnything, &LIBID_SHELLEXTENSIONLib>,
	public IShellExtInit,
	public IContextMenu
{
public:
  typedef std::map<std::string /*fileextension*/, shellanything::Icon> IconMap;

public:
  CContextMenu();
  ~CContextMenu();

  DECLARE_REGISTRY_RESOURCEID(IDR_SHELLANYTHING)

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  BEGIN_COM_MAP(CContextMenu)
    COM_INTERFACE_ENTRY(IShellAnything)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IShellExtInit)
    COM_INTERFACE_ENTRY(IContextMenu)
  END_COM_MAP()

  //IContextMenu interface
  HRESULT STDMETHODCALLTYPE QueryContextMenu(HMENU hMenu, UINT menu_index, UINT first_command_id, UINT max_command_id, UINT flags);
  HRESULT STDMETHODCALLTYPE InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi);
  HRESULT STDMETHODCALLTYPE GetCommandString(UINT_PTR command_id, UINT flags, UINT FAR* reserved, LPSTR pszName, UINT cchMax);

  //IShellExtInit interface
  HRESULT STDMETHODCALLTYPE Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hKeyID);

private:
  void BuildTopMenuTree(HMENU hMenu);
  void BuildSubMenuTree(HMENU hMenu, shellanything::Menu* menu, UINT& insert_pos, bool& next_menu_is_column);
  void PrintVerboseMenuStructure() const;

  CCriticalSection            m_CS; //protects class members
  ULONG                       m_refCount;
  UINT                        m_FirstCommandId;
  bool                        m_IsBackGround;
  int                         m_BuildMenuTreeCount; //number of times that BuildMenuTree() was called
  shellanything::BitmapCache  m_BitmapCache;
  IconMap                     m_FileExtensionCache;
  static HMENU                m_previousMenu; // issue #6. Field must be static
  shellanything::SelectionContext      m_Context;
};
