//#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include <initguid.h>
#include <shlguid.h>
#include <shlobj.h>
#include <cmnquery.h>
#include <dsquery.h>
#include <dsclient.h>

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

#include "ErrorManager.h"
#include "shellext.h"
#include "Win32Registry.h"
#include "Win32Utils.h"
#include "GlogUtils.h"
#include "Unicode.h"

#undef GetEnvironmentVariable
#undef DeleteFile
#undef CreateDirectory
#undef CopyFile
#undef CreateFile

#include "rapidassist/strings.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/process_utf8.h"
#include "rapidassist/errors.h"
#include "rapidassist/user_utf8.h"
#include "rapidassist/unicode.h"
#include "rapidassist/environment.h"

#include "shellanything/version.h"
#include "ConfigManager.h"
#include "PropertyManager.h"
#include "SaUtils.h"

#include <assert.h>

//Declarations
UINT      g_cRefDll = 0;            // Reference counter of this DLL
HINSTANCE g_hmodDll = 0;            // HINSTANCE of the DLL

static const GUID CLSID_UNDOCUMENTED_01 = { 0x924502a7, 0xcc8e, 0x4f60, { 0xae, 0x1f, 0xf7, 0x0c, 0x0a, 0x2b, 0x7a, 0x7c } };

HMENU CContextMenu::m_previousMenu = 0;

