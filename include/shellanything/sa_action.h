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

#ifndef SA_API_ACTION_H
#define SA_API_ACTION_H

#include "shellanything/sa_error.h"
#include "shellanything/sa_context.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sa_action_t  {
  void* opaque;
} sa_action_t;

typedef struct sa_action_immutable_t  {
  void* opaque;
} sa_action_immutable_t;

/// <summary>
/// Converts a mutable action to an immutable action.
/// </summary>
/// <param name="action">The mutable object to convert.</param>
/// <returns>Returns an immutable action</returns>
sa_action_immutable_t sa_action_to_immutable(sa_action_t* action);

/// <summary>
/// Execute the given action.
/// </summary>
/// <param name="action">The action structure object.</param>
/// <param name="ctx">The context for executing the action.</param>
sa_error_t sa_action_execute(sa_action_immutable_t* action, sa_context_immutable_t* ctx);

#ifdef __cplusplus
}
#endif

#endif //SA_API_CONTEXT_H
