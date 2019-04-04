#include "shellext.h"

//#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
//#include <Rpc.h>

#include "extRegistry\extRegistry.h"
#include "extBase\extString.h"
#include "extBase\extStringConverter.h"
#include "extBase\extFiles.h"
#include "extBase\extTime.h"
#include "extBase\extRandom.h"
#include "extBase\extStringTokenizer.h"
#include "extBase\extMenuTools.h"
#include "extBase\extEnv.h"
#include "extProcess\extProcessHandler.h"

#include "extBase\extWindowsHeader.h"
#include <initguid.h>
#include <shlguid.h>
#include <shlobj.h>
#include <assert.h>

#include "Logger.h"

#include "ShellMenuLibrary.h"
#include "extToolsShell.h"
#include "dllutilLib.h"

using namespace EXTLIB;
using namespace EXTLIB::registry;
using namespace EXTLIB::win32;
using namespace EXTLIB::process;

//Declarations
UINT      g_cRefDll = 0;   // Reference counter
HINSTANCE g_hmodDll = 0;   // HINSTANCE of the DLL
static extILogger * g_Logger = getAppLogger();

// Constructeur de l'interface IContextMenu (incluant IShellExtInit):
CContextMenu::CContextMenu()
{
  m_cRef = 0L;
  m_pDataObj = NULL;
  g_cRefDll++;
}

// Destructeur de l'interface IContextMenu:
CContextMenu::~CContextMenu()
{
  if (m_pDataObj)  m_pDataObj->Release();
  g_cRefDll--;
}

// Implémentation de la méthode QueryContextMenu() de l'interface IContextMenu:
HRESULT STDMETHODCALLTYPE CContextMenu::QueryContextMenu(HMENU hMenu,  UINT indexMenu,  UINT idCmdFirst,  UINT idCmdLast, UINT uFlags)
{
  g_Logger->print("begin of QueryContextMenu");

  ////Debugging:
  //{
  //  extProcess * currentProcess = extProcessHandler::getCurrentProcess();
  //  extString message = extStringConverter::format("Attach to process %d and press OK to continue...", currentProcess->getPid());
  //  MessageBox(NULL, message.c_str(), "DEBUG!", MB_OK);
  //}

  UINT nextCommandId = idCmdFirst;
  long nextSequentialId = 0;
  extMenuTools::insertMenu(hMenu, indexMenu, mMenus, nextCommandId, nextSequentialId);

  indexMenu++;

  g_Logger->print("end of QueryContextMenu");
    
  return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, nextCommandId - idCmdFirst );
}

// Implémentation de la méthode InvokeCommand() de l'interface IContextMenu:
HRESULT STDMETHODCALLTYPE CContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
  g_Logger->print("begin of InvokeCommand");

  if (!HIWORD(lpcmi->lpVerb))
  {
    long sequentialId = LOWORD(lpcmi->lpVerb);

    //check for a clicked menu command
    ShellMenu * selectedMenu = findMenuBySequentialId(mMenus, sequentialId);
    if (selectedMenu)
    {
      runMenu( (*selectedMenu), mSelectedItems );
    }
    else
    {
      //Build error message
      extString message = extStringConverter::format("Menu item with system id %d not found!\n\n", sequentialId);
      MessageBox(NULL, message.c_str(), "Unable to invoke command", MB_OK | MB_ICONERROR);
    }
  }
  g_Logger->print("end of InvokeCommand");

  return E_INVALIDARG;
}

