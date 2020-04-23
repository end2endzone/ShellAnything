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
#include "shellanything/Validator.h"
#include "PropertyManager.h"
#include "rapidassist/strings.h"
#include "rapidassist/filesystem_utf8.h"

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

  Validator::Validator(const Validator & validator)
  {
    (*this) = validator;
  }

  Validator::~Validator()
  {
  }

  const Validator & Validator::operator =(const Validator & validator)
  {
    if (this != &validator)
    {
      mMaxFiles       = validator.mMaxFiles       ;
      mMaxDirectories = validator.mMaxDirectories ;
      mProperties     = validator.mProperties     ;
      mFileExtensions = validator.mFileExtensions ;
      mFileExists     = validator.mFileExists     ;
    }
    return (*this);
  }

  const int & Validator::GetMaxFiles() const
  {
    return mMaxFiles;
  }

  void Validator::SetMaxFiles(const int & iMaxFiles)
  {
    mMaxFiles = iMaxFiles;
  }

  const int & Validator::GetMaxDirectories() const
  {
    return mMaxDirectories;
  }

  void Validator::SetMaxDirectories(const int & iMaxDirectories)
  {
    mMaxDirectories = iMaxDirectories;
  }

  const std::string & Validator::GetProperties() const
  {
    return mProperties;
  }

  void Validator::SetProperties(const std::string & iProperties)
  {
    mProperties = iProperties;
  }

  const std::string & Validator::GetFileExtensions() const
  {
    return mFileExtensions;
  }

  void Validator::SetFileExtensions(const std::string & iFileExtensions)
  {
    mFileExtensions = iFileExtensions;
  }

  const std::string & Validator::GetFileExists() const
  {
    return mFileExists;
  }

  void Validator::SetFileExists(const std::string & iFileExists)
  {
    mFileExists = iFileExists;
  }

  bool Validator::Validate(const Context & iContext) const
  {
    if (iContext.GetNumFiles() > mMaxFiles)
      return false; //too many files selected

    if (iContext.GetNumDirectories() > mMaxDirectories)
      return false; //too many directories selected

    //validate properties
    PropertyManager & pmgr = PropertyManager::GetInstance();
    const std::string properties = pmgr.Expand(mProperties);
    if (!properties.empty())
    {
      //split
      ra::strings::StringVector property_list = ra::strings::Split(properties, ";");

      //each property specified must exists and be non-empty
      for(size_t i=0; i<property_list.size(); i++)
      {
        const std::string & p = property_list[i];
        if (!pmgr.HasProperty(p))
          return false; //missing property
        const std::string & p_value = pmgr.GetProperty(p);
        if (p_value.empty())
          return false; //empty
      }
    }

    //validate file extentions
    const std::string file_extensions = pmgr.Expand(mFileExtensions);
    if (!file_extensions.empty())
    {
      //split
      ra::strings::StringVector accepted_file_extensions = ra::strings::Split(file_extensions, ";");
      Uppercase(accepted_file_extensions);

      //for each file selected
      const Context::ElementList & elements = iContext.GetElements();
      for(size_t i=0; i<elements.size(); i++) 
      {
        const std::string & element = elements[i];
        std::string current_file_extension = ra::strings::Uppercase(ra::filesystem::GetFileExtention(element));

        //each file extension must be part of accepted_file_extensions
        bool found = HasValue(accepted_file_extensions, current_file_extension);
        if (!found)
          return false; //current file extension is not accepted
      }
    }

    //validate file/directory exists
    const std::string file_exists = pmgr.Expand(mFileExists);
    if (!file_exists.empty())
    {
      //split
      ra::strings::StringVector mandatory_files = ra::strings::Split(file_exists, ";");

      //for each file
      for(size_t i=0; i<mandatory_files.size(); i++)
      {
        const std::string & element = mandatory_files[i];
        bool isFile = ra::filesystem::FileExistsUtf8(element.c_str());
        bool isDir  = ra::filesystem::DirectoryExistsUtf8(element.c_str());
        if (!isFile && !isDir)
          return false; //mandatory file/directory not found
      }
    }

    return true;
  }

} //namespace shellanything
