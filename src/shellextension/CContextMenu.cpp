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

#include "CContextMenu.h"
#include "shellext.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

#include "ErrorManager.h"
#include "Win32Registry.h"
#include "Win32Utils.h"
#include "GlogUtils.h"
#include "Unicode.h"

#include "PropertyManager.h"
#include "ConfigManager.h"
#include "PropertyManager.h"
#include "SaUtils.h"

#include "rapidassist/undef_windows_macros.h"
#include "rapidassist/strings.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/process_utf8.h"
#include "rapidassist/errors.h"
#include "rapidassist/user_utf8.h"
#include "rapidassist/unicode.h"
#include "rapidassist/environment.h"

static const GUID CLSID_UNDOCUMENTED_01 = { 0x924502a7, 0xcc8e, 0x4f60, { 0xae, 0x1f, 0xf7, 0x0c, 0x0a, 0x2b, 0x7a, 0x7c } };

void CContextMenu::BuildMenuTree(HMENU hMenu, shellanything::Menu* menu, UINT& insert_pos, bool& next_menu_is_column)
{
  //Expanded the menu's strings
  shellanything::PropertyManager& pmgr = shellanything::PropertyManager::GetInstance();
  std::string title = pmgr.Expand(menu->GetName());
  std::string description = pmgr.Expand(menu->GetDescription());

  //Get visible/enable properties based on current context.
  bool menu_visible = menu->IsVisible();
  bool menu_enabled = menu->IsEnabled();
  bool menu_separator = menu->IsSeparator();
  bool menu_column = menu->IsColumnSeparator();

  //Skip column separator, those are not menu item
  if (menu_column)
  {
    next_menu_is_column = true;
    return;
  }

  //Skip this menu if not visible
  if (!menu_visible)
  {
    menu->TruncateName(title);
    LOG(INFO) << __FUNCTION__ << "(), skipped menu '" << title << "', not visible.";
    return;
  }

  //Validate menus integrity
  const uint32_t& menu_command_id = menu->GetCommandId();
  if (menu_command_id == shellanything::Menu::INVALID_COMMAND_ID)
  {
    menu->TruncateName(title);
    LOG(ERROR) << __FUNCTION__ << "(), menu '" << title << "' have invalid command id.";
    return;
  }

  // Truncate if required, issue #55.
  menu->TruncateName(title);
  menu->TruncateName(description);

  //convert to windows unicode...
  std::wstring title_utf16 = ra::unicode::Utf8ToUnicode(title);
  std::wstring desc_utf16 = ra::unicode::Utf8ToUnicode(description);

  MENUITEMINFOW menuinfo = { 0 };

  menuinfo.cbSize = sizeof(MENUITEMINFOW);
  menuinfo.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_STRING;
  menuinfo.fType = (menu_separator ? MFT_SEPARATOR : MFT_STRING);
  menuinfo.fType += (next_menu_is_column ? MFT_MENUBARBREAK : 0);
  menuinfo.fState = (menu_enabled ? MFS_ENABLED : MFS_DISABLED);
  menuinfo.wID = menu_command_id;
  menuinfo.dwTypeData = (wchar_t*)title_utf16.c_str();
  menuinfo.cch = (UINT)title_utf16.size();

  //add an icon
  const shellanything::Icon& icon = menu->GetIcon();
  if (!menu_separator && icon.IsValid())
  {
    shellanything::PropertyManager& pmgr = shellanything::PropertyManager::GetInstance();
    std::string file_extension = pmgr.Expand(icon.GetFileExtension());
    std::string icon_filename = pmgr.Expand(icon.GetPath());
    int icon_index = icon.GetIndex();

    //if the icon is pointing to a file extension
    if (!file_extension.empty())
    {
      //resolve the file extension to a system icon.

      //did we already resolved this icon?
      IconMap::iterator wExtensionsIterator = m_FileExtensionCache.find(file_extension);
      bool found = (wExtensionsIterator != m_FileExtensionCache.end());
      if (found)
      {
        //already resolved
        const shellanything::Icon& resolved_icon = wExtensionsIterator->second;
        icon_filename = resolved_icon.GetPath();
        icon_index = resolved_icon.GetIndex();
      }
      else
      {
        //not found

        //make a copy of the icon and resolve the file extension to a system icon.
        shellanything::Icon resolved_icon = icon;
        resolved_icon.ResolveFileExtensionIcon();

        //save the icon for a future use
        m_FileExtensionCache[file_extension] = resolved_icon;

        //use the resolved icon location
        icon_filename = resolved_icon.GetPath();
        icon_index = resolved_icon.GetIndex();
      }
    }

    //ask the cache for an existing icon.
    //this will identify the icon in the cache as "used" or "active".
    HBITMAP hBitmap = m_BitmapCache.FindHandle(icon_filename, icon_index);

    //if nothing in cache, create a new one
    if (hBitmap == shellanything::BitmapCache::INVALID_BITMAP_HANDLE && !icon_filename.empty())
    {
      // #117 - ExtractIconEx() behavior is different when the monitor is scaled. A small and a large icon
      // cannot always be extracted when scaling factor is greater than 1.0. The solution is to only extract a small icon.
      // Small icons matches the size of a menu icon (16x16). A "large" (or larger) icon is not necessary.
      // The size of a small icon is scaled to match the monitor scaling. For 300% scaling, menu icons are 48x48.
      HICON hIconSmall = NULL;

      std::wstring icon_filename_wide = ra::unicode::Utf8ToUnicode(icon_filename);

      //check how many icons the file contains
      UINT num_icon_in_file = ExtractIconExW(icon_filename_wide.c_str(), -1, NULL, NULL, 1);
      if (num_icon_in_file == 0)
        LOG(WARNING) << __FUNCTION__ << "(), File '" << icon_filename << "' does not contains an icon.";
      else
      {
        //the file contains 1 or more icons, try to load a small one
        UINT num_icon_loaded = 0;
        if (num_icon_in_file >= 1)
          num_icon_loaded = ExtractIconExW(icon_filename_wide.c_str(), icon_index, NULL, &hIconSmall, 1);
        if (num_icon_in_file >= 1 && num_icon_loaded == 0)
          LOG(WARNING) << __FUNCTION__ << "(), Failed to load icon index " << icon_index << " from file '" << icon_filename << "'.";
        else
        {
          SIZE menu_icon_size = Win32Utils::GetIconSize(hIconSmall);
          // LOG(INFO) << __FUNCTION__ << "(), Loaded icon " << icon_index << " from file '" << icon_filename << "' is " << menu_icon_size.cx << "x" << menu_icon_size.cy << ".";

          //Convert the icon to a 32bpp bitmap with alpha channel (invisible background)
          hBitmap = Win32Utils::CopyAsBitmap(hIconSmall);
          if (hBitmap == shellanything::BitmapCache::INVALID_BITMAP_HANDLE)
            LOG(ERROR) << __FUNCTION__ << "(), Icon " << icon_index << " from file '" << icon_filename << "' has failed to convert to bitmap.";

          if (hIconSmall != NULL)
            DestroyIcon(hIconSmall);

          //add the bitmap to the cache for future use
          if (hBitmap != shellanything::BitmapCache::INVALID_BITMAP_HANDLE)
            m_BitmapCache.AddHandle(icon_filename.c_str(), icon_index, hBitmap);
        }
      }
    }

    //if a bitmap is created
    if (hBitmap != shellanything::BitmapCache::INVALID_BITMAP_HANDLE)
    {
      //enable bitmap handling for the menu
      menuinfo.fMask |= MIIM_BITMAP;

      //assign the HBITMAP to the HMENU
      menuinfo.hbmpItem = hBitmap;
    }
  }

  //handle column separator
  if (next_menu_is_column)
  {
    menuinfo.fType |= MFT_MENUBARBREAK;
  }
  next_menu_is_column = false;

  //handle submenus
  if (menu->IsParentMenu())
  {
    menuinfo.fMask |= MIIM_SUBMENU;
    HMENU hSubMenu = CreatePopupMenu();

    bool next_sub_menu_is_column = false;

    shellanything::Menu::MenuPtrList subs = menu->GetSubMenus();
    UINT sub_insert_pos = 0;
    for (size_t i = 0; i < subs.size(); i++)
    {
      shellanything::Menu* submenu = subs[i];
      BuildMenuTree(hSubMenu, submenu, sub_insert_pos, next_sub_menu_is_column);
    }

    menuinfo.hSubMenu = hSubMenu;
  }

  BOOL result = InsertMenuItemW(hMenu, insert_pos, TRUE, &menuinfo);
  insert_pos++; //next menu is below this one

  LOG(INFO) << __FUNCTION__ << "(), insert.pos=" << ra::strings::Format("%03d", insert_pos) << ", id=" << ra::strings::Format("%06d", menuinfo.wID) << ", result=" << result << ", title=" << title;
}

