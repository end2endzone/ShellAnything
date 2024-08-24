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

#include "shellanything/sa_plugin_definitions.h"
#include <string>
#include <limits>
#include "Validator.h"
#include "PropertyManager.h"
#include "ConfigFile.h"
#include "DriveClass.h"
#include "Wildcard.h"
#include "LoggerHelper.h"
#include "KeyboardHelper.h"
#include "SaUtils.h"
#include "libexprtk.h"
#include "rapidassist/strings.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/environment.h"


namespace shellanything
{
  const std::string& Validator::ATTRIBUTE_MAXFILES = "maxfiles";
  const std::string& Validator::ATTRIBUTE_MAXDIRECTORIES = "maxfolders";
  const std::string& Validator::ATTRIBUTE_PROPERTIES = "properties";
  const std::string& Validator::ATTRIBUTE_FILEEXTENSIONS = "fileextensions";
  const std::string& Validator::ATTRIBUTE_EXISTS = "exists";
  const std::string& Validator::ATTRIBUTE_CLASS = "class";
  const std::string& Validator::ATTRIBUTE_PATTERN = "pattern";
  const std::string& Validator::ATTRIBUTE_EXPRTK = "exprtk";
  const std::string& Validator::ATTRIBUTE_ISTRUE = "istrue";
  const std::string& Validator::ATTRIBUTE_ISFALSE = "isfalse";
  const std::string& Validator::ATTRIBUTE_ISEMPTY = "isempty";
  const std::string& Validator::ATTRIBUTE_KEYBOARD = "keyboard";
  const std::string& Validator::ATTRIBUTE_INSERVE = "inverse";

  const std::string& INSERVE_ATTR_PREFIX = "inverse ";

  bool HasValue(const ra::strings::StringVector& values, const std::string& token)
  {
    for (size_t i = 0; i < values.size(); i++)
    {
      const std::string& v = values[i];
      if (token == v)
        return true;
    }
    return false;
  }

  void Uppercase(ra::strings::StringVector& values)
  {
    for (size_t i = 0; i < values.size(); i++)
    {
      std::string tmp = ra::strings::Uppercase(values[i]);
      values[i] = tmp;
    }
  }

  inline std::string GetPrevalidateMessage(const char* attr_name, bool inversed, const std::string & value)
  {
    std::string s;
    s.reserve(128);

    s += "Validating ";
    if (inversed)
      s += "inversed ";
    s += "attribute ";
    s += "'";
    s += attr_name;
    s += "' against value '";
    s += value;
    s += "'.";
    return s;
  }

  inline std::string GetAttrFailMessage(const Validator* this_instance, const char* attr_name, bool inversed)
  {
    std::string s;
    s.reserve(128);

    s += "Validator ";
    s += ToHexString(this_instance);
    s += " has failed ";
    if (inversed)
      s += "inversed ";
    s += "'";
    s += attr_name;
    s += "' validation.";
    return s;
  }

  inline std::string GetCheckFailMessage(const Validator* this_instance, bool inversed)
  {
    std::string s;
    s.reserve(128);

    s += "Validator ";
    s += ToHexString(this_instance);
    s += " ";
    if (inversed)
      s += "inversed ";
    s += "check fail: ";
    return s;
  }

  static const int MAX_FILES = std::numeric_limits<int>::max();
  static const int MAX_DIRS = std::numeric_limits<int>::max();


  Validator::Validator() :
    mMaxFiles(MAX_FILES),
    mMaxDirectories(MAX_DIRS),
    mLastValidateSuccessful(true)
  {
  }

  Validator::~Validator()
  {
  }

  Menu* Validator::GetParentMenu()
  {
    return mParentMenu;
  }

  const Menu* Validator::GetParentMenu() const
  {
    return mParentMenu;
  }

  void Validator::SetParentMenu(Menu* menu)
  {
    mParentMenu = menu;
  }

  const int& Validator::GetMaxFiles() const
  {
    return mMaxFiles;
  }

  void Validator::SetMaxFiles(const int& max_files)
  {
    std::string str_value = ra::strings::ToString(max_files);
    mAttributes.SetProperty(ATTRIBUTE_MAXFILES, str_value);
    mMaxFiles = max_files;
  }

  const int& Validator::GetMaxDirectories() const
  {
    return mMaxDirectories;
  }

  void Validator::SetMaxDirectories(const int& max_directories)
  {
    std::string str_value = ra::strings::ToString(max_directories);
    mAttributes.SetProperty(ATTRIBUTE_MAXDIRECTORIES, str_value);
    mMaxDirectories = max_directories;
  }

  const std::string& Validator::GetProperties() const
  {
    return mAttributes.GetProperty(ATTRIBUTE_PROPERTIES);
  }

  void Validator::SetProperties(const std::string& properties)
  {
    mAttributes.SetProperty(ATTRIBUTE_PROPERTIES, properties);
  }

  const std::string& Validator::GetFileExtensions() const
  {
    return mAttributes.GetProperty(ATTRIBUTE_FILEEXTENSIONS);
  }

  void Validator::SetFileExtensions(const std::string& file_extensions)
  {
    mAttributes.SetProperty(ATTRIBUTE_FILEEXTENSIONS, file_extensions);
  }

  const std::string& Validator::GetFileExists() const
  {
    return mAttributes.GetProperty(ATTRIBUTE_EXISTS);
  }

