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

#ifndef SA_API_ERROR_MANAGER_H
#define SA_API_ERROR_MANAGER_H

#include "shellanything/sa_enums.h"

#ifdef __cplusplus
extern "C" {
#if 0
}  // do not indent code inside extern C
#endif
#endif

/// <summary>
/// Get a string description of the given error code.
/// </summary>
/// <param name="code">The error code</param>
/// <returns>Returns a string description of the given error code.</returns>
const char* sa_error_get_error_description(sa_error_t code);

/// <summary>
/// Show an error message encoded in ansi to the user.
/// </summary>
/// <param name="title">The caption title of the window.</param>
/// <param name="message">The message to display to the user.</param>
void sa_error_show_message(const char* title, const char* message);

/// <summary>
/// Show an error message encoded in utf8 to the user.
/// </summary>
/// <param name="title">The caption title of the window.</param>
/// <param name="message">The message to display to the user.</param>
void sa_error_show_message_utf8(const char* title, const char* message);

#ifdef __cplusplus
#if 0
{  // do not indent code inside extern C
#endif
}  // extern "C"
#endif

#endif //SA_API_ERROR_MANAGER_H
