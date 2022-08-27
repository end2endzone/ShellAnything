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

#include "shellanything/version.h"
#include "Plugin.h"
#include "PropertyManager.h"
#include "Validator.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

#include "rapidassist/strings.h"
#include "rapidassist/errors.h"
#include "rapidassist/environment.h"
#include "rapidassist/filesystem.h"

#include "shellanything\sa_plugin_definitions.h"
#include "../api/sa_error.cpp"  // to get a local implementation for sa_error_get_error_description()

#include <Windows.h>

#define xstr(a) str(a)
#define str(a) #a

namespace shellanything
{
  struct Plugin::ENTRY_POINTS
  {
    HMODULE hModule;
    sa_plugin_initialize_func initialize_func;
    sa_plugin_register_func register_func;
  };

  Plugin* gLoadingPlugin = NULL;

  Plugin* Plugin::GetLoadingPlugin()
  {
    return gLoadingPlugin;
  }

  Plugin::Plugin() :
    mParentConfiguration(NULL),
    mLoaded(false),
    mEntryPoints(new Plugin::ENTRY_POINTS)
  {
    memset(mEntryPoints, 0, sizeof(Plugin::ENTRY_POINTS));
  }

  Plugin::Plugin(const Plugin& p)
  {
    (*this) = p;
  }

  Plugin::~Plugin()
  {
    Unload();

    if (mEntryPoints)
      delete mEntryPoints;
    mEntryPoints = NULL;
  }

  const Plugin& Plugin::operator =(const Plugin& p)
  {
    if (this != &p)
    {
      mPath = p.mPath;
      mDescription = p.mDescription;
      mConditions = p.mConditions;
      mActions = p.mActions;

      // do not copy loaded properties this is instance specific.
      // mEntryPoints skipped on purpose for hModule safety
      // mRegistry skipped on purpose
      mLoaded = false;
    }
    return (*this);
  }

  Configuration* Plugin::GetParentConfiguration()
  {
    return mParentConfiguration;
  }

  const Configuration* Plugin::GetParentConfiguration() const
  {
    return mParentConfiguration;
  }

  void Plugin::SetParentConfiguration(Configuration* configuration)
  {
    mParentConfiguration = configuration;
  }

  const std::string& Plugin::GetPath() const
  {
    return mPath;
  }

  void Plugin::SetPath(const std::string& path)
  {
    mPath = path;
  }

  const std::string& Plugin::GetDescription() const
  {
    return mDescription;
  }

  void Plugin::SetDescription(const std::string& description)
  {
    mDescription = description;
  }

  const std::string& Plugin::GetConditions() const
  {
    return mConditions;
  }

  void Plugin::SetConditions(const std::string& conditions)
  {
    mConditions = conditions;
  }

  const std::string& Plugin::GetActions() const
  {
    return mActions;
  }

  void Plugin::SetActions(const std::string& actions)
  {
    mActions = actions;
  }

  bool Plugin::SupportCondition(const std::string& name)
  {
    PluginPtrList tmp;
    tmp.push_back(this);
    bool support = (Plugin::FindPluginByConditionName(tmp, name) == this);
    return support;
  }

  bool Plugin::SupportAction(const std::string& name)
  {
    PluginPtrList tmp;
    tmp.push_back(this);
    bool support = (Plugin::FindPluginByActionName(tmp, name) == this);
    return support;
  }

  bool Plugin::IsLoaded() const
  {
    return mLoaded;
  }

