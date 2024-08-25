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
#include "utils.h"

#include "ErrorManager.h"
#include "Win32Registry.h"
#include "Win32Utils.h"
#include "GlogUtils.h"
#include "SaUtils.h"

#include "rapidassist/undef_windows_macros.h"
#include "rapidassist/strings.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/process_utf8.h"
#include "rapidassist/errors.h"
#include "rapidassist/user_utf8.h"
#include "rapidassist/unicode.h"
#include "rapidassist/environment.h"

#include "shellanything/version.h"
#include "shellanything/config.h"

#include "ConfigManager.h"
#include "PropertyManager.h"
#include "LoggerHelper.h"

#include <assert.h>
#include <ShlObj.h> // for IID_IShellExtInit, IID_IContextMenu
#include "shellext.h" // for CLSID_ShellAnything

std::string GuidToString(GUID guid)
{
  char buffer[64];
  sprintf_s(buffer, sizeof(buffer), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
  return buffer;
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

  if (IsEqualGUID(guid, CLSID_ShellAnything))       return "ShellAnything";

  //unknown GUID, return the string representation:
  //ie: CLSID_UNDOCUMENTED_01, {924502A7-CC8E-4F60-AE1F-F70C0A2B7A7C}
  return GuidToString(guid);
}

std::string GetProcessContextDesc()
{
  std::string desc;
  desc.reserve(32);
  desc += "pid=";
  desc += ra::strings::ToString(ra::process::GetCurrentProcessId());
  desc += " tid=";
  desc += ra::strings::ToString((uint32_t)GetCurrentThreadId());
  return desc;
}

template <class T> class FlagDescriptor
{
public:
  struct FLAGS
  {
    T value;
    const char* name;
  };

  static std::string ToBitString(T value, const FLAGS* flags)
  {
    std::string desc;

    size_t index = 0;
    while (flags[index].name)
    {
      const T& flag = flags[index].value;
      const char* name = flags[index].name;

      // Test special case where a flag has no bit set (flag == 0x00)
      // The only time where it should be reported is when the value is also 0.
      if (flag == 0)
      {
        if (flag == value)
        {
          desc = name;
          break;
        }
      }
      else
      {
        //if flag is set
        if ((value & flag) == flag)
        {
          if (!desc.empty())
            desc.append("|");
          desc.append(name);
        }
      }

      //next flag
      index++;
    }
    return desc;
  }

  static std::string ToValueString(T value, const FLAGS* flags)
  {
    std::string desc;

    size_t index = 0;
    while (flags[index].name)
    {
      const T& flag = flags[index].value;
      const char* name = flags[index].name;

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

std::string GetQueryContextMenuFlags(UINT flags)
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
  return flags_str;
}

std::string GetGetCommandStringFlags(UINT flags)
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
  std::string flags_str = FlagDescriptor<UINT>::ToBitString(flags, descriptors);
  return flags_str;
}

void LogEnvironment()
{
  SA_LOG(INFO) << "Enabling logging";

  int windows_major = 0, windows_minor = 0;
  Win32Utils::GetWindowsVersion(windows_major, windows_minor);
  SA_LOG(INFO) << "Windows version " << windows_major << "." << windows_minor << "\n";
  SA_LOG(INFO) << "Windows product name: " << Win32Utils::GetWindowsProductName() << "\n";

  SA_LOG(INFO) << "Process id: " << ra::strings::ToString(ra::process::GetCurrentProcessId());
  SA_LOG(INFO) << "Thread id: " << ra::strings::ToString((uint32_t)GetCurrentThreadId());
  SA_LOG(INFO) << "DLL path: " << GetCurrentModulePathUtf8();
  SA_LOG(INFO) << "EXE path: " << ra::process::GetCurrentProcessPathUtf8().c_str();

  SA_LOG(INFO) << "System metrics:";
  SA_LOG(INFO) << "SM_CXSCREEN    : " << GetSystemMetrics(SM_CXSCREEN);
  SA_LOG(INFO) << "SM_CYSCREEN    : " << GetSystemMetrics(SM_CYSCREEN);
  SA_LOG(INFO) << "SM_CXSMICON    : " << GetSystemMetrics(SM_CXSMICON);
  SA_LOG(INFO) << "SM_CYSMICON    : " << GetSystemMetrics(SM_CYSMICON);
  SA_LOG(INFO) << "SM_CXICON      : " << GetSystemMetrics(SM_CXICON);
  SA_LOG(INFO) << "SM_CYICON      : " << GetSystemMetrics(SM_CYICON);

  bool monitor_dpi_aware = Win32Utils::IsMonitorDpiAwarenessEnabled();
  SA_LOG(INFO) << "Process is monitor DPI aware : " << ra::strings::ToString(monitor_dpi_aware);
  SA_LOG(INFO) << "System DPI     : " << Win32Utils::GetSystemDPI();
  SA_LOG(INFO) << "System Scaling : " << Win32Utils::GetSystemScalingPercent() << "%";

  // Print information about monitor and their scaling
  int monitor_count = Win32Utils::GetMonitorCount();
  SA_LOG(INFO) << "System monitor count : " << monitor_count;
  for (int i = 0; i < monitor_count; i++)
  {
    int dpi = Win32Utils::GetMonitorDPI(i);
    int scaling = Win32Utils::GetMonitorScalingPercent(i);
    SA_LOG(INFO) << "Monitor " << i << " is scaled to " << scaling << "% (dpi " << dpi << ")";
  }

  SA_LOG(INFO) << "IID_IUnknown      : " << GuidToString(IID_IUnknown).c_str();
  SA_LOG(INFO) << "IID_IClassFactory : " << GuidToString(IID_IClassFactory).c_str();
  SA_LOG(INFO) << "IID_IShellExtInit : " << GuidToString(IID_IShellExtInit).c_str();
  SA_LOG(INFO) << "IID_IContextMenu  : " << GuidToString(IID_IContextMenu).c_str();
  SA_LOG(INFO) << "IID_IContextMenu2 : " << GuidToString(IID_IContextMenu2).c_str();  //{000214f4-0000-0000-c000-000000000046}
  SA_LOG(INFO) << "IID_IContextMenu3 : " << GuidToString(IID_IContextMenu3).c_str();  //{BCFCE0A0-EC17-11d0-8D10-00A0C90F2719}
}

void DebugHook(const char* fname)
{
  std::string text;
  text += fname;
  text += "()\n";

  uint32_t pid = GetCurrentProcessId();
  uint32_t tid = (uint32_t)GetCurrentThreadId();

  text += "pid=";
  text += ra::strings::ToString(pid);
  text += " (";
  text += ra::strings::Format("0x%x", pid);
  text += ")\n";

  text += "tid=";
  text += ra::strings::ToString(tid);
  text += " (";
  text += ra::strings::Format("0x%x", tid);
  text += ")\n";

  std::string process_path = ra::process::GetCurrentProcessPath();
  std::string process_filename = ra::filesystem::GetFilename(process_path.c_str());
  std::string caption;
  caption += "ShellAnything's debuging hook";
  caption += ", ";
  caption += process_filename;
  caption += ", ";
  caption += ra::strings::ToString(pid);

  MessageBoxA(NULL, text.c_str(), caption.c_str(), MB_OK | MB_ICONEXCLAMATION);
}

bool IsFileExplorerProcess()
{
  const std::string path = ra::process::GetCurrentProcessPathUtf8();
  const std::string filename = ra::filesystem::GetFilename(path.c_str());
  const std::string filename_upper = ra::strings::Uppercase(filename);
  if (filename_upper == "EXPLORER.EXE")
    return true;
  return false;
}

bool IsRegsvr32Process()
{
  const std::string path = ra::process::GetCurrentProcessPathUtf8();
  const std::string filename = ra::filesystem::GetFilename(path.c_str());
  const std::string filename_upper = ra::strings::Uppercase(filename);
  if (filename_upper == "REGSVR32.EXE")
    return true;
  return false;
}
