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

#ifndef SA_API_CONTEXT_H
#define SA_API_CONTEXT_H

#include "shellanything/sa_string.h"
#include "shellanything/sa_error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  void* opaque;
} sa_context_t;

typedef struct {
  void* opaque;
} sa_context_immutable_t;

/// <summary>
/// Converts a mutable context to an immutable context.
/// </summary>
/// <param name="icon">The mutable object to convert.</param>
/// <returns>Returns an immutable icon</returns>
sa_context_immutable_t sa_context_to_immutable(sa_context_t* context);

/// <summary>
/// Registers properties associated with this context.
/// </summary>
/// <param name="ctx">The context structure object.</param>
void sa_context_register_properties(sa_context_immutable_t* ctx);

/// <summary>
/// Unregisters properties associated with this context.
/// </summary>
/// <param name="ctx">The context structure object.</param>
void sa_context_unregister_properties(sa_context_immutable_t* ctx);

/// <summary>
/// Set the selected elements of this context.
/// </summary>
/// <param name="ctx">The context structure object.</param>
/// <param name="strarray">An array of const char*</param>
/// <param name="count">The number of element in the given array</param>
void sa_context_set_elements_buffer(sa_context_t* ctx ,const char ** strarray, size_t count);

/// <summary>
/// Get how many elements are selected in the given context.
/// </summary>
/// <param name="ctx">The context structure object.</param>
/// <returns>Returns how many elements are selected in the given context.</returns>
size_t sa_context_get_element_count(sa_context_immutable_t* ctx);

/// <summary>
/// Get the nth string element from the context.
/// </summary>
/// <param name="ctx">The context structure object.</param>
/// <param name="idx">The index of the element</param>
/// <param name="length">The length of the element value in bytes.</param>
/// <param name="buffer">The output buffer for the value.</param>
/// <param name="buffer_size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_context_get_element_buffer(sa_context_immutable_t* ctx, size_t idx, int* length, char* buffer, size_t buffer_size);

/// <summary>
/// Get the nth string element from the context.
/// </summary>
/// <param name="ctx">The context structure object.</param>
/// <param name="idx">The index of the element</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_context_get_element_string(sa_context_immutable_t* ctx, size_t idx, sa_string_t* str);

/// <summary>
/// Get the number of files selected by the context.
/// </summary>
/// <param name="ctx">The context structure object.</param>
/// <returns>Returns the number of files selected by the context.</returns>
int sa_context_get_num_files(sa_context_immutable_t* ctx);

/// <summary>
/// Get the number of directories selected by the context.
/// </summary>
/// <param name="ctx">The context structure object.</param>
/// <returns>Returns the number of directories selected by the context.</returns>
int sa_context_get_num_directories(sa_context_immutable_t* ctx);

#ifdef __cplusplus
}
#endif

#endif //SA_API_CONTEXT_H
