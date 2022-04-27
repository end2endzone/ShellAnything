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

using namespace shellanything;

#define SA_API_LOG_IDDENTIFIER "API"

sa_selection_context_immutable_t g_update_selection_context;
sa_selection_context_immutable_t g_validation_selection_context;
sa_property_store_immutable_t g_validation_property_store;

void ToCStringArray(std::vector<const char*> & destination, const std::vector<std::string>& values)
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
  {}
  virtual ~PluginAttributeValidator() {}

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

    // check callback
    if (mValidationFunc == NULL)
    {
      sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Missing validation function for attribute '%s'.", mNames[0].c_str());
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
    int valid = mValidationFunc();

    // invalidate global validation objects
    memset(&g_validation_selection_context, 0, sizeof(g_validation_selection_context));
    memset(&g_validation_property_store, 0, sizeof(g_validation_property_store));

    if (valid)
      return true;
    return false;
  }

  void SetValidationFunction(sa_plugin_validate_callback_func func)
  {
    mValidationFunc = func;
  }

private:
  StringList mNames;
  const SelectionContext* mSelection;
  const PropertyStore* mAttributes;
  sa_plugin_validate_callback_func mValidationFunc;
};

class PluginUpdateCallback : public virtual IUpdateCallback
{
public:
  PluginUpdateCallback() :
    mSelection(NULL),
    mValidationFunc(NULL)
  {}
  virtual ~PluginUpdateCallback() {}

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

  void SetUpdateCallbackFunction(sa_plugin_update_callback_func func)
  {
    mValidationFunc = func;
  }

private:
  const SelectionContext* mSelection;
  sa_plugin_update_callback_func mValidationFunc;
};

sa_selection_context_immutable_t* sa_plugin_validation_get_selection_context()
{
  return &g_validation_selection_context;
}

sa_property_store_immutable_t* sa_plugin_validation_get_property_store()
{
  return &g_validation_property_store;
}

sa_selection_context_immutable_t* sa_plugin_update_get_selection_context()
{
  return &g_update_selection_context;
}

sa_error_t sa_plugin_register_attribute_validation(const char* names[], size_t count, sa_plugin_validate_callback_func func)
{
  Plugin* plugin = Plugin::GetLoadingPlugin();
  if (plugin == NULL)
  {
    sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Failed to register a validator for attribute '%s'. Current plugin is unknown.", names[0]);
    return SA_ERROR_MISSING_RESOURCE;
  }

  PluginAttributeValidator* validator = new PluginAttributeValidator();
  validator->SetAttributeNames(names, count);
  validator->SetValidationFunction(func);

  plugin->GetRegistry().AddAttributeValidator(validator);

  return SA_ERROR_SUCCESS;
}

sa_error_t sa_plugin_register_update_callback(sa_plugin_update_callback_func func)
{
  Plugin* plugin = Plugin::GetLoadingPlugin();
  if (plugin == NULL)
  {
    sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Failed to register an update callback. Current plugin is unknown.");
    return SA_ERROR_MISSING_RESOURCE;
  }

  PluginUpdateCallback* update_callback = new PluginUpdateCallback();
  update_callback->SetUpdateCallbackFunction(func);

  plugin->GetRegistry().AddUpdateCallback(update_callback);

  return SA_ERROR_SUCCESS;
}