void CContextMenu::BuildMenuTree(HMENU hMenu)
{
  //Bitmap ressources must be properly destroyed.
  //When a menu (HMENU handle) is destroyed using win32 DestroyMenu() function, it also destroy the child menus:
  //https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-destroymenu
  //
  //However the bitmap assigned to menus are not deleted with DestroyMenu() function.
  //Bitmap is a limited resource. If you ran out of GDI resources, you may see black menus 
  //and Windows Explorer will have difficulties to render all the window. For details, see
  //https://www.codeproject.com/Questions/1228261/Windows-shell-extension
  //
  //To prevent running out of bitmap ressource we use the shellanything::BitmapCache class.
  //Each bitmap is identified as 'used' in CContextMenu::BuildMenuTree() with 'm_BitmapCache.FindHandle()'.
  //Every 5 times the shell extension popup is displayed, we look for 'unused' bitmap and delete them.
  //

  //handle destruction of old bitmap in the cache
  m_BuildMenuTreeCount++;
  if (m_BuildMenuTreeCount > 0 && (m_BuildMenuTreeCount % 5) == 0)
  {
    //every 10 calls, refresh the cache
    m_BitmapCache.DestroyOldHandles();

    //reset counters
    m_BitmapCache.ResetCounters();
  }

  bool next_menu_is_column = false;

  //browse through all shellanything menus and build the win32 popup menus

  //for each configuration
  shellanything::ConfigManager& cmgr = shellanything::ConfigManager::GetInstance();
  shellanything::Configuration::ConfigurationPtrList configs = cmgr.GetConfigurations();
  UINT insert_pos = 0;
  for (size_t i = 0; i < configs.size(); i++)
  {
    shellanything::Configuration* config = configs[i];
    if (config)
    {
      //for each menu child
      shellanything::Menu::MenuPtrList menus = config->GetMenus();
      for (size_t j = 0; j < menus.size(); j++)
      {
        shellanything::Menu* menu = menus[j];

        //Add this menu to the tree
        BuildMenuTree(hMenu, menu, insert_pos, next_menu_is_column);
      }
    }
  }
}

