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

#include "PropertyManager.h"
#include "App.h"
#include "SaUtils.h"
#include "SelectionContext.h"
#include "LoggerHelper.h"
#include "RandomHelper.h"

#include "shellanything/version.h"

#include "rapidassist/environment_utf8.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/timing.h"

namespace shellanything
{
  static const int EXPANDING_MAX_ITERATIONS = 20;

  static const std::string EMPTY_VALUE;

  const std::string PropertyManager::SYSTEM_TRUE_PROPERTY_NAME = "system.true";
  const std::string PropertyManager::SYSTEM_TRUE_DEFAULT_VALUE = "true";
  const std::string PropertyManager::SYSTEM_FALSE_PROPERTY_NAME = "system.false";
  const std::string PropertyManager::SYSTEM_FALSE_DEFAULT_VALUE = "false";
  const std::string PropertyManager::SYSTEM_CLIPBOARD_PROPERTY_NAME = "clipboard";
  const std::string PropertyManager::SYSTEM_KEYBOARD_CTRL_PROPERTY_NAME   = "keyboard.ctrl";
  const std::string PropertyManager::SYSTEM_KEYBOARD_ALT_PROPERTY_NAME    = "keyboard.alt";
  const std::string PropertyManager::SYSTEM_KEYBOARD_SHIFT_PROPERTY_NAME  = "keyboard.shift";
  const std::string PropertyManager::SYSTEM_DATETIME_FORMAT_PROPERTY_NAME = "datetime.format";
  const std::string PropertyManager::SYSTEM_DATETIME_FORMAT_DEFAULT_VALUE = "yyyy-mm-dd HHhMMmSS";
  const std::string PropertyManager::SYSTEM_RANDOM_GUID_PROPERTY_NAME = "random.guid";
  const std::string PropertyManager::SYSTEM_RANDOM_FILE_PROPERTY_NAME = "random.file";
  const std::string PropertyManager::SYSTEM_RANDOM_PATH_PROPERTY_NAME = "random.path";
  const std::string PropertyManager::SYSTEM_LOGGING_VERBOSE_PROPERTY_NAME = "system.logging.verbose";

  PropertyManager::PropertyManager() :
    mInitialized(false)
  {
  }

  PropertyManager::~PropertyManager()
  {
    ClearLiveProperties();
  }

  PropertyManager& PropertyManager::GetInstance()
  {
    static PropertyManager _instance;
    if (!_instance.mInitialized)
    {
      _instance.mInitialized = true;

      // Initialize PropertyManager with default properties.
      // Note: The next calls will likely lead to another call to PropertyManager::GetInstance().
      // We are using the PropertyManager::mInitialized flag to prevent running into a circular reference
      _instance.RegisterEnvironmentVariables();
      _instance.RegisterFixedAndDefaultProperties();
      _instance.RegisterLiveProperties();
    }
    return _instance;
  }

  void PropertyManager::Clear()
  {
    properties.Clear();
    RegisterEnvironmentVariables();
    RegisterFixedAndDefaultProperties();
  }

  void PropertyManager::ClearProperty(const std::string& name)
  {
    properties.ClearProperty(name);
  }

  bool PropertyManager::HasProperty(const std::string& name) const
  {
    bool found = properties.HasProperty(name);
    if (!found)
      found = (GetLiveProperty(name) != NULL);
    return found;
  }

  bool PropertyManager::HasProperties(const StringList& properties_) const
  {
    for (size_t i = 0; i < properties_.size(); i++)
    {
      const std::string& name = properties_[i];
      if (!HasProperty(name))
        return false;
    }
    return true;
  }

  void PropertyManager::SetProperty(const std::string& name, const std::string& value)
  {
    // Prevent polluting the PropertyStore with live property names
    bool found = (GetLiveProperty(name) != NULL);
    if (found)
      return;

    SA_VERBOSE_LOG(INFO) << "Setting property '" << name << "' to value '" << value << "'.";

    properties.SetProperty(name, value);
  }