  void Validator::SetFileExists(const std::string& file_exists)
  {
    mAttributes.SetProperty(ATTRIBUTE_EXISTS, file_exists);
  }

  const std::string& Validator::GetClass() const
  {
    return mAttributes.GetProperty(ATTRIBUTE_CLASS);
  }

  void Validator::SetClass(const std::string& classes)
  {
    mAttributes.SetProperty(ATTRIBUTE_CLASS, classes);
  }

  const std::string& Validator::GetPattern() const
  {
    return mAttributes.GetProperty(ATTRIBUTE_PATTERN);
  }

  void Validator::SetPattern(const std::string& pattern)
  {
    mAttributes.SetProperty(ATTRIBUTE_PATTERN, pattern);
  }

  const std::string& Validator::GetExprtk() const
  {
    return mAttributes.GetProperty(ATTRIBUTE_EXPRTK);
  }

  void Validator::SetExprtk(const std::string& exprtk)
  {
    mAttributes.SetProperty(ATTRIBUTE_EXPRTK, exprtk);
  }

  const std::string& Validator::GetIsTrue() const
  {
    return mAttributes.GetProperty(ATTRIBUTE_ISTRUE);
  }

  void Validator::SetIsTrue(const std::string& istrue)
  {
    mAttributes.SetProperty(ATTRIBUTE_ISTRUE, istrue);
  }

  const std::string& Validator::GetIsFalse() const
  {
    return mAttributes.GetProperty(ATTRIBUTE_ISFALSE);
  }

  void Validator::SetIsFalse(const std::string& isfalse)
  {
    mAttributes.SetProperty(ATTRIBUTE_ISFALSE, isfalse);
  }

  const std::string& Validator::GetIsEmpty() const
  {
    return mAttributes.GetProperty(ATTRIBUTE_ISEMPTY);
  }

  void Validator::SetIsEmpty(const std::string& isempty)
  {
    mAttributes.SetProperty(ATTRIBUTE_ISEMPTY, isempty);
  }

  const std::string& Validator::GetKeyboard() const
  {
    return mAttributes.GetProperty(ATTRIBUTE_KEYBOARD);
  }

  void Validator::SetKeyboard(const std::string& keyboard)
  {
    mAttributes.SetProperty(ATTRIBUTE_KEYBOARD, keyboard);
  }

  const PropertyStore& Validator::GetCustomAttributes() const
  {
    return mCustomAttributes;
  }

  void Validator::SetCustomAttributes(const PropertyStore& attributes)
  {
    mCustomAttributes = attributes;
  }

  const std::string& Validator::GetInserve() const
  {
    return mAttributes.GetProperty(ATTRIBUTE_INSERVE);
  }

  void Validator::SetInserve(const std::string& inserve)
  {
    mAttributes.SetProperty(ATTRIBUTE_INSERVE, inserve);
  }

  bool Validator::IsInversed(const char* name) const
  {
    if (name == NULL)
      return false;
    if (name[0] == '\0')
      return false;

    std::string tmp_name = name;

    // Validate with 'all' attribute
    if (tmp_name != "all")
    {
      if (IsInversed("all"))
        return true;
    }

    const std::string& inverse_attr = mAttributes.GetProperty(Validator::ATTRIBUTE_INSERVE);

    size_t name_length = tmp_name.size();
    size_t search_index = 0;
    size_t pos = inverse_attr.find(name, search_index);
    while (pos != std::string::npos)
    {
      // The name of the attribute was found.

      // To prevent finding attribute 'foo' from attribute 'foobar',
      // The end of the attribute should match the end of the mInverse string or
      // followed by a ';' character.
      char next = '\0';
      if (pos + name_length < inverse_attr.size())
        next = inverse_attr[pos + name_length];

      if (next == '\0' || next == SA_INVERSE_ATTR_SEPARATOR_CHAR)
      {
        // Good. Keep looking

        // To prevent finding attribute 'bar' from attribute 'foobar',
        // The beginning of the attribute should match the beginning of the mInverse string or
        // the previous character must be a ';' character.
        if (pos == 0)
          return true; // We have a match!

        char previous = SA_INVERSE_ATTR_SEPARATOR_CHAR;
        if (pos >= 1)
          previous = inverse_attr[pos - 1];

        if (previous == SA_INVERSE_ATTR_SEPARATOR_CHAR)
          return true; // We have a match!
      }

      // This is not the attribute we are looking for.
      // Move the searched position at the next possible offset
      // and search again.
      search_index += name_length;
      pos = inverse_attr.find(name, search_index);
    }

    return false;
  }

