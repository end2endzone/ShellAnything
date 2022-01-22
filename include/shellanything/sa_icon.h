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

#ifndef SA_API_ICON_H
#define SA_API_ICON_H

#include "shellanything/sa_error.h"
#include "shellanything/sa_string.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sa_icon_t  {
  void* opaque;
} sa_icon_t;

typedef struct sa_icon_immutable_t  {
  void* opaque;
} sa_icon_immutable_t;

/// <summary>
/// Converts a mutable icon to an immutable icon.
/// </summary>
/// <param name="icon">The mutable object to convert.</param>
/// <returns>Returns an immutable icon</returns>
sa_icon_immutable_t sa_icon_to_immutable(sa_icon_t * icon);

/// <summary>
/// Validate an icon
/// </summary>
/// <param name="icon">The icon structure object.</param>
/// <returns>Returns 1 when the icon is valid. Returns 0 otherwise.</returns>
int sa_icon_is_valid(sa_icon_immutable_t* icon);

/// <summary>
/// Get the file extension.
/// </summary>
/// <param name="icon">The icon structure object.</param>
/// <param name="length">The length of the output value in bytes.</param>
/// <param name="buffer">The output buffer for the value.</param>
/// <param name="size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_icon_get_file_extension_buffer(sa_icon_immutable_t* icon, int* length, char* buffer, size_t size);

/// <summary>
/// Get the file extension.
/// </summary>
/// <param name="icon">The icon structure object.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_icon_get_file_extension_string(sa_icon_immutable_t* icon, sa_string_t* str);

/// <summary>
/// Set the file extension.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <param name="fileextension">The new fileextension.</param>
void sa_icon_set_file_extension(sa_icon_t* icon, const char* fileextension);

/// <summary>
/// Get the path.
/// </summary>
/// <param name="icon">The icon structure object.</param>
/// <param name="length">The length of the output value in bytes.</param>
/// <param name="buffer">The output buffer for the value.</param>
/// <param name="size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_icon_get_path_buffer(sa_icon_immutable_t* icon, int* length, char* buffer, size_t size);

/// <summary>
/// Get the path.
/// </summary>
/// <param name="icon">The icon structure object.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_icon_get_path_string(sa_icon_immutable_t* icon, sa_string_t* str);

/// <summary>
/// Set the path.
/// </summary>
/// <param name="icon">The icon structure object.</param>
/// <param name="path">The new path.</param>
void sa_icon_set_path(sa_icon_t* icon, const char* path);

/// <summary>
/// Get the index.
/// </summary>
/// <param name="icon">The icon structure object.</param>
/// <returns>Returns the index of the icon.</returns>
int sa_icon_get_index(sa_icon_immutable_t* icon);

/// <summary>
/// Set the index.
/// </summary>
/// <param name="icon">The icon structure object.</param>
/// <param name="index">The new index.</param>
void sa_icon_set_index(sa_icon_t* icon, int index);

#ifdef __cplusplus
}
#endif

#endif //SA_API_ICON_H
