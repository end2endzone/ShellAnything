//#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include <initguid.h>
#include <shlguid.h>
#include <shlobj.h>
#include <cmnquery.h>
#include <dsquery.h>
#include <dsclient.h>

#include <glog/logging.h>

#include "Platform.h"
#include "shellext.h"
#include "win32_registry.h"
#include "win32_utils.h"
#include "utf_strings.h"

#include "rapidassist/strings.h"
#include "rapidassist/environment.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/process.h"

#include "shellanything/ConfigManager.h"
#include "PropertyManager.h"

#include <assert.h>

//Declarations
UINT      g_cRefDll = 0;            // Reference counter of this DLL
HINSTANCE g_hmodDll = 0;            // HINSTANCE of the DLL
char      g_Path[MAX_PATH];         // Path to this DLL. 
char *    g_argv[] = {g_Path, ""};  //simulate 'main(int argc, char **argv)' for google::InitGoogleLogging()

static const std::string  EMPTY_STRING;
static const std::wstring EMPTY_WIDE_STRING;
static const GUID CLSID_UNDOCUMENTED_01 = { 0x924502a7, 0xcc8e, 0x4f60, { 0xae, 0x1f, 0xf7, 0x0c, 0x0a, 0x2b, 0x7a, 0x7c } };

std::string GetCurrentModulePath()
{
  std::string path;
  char buffer[MAX_PATH] = {0};
  HMODULE hModule = NULL;
  if (!GetModuleHandleEx( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          (LPCSTR) __FUNCTION__,
                          &hModule))
  {
    DWORD dwError = GetLastError();
    std::string desc = shellanything::GetSystemErrorDescription(dwError);
    std::string message = std::string() +
      "Error in function '" + __FUNCTION__ + "()', file '" + __FILE__ + "', line " + ra::strings::toString(__LINE__) + ".\n" +
      "\n" +
      "Failed getting the file path of the current module.\n" +
      "The following error code was returned:\n" +
      "\n" +
      ra::strings::format("0x%08x", dwError) + ": " + desc;

    //display an error on 
    shellanything::ShowErrorMessage("ShellAnything Error", message);

    return EMPTY_STRING;
  }

  /*get the path of this DLL*/
  GetModuleFileName(hModule, buffer, sizeof(buffer));
  if (buffer[0] != '\0')
  {
    path = buffer;
  }
  return path;
}

