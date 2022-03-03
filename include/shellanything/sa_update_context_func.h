///**********************************************************************************
//* MIT License
//*
//* Copyright (c) 2018 Antoine Beauchamp
//*
//* Permission is hereby granted, free of charge, to any person obtaining a copy
//* of this software and associated documentation files (the "Software"), to deal
//* in the Software without restriction, including without limitation the rights
//* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//* copies of the Software, and to permit persons to whom the Software is
//* furnished to do so, subject to the following conditions:
//*
//* The above copyright notice and this permission notice shall be included in all
//* copies or substantial portions of the Software.
//*
//* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//* SOFTWARE.
//*********************************************************************************/

#ifndef SA_API_UPDATE_CONTEXT_FUNC_H
#define SA_API_UPDATE_CONTEXT_FUNC_H

#include "shellanything/sa_update_context_type.h"
#include "shellanything/sa_string.h"
#include "shellanything/sa_error.h"
#include "shellanything/sa_selection_context.h"
#include "shellanything/sa_configuration.h"
#include "shellanything/sa_menu.h"

#ifdef __cplusplus
extern "C" {
#endif
   
/// <summary>
/// Converts a mutable context to an immutable context.
/// </summary>
/// <param name="context">The mutable object to convert.</param>
/// <returns>Returns an immutable context</returns>
sa_update_context_immutable_t sa_update_context_to_immutable(sa_update_context_t* context);

/// <summary>
/// Clear all active objects that are currently set in the update context
/// </summary>
void sa_update_context_clear(sa_update_context_t* ctx);

/// <summary>
/// Get the selection context
/// </summary>
/// <param name="ctx">The context structure object.</param>
sa_selection_context_immutable_t* sa_update_context_get_selection(sa_update_context_immutable_t* ctx);

/// <summary>
/// Set the selection context
/// </summary>
/// <param name="ctx">The context structure object.</param>
/// <param name="selection">The selection context structure object.</param>
void sa_update_context_set_selection(sa_update_context_t* ctx, sa_selection_context_immutable_t* selection);

/// <summary>
/// Get the current updated configuration
/// </summary>
/// <returns>Get the current updated configuration. Returns NULL if no configuration is updated.</returns>
sa_configuration_immutable_t* sa_update_context_get_configuration(sa_update_context_immutable_t* ctx);

/// <summary>
/// Set the current updated configuration
/// </summary>
/// <param name="ctx">The context structure object.</param>
/// <param name="cfg">The updating configuration</param>
void sa_update_context_set_configuration(sa_update_context_t* ctx, sa_configuration_immutable_t* cfg);

/// <summary>
/// Get the current updated menu
/// </summary>
/// <returns>Get the current updated menu. Returns NULL if no menu is updated.</returns>
sa_menu_immutable_t* sa_update_context_get_menu(sa_update_context_immutable_t* ctx);

/// <summary>
/// Set the current updated menu
/// </summary>
/// <param name="ctx">The context structure object.</param>
/// <param name="menu">The updating menu</param>
void sa_update_context_set_menu(sa_update_context_t* ctx, sa_menu_immutable_t* menu);

#ifdef __cplusplus
}
#endif

#endif //SA_API_UPDATE_CONTEXT_FUNC_H
