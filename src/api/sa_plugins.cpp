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

#include "shellanything/sa_plugins.h"
#include "shellanything/sa_logging.h"
#include "sa_string_private.h"
#include "sa_types_private.h"
#include "IAttributeValidator.h"
#include "Plugin.h"

using namespace shellanything;

#define SA_API_LOG_IDDENTIFIER "API"

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
  PluginAttributeValidator() : mValidationFunc(NULL) {}
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

  virtual void SetAttributeValues(const StringList& values)
  {
    mValues = values;
  }

  virtual void SetAttributeFlags(const IntList& flags)
  {
    mFlags = flags;
  }

  bool Validate(const SelectionContext& context) const
  {
    // check names
    if (mNames.empty())
    {
      sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Missing attribute names.");
      return false;
    }

    //check values and flags
    for (size_t i = 0; i < mNames.size(); i++)
    {
      const std::string& name = mNames[i];
      if (i >= mValues.size())
      {
        sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Missing value for attribute name '%s'.", name.c_str());
        return false;
      }
      if (i >= mFlags.size())
      {
        sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Missing flag for attribute name '%s'.", name.c_str());
        return false;
      }
    }
    if (mNames.size() < mValues.size())
    {
      sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Too many values specified for the attribute validator.");
      return false;
    }
    if (mNames.size() < mFlags.size())
    {
      sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Too many flags specified for the attribute validator.");
      return false;
    }

    // check callback
    if (mValidationFunc == NULL)
    {
      sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Missing validation function for attribute '%s'.", mNames[0].c_str());
      return false;
    }

    // call the validation function of the plugin
    std::vector<const char*> names;
    std::vector<const char*> values;
    ToCStringArray(names, mNames);
    ToCStringArray(values, mValues);
    sa_context_immutable_t ctx = AS_TYPE_SELECTIONCONTEXT(&context);
    int valid = mValidationFunc(&ctx, &names[0], &values[0], &mFlags[0], names.size());
    if (valid)
      return true;
    return false;
  }

  void SetAttributeNames(const char* names[], size_t count)
  {
    for (size_t i = 0; i < count; i++)
    {
      const char* name = names[i];
      mNames.push_back(name);
    }
  }

  void SetValidationFunction(sa_plugin_attribute_validate_func func)
  {
    mValidationFunc = func;
  }

private:
  StringList mNames;
  StringList mValues;
  IntList mFlags;
  sa_plugin_attribute_validate_func mValidationFunc;
};

sa_error_t sa_plugins_register_attribute_validation(const char* names[], size_t count, sa_plugin_attribute_validate_func func)
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