  bool Validator::Validate(const SelectionContext& context) const
  {
    SA_DECLARE_SCOPE_LOGGER_ARGS(sli);
    sli.verbose = true;
    sli.instance = this;
    ScopeLogger logger(&sli);

    PropertyManager& pmgr = PropertyManager::GetInstance();
    const char * attr_name = "";

    // assume validation will fail
    mLastValidateSuccessful = false;

    attr_name = "maxfiles";
    bool maxfiles_inversed = IsInversed(attr_name);
    if (!maxfiles_inversed && context.GetNumFiles() > mMaxFiles)
    {
      SA_VERBOSE_LOG(DEBUG) << GetAttrFailMessage(this, attr_name, maxfiles_inversed);
      return false; //too many files selected
    }
    if (maxfiles_inversed && context.GetNumFiles() <= mMaxFiles)
    {
      SA_VERBOSE_LOG(DEBUG) << GetAttrFailMessage(this, attr_name, maxfiles_inversed);
      return false; //too many files selected
    }

    attr_name = "maxfolders";
    bool maxfolders_inversed = IsInversed(attr_name);
    if (!maxfolders_inversed && context.GetNumDirectories() > mMaxDirectories)
    {
      SA_VERBOSE_LOG(DEBUG) << GetAttrFailMessage(this, attr_name, maxfolders_inversed);
      return false; //too many directories selected
    }
    if (maxfolders_inversed && context.GetNumDirectories() <= mMaxDirectories)
    {
      SA_VERBOSE_LOG(DEBUG) << GetAttrFailMessage(this, attr_name, maxfolders_inversed);
      return false; //too many directories selected
    }

    //validate properties
    const std::string properties = pmgr.Expand(mAttributes.GetProperty(ATTRIBUTE_PROPERTIES));
    if (!properties.empty())
    {
      attr_name = "properties";
      bool inversed = IsInversed(attr_name);
      SA_VERBOSE_LOG(DEBUG) << GetPrevalidateMessage(attr_name, inversed, properties);
      bool valid = ValidateProperties(context, properties, inversed);
      if (!valid)
      {
        SA_VERBOSE_LOG(DEBUG) << GetAttrFailMessage(this, attr_name, inversed);
        return false;
      }
    }

    //validate file extentions
    const std::string file_extensions = pmgr.Expand(mAttributes.GetProperty(ATTRIBUTE_FILEEXTENSIONS));
    if (!file_extensions.empty())
    {
      attr_name = "fileextensions";
      bool inversed = IsInversed(attr_name);
      SA_VERBOSE_LOG(DEBUG) << GetPrevalidateMessage(attr_name, inversed, file_extensions);
      bool valid = ValidateFileExtensions(context, file_extensions, inversed);
      if (!valid)
      {
        SA_VERBOSE_LOG(DEBUG) << GetAttrFailMessage(this, attr_name, inversed);
        return false;
      }
    }

    //validate file/directory exists
    const std::string file_exists = pmgr.Expand(mAttributes.GetProperty(ATTRIBUTE_EXISTS));
    if (!file_exists.empty())
    {
      attr_name = "exists";
      bool inversed = IsInversed(attr_name);
      SA_VERBOSE_LOG(DEBUG) << GetPrevalidateMessage(attr_name, inversed, file_exists);
      bool valid = ValidateExists(context, file_exists, inversed);
      if (!valid)
      {
        SA_VERBOSE_LOG(DEBUG) << GetAttrFailMessage(this, attr_name, inversed);
        return false;
      }
    }

    //validate class
    const std::string class_ = pmgr.Expand(mAttributes.GetProperty(ATTRIBUTE_CLASS));
    if (!class_.empty())
    {
      attr_name = "class";
      bool inversed = IsInversed(attr_name);
      SA_VERBOSE_LOG(DEBUG) << GetPrevalidateMessage(attr_name, inversed, class_);
      bool valid = ValidateClass(context, class_, inversed);
      if (!valid)
      {
        SA_VERBOSE_LOG(DEBUG) << GetAttrFailMessage(this, attr_name, inversed);
        return false;
      }
    }

    //validate pattern
    const std::string pattern = pmgr.Expand(mAttributes.GetProperty(ATTRIBUTE_PATTERN));
    if (!pattern.empty())
    {
      attr_name = "pattern";
      bool inversed = IsInversed(attr_name);
      SA_VERBOSE_LOG(DEBUG) << GetPrevalidateMessage(attr_name, inversed, pattern);
      bool valid = ValidatePattern(context, pattern, inversed);
      if (!valid)
      {
        SA_VERBOSE_LOG(DEBUG) << GetAttrFailMessage(this, attr_name, inversed);
        return false;
      }
    }

    //validate exprtx
    const std::string exprtk = pmgr.Expand(mAttributes.GetProperty(ATTRIBUTE_EXPRTK));
    if (!exprtk.empty())
    {
      attr_name = "exprtk";
      bool inversed = IsInversed(attr_name);
      SA_VERBOSE_LOG(DEBUG) << GetPrevalidateMessage(attr_name, inversed, exprtk);
      bool valid = ValidateExprtk(context, exprtk, inversed);
      if (!valid)
      {
        SA_VERBOSE_LOG(DEBUG) << GetAttrFailMessage(this, attr_name, inversed);
        return false;
      }
    }

    //validate istrue
    const std::string istrue = pmgr.Expand(mAttributes.GetProperty(ATTRIBUTE_ISTRUE));
    if (!istrue.empty())
    {
      attr_name = "istrue";
      bool inversed = IsInversed(attr_name);
      SA_VERBOSE_LOG(DEBUG) << GetPrevalidateMessage(attr_name, inversed, istrue);
      bool valid = ValidateIsTrue(context, istrue, inversed);
      if (!valid)
      {
        SA_VERBOSE_LOG(DEBUG) << GetAttrFailMessage(this, attr_name, inversed);
        return false;
      }
    }

    //validate isfalse
    const std::string isfalse = pmgr.Expand(mAttributes.GetProperty(ATTRIBUTE_ISFALSE));
    if (!isfalse.empty())
    {
      attr_name = "isfalse";
      bool inversed = IsInversed(attr_name);
      SA_VERBOSE_LOG(DEBUG) << GetPrevalidateMessage(attr_name, inversed, isfalse);
      bool valid = ValidateIsFalse(context, isfalse, inversed);
      if (!valid)
      {
        SA_VERBOSE_LOG(DEBUG) << GetAttrFailMessage(this, attr_name, inversed);
        return false;
      }
    }

    //validate isempty
    const std::string& isempty_attr = mAttributes.GetProperty(ATTRIBUTE_ISEMPTY);
    const std::string isempty = pmgr.Expand(isempty_attr);
    if (!isempty_attr.empty())  // note, testing with non-expanded value instead of expanded value
    {
      attr_name = "isempty";
      bool inversed = IsInversed(attr_name);
      SA_VERBOSE_LOG(DEBUG) << GetPrevalidateMessage(attr_name, inversed, isempty_attr);
      bool valid = ValidateIsEmpty(context, isempty, inversed);
      if (!valid)
      {
        SA_VERBOSE_LOG(DEBUG) << GetAttrFailMessage(this, attr_name, inversed);
        return false;
      }
    }

    //validate using plugins
    //for each plugins
    for (size_t i = 0; i < mPlugins.size(); i++)
    {
      Plugin* p = mPlugins[i];
      bool valid = ValidatePlugin(context, p);
      if (!valid)
      {
        std::string plugin_desc = std::string("plugin '") + p->GetPath() + "'";
        SA_VERBOSE_LOG(DEBUG) << GetAttrFailMessage(this, plugin_desc.c_str(), false);
        return false;
      }
    }

    //validate keyboard
    const std::string& keyboard_attr = mAttributes.GetProperty(ATTRIBUTE_KEYBOARD);
    const std::string keyboard = pmgr.Expand(keyboard_attr);
    if (!keyboard_attr.empty())  // note, testing with non-expanded value instead of expanded value
    {
      attr_name = "keyboard";
      bool inversed = IsInversed(attr_name);
      SA_VERBOSE_LOG(DEBUG) << GetPrevalidateMessage(attr_name, inversed, keyboard_attr);
      bool valid = ValidateKeyboard(context, keyboard, inversed);
      if (!valid)
      {
        SA_VERBOSE_LOG(DEBUG) << GetAttrFailMessage(this, attr_name, inversed);
        return false;
      }
    }

    //check if we are updating a ConfigFile.
    ConfigFile* updating_config = ConfigFile::GetUpdatingConfigFile();
    if (updating_config != NULL)
    {
      const Plugin::PluginPtrList& config_plugins = updating_config->GetPlugins();
      //for each plugins
      for (size_t i = 0; i < config_plugins.size(); i++)
      {
        Plugin* p = config_plugins[i];
        bool valid = ValidatePlugin(context, p);
        if (!valid)
        {
          std::string plugin_desc = std::string("plugin '") + p->GetPath() + "'";
          SA_VERBOSE_LOG(DEBUG) << GetAttrFailMessage(this, plugin_desc.c_str(), false);
          return false;
        }
      }
    }

    mLastValidateSuccessful = true;
    return true;
  }

