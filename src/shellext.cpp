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

#include "rapidassist/strings.h"
#include "rapidassist/environment.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/process.h"

#include <assert.h>

//Declarations
UINT      g_cRefDll = 0;   // Reference counter of this DLL
HINSTANCE g_hmodDll = 0;   // HINSTANCE of the DLL

static const std::string  EMPTY_STRING;
static const std::wstring EMPTY_WIDE_STRING;

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
    int ret = GetLastError();
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

std::string GuidToString(GUID guid) {
  std::string output;
  output.assign(40, 0);
  sprintf_s((char*)output.c_str(), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
  return output;
}
 
CContextMenu::CustomMenu * CContextMenu::FindMenuByCommandId(CContextMenu::CustomMenuVector & menus, UINT target_command_id)
{
  for(size_t i=0; i<menus.size(); i++)
  {
    CContextMenu::CustomMenu * menu = &menus[i];
    if (target_command_id == menu->command_id)
      return menu;

    for(size_t j=0; j<menu->children.size(); j++)
    {
      CContextMenu::CustomMenu * match = FindMenuByCommandId(menu->children, target_command_id);
      if (match)
        return match;
    }
  }

  return NULL;
}

void CContextMenu::BuildMenuTree(HMENU hMenu, CContextMenu::CustomMenu & menu, UINT insert_pos, int & debug_icon_offset)
{
  MENUITEMINFOA menuinfo = {0};

  menuinfo.cbSize = sizeof(MENUITEMINFOA);
  menuinfo.fMask = MIIM_FTYPE | MIIM_ID | MIIM_STRING;
  menuinfo.fType = MFT_STRING;
  menuinfo.fState = MFS_ENABLED;
  menuinfo.wID = menu.command_id;
  menuinfo.dwTypeData = (char*)menu.title.c_str();
  menuinfo.cch = (UINT)menu.title.size();

  //add an icon
  {
    const std::string icon_filename = "c:\\windows\\system32\\shell32.dll";
    const int icon_index = debug_icon_offset;

    //next menu shall use another icon
    debug_icon_offset++;

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

  if (menu.children.size())
  {
    menuinfo.fMask |= MIIM_SUBMENU;
    HMENU hSubMenu = CreatePopupMenu();

    for(size_t i=0; i<menu.children.size(); i++)
    {
      CContextMenu::CustomMenu & submenu = menu.children[i];
      BuildMenuTree(hSubMenu, submenu, (UINT)i, debug_icon_offset);
    }

    menuinfo.hSubMenu = hSubMenu;
  }

  BOOL result = InsertMenuItemA(hMenu, insert_pos, TRUE, &menuinfo);

  LOG(INFO) << __FUNCTION__ << "(), insert.pos=" << insert_pos << ", id=" << menuinfo.wID << ", result=" << result;
}

void CContextMenu::BuildMenuTree(HMENU hMenu, CContextMenu::CustomMenuVector & menus)
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

  int debug_icon_offset = 0;
  for(size_t i=0; i<menus.size(); i++)
  {
    CContextMenu::CustomMenu & menu = menus[i];
    BuildMenuTree(hMenu, menu, (UINT)i, debug_icon_offset);
  }
}

// Convert a wide Unicode string to an UTF8 string
std::string unicode_to_utf8(const std::wstring & wstr)
{
  if (wstr.empty()) return std::string();
  int num_characters = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
  if (num_characters == 0)
    return EMPTY_STRING;
	std::string strTo(num_characters, 0);
  WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], num_characters, NULL, NULL);
	return strTo;
}

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_to_unicode(const std::string & str)
{
  if (str.empty()) return std::wstring();
  int num_characters = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
  if (num_characters == 0)
    return EMPTY_WIDE_STRING;
	std::wstring wstrTo(num_characters, 0);
  MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], num_characters);
	return wstrTo;
}

// Convert an wide Unicode string to ANSI string
std::string unicode_to_ansi(const std::wstring & wstr)
{
  if (wstr.empty()) return std::string();
	int num_characters = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
  if (num_characters == 0)
    return EMPTY_STRING;
	std::string strTo(num_characters, 0);
	WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &strTo[0], num_characters, NULL, NULL);
	return strTo;
}

