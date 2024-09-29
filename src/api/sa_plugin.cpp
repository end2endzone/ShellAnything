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

#include "shellanything/sa_plugin.h"
#include "shellanything/sa_logging.h"
#include "sa_string_private.h"
#include "sa_private_casting.h"
#include "IAttributeValidator.h"
#include "IUpdateCallback.h"
#include "Plugin.h"

#include "rapidassist/strings.h"

using namespace shellanything;

#define SA_API_LOG_IDDENTIFIER "PLUGIN API"

sa_selection_context_immutable_t g_update_selection_context;
sa_selection_context_immutable_t g_validation_selection_context;
sa_property_store_immutable_t g_validation_property_store;
sa_property_store_t g_action_property_store;
sa_selection_context_immutable_t g_action_selection_context;
const char* g_action_name;
const char* g_action_xml;
void* g_action_data;

void ToCStringArray(std::vector<const char*>& destination, const std::vector<std::string>& values)
{
  destination.clear();
  for (size_t i = 0; i < values.size(); i++)
  {
    const std::string& value_str = values[i];
    const char* value_cstr = value_str.c_str();
    destination.push_back(value_cstr);
  }
}

class PluginAttributeValidator : public virtual IAttributeValidator
{
public:
  PluginAttributeValidator() :
    mSelection(NULL),
    mAttributes(NULL),
    mValidationFunc(NULL)
  {
  }
  virtual ~PluginAttributeValidator()
  {
  }

  virtual bool IsAttributesSupported(const StringList& names) const
  {
    for (size_t i = 0; i < names.size(); i++)
    {
      bool found = false;
      const std::string& expected = names[i];

      // search this expected name within known names
      for (size_t j = 0; j < mNames.size() && !found; j++)
      {
        const std::string& current = mNames[j];
        if (current == expected)
          found = true;
      }

      // if the expected name is not found, this instance does not support this list of attribute names
      if (!found)
        return false;
    }

    return true;
  }

  const StringList& GetAttributeNames() const
  {
    return mNames;
  }

  void SetAttributeNames(const char* names[], size_t count)
  {
    for (size_t i = 0; i < count; i++)
    {
      const char* name = names[i];
      mNames.push_back(name);
    }
  }

  virtual void SetSelectionContext(const SelectionContext* context)
  {
    mSelection = context;
  }

  virtual const SelectionContext* GetSelectionContext() const
  {
    return mSelection;
  }

  virtual void SetCustomAttributes(const PropertyStore* store)
  {
    mAttributes = store;
  }

  virtual const PropertyStore* GetCustomAttributes() const
  {
    return mAttributes;
  }

  bool Validate() const
  {
    // check names
    if (mNames.empty())
    {
      sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Missing attribute names.");
      return false;
    }

    std::string names_csv = ra::strings::Join(mNames, ",").c_str();

    // check callback
    if (mValidationFunc == NULL)
    {
      sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Missing validation function for attributes '%s'.", names_csv.c_str());
      return false;
    }

    // initialize the global objects for the validation
    memset(&g_validation_selection_context, 0, sizeof(g_validation_selection_context));
    memset(&g_validation_property_store, 0, sizeof(g_validation_property_store));
    if (mSelection != NULL)
      g_validation_selection_context = AS_TYPE_SELECTION_CONTEXT(mSelection);
    if (mAttributes != NULL)
      g_validation_property_store = AS_TYPE_PROPERTY_STORE(mAttributes);

    // call the validation function of the plugin
    sa_logging_print_format(SA_LOG_LEVEL_INFO, SA_API_LOG_IDDENTIFIER, "Processing validation of attributes '%s'.", names_csv.c_str());
    int valid = mValidationFunc();

    // invalidate global validation objects
    memset(&g_validation_selection_context, 0, sizeof(g_validation_selection_context));
    memset(&g_validation_property_store, 0, sizeof(g_validation_property_store));

    if (valid)
      return true;
    return false;
  }

  void SetValidationFunction(sa_plugin_validation_attributes_func func)
  {
    mValidationFunc = func;
  }

private:
  StringList mNames;
  const SelectionContext* mSelection;
  const PropertyStore* mAttributes;
  sa_plugin_validation_attributes_func mValidationFunc;
};

class PluginUpdateCallback : public virtual IUpdateCallback
{
public:
  PluginUpdateCallback() :
    mSelection(NULL),
    mValidationFunc(NULL)
  {
  }
  virtual ~PluginUpdateCallback()
  {
  }

  virtual void SetSelectionContext(const SelectionContext* context)
  {
    mSelection = context;
  }

  virtual const SelectionContext* GetSelectionContext() const
  {
    return mSelection;
  }

  void OnNewSelection() const
  {
    // check callback
    if (mValidationFunc == NULL)
    {
      sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Missing update callback function.");
      return;
    }

    // initialize the global objects for the update
    memset(&g_update_selection_context, 0, sizeof(g_update_selection_context));
    if (mSelection != NULL)
      g_update_selection_context = AS_TYPE_SELECTION_CONTEXT(mSelection);

    // call the update callback function of the plugin
    mValidationFunc();

    // invalidate global update objects
    memset(&g_update_selection_context, 0, sizeof(g_update_selection_context));
  }