CContextMenu::CContextMenu()
{
  LOG(INFO) << __FUNCTION__ << "(), new instance " << ToHexString(this);

#if SA_QUERYINTERFACE_IMPL == 0
  m_refCount = 0; // reference counter must be initialized to 0 even if we are actually creating an instance. A reference to this instance will be added when the instance will be queried by explorer.exe.
#elif SA_QUERYINTERFACE_IMPL == 1
  m_refCount = 1;
#endif

  m_FirstCommandId = 0;
  m_IsBackGround = false;
  m_BuildMenuTreeCount = 0;
  m_previousMenu = 0;

  // Increment the dll's reference counter.
  DllAddRef();
}

CContextMenu::~CContextMenu()
{
  LOG(INFO) << __FUNCTION__ << "(), delete instance " << ToHexString(this);

  // Decrement the dll's reference counter.
  DllRelease();
}

HRESULT STDMETHODCALLTYPE CContextMenu::QueryContextMenu(HMENU hMenu, UINT menu_index, UINT first_command_id, UINT max_command_id, UINT flags)
{
  std::string flags_str = GetQueryContextMenuFlags(flags);
  std::string flags_hex = ra::strings::Format("0x%08x", flags);

  LOG(INFO) << __FUNCTION__ << "(), hMenu=0x" << ra::strings::Format("0x%08x", hMenu).c_str() << ",count=" << GetMenuItemCount(hMenu) << ", menu_index=" << menu_index << ", first_command_id=" << first_command_id << ", max_command_id=" << max_command_id << ", flags=" << flags_hex << "=(" << flags_str << ")" << " this=" << ToHexString(this);

  //https://docs.microsoft.com/en-us/windows/desktop/shell/how-to-implement-the-icontextmenu-interface

  //From this point, it is safe to use class members without other threads interference
  CCriticalSectionGuard cs_guard(&m_CS);

  //Note on flags...
  //Right-click on a file or directory with Windows Explorer on the right area:  flags=0x00020494=132244(dec)=(CMF_NORMAL|CMF_EXPLORE|CMF_CANRENAME|CMF_ITEMMENU|CMF_ASYNCVERBSTATE)
  //Right-click on the empty area      with Windows Explorer on the right area:  flags=0x00020424=132132(dec)=(CMF_NORMAL|CMF_EXPLORE|CMF_NODEFAULT|CMF_ASYNCVERBSTATE)
  //Right-click on a directory         with Windows Explorer on the left area:   flags=0x00000414=001044(dec)=(CMF_NORMAL|CMF_EXPLORE|CMF_CANRENAME|CMF_ASYNCVERBSTATE)
  //Right-click on a drive             with Windows Explorer on the left area:   flags=0x00000414=001044(dec)=(CMF_NORMAL|CMF_EXPLORE|CMF_CANRENAME|CMF_ASYNCVERBSTATE)
  //Right-click on the empty area      on the Desktop:                           flags=0x00020420=132128(dec)=(CMF_NORMAL|CMF_NODEFAULT|CMF_ASYNCVERBSTATE)
  //Right-click on a directory         on the Desktop:                           flags=0x00020490=132240(dec)=(CMF_NORMAL|CMF_CANRENAME|CMF_ITEMMENU|CMF_ASYNCVERBSTATE)

  //Filter out queries that have nothing selected.
  //This can happend if user is copy & pasting files (using CTRL+C and CTRL+V)
  //and if the shell extension is registered as a DragDropHandlers.
  if (m_Context.GetElements().size() == 0)
  {
    //Don't know what to do with this
    LOG(INFO) << __FUNCTION__ << "(), skipped, nothing is selected.";
    return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0); //nothing inserted
  }

  //Filter out queries that are called twice for the same directory.
  if (hMenu == m_previousMenu)
  {
    //Issue  #6 - Right-click on a directory with Windows Explorer in the left panel shows the menus twice.
    //Issue #31 - Error in logs for CContextMenu::GetCommandString().
    //Using a static variable is a poor method for solving the issue but it is a "good enough" strategy.
    LOG(INFO) << __FUNCTION__ << "(), skipped, QueryContextMenu() called twice and menu is already populated once.";
    return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0); //nothing inserted
  }

  //Remember current menu to prevent issues calling twice QueryContextMenu()
  m_previousMenu = hMenu;

  //Log what is selected by the user
  const shellanything::StringList& elements = m_Context.GetElements();
  size_t num_selected_total = elements.size();
  int num_files = m_Context.GetNumFiles();
  int num_directories = m_Context.GetNumDirectories();
  LOG(INFO) << __FUNCTION__ << "(), SelectionContext have " << num_selected_total << " element(s): " << num_files << " files and " << num_directories << " directories.";

  //Keep a reference the our first command id. We will need it when InvokeCommand is called.
  m_FirstCommandId = first_command_id;

  //Refresh the list of loaded configuration files
  shellanything::ConfigManager& cmgr = shellanything::ConfigManager::GetInstance();
  cmgr.Refresh();

  //Update all menus with the new context
  //This will refresh the visibility flags which is required before calling ConfigManager::AssignCommandIds()
  cmgr.Update(m_Context);

  //Assign unique command id to visible menus. Issue #5
  UINT next_command_id = cmgr.AssignCommandIds(first_command_id);

  //Build the menus
  BuildMenuTree(hMenu);

  //Log information about menu statistics.
  UINT menu_last_command_id = (UINT)-1; //confirmed last command id
  if (next_command_id != first_command_id)
    menu_last_command_id = next_command_id - 1;
  UINT num_menu_items = next_command_id - first_command_id;
  LOG(INFO) << __FUNCTION__ << "(), Menu: first_command_id=" << first_command_id << " menu_last_command_id=" << menu_last_command_id << " next_command_id=" << next_command_id << " num_menu_items=" << num_menu_items << ".\n";

  //debug the constructed menu tree
