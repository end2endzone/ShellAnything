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

#include "shellanything/sa_menu.h"
#include "sa_string_private.h"
#include "Menu.h"
#include "Icon.h"
#include "IAction.h"
#include "sa_private_casting.h"

using namespace shellanything;

sa_menu_immutable_t sa_menu_to_immutable(sa_menu_t* menu)
{
  sa_menu_immutable_t output;
  output.opaque = menu->opaque;
  return output;
}

int sa_menu_is_separator(sa_menu_immutable_t* menu)
{
  bool is_separator = AS_CLASS_MENU(menu)->IsSeparator();
  if (is_separator)
    return 1;
  return 0;
}

void sa_menu_set_separator(sa_menu_t* menu, sa_boolean separator)
{
  AS_CLASS_MENU(menu)->SetSeparator(separator != 0);
}

sa_error_t sa_menu_get_name_buffer(sa_menu_immutable_t* menu, int* length, char* buffer, size_t size)
{
  if (length)
    *length = -1;
  const std::string& name = AS_CLASS_MENU(menu)->GetName();
  sa_error_t result = sa_cstr_copy_buffer(buffer, size, length, name);
  return result;
}

sa_error_t sa_menu_get_name_string(sa_menu_immutable_t* menu, sa_string_t* str)
{
  const std::string& name = AS_CLASS_MENU(menu)->GetName();
  sa_string_copy_stdstr(str, name);
  return SA_ERROR_SUCCESS;
}

const char* sa_menu_get_name_cstr(sa_menu_immutable_t* menu)
{
  const std::string& name = AS_CLASS_MENU(menu)->GetName();
  const char* output = name.c_str();
  return output;
}

const char* sa_menu_get_name_alloc(sa_menu_immutable_t* menu)
{
  const std::string& name = AS_CLASS_MENU(menu)->GetName();
  const char* output = name.c_str();
  return _strdup(output);
}

void sa_menu_set_name(sa_menu_t* menu, const char* name)
{
  AS_CLASS_MENU(menu)->SetName(name);
}

sa_error_t sa_menu_get_description_buffer(sa_menu_immutable_t* menu, int* length, char* buffer, size_t size)
{
  if (length)
    *length = -1;
  const std::string& description = AS_CLASS_MENU(menu)->GetDescription();
  sa_error_t result = sa_cstr_copy_buffer(buffer, size, length, description);
  return result;
}

sa_error_t sa_menu_get_description_string(sa_menu_immutable_t* menu, sa_string_t* str)
{
  const std::string& description = AS_CLASS_MENU(menu)->GetDescription();
  sa_string_copy_stdstr(str, description);
  return SA_ERROR_SUCCESS;
}

const char* sa_menu_get_description_cstr(sa_menu_immutable_t* menu)
{
  const std::string& description = AS_CLASS_MENU(menu)->GetDescription();
  const char* output = description.c_str();
  return output;
}

const char* sa_menu_get_description_alloc(sa_menu_immutable_t* menu)
{
  const std::string& description = AS_CLASS_MENU(menu)->GetDescription();
  const char* output = description.c_str();
  return _strdup(output);
}

void sa_menu_set_description(sa_menu_t* menu, const char* description)
{
  AS_CLASS_MENU(menu)->SetDescription(description);
}

sa_icon_immutable_t sa_menu_get_icon(sa_menu_immutable_t* menu)
{
  const shellanything::Icon& icon = AS_CLASS_MENU(menu)->GetIcon();
  const shellanything::Icon* icon_ptr = &icon;
  return AS_TYPE_ICON(icon_ptr);
}

void sa_menu_set_icon(sa_menu_t* menu, sa_icon_immutable_t* icon)
{
  const shellanything::Icon* object = AS_CLASS_ICON(icon);
  AS_CLASS_MENU(menu)->SetIcon(*object);
}

void sa_menu_update(sa_menu_t* menu, sa_selection_context_immutable_t* ctx)
{
  AS_CLASS_MENU(menu)->Update(*AS_CLASS_SELECTION_CONTEXT(ctx));
}