  bool Validator::IsTrue(const std::string& value)
  {
    std::string upper_case_value = ra::strings::Uppercase(value);
    if (upper_case_value == "TRUE" ||
        upper_case_value == "YES" ||
        upper_case_value == "OK" ||
        upper_case_value == "ON" ||
        upper_case_value == "1")
      return true;

    //check with system true
    PropertyManager& pmgr = PropertyManager::GetInstance();
    std::string system_true = pmgr.GetProperty(PropertyManager::SYSTEM_TRUE_PROPERTY_NAME);
    std::string upper_case_system_true = ra::strings::Uppercase(system_true);
    if (upper_case_value == upper_case_system_true)
      return true;

    return false;
  }

  bool Validator::IsFalse(const std::string& value)
  {
    std::string upper_case_value = ra::strings::Uppercase(value);
    if (upper_case_value == "FALSE" ||
        upper_case_value == "NO" ||
        upper_case_value == "FAIL" ||
        upper_case_value == "OFF" ||
        upper_case_value == "0")
      return true;

    //check with system false
    PropertyManager& pmgr = PropertyManager::GetInstance();
    std::string system_false = pmgr.GetProperty(PropertyManager::SYSTEM_FALSE_PROPERTY_NAME);
    std::string upper_case_system_false = ra::strings::Uppercase(system_false);
    if (upper_case_value == upper_case_system_false)
      return true;

    return false;
  }

  bool Validator::ValidateProperties(const SelectionContext& context, const std::string& properties, bool inversed) const
  {
    if (properties.empty())
      return true;

    PropertyManager& pmgr = PropertyManager::GetInstance();

    //split
    ra::strings::StringVector property_list = ra::strings::Split(properties, SA_PROPERTIES_ATTR_SEPARATOR_STR);

    //each property specified must exists and be non-empty
    for (size_t i = 0; i < property_list.size(); i++)
    {
      const std::string& p = property_list[i];

      if (!inversed)
      {
        if (!pmgr.HasProperty(p))
        {
          SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Property '" << p << "' is not set.";
          return false; //missing property
        }
        const std::string& p_value = pmgr.GetProperty(p);
        if (p_value.empty())
        {
          SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Property '" << p << "' is empty.";
          return false; //empty
        }
      }
      else
      {
        // inversed
        if (pmgr.HasProperty(p))
        {
          const std::string& p_value = pmgr.GetProperty(p);
          if (!p_value.empty())
          {
            SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Property '" << p << "' is not empty and set to '" << p_value << "'.";
            return false; //not empty
          }
        }
      }
    }

    return true;
  }

