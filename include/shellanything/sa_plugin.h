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

#ifndef SA_API_PLUGIN_H
#define SA_API_PLUGIN_H

#include <stdint.h>
#include "shellanything/sa_object_types.h"
#include "shellanything/sa_plugin_definitions.h"
#include "shellanything/sa_error.h"
#include "shellanything/sa_selection_context.h"
#include "shellanything/sa_property_store.h"
#include "shellanything/sa_plugin_definitions.h"

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Function pointer definition for validating a custom attributes.
/// </summary>
/// <param name="ctx">The selection context that identifies which files and directories is selected.</param>
/// <param name="store">The property store which contains the attributes for validation.</param>
/// <returns>Returns 1 on validation success. Returns NULL on error.</returns>
typedef int (*sa_plugin_attribute_validate_func)(sa_selection_context_immutable_t* ctx, sa_property_store_immutable_t* store);

/// <summary>
/// Register a custom attribute validation function for a given list of attributes.
/// </summary>
/// <param name="names">The names of the attributes as an array of strings.</param>
/// <param name="count">Defines how many elements are in the names array.</param>
/// <param name="func">A function pointer which definition matches sa_plugin_attribute_validate_func.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_plugin_register_attribute_validation(const char* names[], size_t count, sa_plugin_attribute_validate_func func);

#ifdef __cplusplus
}
#endif

#endif //SA_API_PLUGIN_H