std::string GuidToString(GUID guid)
{
  std::string output;
  output.assign(40, 0);
  sprintf_s((char*)output.c_str(), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
  return output;
}

std::string GuidToInterfaceName(GUID guid)
{
  if (IsEqualGUID(guid, IID_IUnknown))                  return "IUnknown";                  //{00000000-0000-0000-C000-000000000046}
  if (IsEqualGUID(guid, IID_IClassFactory))             return "IClassFactory";             //{00000001-0000-0000-C000-000000000046}
  if (IsEqualGUID(guid, IID_IShellExtInit))             return "IShellExtInit";             //{000214E8-0000-0000-C000-000000000046}
  if (IsEqualGUID(guid, IID_IContextMenu))              return "IContextMenu";              //{000214E4-0000-0000-C000-000000000046}
  if (IsEqualGUID(guid, IID_IContextMenu2))             return "IContextMenu2";             //{000214F4-0000-0000-C000-000000000046}
  if (IsEqualGUID(guid, IID_IContextMenu3))             return "IContextMenu3";             //{BCFCE0A0-EC17-11D0-8D10-00A0C90F2719}
  if (IsEqualGUID(guid, IID_IObjectWithSite))           return "IObjectWithSite";           //{FC4801A3-2BA9-11CF-A229-00AA003D7352}
  if (IsEqualGUID(guid, IID_IInternetSecurityManager))  return "IInternetSecurityManager";  //{79EAC9EE-BAF9-11CE-8C82-00AA004BA90B}

  //unknown GUID, return the string representation:
  //ie: CLSID_UNDOCUMENTED_01, {924502A7-CC8E-4F60-AE1F-F70C0A2B7A7C}
  return GuidToString(guid);
}

/// <summary>
/// Returns true if the application is run for the first time.
/// Note, for Windows users, the implementation is based on registry keys in HKEY_CURRENT_USER\Software\name\version.
/// </summary>
/// <param name="name">The name of the application.</param>
/// <param name="version">The version of the application.</param>
/// <returns>Returns true if the application is run for the first time. Returns false otherwise.</returns>
bool IsFirstApplicationRun(const std::string & name, const std::string & version)
{
  std::string key = ra::strings::Format("HKEY_CURRENT_USER\\Software\\%s\\%s", name.c_str(), version.c_str());
  if (!Win32Registry::CreateKey(key.c_str(), NULL))
  {
    // unable to get to the application's key.
    // assume it is not the first run.
    return false;  
  }
 
  static const char * FIRST_RUN_VALUE_NAME = "first_run";
 
  // try to read the value
  Win32Registry::MemoryBuffer value;
  Win32Registry::REGISTRY_TYPE value_type;
  if (!Win32Registry::GetValue(key.c_str(), FIRST_RUN_VALUE_NAME, value_type, value))
  {
    // the registry value is not found.
    // assume the application is run for the first time.
 
    // update the flag to "false" for the next call
    Win32Registry::SetValue(key.c_str(), FIRST_RUN_VALUE_NAME, "false"); //don't look at the write result
 
    return true;
  }

  bool first_run = ra::strings::ParseBoolean(value);
 
  if (first_run)
  {
    //update the flag to "false"
    Win32Registry::SetValue(key.c_str(), FIRST_RUN_VALUE_NAME, "false"); //don't look at the write result
  }
 
  return first_run;
}

template <class T> class FlagDescriptor
{
public:
  struct FLAGS
  {
    T value;
    const char * name;
  };
 
  static std::string ToBitString(T value, const FLAGS * flags)
  {
    std::string desc;
 
    size_t index = 0;
    while(flags[index].name)
    {
      const T & flag = flags[index].value;
      const char * name = flags[index].name;
 
      //if flag is set
      if ((value & flag) == flag)
      {
        if (!desc.empty())
          desc.append("|");
        desc.append(name);
      }
 
      //next flag
      index++;
    }
    return desc;
  }

  static std::string ToValueString(T value, const FLAGS * flags)
  {
    std::string desc;
 
    size_t index = 0;
    while(flags[index].name)
    {
      const T & flag = flags[index].value;
      const char * name = flags[index].name;
 
      //if flag is set
      if (value == flag)
      {
        if (!desc.empty())
          desc.append(",");
        desc.append(name);
      }
 
      //next flag
      index++;
    }
    return desc;
  }
};

void CContextMenu::BuildMenuTree(HMENU hMenu, shellanything::Menu * menu, UINT & insert_pos, bool & next_menu_is_column)
{
  //Expanded the menu's strings
  shellanything::PropertyManager & pmgr = shellanything::PropertyManager::GetInstance();
  std::string title       = pmgr.Expand(menu->GetName());
  std::string description = pmgr.Expand(menu->GetDescription());

  //Get visible/enable properties based on current context.
  bool menu_visible   = menu->IsVisible();
  bool menu_enabled   = menu->IsEnabled();
  bool menu_separator = menu->IsSeparator();
  bool menu_column    = menu->IsColumnSeparator();

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
  const uint32_t & menu_command_id = menu->GetCommandId();
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
  std::wstring desc_utf16  = ra::unicode::Utf8ToUnicode(description);

  MENUITEMINFOW menuinfo = {0};

  menuinfo.cbSize = sizeof(MENUITEMINFOW);
  menuinfo.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_STRING;
  menuinfo.fType = (menu_separator ? MFT_SEPARATOR : MFT_STRING);
  menuinfo.fType += (next_menu_is_column ? MFT_MENUBARBREAK : 0);
  menuinfo.fState = (menu_enabled ? MFS_ENABLED : MFS_DISABLED);
  menuinfo.wID = menu_command_id;
  menuinfo.dwTypeData = (wchar_t*)title_utf16.c_str();
  menuinfo.cch = (UINT)title_utf16.size();

  //add an icon
  const shellanything::Icon & icon = menu->GetIcon();
  if (!menu_separator && icon.IsValid())
  {
    shellanything::PropertyManager & pmgr = shellanything::PropertyManager::GetInstance();
    std::string file_extension  = pmgr.Expand(icon.GetFileExtension());
    std::string icon_filename   = pmgr.Expand(icon.GetPath());
    int icon_index              = icon.GetIndex();

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
        const shellanything::Icon & resolved_icon = wExtensionsIterator->second;
        icon_filename = resolved_icon.GetPath();
        icon_index    = resolved_icon.GetIndex();
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
        icon_index    = resolved_icon.GetIndex();
      }
    }

    //ask the cache for an existing icon.
    //this will identify the icon in the cache as "used" or "active".
    HBITMAP hBitmap = m_BitmapCache.FindHandle(icon_filename, icon_index);

    //if nothing in cache, create a new one
    if (hBitmap == shellanything::BitmapCache::INVALID_BITMAP_HANDLE)
    {
      HICON hIconLarge = NULL;
      HICON hIconSmall = NULL;
      
      //
      std::wstring icon_filename_wide = ra::unicode::Utf8ToUnicode(icon_filename);
      UINT numIconInFile = ExtractIconExW( icon_filename_wide.c_str(), -1, NULL, NULL, 1 );
      UINT numIconLoaded = ExtractIconExW( icon_filename_wide.c_str(), icon_index, &hIconLarge, &hIconSmall, 1 );
      if (numIconLoaded >= 1)
      {
        //Find the best icon
        HICON hIcon = Win32Utils::GetBestIconForMenu(hIconLarge, hIconSmall);

        //Convert the icon to a bitmap (with invisible background)
        hBitmap = Win32Utils::CopyAsBitmap(hIcon);

        DestroyIcon(hIconLarge);
        DestroyIcon(hIconSmall);

        //add the bitmap to the cache for future use
        m_BitmapCache.AddHandle( icon_filename.c_str(), icon_index, hBitmap );
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
    for(size_t i=0; i<subs.size(); i++)
    {
      shellanything::Menu * submenu = subs[i];
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
  shellanything::ConfigManager & cmgr = shellanything::ConfigManager::GetInstance();
  shellanything::Configuration::ConfigurationPtrList configs = cmgr.GetConfigurations();
  UINT insert_pos = 0;
  for(size_t i=0; i<configs.size(); i++)
  {
    shellanything::Configuration * config = configs[i];
    if (config)
    {
      //for each menu child
      shellanything::Menu::MenuPtrList menus = config->GetMenus();
      for(size_t j=0; j<menus.size(); j++)
      {
        shellanything::Menu * menu = menus[j];

        //Add this menu to the tree
        BuildMenuTree(hMenu, menu, insert_pos, next_menu_is_column);
      }
    }
  }
}

CCriticalSection::CCriticalSection()
{
  InitializeCriticalSection(&mCS);
}

CCriticalSection::~CCriticalSection()
{
  DeleteCriticalSection(&mCS);
}

void CCriticalSection::Enter()
{
  EnterCriticalSection(&mCS);
}

void CCriticalSection::Leave()
{
  LeaveCriticalSection(&mCS);
}

CCriticalSectionGuard::CCriticalSectionGuard(CCriticalSection * cs)
{
  mCS = cs;
  if (mCS)
  {
    mCS->Enter();
  }
}

CCriticalSectionGuard::~CCriticalSectionGuard()
{
  if (mCS)
  {
    mCS->Leave();
  }
  mCS = NULL;
}

CContextMenu::CContextMenu()
{
  LOG(INFO) << __FUNCTION__ << "(), new";
  //MessageBox(NULL, __FUNCTION__, __FUNCTION__, MB_OK);

  m_cRef = 0L;
  m_FirstCommandId = 0;
  m_IsBackGround = false;
  m_BuildMenuTreeCount = 0;

  // Increment the dll's reference counter.
  InterlockedIncrement(&g_cRefDll);
}

CContextMenu::~CContextMenu()
{
  LOG(INFO) << __FUNCTION__ << "(), delete";
  //MessageBox(NULL, __FUNCTION__, __FUNCTION__, MB_OK);

  // Decrement the dll's reference counter.
  InterlockedDecrement(&g_cRefDll);
}

HRESULT STDMETHODCALLTYPE CContextMenu::QueryContextMenu(HMENU hMenu, UINT menu_index, UINT first_command_id, UINT max_command_id, UINT flags)
{
  //build function descriptor
  static const FlagDescriptor<UINT>::FLAGS descriptors[] = {
    {CMF_NORMAL           , "CMF_NORMAL"           },
    {CMF_DEFAULTONLY      , "CMF_DEFAULTONLY"      },
    {CMF_VERBSONLY        , "CMF_VERBSONLY"        },
    {CMF_EXPLORE          , "CMF_EXPLORE"          },
    {CMF_NOVERBS          , "CMF_NOVERBS"          },
    {CMF_CANRENAME        , "CMF_CANRENAME"        },
    {CMF_NODEFAULT        , "CMF_NODEFAULT"        },
    {CMF_ITEMMENU         , "CMF_ITEMMENU"         },
    {CMF_EXTENDEDVERBS    , "CMF_EXTENDEDVERBS"    },
    {CMF_DISABLEDVERBS    , "CMF_DISABLEDVERBS"    },
    {CMF_ASYNCVERBSTATE   , "CMF_ASYNCVERBSTATE"   },
    {CMF_OPTIMIZEFORINVOKE, "CMF_OPTIMIZEFORINVOKE"},
    {CMF_SYNCCASCADEMENU  , "CMF_SYNCCASCADEMENU"  },
    {CMF_DONOTPICKDEFAULT , "CMF_DONOTPICKDEFAULT" },
    {NULL, NULL},
  };
  std::string flags_str = FlagDescriptor<UINT>::ToBitString(flags, descriptors);
  std::string flags_hex = ra::strings::Format("0x%08x", flags);

  //MessageBox(NULL, __FUNCTION__, __FUNCTION__, MB_OK);
  LOG(INFO) << __FUNCTION__ << "(), hMenu=0x" << ra::strings::Format("0x%08x", hMenu).c_str() << ",count=" << GetMenuItemCount(hMenu) << ", menu_index=" << menu_index << ", first_command_id=" << first_command_id << ", max_command_id=" << max_command_id << ", flags=" << flags_hex << "=(" << flags_str << ")";

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
  if ( m_Context.GetElements().size() == 0 )
  {
    //Don't know what to do with this
    LOG(INFO) << __FUNCTION__ << "(), skipped, nothing is selected.";
    return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 ); //nothing inserted
  }

  //Filter out queries that are called twice for the same directory.
  if (hMenu == CContextMenu::m_previousMenu)
  {
    //Issue  #6 - Right-click on a directory with Windows Explorer in the left panel shows the menus twice.
    //Issue #31 - Error in logs for CContextMenu::GetCommandString().
    //Using a static variable is a poor method for solving the issue but it is a "good enough" strategy.
    LOG(INFO) << __FUNCTION__ << "(), skipped, QueryContextMenu() called twice and menu is already populated once.";
    return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 ); //nothing inserted
  }

  //Remember current menu to prevent issues calling twice QueryContextMenu()
  m_previousMenu = hMenu;

  //MessageBox(NULL, "ATTACH NOW!", (std::string("ATTACH NOW!") + " " + __FUNCTION__).c_str(), MB_OK);

  //Log what is selected by the user
  const shellanything::Context::ElementList & elements = m_Context.GetElements();
  size_t num_selected_total = elements.size();
  int num_files       = m_Context.GetNumFiles();
  int num_directories = m_Context.GetNumDirectories();
  LOG(INFO) << "Context have " << num_selected_total << " element(s): " << num_files << " files and " << num_directories << " directories.";

  //Keep a reference the our first command id. We will need it when InvokeCommand is called.
  m_FirstCommandId = first_command_id;

  //Refresh the list of loaded configuration files
  shellanything::ConfigManager & cmgr = shellanything::ConfigManager::GetInstance();
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
  LOG(INFO) << "Menu: first_command_id=" << first_command_id << " menu_last_command_id=" << menu_last_command_id << " next_command_id=" << next_command_id << " num_menu_items=" << num_menu_items << ".\n";

  //debug the constructed menu tree
#ifdef _DEBUG
  std::string menu_tree = Win32Utils::GetMenuTree(hMenu, 2);
  LOG(INFO) << "Menu tree:\n" << menu_tree.c_str();
#endif

  HRESULT hr = MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, num_menu_items );
  return hr;
}

