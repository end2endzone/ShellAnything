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

#include "shellanything/sa_cfgmgr.h"
#include "ConfigManager.h"
#include "sa_types_private.h"
#include "sa_string_private.h"

using namespace shellanything;

size_t sa_cfgmgr_get_configuration_count()
{
  Configuration::ConfigurationPtrList configs = ConfigManager::GetInstance().GetConfigurations();
  size_t count = configs.size();
  return count;
}

sa_error_t sa_cfgmgr_get_configuration_element(size_t index, sa_configuration_t* configuration)
{
  if (configuration == NULL)
    return SA_ERROR_INVALID_ARGUMENTS;
  Configuration::ConfigurationPtrList configs = ConfigManager::GetInstance().GetConfigurations();
  if (configs.empty() || index > (configs.size() - 1))
    return SA_ERROR_VALUE_OUT_OF_BOUNDS;
  shellanything::Configuration* config_element = configs[index];
  *configuration = AS_TYPE_CONFIGURATION(config_element);
  return SA_ERROR_SUCCESS;
}

int sa_cfgmgr_is_configuration_file_loaded(const char* path)
{
  bool loaded = ConfigManager::GetInstance().IsConfigFileLoaded(path);
  if (loaded)
    return 1;
  return 0;
}

void sa_cfgmgr_clear()
{
  ConfigManager::GetInstance().Clear();
}

void sa_cfgmgr_refresh()
{
  ConfigManager::GetInstance().Refresh();
}

void sa_cfgmgr_update(sa_context_immutable_t* ctx)
{
  const Context* context = AS_CLASS_CONTEXT(ctx);
  ConfigManager::GetInstance().Update(*context);
}

void sa_cfgmgr_clear_search_path()
{
  ConfigManager::GetInstance().ClearSearchPath();
}

void sa_cfgmgr_add_search_path(const char* path)
{
  ConfigManager::GetInstance().AddSearchPath(path);
}