#ifdef _DEBUG
  std::string menu_tree = Win32Utils::GetMenuTree(hMenu, 2);
  LOG(INFO) << __FUNCTION__ << "(), Menu tree:\n" << menu_tree.c_str();
#endif

  HRESULT hr = MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, num_menu_items);
  return hr;
}

HRESULT STDMETHODCALLTYPE CContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO pici)
{
  //define the type of structure pointed by pici
  const char* struct_name = "UNKNOWN";
  if (pici->cbSize == sizeof(CMINVOKECOMMANDINFO))
    struct_name = "CMINVOKECOMMANDINFO";
  else if (pici->cbSize == sizeof(CMINVOKECOMMANDINFOEX))
    struct_name = "CMINVOKECOMMANDINFOEX";

  //define how we should interpret pici->lpVerb
  std::string verb;
  if (IS_INTRESOURCE(pici->lpVerb))
  {
    // D:\Projects\ShellAnything\src\shellext.cpp(632) : warning C4311 : 'reinterpret_cast' : pointer truncation from 'LPCSTR' to 'int'
    // D:\Projects\ShellAnything\src\shellext.cpp(632) : warning C4302 : 'reinterpret_cast' : truncation from 'LPCSTR' to 'int'
#pragma warning( push )
#pragma warning( disable: 4302 )
#pragma warning( disable: 4311 )
    verb = ra::strings::ToString(reinterpret_cast<int>(pici->lpVerb));
#pragma warning( pop )
  }
  else
    verb = pici->lpVerb;

  LOG(INFO) << __FUNCTION__ << "(), pici->cbSize=" << struct_name << ", pici->fMask=" << pici->fMask << ", pici->lpVerb=" << verb << " this=" << ToHexString(this);

  //validate
  if (!IS_INTRESOURCE(pici->lpVerb))
    return E_INVALIDARG; //don't know what to do with pici->lpVerb

  UINT target_command_offset = LOWORD(pici->lpVerb); //matches the command_id offset (command id of the selected menu - command id of the first menu)
  UINT target_command_id = m_FirstCommandId + target_command_offset;

  //From this point, it is safe to use class members without other threads interference
  CCriticalSectionGuard cs_guard(&m_CS);

  //find the menu that is requested
  shellanything::ConfigManager& cmgr = shellanything::ConfigManager::GetInstance();
  shellanything::Menu* menu = cmgr.FindMenuByCommandId(target_command_id);
  if (menu == NULL)
  {
    LOG(ERROR) << __FUNCTION__ << "(), unknown menu for pici->lpVerb=" << verb;
    return E_INVALIDARG;
  }

  //compute the visual menu title
  shellanything::PropertyManager& pmgr = shellanything::PropertyManager::GetInstance();
  std::string title = pmgr.Expand(menu->GetName());

  //found a menu match, execute menu action
  LOG(INFO) << __FUNCTION__ << "(), executing action(s) for menu '" << title.c_str() << "'...";

  //execute actions
  const shellanything::IAction::ActionPtrList& actions = menu->GetActions();
  for (size_t i = 0; i < actions.size(); i++)
  {
    LOG(INFO) << __FUNCTION__ << "(), executing action " << (i + 1) << " of " << actions.size() << ".";
    const shellanything::IAction* action = actions[i];
    if (action)
    {
      ra::errors::ResetLastErrorCode(); //reset win32 error code in case the action fails.
      bool success = action->Execute(m_Context);

      if (!success)
      {
        //try to get an error message from win32
        ra::errors::errorcode_t dwError = ra::errors::GetLastErrorCode();
        if (dwError)
        {
          std::string error_message = ra::errors::GetErrorCodeDescription(dwError);
          LOG(ERROR) << __FUNCTION__ << "(), action #" << (i + 1) << " has failed: " << error_message;
        }
        else
        {
          //simply log an error
          LOG(ERROR) << __FUNCTION__ << "(), action #" << (i + 1) << " has failed.";
        }

        //stop executing the next actions
        i = actions.size();
      }
    }
  }

  LOG(INFO) << __FUNCTION__ << "(), executing action(s) for menu '" << title.c_str() << "' completed.";
  return S_OK;
}

