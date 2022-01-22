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

#ifndef SA_API_CFGMGR_H
#define SA_API_CFGMGR_H

#include <stdint.h>
#include "shellanything/sa_error.h"
#include "shellanything/sa_configuration.h"
#include "shellanything/sa_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Get how many configurations are loaded in the configuration manager.
/// </summary>
/// <returns>Returns how many configurations are in the configuration manager</returns>
size_t sa_cfgmgr_get_configuration_count();

/// <summary>
/// Get a loaded configuration in the configuration manager.
/// </summary>
/// <param name="index">The configuration index</param>
/// <param name="configuration">The output menu structure object</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_cfgmgr_get_configuration_element(size_t index, sa_configuration_t* configuration);

/// <summary>
/// Returns 1 if the given path is a loaded configuration by the manager.
/// </summary>
/// <param name="path">The path of a Configuration file</param>
/// <returns>Returns 1 if the given path is a loaded configuration by the manager. Returns 0 otherwise.</returns>
int sa_cfgmgr_is_configuration_file_loaded(const char * path);

/// <summary>
/// Clears the configuration manager of all loaded configuration
/// </summary>
void sa_cfgmgr_clear();

/// <summary>
/// Refresh the content of the configuration manager:
/// * Reload configuration files that were modified.
/// * Deleted loaded configurations whose file are missing.
/// * Discover new unloaded configuration files.
/// </summary>
void sa_cfgmgr_refresh();

/// <summary>
/// Updates all configuration files loaded by the manager.
/// </summary>
/// <param name="ctx">The context used for updating the configuration files.</param>
void sa_cfgmgr_update(sa_context_immutable_t* ctx);

/// <summary>
/// Clears all the registered search paths
/// </summary>
void sa_cfgmgr_clear_search_path();

/// <summary>
/// Add a new search path to the path list.
/// </summary>
/// <param name="path">The path to add to the search list.</param>
void sa_cfgmgr_add_search_path(const char * path);

#ifdef __cplusplus
}
#endif

#endif //SA_API_CFGMGR_H