  bool Validator::ValidateFileExtensions(const SelectionContext& context, const std::string& file_extensions, bool inversed) const
  {
    if (file_extensions.empty())
      return true;

    PropertyManager& pmgr = PropertyManager::GetInstance();

    //split
    std::string file_extensions_uppercase = ra::strings::Uppercase(file_extensions);
    ra::strings::StringVector accepted_file_extensions = ra::strings::Split(file_extensions_uppercase, SA_FILEEXTENSION_ATTR_SEPARATOR_STR);

    //for each file selected
    const StringList& context_elements = context.GetElements();
    for (size_t i = 0; i < context_elements.size(); i++)
    {
      const std::string& path = context_elements[i];
      std::string current_file_extension_uppercase = ra::strings::Uppercase(ra::filesystem::GetFileExtention(path));

      //each file extension must be part of accepted_file_extensions
      bool found = HasValue(accepted_file_extensions, current_file_extension_uppercase);
      if (!inversed && !found)
      {
        SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " File extension '" << current_file_extension_uppercase << "' from selected file '" << path << "' is not allowed because only the extensions '" << file_extensions_uppercase << "' are accepted.";
        return false; //current file extension is not accepted
      }
      if (inversed && found)
      {
        SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " File extension '" << current_file_extension_uppercase << "' from selected file '" << path << "' is not allowed because the extensions '" << file_extensions_uppercase << "' are forbidden.";
        return false; //current file extension is not accepted
      }
    }

    return true;
  }

  bool Validator::ValidateExists(const SelectionContext& context, const std::string& file_exists, bool inversed) const
  {
    if (file_exists.empty())
      return true;

    PropertyManager& pmgr = PropertyManager::GetInstance();

    //split
    ra::strings::StringVector mandatory_files = ra::strings::Split(file_exists, SA_EXISTS_ATTR_SEPARATOR_STR);

    //for each file
    for (size_t i = 0; i < mandatory_files.size(); i++)
    {
      const std::string& element = mandatory_files[i];
      bool element_exists = false;
      element_exists |= ra::filesystem::FileExistsUtf8(element.c_str());
      element_exists |= ra::filesystem::DirectoryExistsUtf8(element.c_str());
      if (!inversed && !element_exists)
      {
        SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " File/Directory '" << element.c_str() << "' does not exists.";
        return false; //mandatory file/directory not found
      }
      if (inversed && element_exists)
      {
        SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " File/Directory '" << element.c_str() << "' exists.";
        return false; //mandatory file/directory not found
      }
    }

    return true;
  }

  bool Validator::ValidateSingleFileSingleClass(const std::string& path, const std::string& class_, bool inversed) const
  {
    PropertyManager& pmgr = PropertyManager::GetInstance();

    if (class_ == "file")
    {
      // Selected element must be a file
      bool is_file = ra::filesystem::FileExistsUtf8(path.c_str());
      if (!inversed && !is_file)
      {
        //SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Selected path '" << path << "' is not a file.";
        return false;
      }
      if (inversed && is_file)
      {
        //SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Selected path '" << path << "' is a file.";
        return false;
      }
    }
    else if (class_ == "folder" || class_ == "directory")
    {
      // Selected elements must be a directory
      bool is_directory = ra::filesystem::DirectoryExistsUtf8(path.c_str());
      if (!inversed && !is_directory)
      {
        //SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Selected path '" << path << "' is not a directory.";
        return false;
      }
      if (inversed && is_directory)
      {
        //SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Selected path '" << path << "' is a directory.";
        return false;
      }
    }
    else if (class_ == "drive")
    {
      // Selected elements must be mapped to a drive
      std::string drive = GetDriveLetter(path);
      if (!inversed && drive.empty())  // All elements must be mapped to a drive
      {
        //SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Selected path '" << path << "' is not on a drive.";
        return false;
      }
      if (inversed && !drive.empty())  // All elements must NOT be mapped to a drive.
      {
        //SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Selected path '" << path << "' is on a drive.";
        return false;
      }
    }
    else if (GetDriveClassFromString(class_.c_str()) != DRIVE_CLASS_UNKNOWN)
    {
      DRIVE_CLASS required_class = GetDriveClassFromString(class_.c_str());

      // Selected elements must be of the same drive class
      DRIVE_CLASS element_class = GetDriveClassFromPath(path);
      if (!inversed && element_class != required_class)
      {
        //SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Selected path '" << path << "' is not of class " << ToString(required_class) <<  ".";
        return false;
      }
      if (inversed && element_class == required_class)
      {
        //SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Selected path '" << path << "' is of class " << ToString(required_class) << ".";
        return false;
      }
    }
    else
    {
      SA_LOG(WARNING) << "Unknown class '" << class_ << "'.";
      return false;
    }

    return true;
  }

  static inline bool IsValidClass(const std::string& class_)
  {
    DRIVE_CLASS parsed_class = GetDriveClassFromString(class_.c_str());
    if (parsed_class != DRIVE_CLASS_UNKNOWN)
      return true;
    return false;
  }

