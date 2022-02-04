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

#ifndef SA_API_TYPES_PRIVATE_H
#define SA_API_TYPES_PRIVATE_H

#include "shellanything/sa_error.h"

#include "Action.h"
#include "Context.h"
#include "Icon.h"
#include "Menu.h"
#include "Configuration.h"
#include "Validator.h"

#include "shellanything/sa_action.h"
#include "shellanything/sa_context.h"
#include "shellanything/sa_icon.h"
#include "shellanything/sa_menu.h"
#include "shellanything/sa_configuration.h"
#include "shellanything/sa_validator.h"

const shellanything::Action*  AS_CLASS_ACTION(sa_action_immutable_t*      object);
      shellanything::Action*  AS_CLASS_ACTION(sa_action_t*                object);
sa_action_immutable_t         AS_TYPE_ACTION(const shellanything::Action* object);
sa_action_t                   AS_TYPE_ACTION(      shellanything::Action* object);

const shellanything::Context* AS_CLASS_CONTEXT(sa_context_immutable_t*      object);
      shellanything::Context* AS_CLASS_CONTEXT(sa_context_t*                object);
sa_context_immutable_t        AS_TYPE_CONTEXT(const shellanything::Context* object);
sa_context_t                  AS_TYPE_CONTEXT(      shellanything::Context* object);

const shellanything::Icon*  AS_CLASS_ICON(sa_icon_immutable_t*      object);
      shellanything::Icon*  AS_CLASS_ICON(sa_icon_t*                object);
sa_icon_immutable_t         AS_TYPE_ICON(const shellanything::Icon* object);
sa_icon_t                   AS_TYPE_ICON(      shellanything::Icon* object);

const shellanything::Menu*  AS_CLASS_MENU(sa_menu_immutable_t*      object);
      shellanything::Menu*  AS_CLASS_MENU(sa_menu_t*                object);
sa_menu_immutable_t         AS_TYPE_MENU(const shellanything::Menu* object);
sa_menu_t                   AS_TYPE_MENU(      shellanything::Menu* object);

const shellanything::Configuration* AS_CLASS_CONFIGURATION(sa_configuration_immutable_t*      object);
      shellanything::Configuration* AS_CLASS_CONFIGURATION(sa_configuration_t*                object);
sa_configuration_immutable_t        AS_TYPE_CONFIGURATION(const shellanything::Configuration* object);
sa_configuration_t                  AS_TYPE_CONFIGURATION(      shellanything::Configuration* object);

const shellanything::Validator* AS_CLASS_VALIDATOR(sa_validator_immutable_t*      object);
      shellanything::Validator* AS_CLASS_VALIDATOR(sa_validator_t*                object);
sa_validator_immutable_t        AS_TYPE_VALIDATOR(const shellanything::Validator* object);
sa_validator_t                  AS_TYPE_VALIDATOR(      shellanything::Validator* object);


#endif //SA_API_STRING_PRIVATE_H