std::string GuidToString(GUID guid)
{
  std::string output;
  output.assign(40, 0);
  sprintf_s((char*)output.c_str(), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
  return output;
}

std::string GuidToInterfaceName(GUID guid)
{
  if (IsEqualGUID(guid, IID_IUnknown))                  return "IID_IUnknown";              //{00000000-0000-0000-C000-000000000046}
  if (IsEqualGUID(guid, IID_IClassFactory))             return "IID_IClassFactory";         //{00000001-0000-0000-C000-000000000046}
  if (IsEqualGUID(guid, IID_IShellExtInit))             return "IID_IShellExtInit";         //{000214E8-0000-0000-C000-000000000046}
  if (IsEqualGUID(guid, IID_IContextMenu))              return "IID_IContextMenu";          //{000214E4-0000-0000-C000-000000000046}
  if (IsEqualGUID(guid, IID_IContextMenu2))             return "IID_IContextMenu2";         //{000214F4-0000-0000-C000-000000000046}
  if (IsEqualGUID(guid, IID_IContextMenu3))             return "IID_IContextMenu3";         //{BCFCE0A0-EC17-11D0-8D10-00A0C90F2719}
  if (IsEqualGUID(guid, IID_IObjectWithSite))           return "IObjectWithSite";           //{FC4801A3-2BA9-11CF-A229-00AA003D7352}
  if (IsEqualGUID(guid, IID_IInternetSecurityManager))  return "IInternetSecurityManager";  //{79EAC9EE-BAF9-11CE-8C82-00AA004BA90B}

  //unknown GUID, return the string representation:
  //ie: CLSID_UNDOCUMENTED_01, {924502A7-CC8E-4F60-AE1F-F70C0A2B7A7C}
  return GuidToString(guid);
}

template <class T> class FlagDescriptor
{
public:
  struct FLAGS
  {
    T value;
    const char * name;
  };
 
  static std::string toBitString(T value, const FLAGS * flags)
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

  static std::string toValueString(T value, const FLAGS * flags)
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

void CContextMenu::BuildMenuTree(HMENU hMenu, shellanything::Menu * menu, UINT & insert_pos)
{
  //Expanded the menu's strings
  shellanything::PropertyManager & pmgr = shellanything::PropertyManager::getInstance();
  std::string title       = pmgr.expand(menu->getName());
  std::string description = pmgr.expand(menu->getDescription());

  //Get visible/enable properties based on current context.
  bool menu_visible   = menu->isVisible(m_Context);
  bool menu_enabled   = menu->isEnabled(m_Context);
  bool menu_separator = menu->isSeparator();

  //Skip this menu if not visible
  if (!menu_visible)
  {
    LOG(INFO) << __FUNCTION__ << "(), skipped menu '" << title << "', not visible.";
    return;
  }

  MENUITEMINFOA menuinfo = {0};

  menuinfo.cbSize = sizeof(MENUITEMINFOA);
  menuinfo.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_STRING;
  menuinfo.fType = (menu_separator ? MFT_SEPARATOR : MFT_STRING);
  menuinfo.fState = (menu_enabled ? MFS_ENABLED : MFS_DISABLED);
  menuinfo.wID = menu->getCommandId();
  menuinfo.dwTypeData = (char*)title.c_str();
  menuinfo.cch = (UINT)title.size();

  //add an icon
  const shellanything::Icon & icon = menu->getIcon();
  if (!menu_separator && icon.isValid())
  {
    const std::string & icon_filename = icon.getPath();
    const int icon_index = icon.getIndex();

    //ask the cache for an existing icon
    HBITMAP hBitmap = m_BitmapCache.find_handle(icon_filename, icon_index);

    //if nothing in cache, create a new one
    if (hBitmap == shellanything::BitmapCache::INVALID_BITMAP_HANDLE)
    {
      HICON hIconLarge = NULL;
      HICON hIconSmall = NULL;
      //
      UINT numIconLoaded = ExtractIconEx( icon_filename.c_str(), icon_index, &hIconLarge, &hIconSmall, 1 );
      if (numIconLoaded >= 1)
      {
        //Find the best icon
        HICON hIcon = win32_utils::GetBestIconForMenu(hIconLarge, hIconSmall);

        //Convert the icon to a bitmap (with invisible background)
        hBitmap = win32_utils::CopyAsBitmap(hIcon);

        //Remove the invisible background and replace by the default popup menu color
        COLORREF menu_background_color = GetSysColor(COLOR_MENU);
        win32_utils::FillTransparentPixels(hBitmap, menu_background_color);

        DestroyIcon(hIconLarge);
        DestroyIcon(hIconSmall);

        //add the bitmap to the cache for future use
        m_BitmapCache.add_handle( icon_filename.c_str(), icon_index, hBitmap );
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

  //handle submenus
  if (menu->isParentMenu())
  {
    menuinfo.fMask |= MIIM_SUBMENU;
    HMENU hSubMenu = CreatePopupMenu();

    shellanything::Menu::MenuPtrList subs = menu->getSubMenus();
    UINT sub_insert_pos = 0;
    for(size_t i=0; i<subs.size(); i++)
    {
      shellanything::Menu * submenu = subs[i];
      BuildMenuTree(hSubMenu, submenu, sub_insert_pos);
    }

    menuinfo.hSubMenu = hSubMenu;
  }

  BOOL result = InsertMenuItemA(hMenu, insert_pos, TRUE, &menuinfo);
  insert_pos++; //next menu is below this one

  LOG(INFO) << __FUNCTION__ << "(), insert.pos=" << ra::strings::format("%03d", insert_pos) << ", id=" << ra::strings::format("%06d", menuinfo.wID) << ", result=" << result << ", title=" << title;
}

void CContextMenu::BuildMenuTree(HMENU hMenu)
{
  //handle destruction of old bitmap in the cache
  m_BuildMenuTreeCount++;
  if (m_BuildMenuTreeCount > 0 && m_BuildMenuTreeCount%10 == 0)
  {
    //every 10 calls, refresh the cache
    m_BitmapCache.destroy_old_handles();
 
    //reset counters
    m_BitmapCache.reset_counters();
  }

  //get all available configurations
  shellanything::ConfigManager & cmgr = shellanything::ConfigManager::getInstance();
  shellanything::Configuration::ConfigurationPtrList configs = cmgr.getConfigurations();
  UINT insert_pos = 0;
  for(size_t i=0; i<configs.size(); i++)
  {
    shellanything::Configuration * config = configs[i];
    if (config)
    {
      //for each menu child
      shellanything::Menu::MenuPtrList menus = config->getMenus();
      for(size_t j=0; j<menus.size(); j++)
      {
        shellanything::Menu * menu = menus[j];

        //Add this menu to the tree
        BuildMenuTree(hMenu, menu, insert_pos);
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

void CCriticalSection::enter()
{
  EnterCriticalSection(&mCS);
}

void CCriticalSection::leave()
{
  LeaveCriticalSection(&mCS);
}

CCriticalSectionGuard::CCriticalSectionGuard(CCriticalSection * cs)
{
  mCS = cs;
  if (mCS)
  {
    mCS->enter();
  }
}

CCriticalSectionGuard::~CCriticalSectionGuard()
{
  if (mCS)
  {
    mCS->leave();
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

std::string GetMenuDescriptor(HMENU hMenu)
{
  std::string output;

  int numItems = GetMenuItemCount(hMenu);
  /*
  for(int i=0; i<numItems; i++)
  {
    UINT id = GetMenuItemID(hMenu, i);

    static const int BUFFER_SIZE = 1024;
    char menu_name[BUFFER_SIZE] = {0};
    char descriptor[BUFFER_SIZE] = {0};

    //try with ansi text
    if (GetMenuStringA(hMenu, i, menu_name, BUFFER_SIZE, 0))
    {
      sprintf(descriptor, "%d:%s", id, menu_name);
    }
    else if (GetMenuStringW(hMenu, i, (WCHAR*)menu_name, BUFFER_SIZE/2, 0))
    {
      //Can't log unicode characters, convert to ansi.
      //Assume some characters might get dropped
      std::wstring wtext = (WCHAR*)menu_name;
      std::string  atext = shellanything::unicode_to_ansi(wtext);
      sprintf(descriptor, "%d:%s", id, atext.c_str());
    }

    //append the descriptor to the total output
    if (!output.empty())
      output.append(",");
    output.append(descriptor);
  }
  */

  output.insert(0, "MENU{");
  output.append("GetMenuItemCount()=");
  output.append(ra::strings::toString(numItems));
  output.append("}");
  return output;
}

HRESULT STDMETHODCALLTYPE CContextMenu::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
  //build function descriptor
  static const FlagDescriptor<UINT>::FLAGS flags[] = {
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
  std::string uFlagsStr = FlagDescriptor<UINT>::toBitString(uFlags, flags);
  std::string uFlagsHex = ra::strings::format("0x%08x", uFlags);

  //MessageBox(NULL, __FUNCTION__, __FUNCTION__, MB_OK);
  LOG(INFO) << __FUNCTION__ << "(), hMenu=" << GetMenuDescriptor(hMenu) << ", indexMenu=" << indexMenu << ", idCmdFirst=" << idCmdFirst << ", idCmdLast=" << idCmdLast << ", uFlags=" << uFlagsHex << "=(" << uFlagsStr << ")";

  //https://docs.microsoft.com/en-us/windows/desktop/shell/how-to-implement-the-icontextmenu-interface

  //From this point, it is safe to use class members without other threads interference
  CCriticalSectionGuard cs_guard(&m_CS);

  //Note on uFlags...
  //Right-click on a file or directory with Windows Explorer on the right area:  uFlags=0x00020494=132244(dec)=(CMF_NORMAL|CMF_EXPLORE|CMF_CANRENAME|CMF_ITEMMENU|CMF_ASYNCVERBSTATE)
  //Right-click on the empty area      with Windows Explorer on the right area:  uFlags=0x00020424=132132(dec)=(CMF_NORMAL|CMF_EXPLORE|CMF_NODEFAULT|CMF_ASYNCVERBSTATE)
  //Right-click on a directory         with Windows Explorer on the left area:   uFlags=0x00000414=001044(dec)=(CMF_NORMAL|CMF_EXPLORE|CMF_CANRENAME|CMF_ASYNCVERBSTATE)
  //Right-click on a drive             with Windows Explorer on the left area:   uFlags=0x00000414=001044(dec)=(CMF_NORMAL|CMF_EXPLORE|CMF_CANRENAME|CMF_ASYNCVERBSTATE)
  //Right-click on the empty area      on the Desktop:                           uFlags=0x00020420=132128(dec)=(CMF_NORMAL|CMF_NODEFAULT|CMF_ASYNCVERBSTATE)
  //Right-click on a directory         on the Desktop:                           uFlags=0x00020490=132240(dec)=(CMF_NORMAL|CMF_CANRENAME|CMF_ITEMMENU|CMF_ASYNCVERBSTATE)

  //Filter out queries that have nothing selected
  //This can happend if user is copy & pasting files (using CTRL+C and CTRL+V)
  if ( m_Context.getElements().size() == 0 )
  {
    //Don't know what to do with this
    LOG(INFO) << __FUNCTION__ << "(), skipped, nothing is selected.";
    return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 ); //nothing inserted
  }

  //MessageBox(NULL, "ATTACH NOW!", (std::string("ATTACH NOW!") + " " + __FUNCTION__).c_str(), MB_OK);

  //Log what is selected by the user
  const shellanything::Context::ElementList & elements = m_Context.getElements();
  size_t num_selected_total = elements.size();
  int num_files       = m_Context.getNumFiles();
  int num_directories = m_Context.getNumDirectories();
  LOG(INFO) << "Context have " << num_selected_total << " element(s): " << num_files << " files and " << num_directories << " directories.";

  UINT nextCommandId = idCmdFirst;
  m_FirstCommandId = idCmdFirst;

  //Refresh the list of loaded configuration files
  shellanything::ConfigManager & cmgr = shellanything::ConfigManager::getInstance();
  cmgr.refresh();

  //Assign unique command id to all available menus (even the one that will not be visible)
  nextCommandId = cmgr.assignCommandIds(m_FirstCommandId);

  //Build the menus
  BuildMenuTree(hMenu);

  return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, nextCommandId - idCmdFirst );
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
    verb = ra::strings::toString((int)lpcmi->lpVerb);
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
  shellanything::ConfigManager & cmgr = shellanything::ConfigManager::getInstance();
  shellanything::Menu * menu = cmgr.findMenuByCommandId(target_command_id);
  if (menu == NULL)
  {
    LOG(ERROR) << __FUNCTION__ << "(), unknown menu for lpcmi->lpVerb=" << verb;
    return E_INVALIDARG;
  }

  //compute the visual menu title
  shellanything::PropertyManager & pmgr = shellanything::PropertyManager::getInstance();
  std::string title = pmgr.expand(menu->getName());

  //found a menu match, execute menu action
  LOG(INFO) << __FUNCTION__ << "(), executing action(s) for menu '" << title.c_str() << "'...";

  //execute actions
  const shellanything::Action::ActionPtrList & actions = menu->getActions();
  for(size_t i=0; i<actions.size(); i++)
  {
    LOG(INFO) << __FUNCTION__ << "(), executing action " << (i+1) << " of " << actions.size() << ".";
    const shellanything::Action * action = actions[i];
    if (action)
    {
      SetLastError(0); //reset win32 error code in case the action fails.
      bool success = action->execute(m_Context);

      if (!success)
      {
        //try to get an error mesage from win32
        uint32_t dwError = shellanything::GetSystemErrorCode();
        if (dwError)
        {
          std::string error_message = shellanything::GetSystemErrorDescription(dwError);
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

HRESULT STDMETHODCALLTYPE CContextMenu::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT FAR *reserved, LPSTR pszName, UINT cchMax)
{
  //build function descriptor
  static const FlagDescriptor<UINT>::FLAGS flags[] = {
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
  std::string uFlagsStr = FlagDescriptor<UINT>::toValueString(uFlags, flags);
  std::string uFlagsHex = ra::strings::format("0x%08x", uFlags);

  //MessageBox(NULL, __FUNCTION__, __FUNCTION__, MB_OK);
  //LOG(INFO) << __FUNCTION__ << "(), idCmd=" << idCmd << ", reserved=" << reserved << ", pszName=" << pszName << ", cchMax=" << cchMax << ", uFlags=" << uFlagsHex << "=(" << uFlagsStr << ")";

  UINT target_command_offset = (UINT)idCmd; //matches the command_id offset (command id of the selected menu substracted by command id of the first menu)
  UINT target_command_id = m_FirstCommandId + target_command_offset;

  //From this point, it is safe to use class members without other threads interference
  CCriticalSectionGuard cs_guard(&m_CS);

  //find the menu that is requested
  shellanything::ConfigManager & cmgr = shellanything::ConfigManager::getInstance();
  shellanything::Menu * menu = cmgr.findMenuByCommandId(target_command_id);
  if (menu == NULL)
  {
    LOG(ERROR) << __FUNCTION__ << "(), unknown menu for idCmd=" << target_command_offset;
    return E_INVALIDARG;
  }

  //compute the visual menu description
  shellanything::PropertyManager & pmgr = shellanything::PropertyManager::getInstance();
  std::string description = pmgr.expand(menu->getDescription());

  //Build up tooltip string
  switch(uFlags)
  {
  case GCS_HELPTEXTA:
    {
      //ANIS tooltip handling
      lstrcpynA(pszName, description.c_str(), cchMax);
      return S_OK;
    }
    break;
  case GCS_HELPTEXTW:
    {
      //UNICODE tooltip handling
      std::wstring title = shellanything::ansi_to_unicode(description);
      lstrcpynW((LPWSTR)pszName, title.c_str(), cchMax);
      return S_OK;
    }
    break;
  case GCS_VERBA:
    {
      //ANIS tooltip handling
      lstrcpynA(pszName, description.c_str(), cchMax);
      return S_OK;
    }
    break;
  case GCS_VERBW:
    {
      //UNICODE tooltip handling
      std::wstring title = shellanything::ansi_to_unicode(description);
      lstrcpynW((LPWSTR)pszName, title.c_str(), cchMax);
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

  LOG(ERROR) << __FUNCTION__ << "(), unknown flags for uFlags=" << uFlags;
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
  m_Context.unregisterProperties(); //Unregister the previous context properties
  m_Context.setElements(files);
  m_IsBackGround = false;

  // Did we clicked on a folder's background or the desktop directory?
  if (pIDFolder)
  {
    LOG(INFO) << "User right-clicked on a background directory.";

    char szPath[MAX_PATH] = {0};

    if (SHGetPathFromIDListA(pIDFolder, szPath))
    {
      if (szPath[0] != '\0')
      {
        LOG(INFO) << "Found directory '" << szPath << "'.";
        m_IsBackGround = true;
        files.push_back(std::string(szPath));
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
  if (pDataObj)
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

    UINT num_files = DragQueryFileA(hDropInfo, 0xFFFFFFFF, NULL, 0);
    LOG(INFO) << "User right-clicked on " << num_files << " files/directories.";

    // For each files
    for (UINT i=0; i<num_files; i++)
    {
      UINT length = DragQueryFileA(hDropInfo, i, NULL, 0);

      // Allocate a temporary buffer
      std::string path(length, '\0');
      if (path.size() != length)
        continue;
      size_t buffer_size = length+1;

      // Copy the element into the temporary buffer
      DragQueryFile(hDropInfo, i, (char*)path.data(), (UINT)buffer_size);

      //add the new file
      LOG(INFO) << "Found file/directory #" << ra::strings::format("%03d",i) << ": '" << path << "'.";
      files.push_back(path);
    }
    GlobalUnlock(stg.hGlobal);
    ReleaseStgMedium(&stg);
  }

  //update the selection context
  m_Context.setElements(files);

  //Register the current context properties so that menus can display the right caption
  m_Context.registerProperties();

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
  LOG(INFO) << __FUNCTION__ << "() -> No, " << ulRefCount << " instance are still in use.";
  return S_FALSE;
}

STDAPI DllRegisterServer(void)
{
  const std::string guid_str_tmp = GuidToString(SHELLANYTHING_SHELLEXTENSION_CLSID).c_str();
  const char * guid_str = guid_str_tmp.c_str();
  const std::string class_name_version1 = std::string(ShellExtensionClassName) + ".1";
  const std::string module_path = GetCurrentModulePath();

  //#define TRACELINE() MessageBox(NULL, (std::string("Line: ") + ra::strings::toString(__LINE__)).c_str(), "DllUnregisterServer() DEBUG", MB_OK);

  //Register version 1 of our class
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\%s", class_name_version1);
    if (!win32_registry::createKey(key.c_str(), ShellExtensionDescription))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\%s.1\\CLSID", ShellExtensionClassName);
    if (!win32_registry::createKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  //Register current version of our class
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\%s", ShellExtensionClassName);
    if (!win32_registry::createKey(key.c_str(), ShellExtensionDescription))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\%s\\CLSID", ShellExtensionClassName);
    if (!win32_registry::createKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\%s\\CurVer", ShellExtensionClassName);
    if (!win32_registry::createKey(key.c_str(), class_name_version1.c_str()))
      return E_ACCESSDENIED;
  }

  // Add the CLSID of this DLL to the registry
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\CLSID\\%s", guid_str);
    if (!win32_registry::createKey(key.c_str(), ShellExtensionDescription))
      return E_ACCESSDENIED;
  }

  // Define the path and parameters of our DLL:
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\CLSID\\%s\\ProgID", guid_str);
    if (!win32_registry::createKey(key.c_str(), class_name_version1.c_str()))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\CLSID\\%s\\VersionIndependentProgID", guid_str);
    if (!win32_registry::createKey(key.c_str(), ShellExtensionClassName))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\CLSID\\%s\\Programmable", guid_str);
    if (!win32_registry::createKey(key.c_str()))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\CLSID\\%s\\InprocServer32", guid_str);
    if (!win32_registry::createKey(key.c_str(), module_path.c_str() ))
      return E_ACCESSDENIED;
    if (!win32_registry::setValue(key.c_str(), "ThreadingModel", "Apartment"))
      return E_ACCESSDENIED;
  }

  // Register the shell extension for all the file types
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\*\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!win32_registry::createKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  // Register the shell extension for directories
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Directory\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!win32_registry::createKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Directory\\shellex\\DragDropHandlers\\%s", ShellExtensionClassName);
    if (!win32_registry::createKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  // Register the shell extension for the desktop or the file explorer's background
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Directory\\Background\\ShellEx\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!win32_registry::createKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Directory\\Background\\ShellEx\\DragDropHandlers\\%s", ShellExtensionClassName);
    if (!win32_registry::createKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  // Register the shell extension for drives
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Drive\\ShellEx\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!win32_registry::createKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Drive\\ShellEx\\DragDropHandlers\\%s", ShellExtensionClassName);
    if (!win32_registry::createKey(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  // Register the shell extension to the system's approved Shell Extensions
  {
    std::string key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved";
    if (!win32_registry::createKey(key.c_str()))
      return E_ACCESSDENIED;
    if (!win32_registry::setValue(key.c_str(), guid_str, ShellExtensionDescription))
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

  //#define TRACELINE() MessageBox(NULL, (std::string("Line: ") + ra::strings::toString(__LINE__)).c_str(), "DllUnregisterServer() DEBUG", MB_OK);

  // Unregister the shell extension from the system's approved Shell Extensions
  {
    std::string key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved";
    if (!win32_registry::deleteValue(key.c_str(), guid_str))
      return E_ACCESSDENIED;
  }

  // Unregister the shell extension for drives
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Drive\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!win32_registry::deleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Drive\\shellex\\DragDropHandlers\\%s", ShellExtensionClassName);
    if (!win32_registry::deleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }

  // Unregister the shell extension for the desktop or the file explorer's background
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Directory\\Background\\ShellEx\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!win32_registry::deleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Directory\\Background\\ShellEx\\DragDropHandlers\\%s", ShellExtensionClassName);
    if (!win32_registry::deleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }

  // Unregister the shell extension for directories
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Directory\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!win32_registry::deleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Directory\\shellex\\DragDropHandlers\\%s", ShellExtensionClassName);
    if (!win32_registry::deleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }

  // Unregister the shell extension for all the file types
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\*\\shellex\\ContextMenuHandlers\\%s", ShellExtensionClassName);
    if (!win32_registry::deleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }

  // Remove the CLSID of this DLL from the registry
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\CLSID\\%s", guid_str);
    if (!win32_registry::deleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }

  // Unregister current and version 1 of our extension
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\%s", class_name_version1.c_str());
    if (!win32_registry::deleteKey(key.c_str()))
      return E_ACCESSDENIED;
  }
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\%s", ShellExtensionClassName);
    if (!win32_registry::deleteKey(key.c_str()))
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

void DeletePreviousLogs(const std::string & iDirectory)
{
  std::string module_path = GetCurrentModulePath();
  std::string module_filename = ra::filesystem::getFilename(module_path.c_str());

  ra::strings::StringVector files;
  bool success = ra::filesystem::findFiles(files, iDirectory.c_str());
  if (!success) return;

  std::string pattern_prefix = iDirectory + ra::filesystem::getPathSeparatorStr() + module_filename;

  //for each files
  for(size_t i=0; i<files.size(); i++)
  {
    const std::string & path = files[i];
    if (path.find(pattern_prefix) != std::string::npos)
    {
      //that's a log file
      ra::filesystem::deleteFile(path.c_str());
    }
  }
}

std::string CreateLogDirectory()
{
  //By default, GLOG will output log files in %TEMP% directory.
  //However, I prefer to use %USERPROFILE%\ShellAnything\Logs

  std::string log_dir = shellanything::getHomeDirectory();
  if (log_dir.empty())
    return log_dir; //FAILED getting HOME directory.

  //We got the %USERPROFILE% directory.
  //Now add our custom path to it
  log_dir.append("\\ShellAnything\\Logs");

  //Try to create the directory
  bool created = shellanything::createFolder(log_dir.c_str());
  if (!created)
  {
    //failed creating directory.
    //fallback to %TEMP%
    return EMPTY_STRING;
  }

  return log_dir;
}

void InitLogger()
{
  //Create and define the LOG directory
  std::string log_dir = CreateLogDirectory();

  //delete previous logs for easier debugging
  const std::string temp_dir = ra::environment::getEnvironmentVariable("TEMP");
  DeletePreviousLogs(temp_dir);
  DeletePreviousLogs(log_dir);

  // Prepare Google's logging library.
  fLB::FLAGS_logtostderr = false; //on error, print to stdout instead of stderr
  fLB::FLAGS_log_prefix = 1; //prefix each message in file/console with 'E0405 19:13:07.577863  6652 shellext.cpp:847]'
  fLI::FLAGS_stderrthreshold = INT_MAX; //disable console output
  fLI::FLAGS_logbufsecs = 0; //flush logs after each LOG() calls
  fLI::FLAGS_minloglevel = google::GLOG_INFO;
  fLI::FLAGS_v = 4; //disables vlog(2), vlog(3), vlog(4)

  //if a log directory is specified, configure it now
  if (!log_dir.empty())
  {
    fLS::FLAGS_log_dir = log_dir;
  }
 
  //Do not change the default output files...
  //google::SetLogDestination(google::GLOG_INFO,    "shellext.dll.INFO");
  //google::SetLogDestination(google::GLOG_WARNING, "shellext.dll.WARNING");
  //google::SetLogDestination(google::GLOG_ERROR,   "shellext.dll.ERROR");
 
  const std::vector<std::string> dirs = google::GetLoggingDirectories();
 
  google::SetLogFilenameExtension(".log");

  // Initialize Google's logging library.
  std::string dll_path = GetCurrentModulePath();
  memset(g_Path, 0, sizeof(g_Path));
  strcpy(g_Path, dll_path.c_str());

  //https://codeyarns.com/2017/10/26/how-to-install-and-use-glog/
  google::InitGoogleLogging(g_argv[0]);
}

void LogEnvironment()
{
  LOG(INFO) << "Enabling logging";
  LOG(INFO) << "DLL path: " << GetCurrentModulePath();
  LOG(INFO) << "EXE path: " << ra::process::getCurrentProcessPath().c_str();

  LOG(INFO) << "IID_IUnknown      : " << GuidToString(IID_IUnknown).c_str();
  LOG(INFO) << "IID_IClassFactory : " << GuidToString(IID_IClassFactory).c_str();
  LOG(INFO) << "IID_IShellExtInit : " << GuidToString(IID_IShellExtInit).c_str();
  LOG(INFO) << "IID_IContextMenu  : " << GuidToString(IID_IContextMenu).c_str();
  LOG(INFO) << "IID_IContextMenu2 : " << GuidToString(IID_IContextMenu2).c_str();  //{000214f4-0000-0000-c000-000000000046}
  LOG(INFO) << "IID_IContextMenu3 : " << GuidToString(IID_IContextMenu3).c_str();  //{BCFCE0A0-EC17-11d0-8D10-00A0C90F2719}
}

void InitConfigManager()
{
  shellanything::ConfigManager & cmgr = shellanything::ConfigManager::getInstance();

  //get home directory of the user
  std::string home_dir = shellanything::getHomeDirectory();
  std::string config_dir = home_dir + "\\ShellAnything";
  LOG(INFO) << "HOME   directory : " << home_dir.c_str();
  LOG(INFO) << "Config directory : " << config_dir.c_str();

  //setup ConfigManager to read files from config_dir
  cmgr.clearSearchPath();
  cmgr.addSearchPath(config_dir);
  cmgr.refresh();
}

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
  if (dwReason == DLL_PROCESS_ATTACH)
  {
    g_hmodDll = hInstance;

    // Initialize Google's logging library.
    InitLogger();

    LogEnvironment();

    // Initialize the configuration manager
    InitConfigManager();
  }
  else if (dwReason == DLL_PROCESS_DETACH)
  {
    // Shutdown Google's logging library.
    google::ShutdownGoogleLogging();
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