  std::string PropertyManager::GetProperty(const std::string& name) const
  {
    // Search within exiting properties
    bool found = properties.HasProperty(name);
    if (found)
    {
      const std::string& value = properties.GetProperty(name);
      return value;
    }

    // Search within live properties
    const ILiveProperty* p = GetLiveProperty(name);
    if (p)
    {
      std::string value = p->GetProperty();
      SA_VERBOSE_LOG(INFO) << "Live property '" << name << "' evaluates to value '" << value << "'.";
      return value;
    }

    return EMPTY_VALUE;
  }

  void PropertyManager::FindMissingProperties(const StringList& input_names, StringList& output_names) const
  {
    output_names.clear();
    for (size_t i = 0; i < input_names.size(); i++)
    {
      const std::string& name = input_names[i];
      if (!HasProperty(name))
        output_names.push_back(name);
    }
  }

  inline bool IsPropertyReference(const std::string& token_open, const std::string& token_close, const std::string& value, size_t offset, std::string& name)
  {
    size_t value_length = value.size();
    name.clear();
    if (offset >= value_length)
      return false;

    //Validate a token_open prefix at value[offset]
    for (size_t i = 0; i < token_open.size(); i++)
    {
      if (value[offset + i] != token_open[i])
        return false;
    }

    //Found a token_open position at value[offset]
    //Search for the token_close position.
    size_t name_start_pos = offset + token_open.size();
    size_t token_close_pos = value.find(token_close, name_start_pos);
    if (token_close_pos == std::string::npos)
      return false; // token_close not found

    //Found a token_close position.
    //Extract the name of the property.
    size_t length = (token_close_pos)-name_start_pos;
    if (length == 0)
      return false;
    std::string temp_name(&value[name_start_pos], length);

    //Validate if name
    PropertyManager& pmgr = PropertyManager::GetInstance();
    bool exists = pmgr.HasProperty(temp_name);
    if (exists)
      name = temp_name;
    return exists;
  }

  std::string PropertyManager::Expand(const std::string& value) const
  {
    int count = 1;
    std::string previous = value;
    std::string output = ExpandOnce(value);

    //Prevent circular reference by expanding at most 20 times.
    while (output != previous && count <= EXPANDING_MAX_ITERATIONS)
    {
      previous = output;
      output = ExpandOnce(output);
      count++;
    }

    return output;
  }

  std::string PropertyManager::ExpandOnce(const std::string& value) const
  {
    //Process expansion in-place
    std::string output;
    output.reserve(value.size() * 2);
    output = value;

    static const std::string token_open = "${";
    static const std::string token_close = "}";

    //Prevent circular reference by dfining a counter which counts how many time a character position was expanded.
    int count = 1;
    size_t previous_pos = std::string::npos;

    for (size_t i = 0; i < output.size(); i++)
    {
      //Count how many time we tried to expand this position
      if (i == previous_pos)
        count++; //same character as previous loop
      else
        count = 0; //that is a new character
      previous_pos = i;

      //If we find a property reference token at this location...
      std::string name;
      if (strncmp(&output[i], token_open.c_str(), token_open.size()) == 0 && IsPropertyReference(token_open, token_close, output, i, name))
      {
        //Found a property reference at output[i]
        const std::string& property_value = this->GetProperty(name);

        //Replace the property reference by the property's value
        size_t token_length = token_open.size() + name.size() + token_close.size();
        output.replace(output.begin() + i, output.begin() + i + token_length, property_value);

        //Prevent circular reference by expanding 20 times maximum.
        if (count < EXPANDING_MAX_ITERATIONS)
        {
          //Keep i at the same value for the next loop to process the same character position.
          //This is required if the property value also contains property references.
          i--; //-1 since the next for loop will increase i by 1.
        }
        else
        {
          //Reached the maximum of 20 loops.
          //Stop looking for property reference at this position.
          int a = 0;
        }
      }
    }

    return output;
  }

  void PropertyManager::AddLiveProperty(ILiveProperty* instance)
  {
    if (!instance)
      return;
    const std::string& name = instance->GetName();

    SA_VERBOSE_LOG(INFO) << "Registering live property '" << name << "'.";

    live_properties[name] = instance;
  }

