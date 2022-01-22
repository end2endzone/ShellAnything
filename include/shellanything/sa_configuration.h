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

#ifndef SA_API_CONFIGURATION_H
#define SA_API_CONFIGURATION_H

#include <stdint.h>
#include "shellanything/sa_error.h"
#include "shellanything/sa_string.h"
#include "shellanything/sa_context.h"
#include "shellanything/sa_menu.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sa_configuration_t  {
  void* opaque;
} sa_configuration_t;

typedef struct sa_configuration_immutable_t  {
  void* opaque;
} sa_configuration_immutable_t;

/// <summary>
/// Converts a mutable configuration to an immutable configuration.
/// </summary>
/// <param name="icon">The mutable object to convert.</param>
/// <returns>Returns an immutable configuration</returns>
sa_configuration_immutable_t sa_configuration_to_immutable(sa_configuration_t* context);

/// <summary>
/// Load a configuration file
/// </summary>
/// <param name="path">Path to the configuration file</param>
/// <param name="error_buffer">A buffer for storing the error.</param>
/// <param name="error_size">Size of the error buffer in bytes. If the buffer is too small. The error is truncated.</param>
/// <param name="configuration">The output configuration structure object</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_configuration_load_file_buffer(const char * path, char * error_buffer, size_t error_size, sa_configuration_t* configuration);

/// <summary>
/// Load a configuration file
/// </summary>
/// <param name="path">Path to the configuration file</param>
/// <param name="error_str">The reported error string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <param name="configuration">The output configuration structure object</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_configuration_load_file_string(const char* path, sa_string_t* error_str, sa_configuration_t* configuration);

/// <summary>
/// Detect if a given file is a valid configuration file.
/// </summary>
/// <param name="path">The file path to load</param>
/// <returns>Returns 1 if the file is a valid configuration file. Returns 0 otherwise.</returns>
int sa_configuration_is_valid_config_file(const char* path);

/// <summary>
/// Get the file path of this configuration.
/// </summary>
/// <param name="configuration">The configuration structure object.</param>
/// <param name="length">The length of the output value in bytes.</param>
/// <param name="buffer">The output buffer for the value.</param>
/// <param name="size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_configuration_get_file_path_buffer(sa_configuration_immutable_t* configuration, int* length, char* buffer, size_t size);

/// <summary>
/// Get the file path of this configuration.
/// </summary>
/// <param name="configuration">The configuration structure object.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_configuration_get_file_path_string(sa_configuration_immutable_t* configuration, sa_string_t* str);

/// <summary>
/// Set the file path of this configuration.
/// </summary>
/// <param name="configuration">The configuration structure object.</param>
/// <param name="name">The file path for the configuration.</param>
void sa_configuration_set_file_path(sa_configuration_t* configuration, const char* path);

/// <summary>
/// Get the file modified date parameter.
/// </summary>
/// <param name="configuration">The configuration structure object.</param>
/// <returns>Return 1 if the menu is visible. Returns 0 otherwise.</returns>
uint64_t sa_configuration_get_file_modified_date(sa_configuration_immutable_t* configuration);

/// <summary>
/// Set the file modified date parameter
/// </summary>
/// <param name="configuration">The configuration structure object.</param>
/// <param name="newdate">The new parameter value</param>
void sa_configuration_set_file_modified_date(sa_configuration_t* configuration, uint64_t newdate);

/// <summary>
/// Recursively calls sa_menu_update() on all menus loaded by the configuration manager.
/// </summary>
/// <param name="configuration">The configuration structure object.</param>
/// <param name="ctx">The context used for updating the menu.</param>
void sa_configuration_update(sa_configuration_t* configuration, sa_context_immutable_t* ctx);

/// <summary>
/// Get how many menu in the configuration.
/// </summary>
/// <param name="configuration">The configuration structure object.</param>
/// <returns>Returns how many sub menus are under this menu</returns>
size_t sa_configuration_get_menu_count(sa_configuration_t* configuration);

/// <summary>
/// Get a menu in the configuration.
/// </summary>
/// <param name="configuration">The configuration structure object.</param>
/// <param name="index">The sub menu index</param>
/// <param name="menu">The output menu structure object</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_configuration_get_menu_element(sa_configuration_t* configuration, size_t index, sa_menu_t* menu);

#ifdef __cplusplus
}
#endif

#endif //SA_API_CONFIGURATION_H
