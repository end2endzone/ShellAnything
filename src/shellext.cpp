//#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include <initguid.h>
#include <shlguid.h>
#include <shlobj.h>

#include "shellext.h"
#include "win32Registry.h"

#include "rapidassist/strings.h"

//Declarations
UINT      g_cRefDll = 0;   // Reference counter of this DLL
HINSTANCE g_hmodDll = 0;   // HINSTANCE of the DLL

//http://www.codeguru.com/cpp/w-p/dll/tips/article.php/c3635
#if _MSC_VER >= 1300    // for VC 7.0
  // from ATL 7.0 sources
  #ifndef _delayimp_h
  extern "C" IMAGE_DOS_HEADER __ImageBase;
  #endif
#endif

HMODULE GetCurrentModule2000()
{
#if _MSC_VER < 1300 // earlier than .NET compiler (VC 6.0)
  // Here's a trick that will get you the handle of the module
  // you're running in without any a-priori knowledge:
  // http://www.dotnet247.com/247reference/msgs/13/65259.aspx

  MEMORY_BASIC_INFORMATION mbi;
  static int dummy;
  VirtualQuery( &dummy, &mbi, sizeof(mbi) );
  return reinterpret_cast<HMODULE>(mbi.AllocationBase);
#else // VC 7.0
  // from ATL 7.0 sources
  return reinterpret_cast<HMODULE>(&__ImageBase);
#endif
}

HMODULE GetCurrentModule()
{
  return GetCurrentModule2000();
}

const char * GetCurrentModulePath()
{
  static char buffer[MAX_PATH];
  GetModuleFileName(GetCurrentModule(), buffer, MAX_PATH);
  return buffer;
}

CContextMenu::CContextMenu()
{
  m_cRef = 0L;
  m_pDataObj = NULL;

  // Increment the dll's reference counter.
  InterlockedIncrement(&g_cRefDll);
}

CContextMenu::~CContextMenu()
{
  if (m_pDataObj) m_pDataObj->Release();

  // Decrement the dll's reference counter.
  InterlockedDecrement(&g_cRefDll);
}

HRESULT STDMETHODCALLTYPE CContextMenu::QueryContextMenu(HMENU hMenu,  UINT indexMenu,  UINT idCmdFirst,  UINT idCmdLast, UINT uFlags)
{
  //g_Logger->print("begin of QueryContextMenu");

  ////Debugging:
  //{
  //  extProcess * currentProcess = extProcessHandler::getCurrentProcess();
  //  std::string message = ra::strings::format("Attach to process %d and press OK to continue...", currentProcess->getPid());
  //  MessageBox(NULL, message.c_str(), "DEBUG!", MB_OK);
  //}

  UINT nextCommandId = idCmdFirst;
  long nextSequentialId = 0;
  //extMenuTools::insertMenu(hMenu, indexMenu, mMenus, nextCommandId, nextSequentialId);

  indexMenu++;

  //g_Logger->print("end of QueryContextMenu");
    
  return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, nextCommandId - idCmdFirst );
}

HRESULT STDMETHODCALLTYPE CContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
  //g_Logger->print("begin of InvokeCommand");

  //if (!HIWORD(lpcmi->lpVerb))
  //{
  //  long sequentialId = LOWORD(lpcmi->lpVerb);

  //  //check for a clicked menu command
  //  ShellMenu * selectedMenu = findMenuBySequentialId(mMenus, sequentialId);
  //  if (selectedMenu)
  //  {
  //    runMenu( (*selectedMenu), mSelectedItems );
  //  }
  //  else
  //  {
  //    //Build error message
  //    std::string message = ra::strings::format("Menu item with system id %d not found!\n\n", sequentialId);
  //    MessageBox(NULL, message.c_str(), "Unable to invoke command", MB_OK | MB_ICONERROR);
  //  }
  //}
  //g_Logger->print("end of InvokeCommand");

  return E_INVALIDARG;
}

