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

#include "sa_types_private.h"

const shellanything::Action*  AS_CLASS_ACTION(sa_action_immutable_t*      object) { return (const shellanything::Action*)(object); }
      shellanything::Action*  AS_CLASS_ACTION(sa_action_t*                object) { return (      shellanything::Action*)(object); }
sa_action_immutable_t         AS_TYPE_ACTION(const shellanything::Action* object) { sa_action_immutable_t  my_type; my_type.opaque = (void*)(object); return my_type; }
sa_action_t                   AS_TYPE_ACTION(      shellanything::Action* object) { sa_action_t            my_type; my_type.opaque = (void*)(object); return my_type; }

const shellanything::Context* AS_CLASS_CONTEXT(sa_context_immutable_t*      object) { return (const shellanything::Context*)(object); }
      shellanything::Context* AS_CLASS_CONTEXT(sa_context_t*                object) { return (      shellanything::Context*)(object); }
sa_context_immutable_t        AS_TYPE_CONTEXT(const shellanything::Context* object) { sa_context_immutable_t  my_type; my_type.opaque = (void*)(object); return my_type; }
sa_context_t                  AS_TYPE_CONTEXT(      shellanything::Context* object) { sa_context_t            my_type; my_type.opaque = (void*)(object); return my_type; }

const shellanything::Icon*  AS_CLASS_ICON(sa_icon_immutable_t*      object) { return (const shellanything::Icon*)(object); }
      shellanything::Icon*  AS_CLASS_ICON(sa_icon_t*                object) { return (      shellanything::Icon*)(object); }
sa_icon_immutable_t         AS_TYPE_ICON(const shellanything::Icon* object) { sa_icon_immutable_t  my_type; my_type.opaque = (void*)(object); return my_type; }
sa_icon_t                   AS_TYPE_ICON(      shellanything::Icon* object) { sa_icon_t            my_type; my_type.opaque = (void*)(object); return my_type; }

const shellanything::Menu*  AS_CLASS_MENU(sa_menu_immutable_t*      object) { return (const shellanything::Menu*)(object); }
      shellanything::Menu*  AS_CLASS_MENU(sa_menu_t*                object) { return (      shellanything::Menu*)(object); }
sa_menu_immutable_t         AS_TYPE_MENU(const shellanything::Menu* object) { sa_menu_immutable_t  my_type; my_type.opaque = (void*)(object); return my_type; }
sa_menu_t                   AS_TYPE_MENU(      shellanything::Menu* object) { sa_menu_t            my_type; my_type.opaque = (void*)(object); return my_type; }

const shellanything::Configuration* AS_CLASS_CONFIGURATION(sa_configuration_immutable_t*      object) { return (const shellanything::Configuration*)(object); }
      shellanything::Configuration* AS_CLASS_CONFIGURATION(sa_configuration_t*                object) { return (      shellanything::Configuration*)(object); }
sa_configuration_immutable_t        AS_TYPE_CONFIGURATION(const shellanything::Configuration* object) { sa_configuration_immutable_t  my_type; my_type.opaque = (void*)(object); return my_type; }
sa_configuration_t                  AS_TYPE_CONFIGURATION(      shellanything::Configuration* object) { sa_configuration_t            my_type; my_type.opaque = (void*)(object); return my_type; }

const shellanything::Validator* AS_CLASS_VALIDATOR(sa_validator_immutable_t*      object) { return (const shellanything::Validator*)(object); }
      shellanything::Validator* AS_CLASS_VALIDATOR(sa_validator_t*                object) { return (      shellanything::Validator*)(object); }
sa_validator_immutable_t        AS_TYPE_VALIDATOR(const shellanything::Validator* object) { sa_validator_immutable_t  my_type; my_type.opaque = (void*)(object); return my_type; }
sa_validator_t                  AS_TYPE_VALIDATOR(      shellanything::Validator* object) { sa_validator_t            my_type; my_type.opaque = (void*)(object); return my_type; }