  bool Validator::ValidateSingleFileMultipleClasses(const std::string& path, const std::string& class_, bool inversed) const
  {
    if (class_.empty())
      return true;

    //split
    ra::strings::StringVector classes = ra::strings::Split(class_, SA_CLASS_ATTR_SEPARATOR_STR);
    std::string valid_classes;

    bool valid = false;
    for (size_t i = 0; i < classes.size(); i++)
    {
      const std::string& class_ = classes[i];
      valid |= ValidateSingleFileSingleClass(path, class_, inversed);

      // Build a valid classes list from the given classes (strings)
      if (IsValidClass(class_))
      {
        if (!valid_classes.empty()) valid_classes.append(1, ',');
        valid_classes += class_;
      }
    }

    if (!valid)
    {
      if (!inversed)
        SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Selected path '" << path << "' is not of classes '" << valid_classes << "'.";
      else
        SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Selected path '" << path << "' is of classes '" << valid_classes << "'.";
      return false;
    }

    return true;
  }

  bool Validator::ValidateClass(const SelectionContext& context, const std::string& class_, bool inversed) const
  {
    if (class_.empty())
      return true;

    PropertyManager& pmgr = PropertyManager::GetInstance();

    //split
    ra::strings::StringVector classes = ra::strings::Split(class_, SA_CLASS_ATTR_SEPARATOR_STR);

    // Search for file extensions. All file extensions must be extracted from the list and evaluated all at once.
    std::string file_extensions;
    if (!classes.empty())
    {
      static const size_t INVALID_INDEX_POSITION = (size_t)-1;

      // Remember each file extension's index to delete them after searching.
      std::vector<size_t> delete_positions;

      bool found_file_extensions = false;

      for (size_t i = classes.size() - 1; i >= 0 && i != INVALID_INDEX_POSITION; i--)
      {
        const std::string& element = classes[i];

        // Is this a class file extension filter?
        if (!element.empty() && element[0] == '.')
        {
          // Extract the file extension
          std::string file_extension;
          if (element.size() >= 2)
            file_extension = element.substr(1);

          // Add to the file extension list
          if (!file_extensions.empty())
            file_extensions.insert(0, 1, SA_CLASS_ATTR_SEPARATOR_CHAR);
          file_extensions.insert(0, file_extension.c_str());

          found_file_extensions = true;

          // Remember this index
          delete_positions.push_back(i);
        }
      }

      // Delete elements of classes which were identified as file extensions
      for (size_t i = 0; i < delete_positions.size(); i++)
      {
        const size_t& delete_position = delete_positions[i];
        classes.erase(classes.begin() + delete_position);
      }

      // Validate file extensions
      if (found_file_extensions)
      {
        bool valid = ValidateFileExtensions(context, file_extensions, inversed);
        if (!valid)
        {
          if (!inversed)
            SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Selected path(s) is/are not of file extensions '" << file_extensions << "'.";
          else
            SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Selected path(s) is/are one of file extensions '" << file_extensions << "'.";
          return false;
        }
      }
    }

    // Continue validation for the remaining class elements
    if (!classes.empty())
    {
      //join remaining classes into a single string
      std::string classes_str = ra::strings::Join(classes, SA_CLASS_ATTR_SEPARATOR_STR);

      //for each file selected
      const StringList& context_elements = context.GetElements();
      for (size_t i = 0; i < context_elements.size(); i++)
      {
        const std::string& path = context_elements[i];

        //each element must match one of the classes
        bool valid = ValidateSingleFileMultipleClasses(path, classes_str, inversed);
        if (!inversed && !valid)
          return false; //verbose log is already printed in ValidateSingleFileMultipleClasses()
        if (inversed && valid)
          return false; //verbose log is already printed in ValidateSingleFileMultipleClasses()
      }
    }

    return true;
  }

  bool WildcardMatch(const ra::strings::StringVector& patterns, const char* value)
  {
    for (size_t j = 0; j < patterns.size(); j++)
    {
      const std::string& pattern = patterns[j];
      bool match = WildcardMatch(pattern.c_str(), value);
      if (match)
        return true;
    }
    return false;
  }

  bool Validator::ValidatePattern(const SelectionContext& context, const std::string& pattern, bool inversed) const
  {
    if (pattern.empty())
      return true;

    PropertyManager& pmgr = PropertyManager::GetInstance();

    //split
    ra::strings::StringVector patterns_uppercase = ra::strings::Split(pattern, SA_PATTERN_ATTR_SEPARATOR_STR);
    Uppercase(patterns_uppercase);

    //for each file selected
    const StringList& context_elements = context.GetElements();
    for (size_t i = 0; i < context_elements.size(); i++)
    {
      const std::string& path = context_elements[i];
      std::string path_uppercase = ra::strings::Uppercase(path);

      //each element must match one of the patterns
      bool match = WildcardMatch(patterns_uppercase, path_uppercase.c_str());
      if (!inversed && !match)
      {
        SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Selected path '" << path << "' does not match the pattern '" << pattern << "'.";
        return false; //current file does not match any patterns
      }
      if (inversed && match)
      {
        SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Selected path '" << path << "' matches with the pattern '" << pattern << "'.";
        return false; //current file does match one pattern
      }
    }

    return true;
  }

