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

#ifndef SA_API_MENU_H
#define SA_API_MENU_H

#include "shellanything/sa_error.h"
#include "shellanything/sa_string.h"
#include "shellanything/sa_icon.h"
#include "shellanything/sa_action.h"
#include "shellanything/sa_validator.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sa_menu_t  {
  void* opaque;
} sa_menu_t;

typedef struct sa_menu_immutable_t  {
  void* opaque;
} sa_menu_immutable_t;

/// <summary>
/// Converts a mutable menu to an immutable menu.
/// </summary>
/// <param name="menu">The mutable object to convert.</param>
/// <returns>Returns an immutable menu</returns>
sa_menu_immutable_t sa_menu_to_immutable(sa_menu_t* menu);

/// <summary>
/// Check if a menu is a separator
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <returns>Returns 1 if the menu is a separator. Returns 0 otherwise.</returns>
int sa_menu_is_separator(sa_menu_immutable_t* menu);

/// <summary>
/// Set a menu as a separator
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <param name="separator">The new separator value. Set to 1 for setting this menu as a separator. Set to 0 otherwise.</param>
void sa_menu_set_separator(sa_menu_t* menu, int separator);

/// <summary>
/// Get the name of the menu.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <param name="length">The length of the output value in bytes.</param>
/// <param name="buffer">The output buffer for the value.</param>
/// <param name="size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_menu_get_name_buffer(sa_menu_immutable_t* menu, int* length, char* buffer, size_t size);

/// <summary>
/// Get the name of the menu.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_menu_get_name_string(sa_menu_immutable_t* menu, sa_string_t* str);

/// <summary>
/// Set the name of the menu.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <param name="name">The new name for the menu.</param>
void sa_menu_set_name(sa_menu_t* menu, const char * name);

/// <summary>
/// Get the description of the menu.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <param name="length">The length of the output value in bytes.</param>
/// <param name="buffer">The output buffer for the value.</param>
/// <param name="size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_menu_get_description_buffer(sa_menu_immutable_t* menu, int* length, char* buffer, size_t size);

/// <summary>
/// Get the description of the menu.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_menu_get_description_string(sa_menu_immutable_t* menu, sa_string_t* str);

/// <summary>
/// Set the description of the menu.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <param name="name">The new description for the menu.</param>
void sa_menu_set_description(sa_menu_t* menu, const char * description);

/// <summary>
/// Get the menu's icon.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <returns>Returns the menu's icon</returns>
sa_icon_immutable_t sa_menu_get_icon(sa_menu_immutable_t* menu);

/// <summary>
/// Set the menu's icon.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <param name="icon">The new icon object.</param>
/// <returns>Returns the menu's icon</returns>
void sa_menu_set_icon(sa_menu_t* menu, sa_icon_immutable_t* icon);

/// <summary>
/// Updates the menu and submenus 'visible' and 'enabled' properties based on the given context.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <param name="ctx">The context used for updating the menu.</param>
void sa_menu_update(sa_menu_t* menu, sa_context_immutable_t* ctx);

/// <summary>
/// Get the visible parameter.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <returns>Return 1 if the menu is visible. Returns 0 otherwise.</returns>
int sa_menu_is_visible(sa_menu_immutable_t* menu);

/// <summary>
/// Set the visible parameter
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <param name="visible">The new parameter value</param>
void sa_menu_set_visible(sa_menu_t* menu, int visible);

/// <summary>
/// Get the enabled parameter.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <returns>Return 1 if the menu is enabled. Returns 0 otherwise.</returns>
int sa_menu_is_enabled(sa_menu_immutable_t* menu);

/// <summary>
/// Set the enabled parameter
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <param name="enabled">The new parameter value</param>
void sa_menu_set_enabled(sa_menu_t* menu, int enabled);

/// <summary>
/// Get how many actions are subscribed to the menu.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <returns>Returns how many actions are subscribed to the menu</returns>
size_t sa_menu_get_action_count(sa_menu_immutable_t* menu);

/// <summary>
/// Get an action subscribed to the menu.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <param name="index">The action index</param>
/// <param name="submenu">The output action structure object</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_menu_get_action_element(sa_menu_t* menu, size_t index, sa_action_t* action);

/// <summary>
/// Get how many visibility validators are subscribed to the menu.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <returns>Returns how many visibility validators are subscribed to the menu</returns>
size_t sa_menu_get_visibility_count(sa_menu_immutable_t* menu);

/// <summary>
/// Get a visibility validators subscribed to the menu.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <param name="index">The validator index</param>
/// <param name="submenu">The output validator structure object</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_menu_get_visibility_element(sa_menu_immutable_t* menu, size_t index, sa_validator_immutable_t* validator);

/// <summary>
/// Get how many validity validators are subscribed to the menu.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <returns>Returns how many validity validators are subscribed to the menu</returns>
size_t sa_menu_get_validity_count(sa_menu_immutable_t* menu);

/// <summary>
/// Get a validity validators subscribed to the menu.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <param name="index">The validator index</param>
/// <param name="submenu">The output validator structure object</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_menu_get_validity_element(sa_menu_immutable_t* menu, size_t index, sa_validator_immutable_t* validator);

/// <summary>
/// Get how many sub menu are under the current menu.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <returns>Returns how many sub menus are under this menu</returns>
size_t sa_menu_get_sub_menu_count(sa_menu_t* menu);

/// <summary>
/// Get a sub menu under this menu.
/// </summary>
/// <param name="menu">The menu structure object.</param>
/// <param name="index">The sub menu index</param>
/// <param name="submenu">The output sub menu structure object</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_menu_get_sub_menu_element(sa_menu_t* menu, size_t index, sa_menu_t* submenu);

#ifdef __cplusplus
}
#endif

#endif //SA_API_CONTEXT_H
