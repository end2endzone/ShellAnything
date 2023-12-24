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


#include <string>

#include "IAction.h"
#include "SelectionContext.h"
#include "Icon.h"
#include "Menu.h"
#include "ConfigFile.h"
#include "Validator.h"
#include "PropertyStore.h"

#include "shellanything/sa_types.h"

const shellanything::IAction* AS_CLASS_ACTION(sa_action_immutable_t* object);
shellanything::IAction* AS_CLASS_ACTION(sa_action_t* object);
sa_action_immutable_t          AS_TYPE_ACTION(const shellanything::IAction* object);
sa_action_t                    AS_TYPE_ACTION(shellanything::IAction* object);

const shellanything::SelectionContext* AS_CLASS_SELECTION_CONTEXT(sa_selection_context_immutable_t* object);
shellanything::SelectionContext* AS_CLASS_SELECTION_CONTEXT(sa_selection_context_t* object);
sa_selection_context_immutable_t        AS_TYPE_SELECTION_CONTEXT(const shellanything::SelectionContext* object);
sa_selection_context_t                  AS_TYPE_SELECTION_CONTEXT(shellanything::SelectionContext* object);

const shellanything::Icon* AS_CLASS_ICON(sa_icon_immutable_t* object);
shellanything::Icon* AS_CLASS_ICON(sa_icon_t* object);
sa_icon_immutable_t         AS_TYPE_ICON(const shellanything::Icon* object);
sa_icon_t                   AS_TYPE_ICON(shellanything::Icon* object);

const shellanything::Menu* AS_CLASS_MENU(sa_menu_immutable_t* object);
shellanything::Menu* AS_CLASS_MENU(sa_menu_t* object);
sa_menu_immutable_t         AS_TYPE_MENU(const shellanything::Menu* object);
sa_menu_t                   AS_TYPE_MENU(shellanything::Menu* object);

const shellanything::ConfigFile* AS_CLASS_CONFIGURATION(sa_configuration_immutable_t* object);
shellanything::ConfigFile* AS_CLASS_CONFIGURATION(sa_configuration_t* object);
sa_configuration_immutable_t        AS_TYPE_CONFIGURATION(const shellanything::ConfigFile* object);
sa_configuration_t                  AS_TYPE_CONFIGURATION(shellanything::ConfigFile* object);

const shellanything::Validator* AS_CLASS_VALIDATOR(sa_validator_immutable_t* object);
shellanything::Validator* AS_CLASS_VALIDATOR(sa_validator_t* object);
sa_validator_immutable_t        AS_TYPE_VALIDATOR(const shellanything::Validator* object);
sa_validator_t                  AS_TYPE_VALIDATOR(shellanything::Validator* object);

const shellanything::PropertyStore* AS_CLASS_PROPERTY_STORE(sa_property_store_immutable_t* object);
shellanything::PropertyStore* AS_CLASS_PROPERTY_STORE(sa_property_store_t* object);
sa_property_store_immutable_t        AS_TYPE_PROPERTY_STORE(const shellanything::PropertyStore* object);
sa_property_store_t                  AS_TYPE_PROPERTY_STORE(shellanything::PropertyStore* object);

//const std::string* AS_CLASS_STRING(sa_string_immutable_t*      object);
std::string* AS_CLASS_STRING(sa_string_t* object);
//sa_string_immutable_t        AS_TYPE_STRING(const std::string* object);
sa_string_t                  AS_TYPE_STRING(std::string* object);


#endif //SA_API_STRING_PRIVATE_H
