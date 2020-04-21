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

#include "shellanything/Context.h"
#include "PropertyManager.h"
#include "Platform.h"

#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/environment_utf8.h"

namespace shellanything
{

  Context::Context() :
    mNumFiles(0),
    mNumDirectories(0)
  {
  }

  Context::Context(const Context & c)
  {
    (*this) = c;
  }

  Context::~Context()
  {
  }

  const Context & Context::operator =(const Context & c)
  {
    if (this != &c)
    {
      mElements       = c.mElements;
      mNumFiles       = c.mNumFiles;
      mNumDirectories = c.mNumDirectories;
    }
    return (*this);
  }

  void Context::RegisterProperties() const
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
   
    const Context::ElementList & elements = GetElements();
 
    if (elements.empty())
      return; //nothing to register
 
    std::string selection_path           ;
    std::string selection_parent_path    ;
    std::string selection_parent_filename;
    std::string selection_filename       ;
    std::string selection_filename_noext ;
    std::string selection_filename_ext   ;
 
    //for each element
    for(size_t i=0; i<elements.size(); i++)
    {
      const std::string & element = elements[i];
 
      //${selection.path} is the full path of the clicked element
      //${selection.parent.path} is the full path of the parent element
      //${selection.parent.filename} is the filename of the parent element
      //${selection.filename} is selection.filename (including file extension)
      //${selection.filename_noext} is selection.filename without file extension
      //${selection.filename.extension} is the file extension of the clicked element.
 
      //build properties for this specific element
      std::string element_selection_path            = element;
      std::string element_selection_parent_path     = ra::filesystem::GetParentPath(element_selection_path);
      std::string element_selection_parent_filename = ra::filesystem::GetFilename(element_selection_parent_path.c_str());
      std::string element_selection_filename        = ra::filesystem::GetFilename(element_selection_path.c_str());
      std::string element_selection_filename_noext  = ra::filesystem::GetFilenameWithoutExtension(element_selection_path.c_str());
      std::string element_selection_filename_ext    = ra::filesystem::GetFileExtention(element_selection_filename);
 
      //append this specific element properties to the global property string
 
      //add a newline if the property value is not empty. This allows printing all file path on individual lines
      static const char * line_separator = ra::environment::GetLineSeparator();
      if (!selection_path           .empty()) selection_path            .append( line_separator );
      if (!selection_parent_path    .empty()) selection_parent_path     .append( line_separator );
      if (!selection_parent_filename.empty()) selection_parent_filename .append( line_separator );
      if (!selection_filename       .empty()) selection_filename        .append( line_separator );
      if (!selection_filename_noext .empty()) selection_filename_noext  .append( line_separator );
      if (!selection_filename_ext   .empty()) selection_filename_ext    .append( line_separator );
 
      selection_path           .append( element_selection_path            );
      selection_parent_path    .append( element_selection_parent_path     );
      selection_parent_filename.append( element_selection_parent_filename );
      selection_filename       .append( element_selection_filename        );
      selection_filename_noext .append( element_selection_filename_noext  );
      selection_filename_ext   .append( element_selection_filename_ext    );
    }
 
    pmgr.SetProperty("selection.path"               , selection_path           );
    pmgr.SetProperty("selection.parent.path"        , selection_parent_path    );
    pmgr.SetProperty("selection.parent.filename"    , selection_parent_filename);
    pmgr.SetProperty("selection.filename"           , selection_filename       );
    pmgr.SetProperty("selection.filename.noext"     , selection_filename_noext );
    pmgr.SetProperty("selection.filename.extension" , selection_filename_ext   );
  }
 
  void Context::UnregisterProperties() const
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
    pmgr.ClearProperty("selection.path"               );
    pmgr.ClearProperty("selection.parent.path"        );
    pmgr.ClearProperty("selection.parent.filename"    );
    pmgr.ClearProperty("selection.filename"           );
    pmgr.ClearProperty("selection.filename.noext"     );
    pmgr.ClearProperty("selection.filename.extension" );
  }
 
  const Context::ElementList & Context::GetElements() const
  {
    return mElements;
  }

  void Context::SetElements(const Context::ElementList & elements)
  {
    mElements = elements;

    mNumFiles = 0;
    mNumDirectories = 0;

    //get elements stats
    for(size_t i=0; i<elements.size(); i++)
    {
      const std::string & element = elements[i];
      bool isFile = ra::filesystem::FileExistsUtf8(element.c_str());
      bool isDir  = ra::filesystem::DirectoryExistsUtf8(element.c_str());
      if (isFile)
        mNumFiles++;
      if (isDir)
        mNumDirectories++;
    }
  }

  int Context::GetNumFiles() const
  {
    return mNumFiles;
  }

  int Context::GetNumDirectories() const
  {
    return mNumDirectories;
  }

} //namespace shellanything