HRESULT STDMETHODCALLTYPE CContextMenu::GetCommandString(UINT_PTR idCmd,  UINT uFlags, UINT FAR *reserved, LPSTR pszName, UINT cchMax)
{
  //g_Logger->print("begin of GetCommandString");
        
  //long menuSequenceId = idCmd;
  ////check for a menu that matches menuSequenceId
  //ShellMenu * selectedMenu = findMenuBySequentialId(mMenus, menuSequenceId);

  //if (selectedMenu == NULL)
  //{
  //  //Build error message
  //  std::string message = ra::strings::format("Menu item with sequential id %d not found!\n\n", menuSequenceId);
  //  MessageBox(NULL, message.c_str(), "Unable to process GetCommandString()", MB_OK | MB_ICONERROR);

  //  //g_Logger->print("end #1 of GetCommandString");
  //  return S_FALSE;
  //}

  ////Build up tooltip string
  //const char * tooltip = " ";
  //if (selectedMenu->getDescription().size() > 0)
  //  tooltip = selectedMenu->getDescription().c_str();

  ////Textes à afficher dans la barre d'état d'une fenêtre de l'explorateur quand
  ////l'un de nos éléments du menu contextuel est pointé par la souris:
  //switch(uFlags)
  //{
  //case GCS_HELPTEXTA:
  //  {
  //    //ANIS tooltip handling
  //    lstrcpynA(pszName, tooltip, cchMax);
  //    //g_Logger->print("end #2 of GetCommandString");
  //    return S_OK;
  //  }
  //  break;
  //case GCS_HELPTEXTW:
  //  {
  //    //UNICODE tooltip handling
  //    extMemoryBuffer unicodeDescription = extStringConverter::toUnicode(tooltip);
  //    bool success = false;
  //    if (unicodeDescription.getSize() <= cchMax)
  //    {
  //      memcpy(pszName, unicodeDescription.getBuffer(), unicodeDescription.getSize());
  //      success = true;
  //    }
  //    //g_Logger->print("end #3 of GetCommandString");
  //    return success ? S_OK : E_FAIL;
  //  }
  //  break;
  //case GCS_VERBA:
  //  break;
  //case GCS_VERBW:
  //  break;
  //case GCS_VALIDATEA:
  //case GCS_VALIDATEW:
  //  {
  //    //VALIDATE ? already validated, selectedMenu is non-NULL
  //    //g_Logger->print("end #4 of GetCommandString");
  //    return S_OK;
  //  }
  //  break;
  //}
  ////g_Logger->print("end #5 of GetCommandString");
  //return S_OK;

  return S_FALSE;
}

HRESULT STDMETHODCALLTYPE CContextMenu::Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hRegKey)
{
  //g_Logger->print("begin of Initialize");

  if (m_pDataObj) m_pDataObj->Release();
  mSelectedItems.clear();

  //if right-click on a file
  if (pDataObj)
  {
    mIsBackGround = false;
    m_pDataObj = pDataObj;
    pDataObj->AddRef();
    STGMEDIUM medium;
    FORMATETC fe = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    HRESULT hres = pDataObj->GetData( &fe, &medium );
    
    UINT numSelectedFiles = DragQueryFile((HDROP)medium.hGlobal, 0xFFFFFFFF, NULL, 0);

    // Extrack the path of the selected files
    for(UINT i=0; i<numSelectedFiles; i++)
    {
      char tmpPath[MAX_PATH];
      DragQueryFile( (HDROP)medium.hGlobal, i, tmpPath, MAX_PATH );
      std::string path = tmpPath;
      mSelectedItems.push_back( path );
    }

    ReleaseStgMedium(&medium);
  }
  else// right-click on the desktop or a file explorer's background
  {
    mIsBackGround = true;
    // Récupérer le chemin complet du dossier courant:
    //g_Logger->print("begin of extracting path");
    char tmpPath[MAX_PATH];
    SHGetPathFromIDList(pIDFolder,tmpPath);
    std::string path = tmpPath;
    mSelectedItems.push_back( path );
    //g_Logger->print("end of extracting path");
  }

  if (1)
  {
    //g_Logger->print(ra::strings::format("Num selected files: %d\n", mSelectedItems.size()).c_str());
    for(size_t i=0; i<mSelectedItems.size(); i++)
    {
      const std::string & itemPath = mSelectedItems[i];
      //g_Logger->printArgs("File %d = %s\n", mSelectedItems.size(), itemPath.c_str());
    }
  }

  //Build command tree
  //g_Logger->print("begin of building command tree");

  //g_Logger->print("begin of initMenuCommands");
  //initMenuCommands(mMenus);
  //g_Logger->print("end of initMenuCommands");

  //extract target path attributes
  //MenuFlags selectedItemsFlags = getSelectionFlags(mSelectedItems);
  //if (mIsBackGround)
  //{
  //  selectedItemsFlags.setFlags(MenuFlags::SingleFolder, false); //turn off singlefolder from getSelectionFlags()
  //  selectedItemsFlags.setFlags(MenuFlags::Background, true);
  //}
  
  //enable menus based on flags
  //g_Logger->print("begin of enableMenus");
  //enableMenus(selectedItemsFlags, mSelectedItems, mMenus);
  //g_Logger->print("end of enableMenus");

  //g_Logger->print("end of building command tree");
  //g_Logger->print("end of initialize");

  return NOERROR;
}