// Convert an ANSI string to a wide Unicode String
std::wstring ansi_to_unicode(const std::string & str)
{
  if (str.empty()) return std::wstring();
	int num_characters = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
  if (num_characters == 0)
    return EMPTY_WIDE_STRING;
	std::wstring wstrTo(num_characters, 0);
	MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], num_characters);
	return wstrTo;
}

std::string utf8_to_ansi(const std::string & str)
{
  std::wstring str_unicode = utf8_to_unicode(str);
  std::string str_ansi = unicode_to_ansi(str_unicode);
  return str_ansi;
}

std::string ansi_to_utf8(const std::string & str)
{
  std::wstring str_unicode = ansi_to_unicode(str);
  std::string str_utf8 = unicode_to_utf8(str_unicode);
  return str_utf8;
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
      std::string  atext = unicode_to_ansi(wtext);
      sprintf(descriptor, "%d:%s", id, atext.c_str());
    }

    //append the descriptor to the total output
    if (!output.empty())
      output.append(",");
    output.append(descriptor);
  }

  output.insert(0, "MENU{");
  output.append("}");
  return output;
}

HRESULT STDMETHODCALLTYPE CContextMenu::QueryContextMenu(HMENU hMenu,  UINT indexMenu,  UINT idCmdFirst,  UINT idCmdLast, UINT uFlags)
{
  //build function descriptor
  //MessageBox(NULL, __FUNCTION__, __FUNCTION__, MB_OK);
  LOG(INFO) << __FUNCTION__ << "(), hMenu=" << GetMenuDescriptor(hMenu) << ", indexMenu=" << indexMenu << ", idCmdFirst=" << idCmdFirst << ", idCmdLast=" << idCmdLast << ", uFlags=" << uFlags;

  //https://docs.microsoft.com/en-us/windows/desktop/shell/how-to-implement-the-icontextmenu-interface

  //filter for unknown flags
  if (  ((uFlags & CMF_EXPLORE ) != CMF_EXPLORE ) ||
        ((uFlags & CMF_ITEMMENU) != CMF_ITEMMENU) )
  {
    //Don't know what to do with this
    LOG(INFO) << __FUNCTION__ << "(), unknown uFlags, skipped";
    return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 ); //nothing inserted
  }

  //MessageBox(NULL, "ATTACH NOW!", __FUNCTION__, MB_OK);

  //From this point, it is safe to use class members without other threads interference
  CCriticalSectionGuard cs_guard(&m_CS);

  //Log what is selected by the user
  const shellanything::Context::ElementList & elements = m_Context.getElements();
  size_t num_selected_total = elements.size();
  int num_directories = m_Context.getNumFiles();
  int num_files       = m_Context.getNumDirectories();
  LOG(INFO) << "Context have " << num_selected_total << " element(s): " << num_files << " files and " << num_directories << " directories.";

  UINT nextCommandId = idCmdFirst;
  m_FirstCommandId = idCmdFirst;

  //declare menus
  m_Menus.clear();

  {
    CustomMenu menu;
    menu.title = "shellanything1";
    menu.command_id = nextCommandId;
    m_Menus.push_back(menu);
  }

  nextCommandId++;

  {
    CustomMenu menu;
    menu.title = "shell.anything2";
    menu.command_id = nextCommandId;
    m_Menus.push_back(menu);
  }

  nextCommandId++;

  {
    CustomMenu menu;
    menu.title = "sa.parent";
    menu.command_id = nextCommandId;
    m_Menus.push_back(menu);

  }

  nextCommandId++;

  CustomMenu * parent = NULL;
  parent = &m_Menus[m_Menus.size()-1];

  {
    CustomMenu menu;
    menu.title = "sa.child.1";
    menu.command_id = nextCommandId;
    parent->children.push_back(menu);
  }

  nextCommandId++;

  {
    CustomMenu menu;
    menu.title = "sa.child.2";
    menu.command_id = nextCommandId;
    parent->children.push_back(menu);
  }

  nextCommandId++;

  {
    CustomMenu menu;
    menu.title = "sa.child.3";
    menu.command_id = nextCommandId;
    parent->children.push_back(menu);
  }

  nextCommandId++;

  {
    CustomMenu menu;
    menu.title = "sa.last";
    menu.command_id = nextCommandId;
    m_Menus.push_back(menu);
  }

  nextCommandId++;

  //convert CustomMenu instances to HMENU intances
  BuildMenuTree(hMenu, m_Menus);

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
  CustomMenu * menu = FindMenuByCommandId(m_Menus, target_command_id);
  if (menu == NULL)
  {
    LOG(ERROR) << __FUNCTION__ << "(), unknown menu for lpcmi->lpVerb=" << verb;
    return E_INVALIDARG;
  }

  //found a menu match, execute menu action
  LOG(INFO) << __FUNCTION__ << "(), executing action '" << menu->title.c_str() << "'!";

  return S_OK;
}