HRESULT STDMETHODCALLTYPE CContextMenu::GetCommandString(UINT_PTR command_id, UINT flags, UINT FAR* reserved, LPSTR pszName, UINT cchMax)
{
  std::string flags_str = GetGetCommandStringFlags(flags);
  std::string flags_hex = ra::strings::Format("0x%08x", flags);

  // only show this log in verbose mode
  //LOG(INFO) << __FUNCTION__ << "(), command_id=" << command_id << ", cchMax=" << cchMax << " this=" << ToHexString(this) << ", flags=" << flags_hex << ":" << flags_str;

  UINT target_command_offset = (UINT)command_id; //matches the command_id offset (command id of the selected menu substracted by command id of the first menu)
  UINT target_command_id = m_FirstCommandId + target_command_offset;

  //From this point, it is safe to use class members without other threads interference
  CCriticalSectionGuard cs_guard(&m_CS);

  //find the menu that is requested
  shellanything::ConfigManager& cmgr = shellanything::ConfigManager::GetInstance();
  shellanything::Menu* menu = cmgr.FindMenuByCommandId(target_command_id);
  if (menu == NULL)
  {
    LOG(ERROR) << __FUNCTION__ << "(), unknown menu for command_id=" << target_command_offset << " m_FirstCommandId=" << m_FirstCommandId << " target_command_id=" << target_command_id;
    return E_INVALIDARG;
  }

  //compute the visual menu description
  shellanything::PropertyManager& pmgr = shellanything::PropertyManager::GetInstance();
  std::string description = pmgr.Expand(menu->GetDescription());

  //convert to windows unicode...
  std::wstring desc_utf16 = ra::unicode::Utf8ToUnicode(description);
  std::string  desc_ansi = ra::unicode::Utf8ToAnsi(description);

  //Build up tooltip string
  switch (flags)
  {
  case GCS_HELPTEXTA:
  {
    //ANIS tooltip handling
    lstrcpynA(pszName, desc_ansi.c_str(), cchMax);
    return S_OK;
  }
  break;
  case GCS_HELPTEXTW:
  {
    //UNICODE tooltip handling
    lstrcpynW((LPWSTR)pszName, desc_utf16.c_str(), cchMax);
    return S_OK;
  }
  break;
  case GCS_VERBA:
  {
    //ANIS tooltip handling
    lstrcpynA(pszName, desc_ansi.c_str(), cchMax);
    return S_OK;
  }
  break;
  case GCS_VERBW:
  {
    //UNICODE tooltip handling
    lstrcpynW((LPWSTR)pszName, desc_utf16.c_str(), cchMax);
    return S_OK;
  }
  break;
  case GCS_VALIDATEA:
  case GCS_VALIDATEW:
  {
    return S_OK;
  }
  break;
  }

  LOG(ERROR) << __FUNCTION__ << "(), unknown flags: " << flags;
  return S_FALSE;
}