HRESULT STDMETHODCALLTYPE CContextMenu::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
  //https://docs.microsoft.com/en-us/office/client-developer/outlook/mapi/implementing-iunknown-in-c-plus-plus

  // Always set out parameter to NULL, validating it first.
  if (!ppvObj)
    return E_INVALIDARG;
  *ppvObj = NULL;
  if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IShellExtInit) || IsEqualGUID(riid, IID_IContextMenu))
  {
    // Increment the reference count and return the pointer.
    *ppvObj = (LPVOID)this;
    AddRef();
    return NOERROR;
  }
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
  m_cRef = 0L;

  // Increment the dll's reference counter.
  InterlockedIncrement(&g_cRefDll);
}

// Destructeur de l'interface IClassFactory:
CClassFactory::~CClassFactory()
{
  // Decrement the dll's reference counter.
  InterlockedDecrement(&g_cRefDll);
}

HRESULT STDMETHODCALLTYPE CClassFactory::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
  //https://docs.microsoft.com/en-us/office/client-developer/outlook/mapi/implementing-iunknown-in-c-plus-plus

  // Always set out parameter to NULL, validating it first.
  if (!ppvObj)
    return E_INVALIDARG;
  *ppvObj = NULL;
  if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IClassFactory))
  {
    // Increment the reference count and return the pointer.
    *ppvObj = (LPVOID)this;
    AddRef();
    return NOERROR;
  }
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

HRESULT STDMETHODCALLTYPE CClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid,LPVOID *ppvObj)
{
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
  *ppvOut = NULL;
  if (IsEqualGUID(rclsid, CLSID_ShellExtension))
  {
    CClassFactory *pcf = new CClassFactory;
    if (!pcf) return E_OUTOFMEMORY;
    HRESULT hr = pcf->QueryInterface(riid, ppvOut);
    if (FAILED(hr))
    {
      delete pcf;
      pcf = NULL;
    }
    return hr;
  }
  return CLASS_E_CLASSNOTAVAILABLE;
}

STDAPI DllCanUnloadNow(void)
{
  ULONG ulRefCount = 0;
  ulRefCount = InterlockedIncrement(&g_cRefDll);
  ulRefCount = InterlockedDecrement(&g_cRefDll);
  if (0 == ulRefCount)
  {
    return S_OK;
  }
  return S_FALSE;
}

