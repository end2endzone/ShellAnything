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

#include <string>
#include <limits>
#include "Validator.h"
#include "PropertyManager.h"
#include "DriveClass.h"
#include "Wildcard.h"
#include "libexprtk.h"
#include "rapidassist/strings.h"
#include "rapidassist/filesystem_utf8.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

namespace shellanything
{
  bool HasValue(const ra::strings::StringVector & values, const std::string & token)
  {
    for(size_t i=0; i<values.size(); i++)
    {
      const std::string & v = values[i];
      if (token == v)
        return true;
    }
    return false;
  }

  void Uppercase(ra::strings::StringVector & values)
  {
    for(size_t i=0; i<values.size(); i++)
    {
      std::string tmp = ra::strings::Uppercase(values[i]);
      values[i] = tmp;
    }
  }





  Validator::Validator() :
    mMaxFiles(std::numeric_limits<int>::max()),
    mMaxDirectories(std::numeric_limits<int>::max())
  {
  }

  Validator::~Validator()
  {
  }

  const int & Validator::GetMaxFiles() const
  {
    return mMaxFiles;
  }

  void Validator::SetMaxFiles(const int & max_files)
  {
    mMaxFiles = max_files;
  }

  const int & Validator::GetMaxDirectories() const
  {
    return mMaxDirectories;
  }

  void Validator::SetMaxDirectories(const int & max_directories)
  {
    mMaxDirectories = max_directories;
  }

  const std::string & Validator::GetProperties() const
  {
    return mProperties;
  }

  void Validator::SetProperties(const std::string & properties)
  {
    mProperties = properties;
  }

  const std::string & Validator::GetFileExtensions() const
  {
    return mFileExtensions;
  }

  void Validator::SetFileExtensions(const std::string & file_extensions)
  {
    mFileExtensions = file_extensions;
  }

  const std::string & Validator::GetFileExists() const
  {
    return mFileExists;
  }

  void Validator::SetFileExists(const std::string & file_exists)
  {
    mFileExists = file_exists;
  }

  const std::string & Validator::GetClass() const
  {
    return mClass;
  }

  void Validator::SetClass(const std::string & classes)
  {
    mClass = classes;
  }

  const std::string & Validator::GetPattern() const
  {
    return mPattern;
  }

  void Validator::SetPattern(const std::string & pattern)
  {
    mPattern = pattern;
  }

  const std::string & Validator::GetExprtk() const
  {
    return mExprtk;
  }

  void Validator::SetExprtk(const std::string & exprtk)
  {
    mExprtk = exprtk;
  }

  const std::string & Validator::GetIsTrue() const
  {
    return mIsTrue;
  }

  void Validator::SetIsTrue(const std::string & istrue)
  {
    mIsTrue = istrue;
  }

  const std::string & Validator::GetIsFalse() const
  {
    return mIsFalse;
  }

  void Validator::SetIsFalse(const std::string & isfalse)
  {
    mIsFalse = isfalse;
  }

  const std::string & Validator::GetIsEmpty() const
  {
    return mIsEmpty;
  }

  void Validator::SetIsEmpty(const std::string & isempty)
  {
    mIsEmpty = isempty;
  }

  const std::string & Validator::GetInserve() const
  {
    return mInverse;
  }

  void Validator::SetInserve(const std::string & inserve)
  {
    mInverse = inserve;
  }

  bool Validator::IsInversed(const char * name) const
  {
    if (name == NULL)
      return false;
    if (name[0] == '\0')
      return false;
    
    std::string tmp_name = name;

    // Validate with 'all' attribute
    if (tmp_name != "all")
    {
      if ( IsInversed("all") )
        return true;
    }

    size_t name_length = tmp_name.size();
    size_t search_index = 0;
    size_t pos = mInverse.find(name, search_index);
    while( pos != std::string::npos)
    {
      // The name of the attribute was found.

      // To prevent finding attribute 'foo' from attribute 'foobar',
      // The end of the attribute should match the end of the mInverse string or
      // followed by a ';' character.
      char next = '\0';
      if (pos + name_length < mInverse.size())
        next = mInverse[pos + name_length];

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
          previous = mInverse[pos - 1];
        
        if (previous == SA_INVERSE_ATTR_SEPARATOR_CHAR)
          return true; // We have a match!
      }

      // This is not the attribute we are looking for.
      // Move the searched position at the next possible offset
      // and search again.
      search_index += name_length;
      pos = mInverse.find(name, search_index);
    }

