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

#include "sa_private_casting.h"

const shellanything::IAction* AS_CLASS_ACTION(sa_action_immutable_t* object)
{
  return (const shellanything::IAction*)(object->opaque);
}
shellanything::IAction* AS_CLASS_ACTION(sa_action_t* object)
{
  return (shellanything::IAction*)(object->opaque);
}
sa_action_immutable_t          AS_TYPE_ACTION(const shellanything::IAction* object)
{
  sa_action_immutable_t  my_type; my_type.opaque = (void*)(object); return my_type;
}
sa_action_t                    AS_TYPE_ACTION(shellanything::IAction* object)
{
  sa_action_t            my_type; my_type.opaque = (void*)(object); return my_type;
}

const shellanything::SelectionContext* AS_CLASS_SELECTION_CONTEXT(sa_selection_context_immutable_t* object)
{
  return (const shellanything::SelectionContext*)(object->opaque);
}
shellanything::SelectionContext* AS_CLASS_SELECTION_CONTEXT(sa_selection_context_t* object)
{
  return (shellanything::SelectionContext*)(object->opaque);
}
sa_selection_context_immutable_t        AS_TYPE_SELECTION_CONTEXT(const shellanything::SelectionContext* object)
{
  sa_selection_context_immutable_t  my_type; my_type.opaque = (void*)(object); return my_type;
}
sa_selection_context_t                  AS_TYPE_SELECTION_CONTEXT(shellanything::SelectionContext* object)
{
  sa_selection_context_t            my_type; my_type.opaque = (void*)(object); return my_type;
}

const shellanything::Icon* AS_CLASS_ICON(sa_icon_immutable_t* object)
{
  return (const shellanything::Icon*)(object->opaque);
}
shellanything::Icon* AS_CLASS_ICON(sa_icon_t* object)
{
  return (shellanything::Icon*)(object->opaque);
}
sa_icon_immutable_t         AS_TYPE_ICON(const shellanything::Icon* object)
{
  sa_icon_immutable_t  my_type; my_type.opaque = (void*)(object); return my_type;
}
sa_icon_t                   AS_TYPE_ICON(shellanything::Icon* object)
{
  sa_icon_t            my_type; my_type.opaque = (void*)(object); return my_type;
}

const shellanything::Menu* AS_CLASS_MENU(sa_menu_immutable_t* object)
{
  return (const shellanything::Menu*)(object->opaque);
}
shellanything::Menu* AS_CLASS_MENU(sa_menu_t* object)
{
  return (shellanything::Menu*)(object->opaque);
}
sa_menu_immutable_t         AS_TYPE_MENU(const shellanything::Menu* object)
{
  sa_menu_immutable_t  my_type; my_type.opaque = (void*)(object); return my_type;
}
sa_menu_t                   AS_TYPE_MENU(shellanything::Menu* object)
{
  sa_menu_t            my_type; my_type.opaque = (void*)(object); return my_type;
}

const shellanything::ConfigFile* AS_CLASS_CONFIGURATION(sa_configuration_immutable_t* object)
{
  return (const shellanything::ConfigFile*)(object->opaque);
}
shellanything::ConfigFile* AS_CLASS_CONFIGURATION(sa_configuration_t* object)
{
  return (shellanything::ConfigFile*)(object->opaque);
}
sa_configuration_immutable_t        AS_TYPE_CONFIGURATION(const shellanything::ConfigFile* object)
{
  sa_configuration_immutable_t  my_type; my_type.opaque = (void*)(object); return my_type;
}
sa_configuration_t                  AS_TYPE_CONFIGURATION(shellanything::ConfigFile* object)
{
  sa_configuration_t            my_type; my_type.opaque = (void*)(object); return my_type;
}

const shellanything::Validator* AS_CLASS_VALIDATOR(sa_validator_immutable_t* object)
{
  return (const shellanything::Validator*)(object->opaque);
}
shellanything::Validator* AS_CLASS_VALIDATOR(sa_validator_t* object)
{
  return (shellanything::Validator*)(object->opaque);
}
sa_validator_immutable_t        AS_TYPE_VALIDATOR(const shellanything::Validator* object)
{
  sa_validator_immutable_t  my_type; my_type.opaque = (void*)(object); return my_type;
}
sa_validator_t                  AS_TYPE_VALIDATOR(shellanything::Validator* object)
{
  sa_validator_t            my_type; my_type.opaque = (void*)(object); return my_type;
}

const shellanything::PropertyStore* AS_CLASS_PROPERTY_STORE(sa_property_store_immutable_t* object)
{
  return (const shellanything::PropertyStore*)(object->opaque);
}
shellanything::PropertyStore* AS_CLASS_PROPERTY_STORE(sa_property_store_t* object)
{
  return (shellanything::PropertyStore*)(object->opaque);
}
sa_property_store_immutable_t         AS_TYPE_PROPERTY_STORE(const shellanything::PropertyStore* object)
{
  sa_property_store_immutable_t  my_type; my_type.opaque = (void*)(object); return my_type;
}
sa_property_store_t                   AS_TYPE_PROPERTY_STORE(shellanything::PropertyStore* object)
{
  sa_property_store_t            my_type; my_type.opaque = (void*)(object); return my_type;
}

//const std::string* AS_CLASS_STRING(sa_string_immutable_t*      object) { return (const std::string*)(object->opaque); }
std::string* AS_CLASS_STRING(sa_string_t* object)
{
  return (std::string*)(object->opaque);
}
//sa_string_immutable_t        AS_TYPE_STRING(const std::string* object) { sa_string_immutable_t  my_type; my_type.opaque = (void*)(object); return my_type; }
sa_string_t                  AS_TYPE_STRING(std::string* object)
{
  sa_string_t            my_type; my_type.opaque = (void*)(object); return my_type;
}