HRESULT STDMETHODCALLTYPE CContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
  //MessageBox(NULL, __FUNCTION__, __FUNCTION__, MB_OK);

  //define the type of structure pointed by lpcmi
  const char * struct_name = "UNKNOWN";
  if (lpcmi->cbSize == sizeof(CMINVOKECOMMANDINFO))
    struct_name = "CMINVOKECOMMANDINFO";
  else if (lpcmi->cbSize == sizeof(CMINVOKECOMMANDINFOEX))
    struct_name = "CMINVOKECOMMANDINFOEX";

  //define how we should interpret lpcmi->lpVerb
  std::string verb;
  if (IS_INTRESOURCE(lpcmi->lpVerb))
    // D:\Projects\ShellAnything\src\shellext.cpp(632) : warning C4311 : 'reinterpret_cast' : pointer truncation from 'LPCSTR' to 'int'
    // D:\Projects\ShellAnything\src\shellext.cpp(632) : warning C4302 : 'reinterpret_cast' : truncation from 'LPCSTR' to 'int'
    #pragma warning( push )
    #pragma warning( disable: 4302 )
    #pragma warning( disable: 4311 )
    verb = ra::strings::ToString(reinterpret_cast<int>(lpcmi->lpVerb));
    #pragma warning( pop )
  else
    verb = lpcmi->lpVerb;

  LOG(INFO) << __FUNCTION__ << "(), lpcmi->cbSize=" << struct_name << ", lpcmi->fMask=" << lpcmi->fMask << ", lpcmi->lpVerb=" << verb;

  //validate
  if (!IS_INTRESOURCE(lpcmi->lpVerb))
    return E_INVALIDARG; //don't know what to do with lpcmi->lpVerb
    
  UINT target_command_offset = LOWORD(lpcmi->lpVerb); //matches the command_id offset (command id of the selected menu - command id of the first menu)
  UINT target_command_id = m_FirstCommandId + target_command_offset;

  //From this point, it is safe to use class members without other threads interference
  CCriticalSectionGuard cs_guard(&m_CS);

  //find the menu that is requested
  shellanything::ConfigManager & cmgr = shellanything::ConfigManager::GetInstance();
  shellanything::Menu * menu = cmgr.FindMenuByCommandId(target_command_id);
  if (menu == NULL)
  {
    LOG(ERROR) << __FUNCTION__ << "(), unknown menu for lpcmi->lpVerb=" << verb;
    return E_INVALIDARG;
  }

  //compute the visual menu title
  shellanything::PropertyManager & pmgr = shellanything::PropertyManager::GetInstance();
  std::string title = pmgr.Expand(menu->GetName());

  //found a menu match, execute menu action
  LOG(INFO) << __FUNCTION__ << "(), executing action(s) for menu '" << title.c_str() << "'...";

  //execute actions
  const shellanything::Action::ActionPtrList & actions = menu->GetActions();
  for(size_t i=0; i<actions.size(); i++)
  {
    LOG(INFO) << __FUNCTION__ << "(), executing action " << (i+1) << " of " << actions.size() << ".";
    const shellanything::Action * action = actions[i];
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
          LOG(ERROR) << __FUNCTION__ << "(), action #" << (i+1) << " has failed: " << error_message;
        }
        else
        {
          //simply log an error
          LOG(ERROR) << __FUNCTION__ << "(), action #" << (i+1) << " has failed.";
        }

        //stop executing the next actions
        i = actions.size();
      }
    }
  }

  LOG(INFO) << __FUNCTION__ << "(), executing action(s) for menu '" << title.c_str() << "' completed.";

  return S_OK;
}

