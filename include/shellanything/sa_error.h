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

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  SA_ERROR_SUCCESS = 0,
  SA_ERROR_INVALID_ARGUMENTS,
  SA_ERROR_BUFFER_TOO_SMALL,
  SA_ERROR_VALUE_OUT_OF_BOUNDS,
  SA_ERROR_NOT_FOUND,
  SA_ERROR_ALREADY_EXIST,
  SA_ERROR_MISSING_RESOURCE,
  SA_ERROR_BUSY,
  SA_ERROR_UNKNOWN = -1,
} sa_error_t;

/// <summary>
/// Show an error message encoded in ansi to the user.
/// </summary>
/// <param name="title">The caption title of the window.</param>
/// <param name="message">The message to display to the user.</param>
void sa_error_show_message(const char * title, const char * message);

/// <summary>
/// Show an error message encoded in utf8 to the user.
/// </summary>
/// <param name="title">The caption title of the window.</param>
/// <param name="message">The message to display to the user.</param>
void sa_error_show_message_utf8(const char* title, const char* message);

#ifdef __cplusplus
}
#endif

#endif //SA_API_ERROR_MANAGER_H
