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

#ifndef SA_API_PLUGIN_H
#define SA_API_PLUGIN_H

#include <stdint.h>
#include "shellanything/sa_types.h"
#include "shellanything/sa_enums.h"
#include "shellanything/sa_plugin_definitions.h"
#include "shellanything/sa_error.h"
#include "shellanything/sa_selection_context.h"
#include "shellanything/sa_property_store.h"
#include "shellanything/sa_plugin_definitions.h"

#ifdef __cplusplus
extern "C" {
#if 0
}  // do not indent code inside extern C
#endif
#endif

/// <summary>
/// Function pointer definition for validating custom attributes.
/// </summary>
/// <returns>Returns 1 on validation success. Returns NULL on error.</returns>
typedef sa_boolean(*sa_plugin_validation_attributes_func)();

/// <summary>
/// Function pointer definition for an update callback function.
/// An update callback is called on a new selection.
/// </summary>
typedef void (*sa_plugin_config_update_func)();

/// <summary>
/// Function pointer definition to process an action event.
/// Basic events are creating, executing and destroying and action.
/// While processing an event, see functions that begins with sa_plugin_action_* for more informations.
/// </summary>
/// <param name="evnt">An event of type sa_action_event_t. See sa_enums.h for details.</param>
/// <returns>Returns 0 on success. Returns a non-zero on error.</returns>
typedef sa_error_t(*sa_plugin_action_event_func)(sa_action_event_t evnt);

/// <summary>
/// Get the selection context that identifies which files and directories are selected while validating.
/// </summary>
/// <returns>Returns a valid pointer to a sa_selection_context_immutable_t. Returns NULL otherwise.</returns>
sa_selection_context_immutable_t* sa_plugin_validation_get_selection_context();

/// <summary>
/// Get the property store which contains the attributes for validation.
/// </summary>
/// <returns>Returns a valid pointer to a sa_selection_context_immutable_t. Returns NULL otherwise.</returns>
sa_property_store_immutable_t* sa_plugin_validation_get_property_store();

/// <summary>
/// Get the selection context that identifies which files and directories are selected while a Configuration is updated.
/// </summary>
/// <returns>Returns a valid pointer to a sa_selection_context_immutable_t. Returns NULL otherwise.</returns>
sa_selection_context_immutable_t* sa_plugin_config_update_get_selection_context();

/// <summary>
/// Get the name of a custom action.
/// </summary>
/// <returns>Returns the action name. Returns NULL if no action name is defined.</returns>
const char* sa_plugin_action_get_name();

/// <summary>
/// Get the xml definition of an action in a Configuration of a custom action.
/// </summary>
/// <returns>Returns the xml definition of an action. Returns NULL if no xml is defined.</returns>
const char* sa_plugin_action_get_xml();

/// <summary>
/// Get a custom data pointer from the action. This same pointer is used while creating, executing and destroying the action.
/// </summary>
/// <returns>Returns the xml definition of an action. Returns NULL if no xml is defined.</returns>
void* sa_plugin_action_get_data();

/// <summary>
/// Set a custom data pointer to the action. This same pointer is used while creating, executing and destroying the action.
/// </summary>
/// <param name="data">A pointer that points to custom data to keep action.</param>
void sa_plugin_action_set_data(void* data);

/// <summary>
/// Get the property store which can store the attributes and settings for the action.
/// </summary>
/// <returns>Returns a valid pointer to a sa_selection_context_t. Returns NULL otherwise.</returns>
sa_property_store_t* sa_plugin_action_get_property_store();

/// <summary>
/// Register a validation function for for a given list of validation attributes.
/// </summary>
/// <param name="names">The names of the attributes as an array of strings.</param>
/// <param name="count">Defines how many elements are in the names array.</param>
/// <param name="func">A function pointer which definition matches sa_plugin_validation_attributes_func.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_plugin_register_validation_attributes(const char* names[], size_t count, sa_plugin_validation_attributes_func func);

/// <summary>
/// Register a function call when a Configuration is updated with a new selection.
/// </summary>
/// <param name="func">A function pointer which definition matches sa_plugin_config_update_func.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_plugin_register_config_update(sa_plugin_config_update_func func);

/// <summary>
/// Register a custom action factory function.
/// </summary>
/// <param name="name">The name of the custom action.</param>
/// <param name="func">A function pointer which definition matches sa_plugin_action_factory_func.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_plugin_register_action_event(const char* name, sa_plugin_action_event_func func);

#ifdef __cplusplus
#if 0
{  // do not indent code inside extern C
#endif
}  // extern "C"
#endif

#endif //SA_API_PLUGIN_H