sa_boolean sa_menu_is_visible(sa_menu_immutable_t* menu)
{
  bool value = AS_CLASS_MENU(menu)->IsVisible();
  if (value)
    return 1;
  return 0;
}

void sa_menu_set_visible(sa_menu_t* menu, sa_boolean visible)
{
  AS_CLASS_MENU(menu)->SetVisible(visible != 0);
}

sa_boolean sa_menu_is_enabled(sa_menu_immutable_t* menu)
{
  bool value = AS_CLASS_MENU(menu)->IsEnabled();
  if (value)
    return 1;
  return 0;
}

void sa_menu_set_enabled(sa_menu_t* menu, sa_boolean enabled)
{
  AS_CLASS_MENU(menu)->SetEnabled(enabled != 0);
}

size_t sa_menu_get_action_count(sa_menu_immutable_t* menu)
{
  shellanything::IAction::ActionPtrList actions = AS_CLASS_MENU(menu)->GetActions();
  size_t count = actions.size();
  return count;
}

sa_error_t sa_menu_get_action_element(sa_menu_t* menu, size_t index, sa_action_t* action)
{
  if (action == NULL)
    return SA_ERROR_INVALID_ARGUMENTS;
  shellanything::IAction::ActionPtrList actions = AS_CLASS_MENU(menu)->GetActions();
  if (actions.empty() || index > (actions.size() - 1))
    return SA_ERROR_VALUE_OUT_OF_BOUNDS;
  shellanything::IAction* action_element = actions[index];
  *action = AS_TYPE_ACTION(action_element);
  return SA_ERROR_SUCCESS;
}

size_t sa_menu_get_visibility_count(sa_menu_immutable_t* menu)
{
  size_t count = AS_CLASS_MENU(menu)->GetVisibilityCount();
  return count;
}

sa_error_t sa_menu_get_visibility_element(sa_menu_immutable_t* menu, size_t index, sa_validator_immutable_t* validator)
{
  if (validator == NULL)
    return SA_ERROR_INVALID_ARGUMENTS;
  if (index >= AS_CLASS_MENU(menu)->GetVisibilityCount())
    return SA_ERROR_VALUE_OUT_OF_BOUNDS;
  const shellanything::Validator* element = AS_CLASS_MENU(menu)->GetVisibility(index);
  *validator = AS_TYPE_VALIDATOR(element);
  return SA_ERROR_SUCCESS;
}

size_t sa_menu_get_validity_count(sa_menu_immutable_t* menu)
{
  size_t count = AS_CLASS_MENU(menu)->GetValidityCount();
  return count;
}

sa_error_t sa_menu_get_validity_element(sa_menu_immutable_t* menu, size_t index, sa_validator_immutable_t* validator)
{
  if (validator == NULL)
    return SA_ERROR_INVALID_ARGUMENTS;
  if (index >= AS_CLASS_MENU(menu)->GetValidityCount())
    return SA_ERROR_VALUE_OUT_OF_BOUNDS;
  const shellanything::Validator* element = AS_CLASS_MENU(menu)->GetValidity(index);
  *validator = AS_TYPE_VALIDATOR(element);
  return SA_ERROR_SUCCESS;
}

size_t sa_menu_get_sub_menu_count(sa_menu_t* menu)
{
  shellanything::Menu::MenuPtrList subs = AS_CLASS_MENU(menu)->GetSubMenus();
  size_t count = subs.size();
  return count;
}

sa_error_t sa_menu_get_sub_menu_element(sa_menu_t* menu, size_t index, sa_menu_t* submenu)
{
  if (submenu == NULL)
    return SA_ERROR_INVALID_ARGUMENTS;
  shellanything::Menu::MenuPtrList subs = AS_CLASS_MENU(menu)->GetSubMenus();
  if (subs.empty() || index > (subs.size() - 1))
    return SA_ERROR_VALUE_OUT_OF_BOUNDS;
  shellanything::Menu* sub_menu_element = subs[index];
  *submenu = AS_TYPE_MENU(sub_menu_element);
  return SA_ERROR_SUCCESS;
}