  void SetUpdateCallbackFunction(sa_plugin_config_update_func func)
  {
    mValidationFunc = func;
  }

private:
  const SelectionContext* mSelection;
  sa_plugin_config_update_func mValidationFunc;
};

class PluginAction : public virtual IAction
{
public:
  PluginAction() :
    mMenu(NULL),
    mStore(NULL),
    mData(NULL),
    mActionEventFunc(NULL)
  {
  }
  virtual ~PluginAction()
  {
    // check callback
    if (mActionEventFunc == NULL)
      sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Missing action event function.");

    // initialize the global objects for the DESTROY event
    memset(&g_action_property_store, 0, sizeof(g_action_property_store));
    if (mStore)
      g_action_property_store = AS_TYPE_PROPERTY_STORE(mStore);
    g_action_name = mName.c_str();
    g_action_xml = NULL;
    g_action_data = mData;

    // call the action event function of the plugin
    sa_logging_print_format(SA_LOG_LEVEL_INFO, SA_API_LOG_IDDENTIFIER, "Destroying action '%s'", mName.c_str());
    sa_action_event_t evnt = SA_ACTION_EVENT_DESTROY;
    sa_error_t result = mActionEventFunc(evnt);

    // invalidate global update objects
    memset(&g_action_property_store, 0, sizeof(g_action_property_store));
    g_action_name = NULL;
    g_action_xml = NULL;
    g_action_data = NULL;

    // if a destruction has failed
    if (result != SA_ERROR_SUCCESS)
    {
      const char* error = sa_error_get_error_description(result);
      sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Failed destroying action '%s': %s", mName.c_str(), error);
    }

    if (mStore)
      delete mStore;
  }

  virtual void SetName(const char* name)
  {
    mName = name;
  }

  virtual void SetData(void* data)
  {
    mData = data;
  }

  virtual void SetPropertyStore(PropertyStore* store)
  {
    mStore = store;
  }

  void SetActionEventFunction(sa_plugin_action_event_func func)
  {
    mActionEventFunc = func;
  }

  virtual Menu* GetParentMenu()
  {
    return mMenu;
  }

  virtual const Menu* GetParentMenu() const
  {
    return mMenu;
  }

  virtual void SetParentMenu(Menu* menu)
  {
    mMenu = menu;
  }

  virtual bool Execute(const SelectionContext& context) const
  {
    // check callback
    if (mActionEventFunc == NULL)
    {
      sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Missing action event function.");
      return false;
    }

    // initialize the global objects for the EXECUTE event
    memset(&g_action_property_store, 0, sizeof(g_action_property_store));
    if (mStore)
      g_action_property_store = AS_TYPE_PROPERTY_STORE(mStore);
    g_action_name = mName.c_str();
    g_action_xml = NULL;
    g_action_data = mData;

    // call the action event function of the plugin
    sa_logging_print_format(SA_LOG_LEVEL_INFO, SA_API_LOG_IDDENTIFIER, "Executing action '%s'", mName.c_str());
    sa_action_event_t evnt = SA_ACTION_EVENT_EXECUTE;
    sa_error_t result = mActionEventFunc(evnt);

    mData = g_action_data;

    // invalidate global update objects
    memset(&g_action_property_store, 0, sizeof(g_action_property_store));
    g_action_name = NULL;
    g_action_xml = NULL;
    g_action_data = NULL;

    // if a execute failed
    if (result != SA_ERROR_SUCCESS)
    {
      const char* error = sa_error_get_error_description(result);
      sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Failed executing action '%s': %s.", mName.c_str(), error);
      return false;
    }

    return true;
  }

private:
  Menu* mMenu;
  PropertyStore* mStore;
  std::string mName;
  mutable void* mData;
  sa_plugin_action_event_func mActionEventFunc;
};

class PluginActionFactory : public virtual IActionFactory
{
public:
  PluginActionFactory() :
    mActionEventFunc(NULL)
  {
  }
  virtual ~PluginActionFactory()
  {
  }

  virtual void SetName(const char* name)
  {
    mName = name;
  }

  virtual const std::string& GetName() const
  {
    return mName;
  }

  void SetActionEventFunction(sa_plugin_action_event_func func)
  {
    mActionEventFunc = func;
  }