    return false;
  }

  bool Validator::Validate(const Context & context) const
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
  	
    bool maxfiles_inversed = IsInversed("maxfiles");
    if (!maxfiles_inversed && context.GetNumFiles() > mMaxFiles)
        return false; //too many files selected
    if (maxfiles_inversed && context.GetNumFiles() <= mMaxFiles)
        return false; //too many files selected

    bool maxfolders_inversed = IsInversed("maxfolders");
    if (!maxfolders_inversed && context.GetNumDirectories() > mMaxDirectories)
      return false; //too many directories selected
    if (maxfolders_inversed && context.GetNumDirectories() <= mMaxDirectories)
      return false; //too many directories selected

    //validate properties
    const std::string properties = pmgr.Expand(mProperties);
    if (!properties.empty())
    {
      bool inversed = IsInversed("properties");
      bool valid = ValidateProperties(context, properties, inversed);
      if (!valid)
        return false;
    }

    //validate file extentions
    const std::string file_extensions = pmgr.Expand(mFileExtensions);
    if (!file_extensions.empty())
    {
      bool inversed = IsInversed("fileextensions");
      bool valid = ValidateFileExtensions(context, file_extensions, inversed);
      if (!valid)
        return false;
    }

    //validate file/directory exists
    const std::string file_exists = pmgr.Expand(mFileExists);
    if (!file_exists.empty())
    {
      bool inversed = IsInversed("exists");
      bool valid = ValidateExists(context, file_exists, inversed);
      if (!valid)
        return false;
    }

    //validate class
    const std::string class_ = pmgr.Expand(mClass);
    if (!class_.empty())
    {
      bool inversed = IsInversed("class");
      bool valid = ValidateClass(context, class_, inversed);
      if (!valid)
        return false;
    }

    //validate pattern
    const std::string pattern = pmgr.Expand(mPattern);
    if (!pattern.empty())
    {
      bool inversed = IsInversed("pattern");
      bool valid = ValidatePattern(context, pattern, inversed);
      if (!valid)
        return false;
    }

    //validate exprtx
    const std::string exprtk = pmgr.Expand(mExprtk);
    if (!exprtk.empty())
    {
      bool inversed = IsInversed("exprtk");
      bool valid = ValidateExprtk(context, exprtk, inversed);
      if (!valid)
        return false;
    }

    //validate istrue
    const std::string istrue = pmgr.Expand(mIsTrue);
    if (!istrue.empty())
    {
      bool inversed = IsInversed("istrue");
      bool valid = ValidateIsTrue(context, istrue, inversed);
      if (!valid)
        return false;
    }

    //validate isfalse
    const std::string isfalse = pmgr.Expand(mIsFalse);
    if (!isfalse.empty())
    {
      bool inversed = IsInversed("isfalse");
      bool valid = ValidateIsFalse(context, isfalse, inversed);
      if (!valid)
        return false;
    }

    //validate isempty
    const std::string isempty = pmgr.Expand(mIsEmpty);
    if (!mIsEmpty.empty())  // note, testing with non-expanded value instead of expanded value
    {
      bool inversed = IsInversed("isempty");
      bool valid = ValidateIsEmpty(context, isempty, inversed);
      if (!valid)
        return false;
    }

    return true;
  }

  bool Validator::IsTrue(const std::string & value)
  {
    std::string upper_case_value = ra::strings::Uppercase(value);
    if (upper_case_value == "TRUE" ||
        upper_case_value == "YES" ||
        upper_case_value == "OK" ||
        upper_case_value == "ON" ||
        upper_case_value == "1"     )
      return true;
    
    //check with system true
    PropertyManager & pmgr = PropertyManager::GetInstance();
    std::string system_true = pmgr.GetProperty(PropertyManager::SYSTEM_TRUE_PROPERTY_NAME);
    std::string upper_case_system_true = ra::strings::Uppercase(system_true);
    if (upper_case_value == upper_case_system_true)
      return true;

    return false;
  }

  bool Validator::IsFalse(const std::string & value)
  {
    std::string upper_case_value = ra::strings::Uppercase(value);
    if (upper_case_value == "FALSE" ||
        upper_case_value == "NO" ||
        upper_case_value == "FAIL" ||
        upper_case_value == "OFF" ||
        upper_case_value == "0"     )
      return true;
    
    //check with system false
    PropertyManager & pmgr = PropertyManager::GetInstance();
    std::string system_false = pmgr.GetProperty(PropertyManager::SYSTEM_FALSE_PROPERTY_NAME);
    std::string upper_case_system_false = ra::strings::Uppercase(system_false);
    if (upper_case_value == upper_case_system_false)
      return true;

    return false;
  }

  bool Validator::ValidateProperties(const Context & context, const std::string & properties, bool inversed) const
  {
    if (properties.empty())
      return true;

    PropertyManager & pmgr = PropertyManager::GetInstance();

    //split
    ra::strings::StringVector property_list = ra::strings::Split(properties, SA_PROPERTIES_ATTR_SEPARATOR_STR);

    //each property specified must exists and be non-empty
    for(size_t i=0; i<property_list.size(); i++)
    {
      const std::string & p = property_list[i];

      if (!inversed)
      {
        if (!pmgr.HasProperty(p))
          return false; //missing property
        const std::string & p_value = pmgr.GetProperty(p);
        if (p_value.empty())
          return false; //empty
      }
      else
      {
        // inversed
        if (pmgr.HasProperty(p))
        {
          const std::string & p_value = pmgr.GetProperty(p);
          if (!p_value.empty())
            return false; //not empty
        }
      }
    }

    return true;
  }

  bool Validator::ValidateFileExtensions(const Context & context, const std::string & file_extensions, bool inversed) const
  {
    if (file_extensions.empty())
      return true;

    PropertyManager & pmgr = PropertyManager::GetInstance();

    //split
    ra::strings::StringVector accepted_file_extensions = ra::strings::Split(file_extensions, SA_FILEEXTENSION_ATTR_SEPARATOR_STR);
    Uppercase(accepted_file_extensions);

    //for each file selected
    const Context::ElementList & context_elements = context.GetElements();
    for(size_t i=0; i<context_elements.size(); i++) 
    {
      const std::string & path = context_elements[i];
      std::string current_file_extension = ra::strings::Uppercase(ra::filesystem::GetFileExtention(path));

      //each file extension must be part of accepted_file_extensions
      bool found = HasValue(accepted_file_extensions, current_file_extension);
      if (!inversed && !found)
        return false; //current file extension is not accepted
      if (inversed && found)
        return false; //current file extension is not accepted
    }

    return true;
  }

  bool Validator::ValidateExists(const Context & context, const std::string & file_exists, bool inversed) const
  {
    if (file_exists.empty())
      return true;

    PropertyManager & pmgr = PropertyManager::GetInstance();

    //split
    ra::strings::StringVector mandatory_files = ra::strings::Split(file_exists, SA_EXISTS_ATTR_SEPARATOR_STR);

    //for each file
    for(size_t i=0; i<mandatory_files.size(); i++)
    {
      const std::string & element = mandatory_files[i];
      bool element_exists = false;
      element_exists |= ra::filesystem::FileExistsUtf8(element.c_str());
      element_exists |= ra::filesystem::DirectoryExistsUtf8(element.c_str());
      if (!inversed && !element_exists)
        return false; //mandatory file/directory not found
      if (inversed && element_exists)
        return false; //mandatory file/directory not found
    }

    return true;
  }

  bool Validator::ValidateSingleFileSingleClass(const std::string & path, const std::string & class_, bool inversed) const
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    if (class_ == "file")
    {
      // Selected element must be a file
      bool is_file = ra::filesystem::FileExistsUtf8(path.c_str());
      if (!inversed && !is_file)
        return false;
      if (inversed && is_file)
        return false;
    }
    else if (class_ == "folder" || class_ == "directory")
    {
      // Selected elements must be a directory
      bool is_directory = ra::filesystem::DirectoryExistsUtf8(path.c_str());
      if (!inversed && !is_directory)
        return false;
      if (inversed && is_directory)
        return false;
    }
    else if (class_ == "drive")
    {
      // Selected elements must be mapped to a drive
      std::string drive = GetDriveLetter(path);
      if (!inversed && drive.empty())  // All elements must be mapped to a drive
        return false;
      if (inversed && !drive.empty())  // All elements must NOT be mapped to a drive.
        return false;
    }
    else if (GetDriveClassFromString(class_.c_str()) != DRIVE_CLASS_UNKNOWN)
    {
      DRIVE_CLASS required_class = GetDriveClassFromString(class_.c_str());

      // Selected elements must be of the same drive class
      DRIVE_CLASS element_class = GetDriveClassFromPath(path);
      if (!inversed && element_class != required_class)
        return false;
      if (inversed && element_class == required_class)
        return false;
    }
    else
    {
      LOG(WARNING) << "Unknown class '" << class_ << "'.";
    }

    return true;
  }

  bool Validator::ValidateSingleFileMultipleClasses(const std::string & path, const std::string & class_, bool inversed) const
  {
    if (class_.empty())
      return true;

    //split
    ra::strings::StringVector classes = ra::strings::Split(class_, SA_CLASS_ATTR_SEPARATOR_STR);

    bool valid = false;
    for (size_t i = 0; i < classes.size(); i++)
    {
      const std::string & class_ = classes[i];
      valid |= ValidateSingleFileSingleClass(path, class_, inversed);
    }

    return valid;
  }

  bool Validator::ValidateClass(const Context & context, const std::string & class_, bool inversed) const
  {
    if (class_.empty())
      return true;

    PropertyManager & pmgr = PropertyManager::GetInstance();

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

      for(size_t i=classes.size()-1; i>=0 && i!=INVALID_INDEX_POSITION; i--)
      {
        const std::string & element = classes[i];

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
      for(size_t i=0; i<delete_positions.size(); i++)
      {
        const size_t & delete_position = delete_positions[i];
        classes.erase(classes.begin() + delete_position);
      }

      // Validate file extensions
      if (found_file_extensions)
      {
        bool valid = ValidateFileExtensions(context, file_extensions, inversed);
        if (!valid)
          return false;
      }
    }

    // Continue validation for the remaining class elements
    if (!classes.empty())
    {
      //join remaining classes into a single string
      std::string classes_str = ra::strings::Join(classes, SA_CLASS_ATTR_SEPARATOR_STR);

      //for each file selected
      const Context::ElementList & context_elements = context.GetElements();
      for (size_t i = 0; i < context_elements.size(); i++)
      {
        const std::string & path = context_elements[i];

        //each element must match one of the classes
        bool valid = ValidateSingleFileMultipleClasses(path, classes_str, inversed);
        if (!inversed && !valid)
          return false; //current file extension is not accepted
        if (inversed && valid)
          return false; //current file extension is not accepted
      }
    }

    return true;
  }

  bool WildcardMatch(const ra::strings::StringVector & patterns, const char * value)
  {
    for(size_t j=0; j<patterns.size(); j++)
    {
      const std::string & pattern = patterns[j];
      bool match = WildcardMatch(pattern.c_str(), value);
      if (match)
        return true;
    }
    return false;
  }

  bool Validator::ValidatePattern(const Context & context, const std::string & pattern, bool inversed) const
  {
    if (pattern.empty())
      return true;

    PropertyManager & pmgr = PropertyManager::GetInstance();

    //split
    ra::strings::StringVector patterns = ra::strings::Split(pattern, SA_PATTERN_ATTR_SEPARATOR_STR);
    Uppercase(patterns);

    //for each file selected
    const Context::ElementList & context_elements = context.GetElements();
    for(size_t i=0; i<context_elements.size(); i++) 
    {
      const std::string & path = context_elements[i];
      std::string path_uppercase = ra::strings::Uppercase(path);

      //each element must match one of the patterns
      bool match = WildcardMatch(patterns, path_uppercase.c_str());
      if (!inversed && !match)
        return false; //current file does not match any patterns
      if (inversed && match)
        return false; //current element is not accepted
    }

    return true;
  }

  bool Validator::ValidateExprtk(const Context & context, const std::string & exprtk, bool inversed) const
  {
    if (exprtk.empty())
      return true;

    PropertyManager & pmgr = PropertyManager::GetInstance();

    static const size_t ERROR_SIZE = 10480;
    char error[ERROR_SIZE];
    error[0] = '\0';

    int result = false;
    int evaluated = EvaluateBoolean(exprtk.c_str(), &result, error, ERROR_SIZE);
    if (!evaluated)
    {
      LOG(WARNING) << "Failed evaluating exprtk expression '" << exprtk << "'.";
      LOG(WARNING) << "Exprtk error: " << error << "'.";
      return false;
    }

    if (inversed)
      result = !result;

    return result;
  }

  bool Validator::ValidateIsTrue(const Context & context, const std::string & istrue, bool inversed) const
  {
    if (istrue.empty())
      return true;

    PropertyManager & pmgr = PropertyManager::GetInstance();

    //split
    ra::strings::StringVector statements = ra::strings::Split(istrue, SA_ISTRUE_ATTR_SEPARATOR_STR);

    //for each boolean statement
    for (size_t i = 0; i < statements.size(); i++)
    {
      const std::string & statement = statements[i];
      bool match = IsTrue(statement);
      if (!inversed && !match)
        return false; //current statement does not evaluate to true
      if (inversed && match)
        return false; //current statement evaluates as true
    }

    return true;
  }

  bool Validator::ValidateIsFalse(const Context & context, const std::string & isfalse, bool inversed) const
  {
    if (isfalse.empty())
      return true;

    PropertyManager & pmgr = PropertyManager::GetInstance();

    //split
    ra::strings::StringVector statements = ra::strings::Split(isfalse, SA_ISTRUE_ATTR_SEPARATOR_STR);

    //for each boolean statement
    for (size_t i = 0; i < statements.size(); i++)
    {
      const std::string & statement = statements[i];
      bool match = IsFalse(statement);
      if (!inversed && !match)
        return false; //current statement does not evaluate to false
      if (inversed && match)
        return false; //current statement evaluates as false
    }

    return true;
  }

  bool Validator::ValidateIsEmpty(const Context & context, const std::string & isempty, bool inversed) const
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    bool match = isempty.empty();
    if (!inversed && !match)
      return false; //current statement is not empty
    if (inversed && match)
      return false; //current statement is empty

    return true;
  }

} //namespace shellanything