  const ILiveProperty* PropertyManager::GetLiveProperty(const std::string& name) const
  {
    LivePropertyMap::const_iterator propertyIt = live_properties.find(name);
    bool found = (propertyIt != live_properties.end());
    if (found)
    {
      const ILiveProperty* instance = propertyIt->second;
      return instance;
    }

    return NULL;
  }

  void PropertyManager::GetLivePropertyNames(StringList& names) const
  {
    names.clear();
    LivePropertyMap::const_iterator it;
    for (it = live_properties.begin(); it != live_properties.end(); it++)
    {
      const ILiveProperty* instance = it->second;
      const std::string& name = instance->GetName();
      names.push_back(name);
    }
  }

  size_t PropertyManager::GetLivePropertyCount() const
  {
    return live_properties.size();
  }

  class ClipboardLiveProperty : public virtual ILiveProperty
  {
  public:

    virtual const std::string& GetName() const
    {
      return PropertyManager::SYSTEM_CLIPBOARD_PROPERTY_NAME;
    }

    virtual std::string GetProperty() const
    {
      // Try to read the clipboard's value.
      IClipboardService* service = App::GetInstance().GetClipboardService();
      if (!service) return EMPTY_VALUE;

      std::string clipboard_value;
      bool clipboard_read = service->GetClipboardText(clipboard_value);
      if (clipboard_read)
      {
        return clipboard_value;
      }
      return EMPTY_VALUE;
    }
  };

  class KeyboardModifierLiveProperty : public virtual ILiveProperty
  {
  private:
    std::string property_name;
    KEYB_MODIFIER_ID keyb_modifier_id;

  public:
    KeyboardModifierLiveProperty(const std::string& name, KEYB_MODIFIER_ID id) :
      property_name(name),
      keyb_modifier_id(id)
    {
    }

    ~KeyboardModifierLiveProperty()
    {
    }

    virtual const std::string& GetName() const
    {
      return property_name;
    }

    virtual std::string GetProperty() const
    {
      // Try to read the keyboard's modifier state.
      IKeyboardService* service = App::GetInstance().GetKeyboardService();
      if (!service) return EMPTY_VALUE;

      PropertyManager& pmgr = PropertyManager::GetInstance();
      if (service->IsModifierKeyDown(keyb_modifier_id))
      {
        return pmgr.GetProperty(PropertyManager::SYSTEM_TRUE_PROPERTY_NAME);
      }
      else
      {
        return pmgr.GetProperty(PropertyManager::SYSTEM_FALSE_PROPERTY_NAME);
      }
    }
  };

  class DateTimeLiveProperty : public virtual ILiveProperty
  {
  private:
    std::string property_name;
    std::string date_format;

  public:
    DateTimeLiveProperty(const std::string& name, const std::string & format) :
      property_name(name),
      date_format(format)
    {
    }

    ~DateTimeLiveProperty()
    {
    }

    virtual const std::string& GetName() const
    {
      return property_name;
    }

    virtual std::string GetProperty() const
    {
      ra::timing::DateTime now = ra::timing::ToDateTime(ra::timing::GetLocalTime());

      PropertyManager& pmgr = PropertyManager::GetInstance();
      std::string datetime = pmgr.Expand(date_format);

      // date
      ra::strings::Replace(datetime, "yyyy", ra::strings::ToString(now.year));
      ra::strings::Replace(datetime, "mm", ra::strings::Format("%02d", now.month));
      ra::strings::Replace(datetime, "dd", ra::strings::Format("%02d", now.day));

      // time
      ra::strings::Replace(datetime, "HH", ra::strings::Format("%02d", now.hour));
      ra::strings::Replace(datetime, "MM", ra::strings::Format("%02d", now.min));
      ra::strings::Replace(datetime, "SS", ra::strings::Format("%02d", now.sec));

      return datetime;
    }
  };

  class RandomGuidLiveProperty : public virtual ILiveProperty
  {
  public:

