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

#ifndef SA_API_PLUGIN_DEFINITIONS_H
#define SA_API_PLUGIN_DEFINITIONS_H

#include "shellanything/sa_error.h"

#ifdef __cplusplus
extern "C" {
#if 0
}  // do not indent code inside extern C
#endif
#endif

typedef struct
{
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
#define SA_PLUGIN_INITIALIZE_FUNCTION_NAME sa_plugin_initialize

/// <summary>
/// Defines the name of a plugin entry point for terminate.
/// </summary>
#define SA_PLUGIN_TERMINATE_FUNCTION_NAME sa_plugin_terminate

/// <summary>
/// Defines the name of a plugin entry point for initialization.
/// </summary>
#define SA_PLUGIN_REGISTER_FUNCTION_NAME sa_plugin_register

/// <summary>
/// Type definition for a plugin's initialization entry point.
/// </summary>
/// <remarks>
/// ShellAnything is expecting a function whose name matches macro
/// SA_PLUGIN_INITIALIZE_FUNCTION_NAME and function signature
/// matches this type definition.
/// </remarks>
/// <param name="version">The version of the application.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
typedef sa_error_t(*sa_plugin_initialize_func)(sa_version_info_t* version);

/// <summary>
/// Type definition for a plugin's terminate entry point.
/// </summary>
/// <remarks>
/// ShellAnything is expecting a function whose name matches macro
/// SA_PLUGIN_TERMINATE_FUNCTION_NAME and function signature
/// matches this type definition.
/// </remarks>
/// <param name="version">The version of the application.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
typedef sa_error_t(*sa_plugin_terminate_func)();

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

#ifdef __cplusplus
#if 0
{  // do not indent code inside extern C
#endif
}  // extern "C"
#endif

#endif //SA_API_PLUGIN_DEFINITIONS_H