HRESULT STDMETHODCALLTYPE CContextMenu::Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hRegKey)
{
  LOG(INFO) << __FUNCTION__ << "(), pIDFolder=" << (void*)pIDFolder << " this=" << ToHexString(this);

  //From this point, it is safe to use class members without other threads interference
  CCriticalSectionGuard cs_guard(&m_CS);

  shellanything::StringList files;

  // Cleanup
  m_Context.UnregisterProperties(); //Unregister the previous context properties
  m_Context.SetElements(files);
  m_IsBackGround = false;

  // Did we clicked on a folder's background or the desktop directory?
  if (pIDFolder)
  {
    LOG(INFO) << __FUNCTION__ << "(), User right-clicked on a background directory.";

    wchar_t szPath[2 * MAX_PATH] = { 0 };

    if (SHGetPathFromIDListW(pIDFolder, szPath))
    {
      if (szPath[0] != '\0')
      {
        std::string path_utf8 = ra::unicode::UnicodeToUtf8(szPath);
        LOG(INFO) << __FUNCTION__ << "(), Found directory '" << path_utf8 << "'.";
        m_IsBackGround = true;
        files.push_back(path_utf8);
      }
      else
      {
        LOG(WARNING) << __FUNCTION__ << "(), found empty path in pIDFolder.";
        return E_INVALIDARG;
      }
    }
    else
    {
      LOG(ERROR) << __FUNCTION__ << "(), SHGetPathFromIDList() has failed.";
      return E_INVALIDARG;
    }
  }

  // User clicked on one or more file or directory
  else if (pDataObj)
  {
    LOG(INFO) << __FUNCTION__ << "(), User right-clicked on selected files/directories.";

    FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stg = { TYMED_HGLOBAL };
    HDROP hDropInfo;

    // The selected files are expected to be in HDROP format.
    if (FAILED(pDataObj->GetData(&fmt, &stg)))
    {
      LOG(WARNING) << __FUNCTION__ << "(), selected files are not in HDROP format.";
      return E_INVALIDARG;
    }

    // Get a locked pointer to the files data.
    hDropInfo = (HDROP)GlobalLock(stg.hGlobal);
    if (NULL == hDropInfo)
    {
      ReleaseStgMedium(&stg);
      LOG(ERROR) << __FUNCTION__ << "(), failed to get lock on selected files.";
      return E_INVALIDARG;
    }

    UINT num_files = DragQueryFileW(hDropInfo, 0xFFFFFFFF, NULL, 0);
    LOG(INFO) << __FUNCTION__ << "(), User right-clicked on " << num_files << " files/directories.";

    // For each files
    for (UINT i = 0; i < num_files; i++)
    {
      UINT length = DragQueryFileW(hDropInfo, i, NULL, 0);

      // Allocate a temporary buffer
      std::wstring path(length, '\0');
      if (path.size() != length)
        continue;
      size_t num_characters = length + 1;

      // Copy the element into the temporary buffer
      DragQueryFileW(hDropInfo, i, (wchar_t*)path.data(), (UINT)num_characters);

      //add the new file
      std::string path_utf8 = ra::unicode::UnicodeToUtf8(path);
      LOG(INFO) << __FUNCTION__ << "(), Found file/directory #" << ra::strings::Format("%03d", i) << ": '" << path_utf8 << "'.";
      files.push_back(path_utf8);
    }
    GlobalUnlock(stg.hGlobal);
    ReleaseStgMedium(&stg);
  }

  //update the selection context
  m_Context.SetElements(files);

  //Register the current context properties so that menus can display the right caption
  m_Context.RegisterProperties();

  return S_OK;
}