  virtual IAction* ParseFromXml(const std::string& xml, std::string& error) const
  {
    // check callback
    if (mActionEventFunc == NULL)
    {
      sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Missing action event function.");
      return NULL;
    }

    void* data = NULL;
    PropertyStore* store = new PropertyStore();

    // initialize the global objects for the CREATE event
    g_action_property_store = AS_TYPE_PROPERTY_STORE(store);
    g_action_name = mName.c_str();
    g_action_xml = xml.c_str();
    g_action_data = data;

    // call the action event function of the plugin
    sa_logging_print_format(SA_LOG_LEVEL_INFO, SA_API_LOG_IDDENTIFIER, "Parsing action '%s'", mName.c_str());
    sa_action_event_t evnt = SA_ACTION_EVENT_CREATE;
    sa_error_t result = mActionEventFunc(evnt);

    data = g_action_data;

    // invalidate global update objects
    memset(&g_action_property_store, 0, sizeof(g_action_property_store));
    g_action_name = NULL;
    g_action_xml = NULL;
    g_action_data = NULL;

    // if a parsing is succesful
    if (result == SA_ERROR_SUCCESS)
    {
      PluginAction* action = new PluginAction();
      action->SetName(mName.c_str());
      action->SetData(data);
      action->SetPropertyStore(store);
      action->SetActionEventFunction(mActionEventFunc);
      return action;
    }
    else
    {
      // a parsing error occured
      delete store;
      error = sa_error_get_error_description(result);
      return NULL;
    }
  }

private:
  std::string mName;
  sa_plugin_action_event_func mActionEventFunc;
};


sa_selection_context_immutable_t* sa_plugin_validation_get_selection_context()
{
  return &g_validation_selection_context;
}

sa_property_store_immutable_t* sa_plugin_validation_get_property_store()
{
  return &g_validation_property_store;
}

sa_selection_context_immutable_t* sa_plugin_config_update_get_selection_context()
{
  return &g_update_selection_context;
}

const char* sa_plugin_action_get_name()
{
  return g_action_name;
}

const char* sa_plugin_action_get_xml()
{
  return g_action_xml;
}

void* sa_plugin_action_get_data()
{
  return g_action_data;
}

void sa_plugin_action_set_data(void* data)
{
  g_action_data = data;
}

sa_property_store_t* sa_plugin_action_get_property_store()
{
  return &g_action_property_store;
}

sa_error_t sa_plugin_register_validation_attributes(const char* names[], size_t count, sa_plugin_validation_attributes_func func)
{
  if (names == NULL || func == NULL || count == 0)
  {
    sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Failed to register a validator. Unknown attribute names or function.");
    return SA_ERROR_INVALID_ARGUMENTS;
  }

  Plugin* plugin = Plugin::GetLoadingPlugin();
  if (plugin == NULL)
  {
    sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Failed to register a validator for attribute '%s'. Current plugin is unknown.", names[0]);
    return SA_ERROR_MISSING_RESOURCE;
  }

  // Check if all regitering conditions are declared by the plugin xml
  for (size_t i = 0; i < count; i++)
  {
    const char* name = names[i];
    if (!plugin->SupportCondition(name))
    {
      sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Failed to register a validator for attribute '%s'. The plugin '%s' does not report this condition.", name, plugin->GetPath().c_str());
      return SA_ERROR_NOT_SUPPORTED;
    }
  }

  PluginAttributeValidator* validator = new PluginAttributeValidator();
  validator->SetAttributeNames(names, count);
  validator->SetValidationFunction(func);

  plugin->GetRegistry().AddAttributeValidator(validator);

  return SA_ERROR_SUCCESS;
}

sa_error_t sa_plugin_register_config_update(sa_plugin_config_update_func func)
{
  if (func == NULL)
  {
    sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Failed to register an update callback function. Unknown function.");
    return SA_ERROR_INVALID_ARGUMENTS;
  }

  Plugin* plugin = Plugin::GetLoadingPlugin();
  if (plugin == NULL)
  {
    sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Failed to register an update callback function. Current plugin is unknown.");
    return SA_ERROR_MISSING_RESOURCE;
  }

  PluginUpdateCallback* update_callback = new PluginUpdateCallback();
  update_callback->SetUpdateCallbackFunction(func);

  plugin->GetRegistry().AddUpdateCallback(update_callback);

  return SA_ERROR_SUCCESS;
}

sa_error_t sa_plugin_register_action_event(const char* name, sa_plugin_action_event_func func)
{
  if (name == NULL || func == NULL)
  {
    sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Failed to register action event function. Unknown action name or function.");
    return SA_ERROR_INVALID_ARGUMENTS;
  }

  Plugin* plugin = Plugin::GetLoadingPlugin();
  if (plugin == NULL)
  {
    sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Failed to register action '%s' event function. Current plugin is unknown.", name);
    return SA_ERROR_MISSING_RESOURCE;
  }

  // Check if the regitering action is declared by the plugin xml
  if (!plugin->SupportAction(name))
  {
    sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Failed to register action '%s' event function. The plugin '%s' does not report this action.", name, plugin->GetPath().c_str());
    return SA_ERROR_NOT_SUPPORTED;
  }

  PluginActionFactory* factory = new PluginActionFactory();
  factory->SetName(name);
  factory->SetActionEventFunction(func);

  plugin->GetRegistry().AddActionFactory(factory);

  return SA_ERROR_SUCCESS;
}
