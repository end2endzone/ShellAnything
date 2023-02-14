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
#include "shellext.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

#include "ErrorManager.h"
#include "Win32Registry.h"
#include "Win32Utils.h"
#include "GlogUtils.h"

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
#include "SaUtils.h"

#include <assert.h>

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

bool IsFirstApplicationRun(const std::string& name, const std::string& version)
{
  std::string key = ra::strings::Format("HKEY_CURRENT_USER\\Software\\%s\\%s", name.c_str(), version.c_str());
  if (!Win32Registry::CreateKey(key.c_str(), NULL))
  {
    // unable to get to the application's key.
    // assume it is not the first run.
    return false;
  }

  static const char* FIRST_RUN_VALUE_NAME = "first_run";

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

void InstallDefaultConfigurations(const std::string& config_dir)
{
  std::string app_path = GetCurrentModulePathUtf8();
  std::string app_dir = ra::filesystem::GetParentPath(app_path);

  static const char* default_files[] = {
    "default.xml",
    "Microsoft Office 2003.xml",
    "Microsoft Office 2007.xml",
    "Microsoft Office 2010.xml",
    "Microsoft Office 2013.xml",
    "Microsoft Office 2016.xml",
    "shellanything.xml",
    "WinDirStat.xml",
  };
  static const size_t num_files = sizeof(default_files) / sizeof(default_files[0]);

  LOG(INFO) << "First application launch. Installing default configurations files.";

  for (size_t i = 0; i < num_files; i++)
  {
    const char* filename = default_files[i];
    std::string source_path = app_dir + "\\configurations\\" + filename;
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
  shellanything::ConfigManager& cmgr = shellanything::ConfigManager::GetInstance();

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

  std::string prop_log_directory = ra::unicode::AnsiToUtf8(shellanything::GetLogDirectory());

  //define global properties
  shellanything::PropertyManager& pmgr = shellanything::PropertyManager::GetInstance();
  pmgr.SetProperty("log.directory", prop_log_directory);
  pmgr.SetProperty("config.directory", config_dir);
  pmgr.SetProperty("home.directory", home_dir);
}