HRESULT STDMETHODCALLTYPE CContextMenu::GetCommandString(UINT_PTR command_id, UINT flags, UINT FAR *reserved, LPSTR pszName, UINT cchMax)
{
  //build function descriptor
  static const FlagDescriptor<UINT>::FLAGS descriptors[] = {
    {GCS_VERBA    , "GCS_VERBA"    },
    {GCS_HELPTEXTA, "GCS_HELPTEXTA"},
    {GCS_VALIDATEA, "GCS_VALIDATEA"},
    {GCS_VERBW    , "GCS_VERBW"    },
    {GCS_HELPTEXTW, "GCS_HELPTEXTW"},
    {GCS_VALIDATEW, "GCS_VALIDATEW"},
    {GCS_VERBICONW, "GCS_VERBICONW"},
    {GCS_UNICODE  , "GCS_UNICODE"  },
    {NULL, NULL},
  };
  std::string flags_str = FlagDescriptor<UINT>::ToValueString(flags, descriptors);
  std::string flags_hex = ra::strings::Format("0x%08x", flags);

  //MessageBox(NULL, __FUNCTION__, __FUNCTION__, MB_OK);
  //LOG(INFO) << __FUNCTION__ << "(), command_id=" << command_id << ", reserved=" << reserved << ", pszName=" << pszName << ", cchMax=" << cchMax << ", flags=" << flags_hex << "=(" << flags_str << ")";

  UINT target_command_offset = (UINT)command_id; //matches the command_id offset (command id of the selected menu substracted by command id of the first menu)
  UINT target_command_id = m_FirstCommandId + target_command_offset;

  //From this point, it is safe to use class members without other threads interference
  CCriticalSectionGuard cs_guard(&m_CS);

  //find the menu that is requested
  shellanything::ConfigManager & cmgr = shellanything::ConfigManager::GetInstance();
  shellanything::Menu * menu = cmgr.FindMenuByCommandId(target_command_id);
  if (menu == NULL)
  {
    LOG(ERROR) << __FUNCTION__ << "(), unknown menu for command_id=" << target_command_offset << " m_FirstCommandId=" << m_FirstCommandId << " target_command_id=" << target_command_id;
    return E_INVALIDARG;
  }

  //compute the visual menu description
  shellanything::PropertyManager & pmgr = shellanything::PropertyManager::GetInstance();
  std::string description = pmgr.Expand(menu->GetDescription());

  //convert to windows unicode...
  std::wstring desc_utf16 = ra::unicode::Utf8ToUnicode(description);
  std::string  desc_ansi  = ra::unicode::Utf8ToAnsi(description);

  //Build up tooltip string
  switch(flags)
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
  LOG(INFO) << __FUNCTION__ << "(), pIDFolder=" << (void*)pIDFolder;
  //MessageBox(NULL, __FUNCTION__, __FUNCTION__, MB_OK);

  //From this point, it is safe to use class members without other threads interference
  CCriticalSectionGuard cs_guard(&m_CS);

  shellanything::Context::ElementList files;

  // Cleanup
  m_Context.UnregisterProperties(); //Unregister the previous context properties
  m_Context.SetElements(files);
  m_IsBackGround = false;

  // Did we clicked on a folder's background or the desktop directory?
  if (pIDFolder)
  {
    LOG(INFO) << "User right-clicked on a background directory.";

    wchar_t szPath[2*MAX_PATH] = {0};

    if (SHGetPathFromIDListW(pIDFolder, szPath))
    {
      if (szPath[0] != '\0')
      {
        std::string path_utf8 = ra::unicode::UnicodeToUtf8(szPath);
        LOG(INFO) << "Found directory '" << path_utf8 << "'.";
        m_IsBackGround = true;
        files.push_back(path_utf8);
      }
      else
        return E_INVALIDARG;
    }
    else
    {
      return E_INVALIDARG;
    }
  }

  // User clicked on one or more file or directory
  else if (pDataObj)
  {
    LOG(INFO) << "User right-clicked on selected files/directories.";

    FORMATETC fmt = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    STGMEDIUM stg = {TYMED_HGLOBAL};
    HDROP hDropInfo;

    // The selected files are expected to be in HDROP format.
    if (FAILED(pDataObj->GetData(&fmt, &stg)))
      return E_INVALIDARG;

    // Get a locked pointer to the files data.
    hDropInfo = (HDROP)GlobalLock(stg.hGlobal);
    if (NULL == hDropInfo)
    {
      ReleaseStgMedium(&stg);
      return E_INVALIDARG;
    }

    UINT num_files = DragQueryFileW(hDropInfo, 0xFFFFFFFF, NULL, 0);
    LOG(INFO) << "User right-clicked on " << num_files << " files/directories.";

    // For each files
    for (UINT i=0; i<num_files; i++)
    {
      UINT length = DragQueryFileW(hDropInfo, i, NULL, 0);

      // Allocate a temporary buffer
      std::wstring path(length, '\0');
      if (path.size() != length)
        continue;
      size_t num_characters = length+1;

      // Copy the element into the temporary buffer
      DragQueryFileW(hDropInfo, i, (wchar_t*)path.data(), (UINT)num_characters);

      //add the new file
      std::string path_utf8 = ra::unicode::UnicodeToUtf8(path);
      LOG(INFO) << "Found file/directory #" << ra::strings::Format("%03d",i) << ": '" << path_utf8 << "'.";
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

HRESULT STDMETHODCALLTYPE CContextMenu::QueryInterface(REFIID riid, LPVOID FAR * ppvObj)
{
  //build function descriptor
  //MessageBox(NULL, __FUNCTION__, __FUNCTION__, MB_OK);
  LOG(INFO) << __FUNCTION__ << "(), riid=" << GuidToInterfaceName(riid).c_str() << ", ppvObj=" << ppvObj;

  //Filter out unimplemented know interfaces so they do not show as WARNINGS
  if (  IsEqualGUID(riid, IID_IObjectWithSite) || //{FC4801A3-2BA9-11CF-A229-00AA003D7352}
        IsEqualGUID(riid, IID_IInternetSecurityManager) || //{79EAC9EE-BAF9-11CE-8C82-00AA004BA90B}
        IsEqualGUID(riid, CLSID_UNDOCUMENTED_01) ||
        IsEqualGUID(riid, IID_IContextMenu2) || //{000214f4-0000-0000-c000-000000000046}
        IsEqualGUID(riid, IID_IContextMenu3)    //{BCFCE0A0-EC17-11d0-8D10-00A0C90F2719}
        )
  {
    return E_NOINTERFACE;
  }

  //https://docs.microsoft.com/en-us/office/client-developer/outlook/mapi/implementing-iunknown-in-c-plus-plus

  // Always set out parameter to NULL, validating it first.
  if (!ppvObj)
    return E_INVALIDARG;

  *ppvObj = NULL;
  if (IsEqualGUID(riid, IID_IUnknown))
  {
    *ppvObj = (LPVOID)this;
  }
  else if (IsEqualGUID(riid, IID_IShellExtInit))
  {
    *ppvObj = (LPSHELLEXTINIT)this;
  }
  else if (IsEqualGUID(riid, IID_IContextMenu))
  {
    *ppvObj = (LPCONTEXTMENU)this;
  }

  if (*ppvObj)
  {
    // Increment the reference count and return the pointer.
    LOG(INFO) << __FUNCTION__ << "(), found interface " << GuidToInterfaceName(riid).c_str();
    AddRef();
    return NOERROR;
  }

  LOG(WARNING) << __FUNCTION__ << "(), NOT FOUND: " << GuidToInterfaceName(riid).c_str();
  return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CContextMenu::AddRef()
{
  //https://docs.microsoft.com/en-us/office/client-developer/outlook/mapi/implementing-iunknown-in-c-plus-plus

  // Increment the object's internal counter.
  InterlockedIncrement(&m_cRef);
  return m_cRef;
}

ULONG STDMETHODCALLTYPE CContextMenu::Release()
{
  //https://docs.microsoft.com/en-us/office/client-developer/outlook/mapi/implementing-iunknown-in-c-plus-plus

  // Decrement the object's internal counter.
  ULONG ulRefCount = InterlockedDecrement(&m_cRef);
  if (0 == m_cRef)
  {
    delete this;
  }
  return ulRefCount;
}

// Constructeur de l'interface IClassFactory:
CClassFactory::CClassFactory()
{
  //MessageBox(NULL, __FUNCTION__, __FUNCTION__, MB_OK);
  LOG(INFO) << __FUNCTION__ << "(), new";

  m_cRef = 0L;

  // Increment the dll's reference counter.
  InterlockedIncrement(&g_cRefDll);
}

// Destructeur de l'interface IClassFactory:
CClassFactory::~CClassFactory()
{
  LOG(INFO) << __FUNCTION__ << "(), delete";

  // Decrement the dll's reference counter.
  InterlockedDecrement(&g_cRefDll);
}

HRESULT STDMETHODCALLTYPE CClassFactory::QueryInterface(REFIID riid, LPVOID FAR * ppvObj)
{
  //build function descriptor
  //MessageBox(NULL, __FUNCTION__, __FUNCTION__, MB_OK);
  LOG(INFO) << __FUNCTION__ << "(), riid=" << GuidToInterfaceName(riid).c_str() << ", ppvObj=" << ppvObj;

  //https://docs.microsoft.com/en-us/office/client-developer/outlook/mapi/implementing-iunknown-in-c-plus-plus

  // Always set out parameter to NULL, validating it first.
  if (!ppvObj)
    return E_INVALIDARG;
  *ppvObj = NULL;

  if (IsEqualGUID(riid, IID_IUnknown))
  {
    *ppvObj = (LPVOID)this;
  }
  else if (IsEqualGUID(riid, IID_IClassFactory))
  {
    *ppvObj = (LPCLASSFACTORY)this;
  }

  if (*ppvObj)
  {
    // Increment the reference count and return the pointer.
    LOG(INFO) << __FUNCTION__ << "(), found interface " << GuidToInterfaceName(riid).c_str();
    AddRef();
    return NOERROR;
  }

  LOG(WARNING) << __FUNCTION__ << "(), NOT FOUND: " << GuidToInterfaceName(riid).c_str();
  return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CClassFactory::AddRef()
{
  //https://docs.microsoft.com/en-us/office/client-developer/outlook/mapi/implementing-iunknown-in-c-plus-plus

  // Increment the object's internal counter.
  InterlockedIncrement(&m_cRef);
  return m_cRef;
}

ULONG STDMETHODCALLTYPE CClassFactory::Release()
{
  //https://docs.microsoft.com/en-us/office/client-developer/outlook/mapi/implementing-iunknown-in-c-plus-plus

  // Decrement the object's internal counter.
  ULONG ulRefCount = InterlockedDecrement(&m_cRef);
  if (0 == m_cRef)
  {
    delete this;
  }
  return ulRefCount;
}

HRESULT STDMETHODCALLTYPE CClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID FAR *ppvObj)
{
  //build function descriptor
  //MessageBox(NULL, __FUNCTION__, __FUNCTION__, MB_OK);
  LOG(INFO) << __FUNCTION__ << "(), pUnkOuter=" << pUnkOuter << ", riid=" << GuidToInterfaceName(riid).c_str();

  *ppvObj = NULL;
  if (pUnkOuter) return CLASS_E_NOAGGREGATION;
  CContextMenu* pContextMenu = new CContextMenu();
  if (!pContextMenu) return E_OUTOFMEMORY;
  HRESULT hr = pContextMenu->QueryInterface(riid, ppvObj);
  if (FAILED(hr))
  {
    delete pContextMenu;
    pContextMenu = NULL;
  }
  return hr;
}

HRESULT STDMETHODCALLTYPE CClassFactory::LockServer(BOOL fLock)
{
  //https://docs.microsoft.com/en-us/windows/desktop/api/unknwnbase/nf-unknwnbase-iclassfactory-lockserver
  //https://docs.microsoft.com/en-us/windows/desktop/api/combaseapi/nf-combaseapi-colockobjectexternal
  return S_OK;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
  //build function descriptor
  //MessageBox(NULL, __FUNCTION__, __FUNCTION__, MB_OK);
  LOG(INFO) << __FUNCTION__ << "(), rclsid=" << GuidToInterfaceName(rclsid).c_str() << ", riid=" << GuidToInterfaceName(riid).c_str() << "";

  *ppvOut = NULL;
  if (IsEqualGUID(rclsid, SHELLANYTHING_SHELLEXTENSION_CLSID))
  {
    CClassFactory *pcf = new CClassFactory;
    if (!pcf) return E_OUTOFMEMORY;
    HRESULT hr = pcf->QueryInterface(riid, ppvOut);
    if (FAILED(hr))
    {
      LOG(ERROR) << __FUNCTION__ << "(), Interface " << GuidToInterfaceName(riid).c_str() << " not found!";
      delete pcf;
      pcf = NULL;
    }
    LOG(INFO) << __FUNCTION__ << "(), found interface " << GuidToInterfaceName(riid).c_str();
    return hr;
  }
  LOG(ERROR) << __FUNCTION__ << "(), ClassFactory " << GuidToInterfaceName(rclsid).c_str() << " not found!";
  return CLASS_E_CLASSNOTAVAILABLE;
}

STDAPI DllCanUnloadNow(void)
{
  //MessageBox(NULL, __FUNCTION__, __FUNCTION__, MB_OK);

  ULONG ulRefCount = 0;
  ulRefCount = InterlockedIncrement(&g_cRefDll);
  ulRefCount = InterlockedDecrement(&g_cRefDll);

  if (0 == ulRefCount)
  {
    LOG(INFO) << __FUNCTION__ << "() -> Yes";
    return S_OK;
  }
  LOG(INFO) << __FUNCTION__ << "() -> No, " << ulRefCount << " instances are still in use.";
  return S_FALSE;
}

STDAPI DllRegisterServer(void)
{
  const std::string guid_str_tmp = GuidToString(SHELLANYTHING_SHELLEXTENSION_CLSID).c_str();
  const char * guid_str = guid_str_tmp.c_str();
  const std::string class_name_version1 = std::string(ShellExtensionClassName) + ".1";
  const std::string module_path = GetCurrentModulePath();

  //#define TRACELINE() MessageBox(NULL, (std::string("Line: ") + ra::strings::ToString(__LINE__)).c_str(), "DllUnregisterServer() DEBUG", MB_OK);

  //Register version 1 of our class
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\%s", class_name_version1);
    if (!Win32Registry::CreateKey(key.c_str(), ShellExtensionDescription))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\%s.1\\CLSID", ShellExtensionClassName);
    if (!Win32Registry::CreateKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  //Register current version of our class
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\%s", ShellExtensionClassName);
    if (!Win32Registry::CreateKey(key.c_str(), ShellExtensionDescription))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\%s\\CLSID", ShellExtensionClassName);
    if (!Win32Registry::CreateKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\%s\\CurVer", ShellExtensionClassName);
    if (!Win32Registry::CreateKey(key.c_str(), class_name_version1.c_str()))
      return E_ACCESSDENIED;
  }

  // Add the CLSID of this DLL to the registry
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\CLSID\\%s", guid_str);
    if (!Win32Registry::CreateKey(key.c_str(), ShellExtensionDescription))
      return E_ACCESSDENIED;
  }

  // Define the path and parameters of our DLL:
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\CLSID\\%s\\ProgID", guid_str);
    if (!Win32Registry::CreateKey(key.c_str(), class_name_version1.c_str()))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\CLSID\\%s\\VersionIndependentProgID", guid_str);
    if (!Win32Registry::CreateKey(key.c_str(), ShellExtensionClassName))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\CLSID\\%s\\Programmable", guid_str);
    if (!Win32Registry::CreateKey(key.c_str()))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\CLSID\\%s\\InprocServer32", guid_str);
    if (!Win32Registry::CreateKey(key.c_str(), module_path.c_str() ))
      return E_ACCESSDENIED;
    if (!Win32Registry::SetValue(key.c_str(), "ThreadingModel", "Apartment"))
      return E_ACCESSDENIED;
  }

  // Register the shell extension for all the file types
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\*\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::CreateKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  // Register the shell extension for directories
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\Directory\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::CreateKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  // Register the shell extension for folders
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\Folder\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::CreateKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  // Register the shell extension for the desktop or the file explorer's background
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\Directory\\Background\\ShellEx\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::CreateKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  // Register the shell extension for drives
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\Drive\\ShellEx\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::CreateKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  // Register the shell extension to the system's approved Shell Extensions
  {
    std::string key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved";
    if (!Win32Registry::CreateKey(key.c_str()))
      return E_ACCESSDENIED;
    if (!Win32Registry::SetValue(key.c_str(), guid_str, ShellExtensionDescription))
      return E_ACCESSDENIED;
  }

  // Notify the Shell to pick the changes:
  // https://docs.microsoft.com/en-us/windows/desktop/shell/reg-shell-exts#predefined-shell-objects
  // Any time you create or change a Shell extension handler, it is important to notify the system that you have made a change.
  // Do so by calling SHChangeNotify, specifying the SHCNE_ASSOCCHANGED event.
  // If you do not call SHChangeNotify, the change might not be recognized until the system is rebooted.
  SHChangeNotify(SHCNE_ASSOCCHANGED, 0, 0, 0);

  return S_OK;
}

STDAPI DllUnregisterServer(void)
{
  const std::string guid_str_tmp = GuidToString(SHELLANYTHING_SHELLEXTENSION_CLSID).c_str();
  const char * guid_str = guid_str_tmp.c_str();
  const std::string class_name_version1 = std::string(ShellExtensionClassName) + ".1";

  //#define TRACELINE() MessageBox(NULL, (std::string("Line: ") + ra::strings::ToString(__LINE__)).c_str(), "DllUnregisterServer() DEBUG", MB_OK);

  // Unregister the shell extension from the system's approved Shell Extensions
  {
    std::string key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved";
    if (!Win32Registry::DeleteValue(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  // Unregister the shell extension for drives
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\Drive\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::DeleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }

  // Unregister the shell extension for the desktop or the file explorer's background
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\Directory\\Background\\ShellEx\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::DeleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }

  // Unregister the shell extension for folders
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\Folders\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::DeleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }

  // Unregister the shell extension for directories
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\Directory\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::DeleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }

  // Unregister the shell extension for all the file types
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\*\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!Win32Registry::DeleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }

  // Remove the CLSID of this DLL from the registry
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\CLSID\\%s", guid_str);
    if (!Win32Registry::DeleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }

  // Unregister current and version 1 of our extension
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\%s", class_name_version1.c_str());
    if (!Win32Registry::DeleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::Format("HKEY_CLASSES_ROOT\\%s", ShellExtensionClassName);
    if (!Win32Registry::DeleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }

  // Notify the Shell to pick the changes:
  // https://docs.microsoft.com/en-us/windows/desktop/shell/reg-shell-exts#predefined-shell-objects
  // Any time you create or change a Shell extension handler, it is important to notify the system that you have made a change.
  // Do so by calling SHChangeNotify, specifying the SHCNE_ASSOCCHANGED event.
  // If you do not call SHChangeNotify, the change might not be recognized until the system is rebooted.
  SHChangeNotify(SHCNE_ASSOCCHANGED, 0, 0, 0);

  return S_OK;
}

void InstallDefaultConfigurations(const std::string & config_dir)
{
  std::string app_path = GetCurrentModulePathUtf8();
  std::string app_dir = ra::filesystem::GetParentPath(app_path);

  static const char * default_files[] = {
    "default.xml",
    "Microsoft Office 2003.xml",
    "Microsoft Office 2007.xml",
    "Microsoft Office 2010.xml",
    "Microsoft Office 2013.xml",
    "Microsoft Office 2016.xml",
    "shellanything.xml",
    "WinDirStat.xml",
  };
  static const size_t num_files = sizeof(default_files)/sizeof(default_files[0]);

  LOG(INFO) << "First application launch. Installing default configurations files.";

  for(size_t i=0; i<num_files; i++)
  {
    const char * filename = default_files[i];
    std::string source_path = app_dir    + "\\configurations\\" + filename;
    std::string target_path = config_dir + "\\" + filename;

    LOG(INFO) << "Installing configuration file: " << target_path;
    bool installed = ra::filesystem::CopyFileUtf8(source_path, target_path);
    if (!installed)
    {
      LOG(ERROR) << "Failed coping file '" << source_path << "' to target file '" << target_path << "'.";
    }
  }
}

void LogEnvironment()
{
  LOG(INFO) << "Enabling logging";
  LOG(INFO) << "DLL path: " << GetCurrentModulePathUtf8();
  LOG(INFO) << "EXE path: " << ra::process::GetCurrentProcessPathUtf8().c_str();

  LOG(INFO) << "IID_IUnknown      : " << GuidToString(IID_IUnknown).c_str();
  LOG(INFO) << "IID_IClassFactory : " << GuidToString(IID_IClassFactory).c_str();
  LOG(INFO) << "IID_IShellExtInit : " << GuidToString(IID_IShellExtInit).c_str();
  LOG(INFO) << "IID_IContextMenu  : " << GuidToString(IID_IContextMenu).c_str();
  LOG(INFO) << "IID_IContextMenu2 : " << GuidToString(IID_IContextMenu2).c_str();  //{000214f4-0000-0000-c000-000000000046}
  LOG(INFO) << "IID_IContextMenu3 : " << GuidToString(IID_IContextMenu3).c_str();  //{BCFCE0A0-EC17-11d0-8D10-00A0C90F2719}
}

void InitConfigManager()
{
  shellanything::ConfigManager & cmgr = shellanything::ConfigManager::GetInstance();

  static const std::string app_name = "ShellAnything";
  static const std::string app_version = SHELLANYTHING_VERSION;

  //get home directory of the user
  std::string home_dir = ra::user::GetHomeDirectoryUtf8();
  std::string config_dir = home_dir + "\\" + app_name;
  LOG(INFO) << "HOME   directory : " << home_dir.c_str();
  LOG(INFO) << "Config directory : " << config_dir.c_str();

  bool first_run = IsFirstApplicationRun(app_name, app_version);
  if (first_run)
  {
    InstallDefaultConfigurations(config_dir);
  }

  //setup ConfigManager to read files from config_dir
  cmgr.ClearSearchPath();
  cmgr.AddSearchPath(config_dir);
  cmgr.Refresh();

  //define global properties
  std::string prop_application_path       = GetCurrentModulePathUtf8();
  std::string prop_application_directory  = ra::filesystem::GetParentPath(prop_application_path);
  std::string prop_log_directory          = ra::unicode::AnsiToUtf8(shellanything::GetLogDirectory());

  shellanything::PropertyManager & pmgr = shellanything::PropertyManager::GetInstance();
  pmgr.SetProperty("application.path"     , prop_application_path     );
  pmgr.SetProperty("application.directory", prop_application_directory);
  pmgr.SetProperty("log.directory"        , prop_log_directory        );
  pmgr.SetProperty("config.directory"     , config_dir                );
  pmgr.SetProperty("home.directory"       , home_dir                  );
}

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
  if (dwReason == DLL_PROCESS_ATTACH)
  {
    g_hmodDll = hInstance;

    if (!shellanything::IsTestingEnvironment())
    {
      // Initialize Google's logging library.
      shellanything::InitLogger();
    }

    LogEnvironment();

    // Initialize the configuration manager
    InitConfigManager();
  }
  else if (dwReason == DLL_PROCESS_DETACH)
  {
    if (!shellanything::IsTestingEnvironment())
    {
      // Shutdown Google's logging library.
      google::ShutdownGoogleLogging();
    }
  }
  return 1;
}

//int main(int argc, char* argv[])
//{
//  {
//    HRESULT result = DllRegisterServer();
//    if (result == S_OK)
//      //MessageBox(NULL, "Manual dll registration successfull", ShellExtensionClassName, MB_OK | MB_ICONINFORMATION);
//    else                                              
//      //MessageBox(NULL, "Manual dll registration FAILED !", ShellExtensionClassName, MB_OK | MB_ICONERROR);
//  }
//
//  {
//    HRESULT result = DllUnregisterServer();
//    if (result == S_OK)
//      //MessageBox(NULL, "Manual dll unregistration successfull", ShellExtensionClassName, MB_OK | MB_ICONINFORMATION);
//    else
//      //MessageBox(NULL, "Manual dll unregistration FAILED !", ShellExtensionClassName, MB_OK | MB_ICONERROR);
//  }
//}