  bool Plugin::Load()
  {
    PropertyManager& pmgr = PropertyManager::GetInstance();
    std::string path = pmgr.Expand(mPath);

    LOG(INFO) << "Loading plugin '" << path << "'.";
    HMODULE hModule = LoadLibrary(path.c_str());

    // Check for a debug plugin. This is specific to ShellAnything's plugins.
    if (ra::environment::IsConfigurationDebug() && !hModule)
    {
      // try to also search for the debug plugin
      std::string path2 = path;
      std::string extension_before = ra::filesystem::GetFileExtention(path2);
      std::string extension_after = extension_before;
      extension_before.insert(0, ".");
      extension_after.insert(0, "-d.");
      ra::strings::Replace(path2, extension_before, extension_after);

      //try to load the plugin again with the debug filename
      hModule = LoadLibrary(path2.c_str());
    }

    if (!hModule)
    {
      ra::errors::errorcode_t error_code = ra::errors::GetLastErrorCode();
      std::string error_str = ra::errors::GetErrorCodeDescription(error_code);
      LOG(ERROR) << "Error code " << ra::strings::Format("0x%x", error_code) << ", " << error_str;
      return false;
    }

    //search for entry points
    sa_plugin_initialize_func initialize_func = (sa_plugin_initialize_func)GetProcAddress(hModule, xstr(SA_PLUGIN_INITIALIZATION_FUNCTION_NAME));
    if (initialize_func == NULL)
    {
      FreeLibrary(hModule);
      LOG(ERROR) << "Missing entry point '" << xstr(SA_PLUGIN_INITIALIZATION_FUNCTION_NAME) << "' in plugin '" << path << "'.";
      return false;
    }

    sa_plugin_register_func register_func = (sa_plugin_register_func)GetProcAddress(hModule, xstr(SA_PLUGIN_REGISTER_FUNCTION_NAME));
    if (register_func == NULL)
    {
      FreeLibrary(hModule);
      LOG(ERROR) << "Missing entry point '" << xstr(SA_PLUGIN_REGISTER_FUNCTION_NAME) << "' in plugin '" << path << "'.";
      return false;
    }

    // remember this plugin while loading. This is required for plugins that registers features through the API.
    gLoadingPlugin = this;

    //initialize & register the plugin
    sa_version_info_t version;
    version.major = SHELLANYTHING_VERSION_MAJOR;
    version.minor = SHELLANYTHING_VERSION_MINOR;
    version.patch = SHELLANYTHING_VERSION_PATCH;
    sa_error_t initialized_error = initialize_func(&version);
    if (initialized_error != SA_ERROR_SUCCESS)
    {
      const char* initialized_error_str = sa_error_get_error_description(initialized_error);
      LOG(WARNING) << "The plugin '" << path << "' has failed to initialize. Error code: " << ra::strings::Format("0x%x", initialized_error) << ", " << initialized_error_str;
      gLoadingPlugin = NULL;
      return false;
    }
    sa_error_t register_error = register_func();
    if (register_error != SA_ERROR_SUCCESS)
    {
      const char* register_error_str = sa_error_get_error_description(register_error);
      LOG(WARNING) << "The plugin '" << path << "' has failed to register a service. Error code: " << ra::strings::Format("0x%x", register_error) << ", " << register_error_str;
      gLoadingPlugin = NULL;
      return false;
    }
    gLoadingPlugin = NULL;

    // this plugin is valid.
    this->mEntryPoints->hModule = hModule;
    this->mEntryPoints->initialize_func = initialize_func;
    this->mEntryPoints->register_func = register_func;
    mLoaded = true;

    // validate declared conditions all have an assigned validator
    ra::strings::StringVector conditions;
    PropertyManager::SplitAndExpand(mConditions, SA_CONDITIONS_ATTR_SEPARATOR_STR, conditions);
    for (size_t i = 0; i < conditions.size(); i++)
    {
      const std::string& condition = conditions[i];
      IAttributeValidator* validator = mRegistry.GetAttributeValidatorFromName(condition);
      if (validator == NULL)
      {
        LOG(WARNING) << "The plugin '" << path << "' is declaring condition '" << condition << "' but did not registered any validator function.";
      }
    }

    return true;
  }

  bool Plugin::Unload()
  {
    if (mEntryPoints->hModule != 0)
    {
      mRegistry.Clear();

      FreeLibrary(mEntryPoints->hModule);
      memset(mEntryPoints, 0, sizeof(Plugin::ENTRY_POINTS));
      mLoaded = false;

      return true;
    }

    return false;
  }

  Registry& Plugin::GetRegistry()
  {
    return mRegistry;
  }

  Plugin* Plugin::FindPluginByConditionName(const PluginPtrList& plugins, const std::string& name)
  {
    PropertyManager& pmgr = PropertyManager::GetInstance();
    for (size_t i = 0; i < plugins.size(); i++)
    {
      Plugin* plugin = plugins[i];
      if (!plugin)
        continue;
      const std::string expanded = pmgr.Expand(plugin->GetConditions());
      const ra::strings::StringVector values = ra::strings::Split(expanded, SA_CONDITIONS_ATTR_SEPARATOR_STR);
      for (size_t j = 0; j < values.size(); j++)
      {
        const std::string& value = values[j];
        if (value == name)
          return plugin;
      }
    }
    return NULL;
  }

  Plugin* Plugin::FindPluginByActionName(const PluginPtrList& plugins, const std::string& name)
  {
    PropertyManager& pmgr = PropertyManager::GetInstance();
    for (size_t i = 0; i < plugins.size(); i++)
    {
      Plugin* plugin = plugins[i];
      if (!plugin)
        continue;
      const std::string expanded = pmgr.Expand(plugin->GetActions());
      const ra::strings::StringVector values = ra::strings::Split(expanded, SA_ACTIONS_ATTR_SEPARATOR_STR);
      for (size_t j = 0; j < values.size(); j++)
      {
        const std::string& value = values[j];
        if (value == name)
          return plugin;
      }
    }
    return NULL;
  }

} //namespace shellanything