HRESULT STDMETHODCALLTYPE CContextMenu::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT FAR *reserved, LPSTR pszName, UINT cchMax)
{
  //build function descriptor
  //MessageBox(NULL, __FUNCTION__, __FUNCTION__, MB_OK);
  LOG(INFO) << __FUNCTION__ << "(), idCmd=" << idCmd << ", uFlags=" << uFlags << ", reserved=" << reserved << ", pszName=" << pszName << ", cchMax=" << cchMax;

  UINT target_command_offset = (UINT)idCmd; //matches the command_id offset (command id of the selected menu - command id of the first menu)
  UINT target_command_id = m_FirstCommandId + target_command_offset;

  //From this point, it is safe to use class members without other threads interference
  CCriticalSectionGuard cs_guard(&m_CS);

  //find the menu that is requested
  CustomMenu * menu = FindMenuByCommandId(m_Menus, target_command_id);
  if (menu == NULL)
  {
    LOG(ERROR) << __FUNCTION__ << "(), unknown menu for idCmd=" << target_command_offset;
    return S_FALSE;
  }

  //Build up tooltip string
  switch(uFlags)
  {
  case GCS_HELPTEXTA:
    {
      //ANIS tooltip handling
      lstrcpynA(pszName, menu->title.c_str(), cchMax);
      return S_OK;
    }
    break;
  case GCS_HELPTEXTW:
    {
      //UNICODE tooltip handling
      std::wstring title = ansi_to_unicode(menu->title);
      lstrcpynW((LPWSTR)pszName, title.c_str(), cchMax);
      return S_OK;
    }
    break;
  case GCS_VERBA:
    {
      //ANIS tooltip handling
      lstrcpynA(pszName, menu->title.c_str(), cchMax);
      return S_OK;
    }
    break;
  case GCS_VERBW:
    {
      //UNICODE tooltip handling
      std::wstring title = ansi_to_unicode(menu->title);
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
  MessageBox(NULL, __FUNCTION__, __FUNCTION__, MB_OK);

  //From this point, it is safe to use class members without other threads interference
  CCriticalSectionGuard cs_guard(&m_CS);

  shellanything::Context::ElementList files;

  // Cleanup
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
      UINT path_size = DragQueryFileA(hDropInfo, i, NULL, 0);

      std::string path(path_size, '\0');
      if (path.size() != path_size)
        continue;

      //add the new file
      LOG(INFO) << "Found file/directory " << ra::strings::format("%02d",i) << ": '" << path << "'.";
      files.push_back(path);
    }
    GlobalUnlock(stg.hGlobal);
    ReleaseStgMedium(&stg);
  }

  //update the selection context
  m_Context.setElements(files);

  return S_OK;
}

