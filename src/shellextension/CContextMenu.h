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

#include "CCriticalSection.h"
#include "BitmapCache.h"
#include "SelectionContext.h"
#include "Menu.h"
#include "Icon.h"

#include <vector>
#include <map>

class CContextMenu : public IContextMenu, IShellExtInit
{
public:
  typedef std::map<std::string /*fileextension*/, shellanything::Icon> IconMap;

protected:
  CCriticalSection            m_CS; //protects class members
  ULONG                       m_refCount;
  UINT                        m_FirstCommandId;
  bool                        m_IsBackGround;
  int                         m_BuildMenuTreeCount; //number of times that BuildMenuTree() was called
  shellanything::BitmapCache  m_BitmapCache;
  IconMap                     m_FileExtensionCache;
  shellanything::SelectionContext      m_Context;

  static HMENU m_previousMenu;

  ~CContextMenu();

public:
  CContextMenu();

  //IUnknown interface
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, LPVOID FAR*);
  ULONG   STDMETHODCALLTYPE AddRef();
  ULONG   STDMETHODCALLTYPE Release();

  //IContextMenu interface
  HRESULT STDMETHODCALLTYPE QueryContextMenu(HMENU hMenu, UINT menu_index, UINT first_command_id, UINT max_command_id, UINT flags);
  HRESULT STDMETHODCALLTYPE InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi);
  HRESULT STDMETHODCALLTYPE GetCommandString(UINT_PTR command_id, UINT flags, UINT FAR* reserved, LPSTR pszName, UINT cchMax);

  //IShellExtInit interface
  HRESULT STDMETHODCALLTYPE Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hKeyID);

private:
  void BuildMenuTree(HMENU hMenu);
  void BuildMenuTree(HMENU hMenu, shellanything::Menu* menu, UINT& insert_pos, bool& next_menu_is_column);
};