    virtual const std::string& GetName() const
    {
      return PropertyManager::SYSTEM_RANDOM_GUID_PROPERTY_NAME;
    }

    virtual std::string GetProperty() const
    {
      // Check for random service
      IRandomService* random_service = App::GetInstance().GetRandomService();
      if (!random_service) return EMPTY_VALUE;

      union GUID
      {
        uint32_t groups[4];
        uint8_t bytes[16];
      };

      GUID g;
      g.groups[0] = random_service->GetRandomValue(); // bytes  0 to  3
      g.groups[1] = random_service->GetRandomValue(); // bytes  4 to  7
      g.groups[2] = random_service->GetRandomValue(); // bytes  8 to 11
      g.groups[3] = random_service->GetRandomValue(); // bytes 12 to 15

      // Version 4
      // https://en.wikipedia.org/wiki/Universally_unique_identifier
      // https://www.intl-spectrum.com/Article/r848/IS_UUID_V4_UUID_V4_Random_Generation

      // Conforming to RFC 4122 Specification
      // Set the four most significant bits of the 7th byte '0100', so that the Hex value always starts with a 4,
      // Set the 2 most significant bits of the 9 th byte to '10', so that the Hex value will always start with a 8, 9, A , or B.
      // AA97B177-9383-4934-8543-0F91A7A02836
      g.bytes[6] = 0x40 + (g.bytes[6] & 0x0F);
      g.bytes[8] = 0x80 + (g.bytes[8] & 0x3F);

      // To string
      char buffer[64];
      sprintf(buffer, "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        (int)g.bytes[0],
        (int)g.bytes[1],
        (int)g.bytes[2],
        (int)g.bytes[3],
        (int)g.bytes[4],
        (int)g.bytes[5],
        (int)g.bytes[6],
        (int)g.bytes[7],
        (int)g.bytes[8],
        (int)g.bytes[9],
        (int)g.bytes[10],
        (int)g.bytes[11],
        (int)g.bytes[12],
        (int)g.bytes[13],
        (int)g.bytes[14],
        (int)g.bytes[15]
      );
      return buffer;
    }
  };

  class RandomFileLiveProperty : public virtual ILiveProperty
  {
  private:
    std::string property_name;
    bool prefix_temp_dir;

  public:
    RandomFileLiveProperty(const std::string& name, bool prefix) :
      property_name(name),
      prefix_temp_dir(prefix)
    {
    }

    ~RandomFileLiveProperty()
    {
    }

    virtual const std::string& GetName() const
    {
      return property_name;
    }

    virtual std::string GetProperty() const
    {
      // Check for random service
      IRandomService* random_service = App::GetInstance().GetRandomService();
      if (!random_service) return EMPTY_VALUE;

      std::string output;

      // Add TEMP directory if requested
      if (prefix_temp_dir)
      {
        output = ra::filesystem::GetTemporaryDirectoryUtf8();
        output.append(1, ra::filesystem::GetPathSeparator());
      }

      // Generate a random file name with 32 characters
      static const std::string pattern = "zzzzzzzzZZZZZZZZzzzzzzzzZZZZZZZZ";
      std::string filename;
      bool success = RandomHelper::GetRandomFromPattern(pattern, filename);
      if (!success)
        return EMPTY_VALUE;

      output += filename;
      output += ".tmp";

      return output;
    }
  };