  bool Validator::ValidateExprtk(const SelectionContext& context, const std::string& exprtk, bool inversed) const
  {
    if (exprtk.empty())
      return true;

    PropertyManager& pmgr = PropertyManager::GetInstance();

    static const size_t ERROR_SIZE = 10480;
    char error[ERROR_SIZE];
    error[0] = '\0';

    int result = false;
    int evaluated = EvaluateBoolean(exprtk.c_str(), &result, error, ERROR_SIZE);
    if (!evaluated)
    {
      SA_LOG(WARNING) << "Failed evaluating exprtk expression '" << exprtk << "'.";
      SA_LOG(WARNING) << "Exprtk error: " << error << "'.";
      return false;
    }

    if (!inversed && !result)
    {
      SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Selected element(s) does not match the exprtk expression '" << exprtk << "'.";
      return false; //current file does not match any patterns
    }
    if (inversed && result)
    {
      SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Selected element(s) matches the exprtk expression '" << exprtk << "'.";
      return false; //current file does match one pattern
    }

    return true;
  }

  bool Validator::ValidateIsTrue(const SelectionContext& context, const std::string& istrue, bool inversed) const
  {
    if (istrue.empty())
      return true;

    PropertyManager& pmgr = PropertyManager::GetInstance();

    //split
    ra::strings::StringVector statements = ra::strings::Split(istrue, SA_ISTRUE_ATTR_SEPARATOR_STR);

    //for each boolean statement
    for (size_t i = 0; i < statements.size(); i++)
    {
      const std::string& statement = statements[i];
      bool match = IsTrue(statement);
      if (!inversed && !match)
      {
        SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Statement '" << statement << "' does not evaluates to true.";
        return false; //current statement does not evaluate to true
      }
      if (inversed && match)
      {
        SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Statement '" << statement << "' evaluates to true.";
        return false; //current statement evaluates as true
      }
    }

    return true;
  }

  bool Validator::ValidateIsFalse(const SelectionContext& context, const std::string& isfalse, bool inversed) const
  {
    if (isfalse.empty())
      return true;

    PropertyManager& pmgr = PropertyManager::GetInstance();

    //split
    ra::strings::StringVector statements = ra::strings::Split(isfalse, SA_ISTRUE_ATTR_SEPARATOR_STR);

    //for each boolean statement
    for (size_t i = 0; i < statements.size(); i++)
    {
      const std::string& statement = statements[i];
      bool match = IsFalse(statement);
      if (!inversed && !match)
      {
        SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Statement '" << statement << "' does not evaluates to false.";
        return false; //current statement does not evaluate to false
      }
      if (inversed && match)
      {
        SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Statement '" << statement << "' evaluates to false.";
        return false; //current statement evaluates as false
      }
    }

    return true;
  }

  bool Validator::ValidateIsEmpty(const SelectionContext& context, const std::string& isempty, bool inversed) const
  {
    PropertyManager& pmgr = PropertyManager::GetInstance();

    bool match = isempty.empty();
    if (!inversed && !match)
    {
      SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Statement '" << isempty << "' is not empty.";
      return false; //current statement is not empty
    }
    if (inversed && match)
    {
      SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Statement '" << isempty << "' is empty.";
      return false; //current statement is empty
    }

    return true;
  }

  bool has_all_elements(const ra::strings::StringVector& all_elements, const ra::strings::StringVector& mandatory_elements)
  {
    for (size_t i = 0; i < mandatory_elements.size(); i++)
    {
      const std::string& mandatory_element = mandatory_elements[i];
      ra::strings::StringVector::const_iterator it = std::find(all_elements.begin(), all_elements.end(), mandatory_element);
      bool found = (it != all_elements.end());
      if (!found)
        return false;
    }
    return true;
  }

  bool contains(IAttributeValidator::IAttributeValidationPtrList& validators, IAttributeValidator* validator)
  {
    for (size_t i = 0; i < validators.size(); i++)
    {
      IAttributeValidator* element = validators[i];
      if (element == validator)
        return true;
    }
    return false;
  }

  bool Validator::ValidatePlugin(const SelectionContext& context, Plugin* plugin) const
  {
    Registry& registry = plugin->GetRegistry();

    //get condition attributes supported by this plugin
    ra::strings::StringVector plugin_conditions;
    PropertyManager::SplitAndExpand(plugin->GetConditions(), SA_CONDITIONS_ATTR_SEPARATOR_STR, plugin_conditions);

    //if there is no condition, it cannot be linked to a validator.
    if (plugin_conditions.empty())
      return true; // no validation required

    //Find which of this plugin's attributes match the one specified in the xml validator.
    //This is required if a plugin specify multiple optional attributes.
    //We should only remember the one that were specified in the xml.
    ra::strings::StringVector matching_conditions;
    for (size_t i = 0; i < plugin_conditions.size(); i++)
    {
      const std::string& condition = plugin_conditions[i];
      if (this->mCustomAttributes.HasProperty(condition))
      {
        matching_conditions.push_back(condition);
      }
    }

    //if there is no matching condition, there is no need to link conditions to a validator.
    if (matching_conditions.empty())
      return true; // no validation required

    //find attribute validators that support these matching conditions
    IAttributeValidator::IAttributeValidationPtrList validators;
    for (size_t i = 0; i < matching_conditions.size(); i++)
    {
      const std::string& condition = matching_conditions[i];
      IAttributeValidator* attr_validator = registry.GetAttributeValidatorFromName(condition);
      if (attr_validator != NULL)
      {
        //if we did not already add this validator
        if (!contains(validators, attr_validator))
        {
          validators.push_back(attr_validator);
        }
      }
    }

    //validate!
    for (size_t i = 0; i < validators.size(); i++)
    {
      IAttributeValidator* attr_validator = validators[i];
      attr_validator->SetSelectionContext(&context);
      attr_validator->SetCustomAttributes(&mCustomAttributes);
      bool valid = attr_validator->Validate();
      attr_validator->SetSelectionContext(NULL);
      attr_validator->SetCustomAttributes(NULL);
      if (!valid)
      {
        //get the names of custom attributes
        shellanything::StringList custom_attribute_names_list;
        mCustomAttributes.GetProperties(custom_attribute_names_list);
        std::string custom_attribute_names = ra::strings::Join(custom_attribute_names_list, ",");

        //get plugin description
        std::string plugin_desc = std::string("plugin '") + plugin->GetPath() + "'";

        SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, false) << " Plugin '" << plugin_desc << "' custom attributes '" << custom_attribute_names << "' have failed validation.";
        return false; //current statement is not empty
      }
    }