STDAPI DllRegisterServer(void)
{
  // Add the CLSID of this DLL to the registry
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\CLSID\\%s", CLSID_ShellExtensionStr);
    if (!win32Registry::createKey(key.c_str()))
      return S_FALSE;
    if (!win32Registry::setValue(key.c_str(), "", ShellExtensionDescription))
      return S_FALSE;
  }

  // Define the path and parameters of our DLL:
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\CLSID\\%s\\InProcServer32", CLSID_ShellExtensionStr);
    if (!win32Registry::createKey(key.c_str()))
      return S_FALSE;
    if (!win32Registry::setValue(key.c_str(), "", GetCurrentModulePath() ))
      return S_FALSE;
    if (!win32Registry::setValue(key.c_str(), "ThreadingModel", "Apartment"))
      return S_FALSE;
  }

  // Register the shell extension for all the file types
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\*\\shellex\\ContextMenuHandlers\\%s", ShellExtensionName);
    if (!win32Registry::createKey(key.c_str()))
      return S_FALSE;
    if (!win32Registry::setValue(key.c_str(), "", CLSID_ShellExtensionStr))
      return S_FALSE;
  }

  // Register the shell extension for directories
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Directory\\shellex\\ContextMenuHandlers\\%s", ShellExtensionName);
    if (!win32Registry::createKey(key.c_str()))
      return S_FALSE;
    if (!win32Registry::setValue(key.c_str(), "", CLSID_ShellExtensionStr))
      return S_FALSE;
  }

  // Notify the Shell to pick the default icon definition change
  SHChangeNotify(SHCNE_ASSOCCHANGED,0,0,0);

  // Register the shell extension for the desktop or the file explorer's background
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Directory\\Background\\ShellEx\\ContextMenuHandlers\\%s", ShellExtensionName);
    if (!win32Registry::createKey(key.c_str()))
      return S_FALSE;
    if (!win32Registry::setValue(key.c_str(), "", CLSID_ShellExtensionStr))
      return S_FALSE;
  }

  // Register the shell extension to the system's approved Shell Extensions
  {
    std::string key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved";
    if (!win32Registry::createKey(key.c_str()))
      return S_FALSE;
    if (!win32Registry::setValue(key.c_str(), CLSID_ShellExtensionStr, ShellExtensionDescription))
      return S_FALSE;
  }

  return S_OK;
}

STDAPI DllUnregisterServer(void)
{
  // Unregister the shell extension from the system's approved Shell Extensions
  {
    std::string key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved";
    if (!win32Registry::deleteValue(key.c_str(), CLSID_ShellExtensionStr))
      return S_FALSE;
  }

  // Unregister the shell extension for the desktop or the file explorer's background
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Directory\\Background\\ShellEx\\ContextMenuHandlers\\%s", ShellExtensionName);
    if (!win32Registry::deleteKey(key.c_str()))
      return S_FALSE;
  }

  // Notify the Shell to pick the default icon definition change
  SHChangeNotify(SHCNE_ASSOCCHANGED,0,0,0);

  // Unregister the shell extension for directories
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\Directory\\shellex\\ContextMenuHandlers\\%s", ShellExtensionName);
    if (!win32Registry::deleteKey(key.c_str()))
      return S_FALSE;
  }

  // Unregister the shell extension for all the file types
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\*\\shellex\\ContextMenuHandlers\\%s", ShellExtensionName);
    if (!win32Registry::deleteKey(key.c_str()))
      return S_FALSE;
  }

  // Remove the CLSID of this DLL from the registry
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\CLSID\\%s\\InProcServer32", CLSID_ShellExtensionStr);
    if (!win32Registry::deleteKey(key.c_str()))
      return S_FALSE;
  }
  {
    std::string key = ra::strings::format("HKEY_CLASSES_ROOT\\CLSID\\%s", CLSID_ShellExtensionStr);
    if (!win32Registry::deleteKey(key.c_str()))
      return S_FALSE;
  }

  return S_OK;
}

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
  if (dwReason == DLL_PROCESS_ATTACH)
  {
    g_hmodDll = hInstance;
  }
  return 1; 
}

//int main(int argc, char* argv[])
//{
//  {
//    HRESULT result = DllRegisterServer();
//    if (result == S_OK)
//      MessageBox(NULL, "Manual dll registration successfull", ShellExtensionName, MB_OK | MB_ICONINFORMATION);
//    else                                              
//      MessageBox(NULL, "Manual dll registration FAILED !", ShellExtensionName, MB_OK | MB_ICONERROR);
//  }
//
//  {
//    HRESULT result = DllUnregisterServer();
//    if (result == S_OK)
//      MessageBox(NULL, "Manual dll unregistration successfull", ShellExtensionName, MB_OK | MB_ICONINFORMATION);
//    else
//      MessageBox(NULL, "Manual dll unregistration FAILED !", ShellExtensionName, MB_OK | MB_ICONERROR);
//  }
//}
