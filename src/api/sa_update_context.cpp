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
 * THE SOFTWARE IS PROVIDED "AS_CLASS_SELECTION_CONTEXT IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#include "shellanything/sa_update_context_type.h"
#include "shellanything/sa_update_context_func.h"
#include "sa_string_private.h"
#include "SelectionContext.h"
#include "sa_types_private.h"

#include "shellanything/sa_selection_context.h"
#include "shellanything/sa_configuration.h"
#include "shellanything/sa_menu.h"

using namespace shellanything;

sa_update_context_immutable_t sa_update_context_to_immutable(sa_update_context_t* context)
{
  sa_update_context_immutable_t output;
  output.opaque = context->opaque;
  return output;
}

void sa_update_context_clear(sa_update_context_t* ctx)
{
  AS_CLASS_UPDATE_CONTEXT(ctx)->Clear();
}

sa_selection_context_immutable_t* sa_update_context_get_selection(sa_update_context_immutable_t* ctx)
{
  //const SelectionContext* selection = AS_CLASS_UPDATE_CONTEXT(ctx)->GetSelection();
  //return AS_TYPE_SELECTION_CONTEXT(selection);
  return NULL;
}

void sa_update_context_set_selection(sa_update_context_t* ctx, sa_selection_context_immutable_t* selection)
{
  const SelectionContext* selection_class = AS_CLASS_SELECTION_CONTEXT(selection);
  AS_CLASS_UPDATE_CONTEXT(ctx)->SetSelection(selection_class);
}

sa_configuration_immutable_t* sa_update_context_get_configuration(sa_update_context_immutable_t* ctx)
{
  //const Configuration* configuration = AS_CLASS_UPDATE_CONTEXT(ctx)->GetConfiguration();
  //return AS_TYPE_CONFIGURATION(configuration);
  return NULL;
}

void sa_update_context_set_configuration(sa_update_context_t* ctx, sa_configuration_immutable_t* cfg)
{
  const Configuration* configuration = AS_CLASS_CONFIGURATION(cfg);
  AS_CLASS_UPDATE_CONTEXT(ctx)->SetConfiguration(configuration);
}

sa_menu_immutable_t* sa_update_context_get_menu(sa_update_context_immutable_t* ctx)
{
  //const Menu* menu = AS_CLASS_UPDATE_CONTEXT(ctx)->GetMenu();
  //return AS_TYPE_MENU(menu);
  return NULL;
}

void sa_update_context_set_menu(sa_update_context_t* ctx, sa_menu_immutable_t* menu)
{
  const Menu* menu_class = AS_CLASS_MENU(menu);
  AS_CLASS_UPDATE_CONTEXT(ctx)->SetMenu(menu_class);
}