// Implémentation de la méthode GetCommandString() de l'interface IContextMenu:
HRESULT STDMETHODCALLTYPE CContextMenu::GetCommandString(UINT_PTR idCmd,  UINT uFlags, UINT FAR *reserved, LPSTR pszName, UINT cchMax)
{
  g_Logger->print("begin of GetCommandString");
        
  long menuSequenceId = idCmd;
  //check for a menu that matches menuSequenceId
  ShellMenu * selectedMenu = findMenuBySequentialId(mMenus, menuSequenceId);

  if (selectedMenu == NULL)
  {
    //Build error message
    extString message = extStringConverter::format("Menu item with sequential id %d not found!\n\n", menuSequenceId);
    MessageBox(NULL, message.c_str(), "Unable to process GetCommandString()", MB_OK | MB_ICONERROR);

    g_Logger->print("end #1 of GetCommandString");
    return S_FALSE;
  }

  //Build up tooltip string
  const char * tooltip = " ";
  if (selectedMenu->getDescription().size() > 0)
    tooltip = selectedMenu->getDescription().c_str();

  //Textes à afficher dans la barre d'état d'une fenêtre de l'explorateur quand
  //l'un de nos éléments du menu contextuel est pointé par la souris:
  switch(uFlags)
  {
  case GCS_HELPTEXTA:
    {
      //ANIS tooltip handling
      lstrcpynA(pszName, tooltip, cchMax);
      g_Logger->print("end #2 of GetCommandString");
      return S_OK;
    }
    break;
  case GCS_HELPTEXTW:
    {
      //UNICODE tooltip handling
      extMemoryBuffer unicodeDescription = extStringConverter::toUnicode(tooltip);
      bool success = false;
      if (unicodeDescription.getSize() <= cchMax)
      {
        memcpy(pszName, unicodeDescription.getBuffer(), unicodeDescription.getSize());
        success = true;
      }
      g_Logger->print("end #3 of GetCommandString");
      return success ? S_OK : E_FAIL;
    }
    break;
  case GCS_VERBA:
    break;
  case GCS_VERBW:
    break;
  case GCS_VALIDATEA:
  case GCS_VALIDATEW:
    {
      //VALIDATE ? already validated, selectedMenu is non-NULL
      g_Logger->print("end #4 of GetCommandString");
      return S_OK;
    }
    break;
  }
  g_Logger->print("end #5 of GetCommandString");
  return S_OK;
}

// Implémentation de la méthode Initialize() de l'interface IShellExtInit:
HRESULT STDMETHODCALLTYPE CContextMenu::Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hRegKey)
{
  g_Logger->print("begin of Initialize");

  if (m_pDataObj) m_pDataObj->Release();
  mSelectedItems.clear();
  if (pDataObj)//clic droit sur fichier
  {
    mIsBackGround = false;
    m_pDataObj = pDataObj;
    pDataObj->AddRef();
    STGMEDIUM medium;
    FORMATETC fe = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    HRESULT hres = pDataObj->GetData( &fe, &medium );
    
    // Obtenir le nombre de fichiers sélectionnés:
    UINT numSelectedFiles = DragQueryFile((HDROP)medium.hGlobal, 0xFFFFFFFF, NULL, 0);

    // Récupérer le chemin complet du fichier ou dossier sélectionné:
    for(UINT i=0; i<numSelectedFiles; i++)
    {
      char tmpPath[MAX_PATH];
      DragQueryFile( (HDROP)medium.hGlobal, i, tmpPath, MAX_PATH );
      extString path = tmpPath;
      mSelectedItems.add( path );
    }

    ReleaseStgMedium(&medium);
  }
  else// clic droit sur bureau ou arrière-plan d'une fenêtre de l'explorateur:
  {
    mIsBackGround = true;
    // Récupérer le chemin complet du dossier courant:
    g_Logger->print("begin of extracting path");
    char tmpPath[MAX_PATH];
    SHGetPathFromIDList(pIDFolder,tmpPath);
    extString path = tmpPath;
    mSelectedItems.add( path );
    g_Logger->print("end of extracting path");
  }

  if (g_Logger)
  {
    g_Logger->print(extStringConverter::format("Num selected files: %d\n", mSelectedItems.size()).c_str());
    for(uint32 i=0; i<mSelectedItems.size(); i++)
    {
      const extString & itemPath = mSelectedItems[i];
      g_Logger->printArgs("File %d = %s\n", mSelectedItems.size(), itemPath.c_str());
    }
  }

  //Build command tree
  g_Logger->print("begin of building command tree");

  g_Logger->print("begin of initMenuCommands");
  initMenuCommands(mMenus);
  g_Logger->print("end of initMenuCommands");

  //extract target path attributes
  MenuFlags selectedItemsFlags = getSelectionFlags(mSelectedItems);
  if (mIsBackGround)
  {
    selectedItemsFlags.setFlags(MenuFlags::SingleFolder, false); //turn off singlefolder from getSelectionFlags()
    selectedItemsFlags.setFlags(MenuFlags::Background, true);
  }
  
  //enable menus based on flags
  g_Logger->print("begin of enableMenus");
  enableMenus(selectedItemsFlags, mSelectedItems, mMenus);
  g_Logger->print("end of enableMenus");

  g_Logger->print("end of building command tree");
  g_Logger->print("end of initialize");

  return NOERROR;
}