  void PropertyManager::RegisterLiveProperties()
  {
    // Check if a live property instance already exists before adding one
    if (GetLiveProperty(SYSTEM_CLIPBOARD_PROPERTY_NAME) == NULL)            AddLiveProperty(new ClipboardLiveProperty());
    if (GetLiveProperty(SYSTEM_KEYBOARD_CTRL_PROPERTY_NAME) == NULL)        AddLiveProperty(new KeyboardModifierLiveProperty(SYSTEM_KEYBOARD_CTRL_PROPERTY_NAME, KMID_CTRL));
    if (GetLiveProperty(SYSTEM_KEYBOARD_ALT_PROPERTY_NAME) == NULL)         AddLiveProperty(new KeyboardModifierLiveProperty(SYSTEM_KEYBOARD_ALT_PROPERTY_NAME, KMID_ALT));
    if (GetLiveProperty(SYSTEM_KEYBOARD_SHIFT_PROPERTY_NAME) == NULL)       AddLiveProperty(new KeyboardModifierLiveProperty(SYSTEM_KEYBOARD_SHIFT_PROPERTY_NAME, KMID_SHIFT));
    if (GetLiveProperty(SYSTEM_RANDOM_GUID_PROPERTY_NAME) == NULL)          AddLiveProperty(new RandomGuidLiveProperty());
    if (GetLiveProperty(SYSTEM_RANDOM_FILE_PROPERTY_NAME) == NULL)          AddLiveProperty(new RandomFileLiveProperty(SYSTEM_RANDOM_FILE_PROPERTY_NAME, false));
    if (GetLiveProperty(SYSTEM_RANDOM_PATH_PROPERTY_NAME) == NULL)          AddLiveProperty(new RandomFileLiveProperty(SYSTEM_RANDOM_PATH_PROPERTY_NAME, true));

    const char* prop = NULL;
    prop = "date.year";         if (GetLiveProperty(prop) == NULL)    AddLiveProperty(new DateTimeLiveProperty(prop, "yyyy"));
    prop = "date.month";        if (GetLiveProperty(prop) == NULL)    AddLiveProperty(new DateTimeLiveProperty(prop, "mm"));
    prop = "date.day";          if (GetLiveProperty(prop) == NULL)    AddLiveProperty(new DateTimeLiveProperty(prop, "dd"));
    prop = "date.full";         if (GetLiveProperty(prop) == NULL)    AddLiveProperty(new DateTimeLiveProperty(prop, "yyyy-mm-dd"));
    prop = "date.short";        if (GetLiveProperty(prop) == NULL)    AddLiveProperty(new DateTimeLiveProperty(prop, "yyyymmdd"));

    prop = "time.hours";        if (GetLiveProperty(prop) == NULL)    AddLiveProperty(new DateTimeLiveProperty(prop, "HH"));
    prop = "time.minutes";      if (GetLiveProperty(prop) == NULL)    AddLiveProperty(new DateTimeLiveProperty(prop, "MM"));
    prop = "time.seconds";      if (GetLiveProperty(prop) == NULL)    AddLiveProperty(new DateTimeLiveProperty(prop, "SS"));
    prop = "time.full";         if (GetLiveProperty(prop) == NULL)    AddLiveProperty(new DateTimeLiveProperty(prop, "HH-MM-SS"));
    prop = "time.short";        if (GetLiveProperty(prop) == NULL)    AddLiveProperty(new DateTimeLiveProperty(prop, "HHMMSS"));

    prop = "datetime.full";     if (GetLiveProperty(prop) == NULL)    AddLiveProperty(new DateTimeLiveProperty(prop, "yyyy-mm-dd--HH-MM-SS"));
    prop = "datetime.short";    if (GetLiveProperty(prop) == NULL)    AddLiveProperty(new DateTimeLiveProperty(prop, "yyyymmddHHMMSS"));
    prop = "datetime.custom";   if (GetLiveProperty(prop) == NULL)    AddLiveProperty(new DateTimeLiveProperty(prop, "${datetime.format}"));
  }

  void PropertyManager::ClearLiveProperty(const std::string& name)
  {
    LivePropertyMap::const_iterator propertyIt = live_properties.find(name);
    bool found = (propertyIt != live_properties.end());
    if (found)
    {
      live_properties.erase(propertyIt);
    }
  }

  void PropertyManager::ClearLiveProperties()
  {
    for (LivePropertyMap::const_iterator it = live_properties.begin(); it != live_properties.end(); ++it)
    {
      const std::string& key = (it->first);
      const ILiveProperty* instance = (it->second);
      delete instance;
    }
    live_properties.clear();
  }