HRESULT STDMETHODCALLTYPE CContextMenu::QueryInterface(REFIID riid, LPVOID FAR* ppv)
{
  std::string riid_str = GuidToInterfaceName(riid);
  LOG(INFO) << __FUNCTION__ << "(), riid=" << riid_str << ", this=" << ToHexString(this);

  HRESULT hr = E_NOINTERFACE;

#if SA_QUERYINTERFACE_IMPL == 0
  //https://docs.microsoft.com/en-us/office/client-developer/outlook/mapi/implementing-iunknown-in-c-plus-plus

  // Always set out parameter to NULL, validating it first.
  if (!ppv)
    return E_INVALIDARG;
  *ppv = NULL;

  ////Filter out unimplemented known interfaces so they do not show as WARNINGS
  //if (IsEqualGUID(riid, IID_IObjectWithSite) ||           //{FC4801A3-2BA9-11CF-A229-00AA003D7352}
  //    IsEqualGUID(riid, IID_IInternetSecurityManager) ||  //{79EAC9EE-BAF9-11CE-8C82-00AA004BA90B}
  //    IsEqualGUID(riid, IID_IContextMenu2) ||             //{000214f4-0000-0000-c000-000000000046}
  //    IsEqualGUID(riid, IID_IContextMenu3) ||             //{BCFCE0A0-EC17-11d0-8D10-00A0C90F2719}
  //    IsEqualGUID(riid, CLSID_UNDOCUMENTED_01)
  //    )
  //{
  //  LOG(INFO) << __FUNCTION__ << "(), interface not supported " << riid_str;
  //  return E_NOINTERFACE;
  //}

  //https://stackoverflow.com/questions/1742848/why-exactly-do-i-need-an-explicit-upcast-when-implementing-queryinterface-in-a
  if (IsEqualGUID(riid, IID_IUnknown))        *ppv = (LPVOID)this;
  if (IsEqualGUID(riid, IID_IShellExtInit))   *ppv = (LPSHELLEXTINIT)this;
  if (IsEqualGUID(riid, IID_IContextMenu))    *ppv = (LPCONTEXTMENU)this;

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
    QITABENT(CContextMenu, IShellExtInit),
    QITABENT(CContextMenu, IContextMenu),
    { 0, 0 },
  };
  hr = QISearch(this, qit, riid, ppv);
#endif

  if (SUCCEEDED(hr))
    LOG(INFO) << __FUNCTION__ << "(), found interface " << riid_str << ", ppv=" << ToHexString(*ppv);
  else
    LOG(INFO) << __FUNCTION__ << "(), unknown interface " << riid_str;
  return hr;
}

ULONG STDMETHODCALLTYPE CContextMenu::AddRef()
{
  //https://docs.microsoft.com/en-us/office/client-developer/outlook/mapi/implementing-iunknown-in-c-plus-plus

  // Increment the object's internal counter.
  return InterlockedIncrement(&m_refCount);
}

ULONG STDMETHODCALLTYPE CContextMenu::Release()
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