HRESULT STDMETHODCALLTYPE CContextMenu::QueryInterface(REFIID riid, LPVOID FAR * ppvObj)
{
  //build function descriptor
  //MessageBox(NULL, __FUNCTION__, __FUNCTION__, MB_OK);
  LOG(INFO) << __FUNCTION__ << "(), riid=" << GuidToString(riid).c_str() << ", ppvObj=" << ppvObj;

  //Filter out unimplemented know interfaces so they do not show as WARNINGS
  static const GUID CLSID_UNDOCUMENTED_01 = { 0x924502a7, 0xcc8e, 0x4f60, { 0xae, 0x1f, 0xf7, 0x0c, 0x0a, 0x2b, 0x7a, 0x7c } };
  if (  IsEqualGUID(riid, IID_IObjectWithSite) || //{FC4801A3-2BA9-11CF-A229-00AA003D7352}
        IsEqualGUID(riid, IID_IInternetSecurityManager) || //{79EAC9EE-BAF9-11CE-8C82-00AA004BA90B}
        IsEqualGUID(riid, CLSID_UNDOCUMENTED_01)      )
  {
    return E_NOINTERFACE;
  }

  //https://docs.microsoft.com/en-us/office/client-developer/outlook/mapi/implementing-iunknown-in-c-plus-plus

  // Always set out parameter to NULL, validating it first.
  if (!ppvObj)
    return E_INVALIDARG;
  *ppvObj = NULL;
  if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IShellExtInit) || IsEqualGUID(riid, IID_IContextMenu))
  {
    // Increment the reference count and return the pointer.
    LOG(INFO) << __FUNCTION__ << "(), found interface " << GuidToString(riid).c_str();
    *ppvObj = (LPVOID)this;
    AddRef();
    return NOERROR;
  }
  LOG(WARNING) << __FUNCTION__ << "(), NOT FOUND: " << GuidToString(riid).c_str();
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
  LOG(INFO) << __FUNCTION__ << "(), riid=" << GuidToString(riid).c_str() << ", ppvObj=" << ppvObj;

  //https://docs.microsoft.com/en-us/office/client-developer/outlook/mapi/implementing-iunknown-in-c-plus-plus

  // Always set out parameter to NULL, validating it first.
  if (!ppvObj)
    return E_INVALIDARG;
  *ppvObj = NULL;
  if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IClassFactory))
  {
    // Increment the reference count and return the pointer.
    LOG(INFO) << __FUNCTION__ << "(), found interface " << GuidToString(riid).c_str();
    *ppvObj = (LPVOID)this;
    AddRef();
    return NOERROR;
  }

  LOG(WARNING) << __FUNCTION__ << "(), NOT FOUND: " << GuidToString(riid).c_str();
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
  LOG(INFO) << __FUNCTION__ << "(), pUnkOuter=" << pUnkOuter << ", riid=" << GuidToString(riid).c_str();

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
  LOG(INFO) << __FUNCTION__ << "(), rclsid=" << GuidToString(rclsid).c_str() << ", riid=" << GuidToString(riid).c_str() << "";

  *ppvOut = NULL;
  if (IsEqualGUID(rclsid, SHELLANYTHING_SHELLEXTENSION_CLSID))
  {
    CClassFactory *pcf = new CClassFactory;
    if (!pcf) return E_OUTOFMEMORY;
    HRESULT hr = pcf->QueryInterface(riid, ppvOut);
    if (FAILED(hr))
    {
      LOG(ERROR) << __FUNCTION__ << "(), Interface " << GuidToString(riid).c_str() << " not found!";
      delete pcf;
      pcf = NULL;
    }
    LOG(INFO) << __FUNCTION__ << "(), found interface " << GuidToString(riid).c_str();
    return hr;
  }
  LOG(ERROR) << __FUNCTION__ << "(), ClassFactory " << GuidToString(rclsid).c_str() << " not found!";
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

  // Initialize Google's logging library.
  std::string dll_path = GetCurrentModulePath();
  const char * argv[] = {
    dll_path.c_str(),
    ""
  };

  //https://codeyarns.com/2017/10/26/how-to-install-and-use-glog/
  google::InitGoogleLogging(argv[0]); //log in %TEMP% directory

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
 
  LOG(INFO) << "Enabling logging";
  LOG(INFO) << "DLL path: " << GetCurrentModulePath();
  LOG(INFO) << "EXE path: " << ra::process::getCurrentProcessPath().c_str();

  LOG(INFO) << "IID_IUnknown      : " << GuidToString(IID_IUnknown).c_str();
  LOG(INFO) << "IID_IClassFactory : " << GuidToString(IID_IClassFactory).c_str();
  LOG(INFO) << "IID_IShellExtInit : " << GuidToString(IID_IShellExtInit).c_str();
  LOG(INFO) << "IID_IContextMenu  : " << GuidToString(IID_IContextMenu).c_str();
}

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
  if (dwReason == DLL_PROCESS_ATTACH)
  {
    g_hmodDll = hInstance;

    // Initialize Google's logging library.
    InitLogger();
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
