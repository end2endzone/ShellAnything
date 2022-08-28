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

#ifndef SA_API_ENUMS_H
#define SA_API_ENUMS_H

#ifdef __cplusplus
extern "C" {
#if 0
}  // do not indent code inside extern C
#endif
#endif

typedef enum
{
  SA_ERROR_SUCCESS = 0,
  SA_ERROR_INVALID_ARGUMENTS,
  SA_ERROR_BUFFER_TOO_SMALL,
  SA_ERROR_VALUE_OUT_OF_BOUNDS,
  SA_ERROR_NOT_FOUND,
  SA_ERROR_NOT_SUPPORTED,
  SA_ERROR_NOT_IMPLEMENTED,
  SA_ERROR_EMPTY,
  SA_ERROR_ALREADY_EXIST,
  SA_ERROR_MISSING_RESOURCE,
  SA_ERROR_ACCESS_DENIED,
  SA_ERROR_BUSY,
  SA_ERROR_TIME_OUT,
  SA_ERROR_CANT_READ,
  SA_ERROR_CANT_WRITE,
  SA_ERROR_INVALID_RESOURCE,
  SA_ERROR_ALREADY_RUNNING,
  SA_ERROR_ABORTED,
  SA_ERROR_CANCELLED,
  SA_ERROR_UNKNOWN = -1,
} sa_error_t;

typedef enum
{
  SA_LOG_LEVEL_INFO = 0,
  SA_LOG_LEVEL_WARNING,
  SA_LOG_LEVEL_ERROR,
  SA_LOG_LEVEL_FATAL,
} sa_log_level_t;

typedef enum
{
  SA_ACTION_EVENT_CREATE = 0,
  SA_ACTION_EVENT_DESTROY,
  SA_ACTION_EVENT_EXECUTE,
} sa_action_event_t;

#ifdef __cplusplus
#if 0
{  // do not indent code inside extern C
#endif
}  // extern "C"
#endif

#endif //SA_API_ERROR_MANAGER_H