  void PropertyManager::SplitAndExpand(const std::string& input_value, const char* separator, StringList& output_list)
  {
    PropertyManager& pmgr = PropertyManager::GetInstance();

    output_list.clear();
    if (separator == NULL)
      return;

    //Loop through each values
    ra::strings::StringVector values = ra::strings::Split(input_value, separator);
    for (size_t i = 0; i < values.size(); i++)
    {
      //Expand each value
      const std::string& tmp = values[i];
      const std::string expanded = pmgr.Expand(tmp);
      output_list.push_back(tmp);
    }
  }

  void PropertyManager::ExpandAndSplit(const std::string& input_value, const char* separator, StringList& output_list)
  {
    PropertyManager& pmgr = PropertyManager::GetInstance();

    output_list.clear();
    if (separator == NULL)
      return;

    //Expand the given value
    const std::string expanded = pmgr.Expand(input_value);

    //Loop through each values
    ra::strings::StringVector values = ra::strings::Split(expanded, separator);
    for (size_t i = 0; i < values.size(); i++)
    {
      const std::string& tmp = values[i];
      output_list.push_back(tmp);
    }
  }

  void PropertyManager::RegisterEnvironmentVariables()
  {
    SA_DECLARE_SCOPE_LOGGER_INFO(sli);
    sli.verbose = true;
    ScopeLogger logger(&sli);

    //Work around for https://github.com/end2endzone/RapidAssist/issues/54
    ra::environment::GetEnvironmentVariableUtf8("foo");

    //register all environment variables
    ra::strings::StringVector vars = ra::environment::GetEnvironmentVariablesUtf8();
    for (size_t i = 0; i < vars.size(); i++)
    {
      const std::string& var = vars[i];

      std::string name = "env." + var;
      std::string value = ra::environment::GetEnvironmentVariableUtf8(var.c_str());

      //register the variable as a valid property
      SetProperty(name, value);
    }
  }

  void PropertyManager::RegisterFixedAndDefaultProperties()
  {
    SA_DECLARE_SCOPE_LOGGER_INFO(sli);
    sli.verbose = true;
    ScopeLogger logger(&sli);

    shellanything::App& app = shellanything::App::GetInstance();

    //define global properties
    std::string prop_core_module_path = GetCurrentModulePathUtf8();
    std::string prop_application_directory = ra::filesystem::GetParentPath(prop_core_module_path);
    std::string prop_install_directory = ra::filesystem::GetParentPath(prop_application_directory);
    std::string prop_path_separator = ra::filesystem::GetPathSeparatorStr();
    std::string prop_line_separator = ra::environment::GetLineSeparator();

    // Issue #124. Define property 'application.path'.
    // We can not call usual function to detect the path of the application.
    // For example,
    //   GetCurrentModulePathUtf8() returns the path of sa.core.dll. Not sa.tests.exe or sa.shellextension.dll.
    //   GetCurrentProcessPath() would return the path of explorer.exe when the shell extension is loaded.
    //
    std::string prop_application_path = app.GetApplicationPath();
    
    SetProperty("application.path", prop_application_path);
    SetProperty("application.directory", prop_application_directory);
    SetProperty("application.install.directory", prop_install_directory);
    SetProperty("application.version", SHELLANYTHING_VERSION);
    SetProperty("path.separator", prop_path_separator);
    SetProperty("line.separator", prop_line_separator);
    SetProperty("newline", prop_line_separator);

    SetProperty(PropertyManager::SYSTEM_TRUE_PROPERTY_NAME, PropertyManager::SYSTEM_TRUE_DEFAULT_VALUE);
    SetProperty(PropertyManager::SYSTEM_FALSE_PROPERTY_NAME, PropertyManager::SYSTEM_FALSE_DEFAULT_VALUE);
    SetProperty(PropertyManager::SYSTEM_DATETIME_FORMAT_PROPERTY_NAME, PropertyManager::SYSTEM_DATETIME_FORMAT_DEFAULT_VALUE);

    // Set default property for multi selection. Issue #52.
    SetProperty(SelectionContext::MULTI_SELECTION_SEPARATOR_PROPERTY_NAME, SelectionContext::DEFAULT_MULTI_SELECTION_SEPARATOR);
  }

} //namespace shellanything
