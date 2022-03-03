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

#ifndef SA_API_PLUGINS_H
#define SA_API_PLUGINS_H

#include <stdint.h>
#include "sa_error.h"
#include "sa_selection_context.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int major;
  int minor;
  int patch;
} sa_version_info_t;

/// <summary>
/// Defines all flags for custom validation attributes
/// </summary>
#define SA_ATTRIBUTE_FLAG_NONE 0
#define SA_ATTRIBUTE_FLAG_INVERSED 1

/// <summary>
/// Defines the name of a plugin entry point for initialization.
/// </summary>
#define SA_PLUGIN_INITIALIZATION_FUNCTION_NAME sa_plugin_initialize

/// <summary>
/// Defines the name of a plugin entry point for initialization.
/// </summary>
#define SA_PLUGIN_REGISTER_FUNCTION_NAME sa_plugin_register

/// <summary>
/// Type definition for a plugin's initialization entry point.
/// </summary>
/// <remarks>
/// ShellAnything is expecting a function whose name matches macro
/// SA_PLUGIN_INITIALIZATION_FUNCTION_NAME and function signature
/// matches this type definition.
/// </remarks>
/// <param name="version">The version of the application.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
typedef sa_error_t(*sa_plugin_initialize_func)(sa_version_info_t* /*version*/);

/// <summary>
/// Function pointer definition for plugins registration function.
/// </summary>
/// <remarks>
/// ShellAnything is expecting a function whose name matches macro
/// SA_PLUGIN_REGISTER_FUNCTION_NAME and function signature
/// matches this type definition.
/// </remarks>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
typedef sa_error_t(*sa_plugin_register_func)();

/// <summary>
/// Function pointer definition for validating a custom attributes.
/// </summary>
/// <param name="names">The names of the attributes as an array of strings.</param>
/// <param name="values">The values of the attributes as an array of strings.</param>
/// <param name="flags">The flags of the attributes. See macros that starts with SA_ATTRIBUTE_FLAG_.</param>
/// <param name="count">Defines how many elements are in the names, values and flags arrays.</param>
/// <returns>Returns 1 on validation success. Returns NULL on error.</returns>
typedef int (*sa_plugin_attribute_validate_func)(sa_selection_context_immutable_t* ctx, const char** /*names*/, const char** /*values*/, const int* /*flags*/, size_t /*count*/);

/// <summary>
/// Register a custom attribute validation function for a given list of attributes.
/// </summary>
/// <param name="names">The names of the attributes as an array of strings.</param>
/// <param name="count">Defines how many elements are in the names array.</param>
/// <param name="func">A function pointer which definition matches sa_plugin_attribute_validate_func.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_plugins_register_attribute_validation(const char* names[], size_t count, sa_plugin_attribute_validate_func func);

#ifdef __cplusplus
}
#endif

#endif //SA_API_MEMORY_H
