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
#include "shellanything/Validator.h"
#include "PropertyManager.h"

#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/environment_utf8.h"

namespace shellanything
{
  const std::string Context::MULTI_SELECTION_SEPARATOR_PROPERTY_NAME = "selection.multi.separator";
  const std::string Context::DEFAULT_MULTI_SELECTION_SEPARATOR = ra::environment::GetLineSeparator();

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
      return; // Nothing to register
 
    std::string selection_path           ;
    std::string selection_parent_path    ;
    std::string selection_parent_filename;
    std::string selection_filename       ;
    std::string selection_filename_noext ;
    std::string selection_filename_ext   ;
    std::string selection_drive_letter   ;
    std::string selection_drive_path     ;

    // Get the separator string for multiple selection 
    const std::string & selection_multi_separator = pmgr.GetProperty(Context::MULTI_SELECTION_SEPARATOR_PROPERTY_NAME);

    // For each element
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
      std::string element_selection_drive_letter    = Validator::GetDriveLetter(element);
      std::string element_selection_drive_path      = Validator::GetDrivePath(element);
 
      // Add a separator between values
      if (!selection_path           .empty()) selection_path            .append( selection_multi_separator );
      if (!selection_parent_path    .empty()) selection_parent_path     .append( selection_multi_separator );
      if (!selection_parent_filename.empty()) selection_parent_filename .append( selection_multi_separator );
      if (!selection_filename       .empty()) selection_filename        .append( selection_multi_separator );
      if (!selection_filename_noext .empty()) selection_filename_noext  .append( selection_multi_separator );
      if (!selection_filename_ext   .empty()) selection_filename_ext    .append( selection_multi_separator );
      if (!selection_drive_letter   .empty()) selection_drive_letter    .append( selection_multi_separator );
      if (!selection_drive_path     .empty()) selection_drive_path      .append( selection_multi_separator );

      // Append this specific element properties to the global property string
      selection_path           .append( element_selection_path            );
      selection_parent_path    .append( element_selection_parent_path     );
      selection_parent_filename.append( element_selection_parent_filename );
      selection_filename       .append( element_selection_filename        );
      selection_filename_noext .append( element_selection_filename_noext  );
      selection_filename_ext   .append( element_selection_filename_ext    );
      selection_drive_letter   .append( element_selection_drive_letter    );
      selection_drive_path     .append( element_selection_drive_path      );
    }
 
    pmgr.SetProperty("selection.path"               , selection_path           );
    pmgr.SetProperty("selection.parent.path"        , selection_parent_path    );
    pmgr.SetProperty("selection.parent.filename"    , selection_parent_filename);
    pmgr.SetProperty("selection.filename"           , selection_filename       );
    pmgr.SetProperty("selection.filename.noext"     , selection_filename_noext );
    pmgr.SetProperty("selection.filename.extension" , selection_filename_ext   );
    pmgr.SetProperty("selection.drive.letter"       , selection_drive_letter   );
    pmgr.SetProperty("selection.drive.path"         , selection_drive_path     );
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
    pmgr.ClearProperty("selection.drive.letter"       );
    pmgr.ClearProperty("selection.drive.path"         );
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

    // Update stats
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