    return true;
  }

  bool Validator::ValidateKeyboard(const SelectionContext& context, const std::string& keyboard, bool inversed) const
  {
    if (keyboard.empty())
      return true;

    IKeyboardService* keyboard_service = App::GetInstance().GetKeyboardService();
    if (keyboard_service == NULL)
    {
      SA_LOG(ERROR) << "No keyboard setup to validate keyboard validation.";
      return false; // invalid. No keyboard setup to validate.
    }

    PropertyManager& pmgr = PropertyManager::GetInstance();

    //split
    ra::strings::StringVector mandatory_keyboard_ids = ra::strings::Split(keyboard, SA_KEYBOARD_ATTR_SEPARATOR_STR);

    //for each modifiers
    for (size_t i = 0; i < mandatory_keyboard_ids.size(); i++)
    {
      const std::string& element = mandatory_keyboard_ids[i];

      KEYB_MODIFIER_ID mid = KeyboardHelper::ParseKeyboardModifierId(element);
      KEYB_TOGGLE_ID tid = KeyboardHelper::ParseKeyboardToggleId(element);
      if (mid == KMID_INVALID && tid == KMID_INVALID)
      {
        // Unknown keyboard id
        SA_LOG(ERROR) << "Unknown keyboard id '" << element << "'.";
        return false;
      }

      // Validate as a modifier
      if (mid != KMID_INVALID)
      {
        bool element_key_down = keyboard_service->IsModifierKeyDown(mid);
        if (!inversed && !element_key_down)
        {
          SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Keyboard modifier '" << element << "' is not set.";
          return false; //mandatory modifier not set
        }
        if (inversed && element_key_down)
        {
          SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Keyboard modifier '" << element << "' is set.";
          return false; //mandatory modifier is set
        }
      }

      // Validate as a toogle
      if (tid != KMID_INVALID)
      {
        bool element_toggle_on = keyboard_service->IsToggleStateOn(tid);
        if (!inversed && !element_toggle_on)
        {
          SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Keyboard modifier '" << element << "' is not set.";
          return false; //mandatory modifier not set
        }
        if (inversed && element_toggle_on)
        {
          SA_VERBOSE_LOG(DEBUG) << GetCheckFailMessage(this, inversed) << " Keyboard modifier '" << element << "' is set.";
          return false; //mandatory modifier is set
        }
      }
    }

    return true;
  }

  std::string Validator::ToShortString() const
  {
    std::string str;
    str += "Validator ";
    str += ToHexString(this);
    //No need to output mMaxFiles or mMaxDirectories, they are already available in mAttributes
    //if (mMaxFiles != MAX_FILES)
    //{
    //  str += ", maxfiles=";
    //  str += ra::strings::ToString(mMaxFiles);
    //}
    //if (mMaxDirectories != MAX_DIRS)
    //{
    //  str += ", maxfolders=";
    //  str += ra::strings::ToString(mMaxDirectories);
    //}
    if (!mAttributes.IsEmpty())
    {
      StringList attribute_names;
      mAttributes.GetProperties(attribute_names);
      for (size_t i = 0; i < attribute_names.size(); i++)
      {
        const std::string& attr_name = attribute_names[i];
        str += ", ";
        str += attr_name;
        str += "=";
        str += mAttributes.GetProperty(attr_name);
      }
    }
    if (!mCustomAttributes.IsEmpty())
    {
      StringList attribute_names;
      mCustomAttributes.GetProperties(attribute_names);
      for (size_t i = 0; i < attribute_names.size(); i++)
      {
        const std::string& attr_name = attribute_names[i];
        str += ", ";
        str += attr_name;
        str += "=";
        str += mCustomAttributes.GetProperty(attr_name);
      }
    }
    if (!mLastValidateSuccessful)
    {
      str += ", FAILED VALIDATION";
    }
    return str;
  }

  void Validator::ToLongString(std::string& str, int indent) const
  {
    static const char* NEW_LINE = ra::environment::GetLineSeparator();
    const std::string indent_str = std::string(indent, ' ');

    const std::string short_string = ToShortString();
    str += indent_str + short_string;
  }

} //namespace shellanything