// Implémentation de la méthode QueryInterface() de l'interface IContextMenu:
HRESULT STDMETHODCALLTYPE CContextMenu::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
  *ppv = NULL;
  if (IsEqualGUID(riid, IID_IShellExtInit) || IsEqualGUID(riid, IID_IUnknown))*ppv = (LPSHELLEXTINIT)this;
  else if (IsEqualGUID(riid, IID_IContextMenu))*ppv = (LPCONTEXTMENU)this;
  if (*ppv)
  {
    AddRef();
    return S_OK;
  }
  return E_NOINTERFACE;
}

// Implémentation de la méthode AddRef() de l'interface IContextMenu:
ULONG STDMETHODCALLTYPE CContextMenu::AddRef()
{
  return ++m_cRef;
}

// Implémentation de la méthode Release() de l'interface IContextMenu:
ULONG STDMETHODCALLTYPE CContextMenu::Release()
{
  if (--m_cRef)  return m_cRef;
  delete this;
  return 0L;
}

// Constructeur de l'interface IClassFactory:
CClassFactory::CClassFactory()
{
  m_cRef = 0L;
  g_cRefDll++;
}

// Destructeur de l'interface IClassFactory:
CClassFactory::~CClassFactory()
{
  g_cRefDll--;
}

// Implémentation de la méthode QueryInterface() de l'interface IClassFactory:
HRESULT STDMETHODCALLTYPE CClassFactory::QueryInterface(REFIID riid,  LPVOID FAR *ppv)
{
  *ppv = NULL;
  // Any interface on this object is the object pointer
  if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IClassFactory))
  {
    *ppv = (LPCLASSFACTORY)this;
    AddRef();
    return NOERROR;
  }
  return E_NOINTERFACE;
}

// Implémentation de la méthode Addref() de l'interface IClassFactory:
ULONG STDMETHODCALLTYPE CClassFactory::AddRef()
{
  return ++m_cRef;
}

// Implémentation de la méthode Release() de l'interface IClassFactory:
ULONG STDMETHODCALLTYPE CClassFactory::Release()
{
  if (--m_cRef) return m_cRef;
  delete this;
  return 0L;
}

// Implémentation de la méthode CreateInstance() de l'interface IClassFactory:
HRESULT STDMETHODCALLTYPE CClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid,LPVOID *ppvObj)
{
  *ppvObj = NULL;
  if (pUnkOuter)  return CLASS_E_NOAGGREGATION;
  CContextMenu* pContextMenu = new CContextMenu();
  if (!pContextMenu) return E_OUTOFMEMORY;
  return pContextMenu->QueryInterface(riid, ppvObj);
}

// Implémentation de la méthode LockServer() de l'interface IClassFactory:
HRESULT STDMETHODCALLTYPE CClassFactory::LockServer(BOOL fLock)
{
  return S_OK;
}

// Implémentation de la fonction exportée DllGetClassObject:
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
  *ppvOut = NULL;
  if (IsEqualGUID(rclsid, CLSID_ShellExtension))
  {
    CClassFactory *pcf = new CClassFactory;
    return pcf->QueryInterface(riid, ppvOut);
  }
  return CLASS_E_CLASSNOTAVAILABLE;
}

// Implémentation de la fonction exportée DllCanUnloadNow:
STDAPI DllCanUnloadNow(void)
{
  if(g_cRefDll == 0) return S_OK;
  return  S_FALSE;
}

// Implémentation de la fonction exportée DllRegisterServer:
STDAPI DllRegisterServer(void)
{
  // Ajouter le CLSID de notre DLL COM à la base de registres:
  {
    extString key = extStringConverter::format("HKEY_CLASSES_ROOT\\CLSID\\%s", CLSID_ShellExtensionStr);
    if (!extRegistry::createKey(key.c_str()))
      return S_FALSE;
    if (!extRegistry::setValue(key.c_str(), "", ShellExtensionDescription))
      return S_FALSE;
  }

  // Définir le chemin et les paramètres de notre DLL COM:
  {
    extString key = extStringConverter::format("HKEY_CLASSES_ROOT\\CLSID\\%s\\InProcServer32", CLSID_ShellExtensionStr);
    if (!extRegistry::createKey(key.c_str()))
      return S_FALSE;
    if (!extRegistry::setValue(key.c_str(), "", GetCurrentModulePath() ))
      return S_FALSE;
    if (!extRegistry::setValue(key.c_str(), "ThreadingModel", "Apartment"))
      return S_FALSE;
  }

  // Enregistrer notre "Extension Shell" pour tous les types de fichiers:
  {
    extString key = extStringConverter::format("HKEY_CLASSES_ROOT\\*\\shellex\\ContextMenuHandlers\\%s", ShellExtensionName);
    if (!extRegistry::createKey(key.c_str()))
      return S_FALSE;
    if (!extRegistry::setValue(key.c_str(), "", CLSID_ShellExtensionStr))
      return S_FALSE;
  }

  // Enregistrer notre "Extension Shell" pour les dossiers:
  {
    extString key = extStringConverter::format("HKEY_CLASSES_ROOT\\Directory\\shellex\\ContextMenuHandlers\\%s", ShellExtensionName);
    if (!extRegistry::createKey(key.c_str()))
      return S_FALSE;
    if (!extRegistry::setValue(key.c_str(), "", CLSID_ShellExtensionStr))
      return S_FALSE;
  }

  // Notifier le Shell pour qu'il prenne en compte la définition de l'icone par défaut:
  SHChangeNotify(SHCNE_ASSOCCHANGED,0,0,0);

  // Enregistrer notre "Extension Shell" pour le bureau ou l'arrière-plan d'une fenêtre de l'explorateur:
  {
    extString key = extStringConverter::format("HKEY_CLASSES_ROOT\\Directory\\Background\\ShellEx\\ContextMenuHandlers\\%s", ShellExtensionName);
    if (!extRegistry::createKey(key.c_str()))
      return S_FALSE;
    if (!extRegistry::setValue(key.c_str(), "", CLSID_ShellExtensionStr))
      return S_FALSE;
  }

  // Ajouter notre "Extension Shell" à la liste approuvée du système:
  {
    extString key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved";
    if (!extRegistry::createKey(key.c_str()))
      return S_FALSE;
    if (!extRegistry::setValue(key.c_str(), CLSID_ShellExtensionStr, ShellExtensionDescription))
      return S_FALSE;
  }

  return S_OK;
}

// Implémentation de la fonction exportée DllUnregisterServer:
STDAPI DllUnregisterServer(void)
{
  // Retirer notre "Extension Shell" de la liste approuvée du système:
  {
    extString key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved";
    if (!extRegistry::deleteValue(key.c_str(), CLSID_ShellExtensionStr))
      return S_FALSE;
  }

  // Retirer notre "Extension Shell" du bureau et de l'arrière-plan des fenêtres de l'explorateur:
  {
    extString key = extStringConverter::format("HKEY_CLASSES_ROOT\\Directory\\Background\\ShellEx\\ContextMenuHandlers\\%s", ShellExtensionName);
    if (!extRegistry::deleteKey(key.c_str()))
      return S_FALSE;
  }

  // Notifier le Shell pour qu'il prenne en compte la suppression de l'icone par défaut:
  SHChangeNotify(SHCNE_ASSOCCHANGED,0,0,0);

  // Retirer notre "Extension Shell" des dossiers:
  {
    extString key = extStringConverter::format("HKEY_CLASSES_ROOT\\Directory\\shellex\\ContextMenuHandlers\\%s", ShellExtensionName);
    if (!extRegistry::deleteKey(key.c_str()))
      return S_FALSE;
  }

  // Retirer notre "Extension Shell" pour tous les types de fichiers:
  {
    extString key = extStringConverter::format("HKEY_CLASSES_ROOT\\*\\shellex\\ContextMenuHandlers\\%s", ShellExtensionName);
    if (!extRegistry::deleteKey(key.c_str()))
      return S_FALSE;
  }

  // Retirer le CLSID de notre DLL COM de la base de registres:
  {
    extString key = extStringConverter::format("HKEY_CLASSES_ROOT\\CLSID\\%s\\InProcServer32", CLSID_ShellExtensionStr);
    if (!extRegistry::deleteKey(key.c_str()))
      return S_FALSE;
  }

  {
    extString key = extStringConverter::format("HKEY_CLASSES_ROOT\\CLSID\\%s", CLSID_ShellExtensionStr);
    if (!extRegistry::deleteKey(key.c_str()))
      return S_FALSE;
  }

  return S_OK;
}

// La fonction d'entrée DllMain:
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
