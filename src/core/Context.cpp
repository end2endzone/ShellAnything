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

#include "FileMagicManager.h"
#include "Context.h"
#include "Validator.h"
#include "PropertyManager.h"
#include "DriveClass.h"

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
   
    FileMagicManager & fm = FileMagicManager::GetInstance();

    const Context::ElementList & elements = GetElements();
 
    if (elements.empty())
      return; // Nothing to register
 
    std::string selection_path           ;
    std::string selection_dir            ;
    std::string selection_dir_count      ;
    std::string selection_dir_empty      ;
    std::string selection_parent_path    ;
    std::string selection_parent_filename;
    std::string selection_filename       ;
    std::string selection_filename_noext ;
    std::string selection_filename_ext   ;
    std::string selection_drive_letter   ;
    std::string selection_drive_path     ;
    std::string selection_count          ;
    std::string selection_files_count    ;
    std::string selection_directories_count;
    std::string selection_mimetype       ;
    std::string selection_description    ;
    //std::string selection_libmagic_ext ;
    std::string selection_charset        ;

    // Get the separator string for multiple selection 
    const std::string & selection_multi_separator = pmgr.GetProperty(Context::MULTI_SELECTION_SEPARATOR_PROPERTY_NAME);

    // For each element
    for(size_t i=0; i<elements.size(); i++)
    {
      const std::string & element = elements[i];
 
      bool isFile = ra::filesystem::FileExistsUtf8(element.c_str());
      bool isDir = ra::filesystem::DirectoryExistsUtf8(element.c_str());

      //${selection.path} is the full path of the clicked element
      //${selection.dir} is the directory of the clicked element
      //${selection.parent.path} is the full path of the parent element
      //${selection.parent.filename} is the filename of the parent element
      //${selection.filename} is selection.filename (including file extension)
      //${selection.filename_noext} is selection.filename without file extension
      //${selection.filename.extension} is the file extension of the clicked element.
 
      // Build properties for this specific element
      std::string element_selection_path            = element;
      std::string element_selection_dir             = isFile ? ra::filesystem::GetParentPath(element_selection_path) : element;
      std::string element_selection_parent_path     = ra::filesystem::GetParentPath(element_selection_path);
      std::string element_selection_parent_filename = ra::filesystem::GetFilename(element_selection_parent_path.c_str());
      std::string element_selection_filename        = ra::filesystem::GetFilename(element_selection_path.c_str());
      std::string element_selection_filename_noext  = ra::filesystem::GetFilenameWithoutExtension(element_selection_path.c_str());
      std::string element_selection_filename_ext    = ra::filesystem::GetFileExtention(element_selection_filename);
      std::string element_selection_drive_letter    = GetDriveLetter(element);
      std::string element_selection_drive_path      = GetDrivePath(element);
      std::string element_selection_mimetype        = fm.GetMIMEType(element_selection_path);
      std::string element_selection_description     = fm.GetDescription(element_selection_path);
      //std::string element_selection_libmagic_ext  = fm.GetExtension(element_selection_path);
      std::string element_selection_charset         = fm.GetCharset(element_selection_path);
 
      // Add a separator between values
      if (!selection_path           .empty()) selection_path            .append( selection_multi_separator );
      if (!selection_dir            .empty()) selection_dir             .append( selection_multi_separator );
      if (!selection_parent_path    .empty()) selection_parent_path     .append( selection_multi_separator );
      if (!selection_parent_filename.empty()) selection_parent_filename .append( selection_multi_separator );
      if (!selection_filename       .empty()) selection_filename        .append( selection_multi_separator );
      if (!selection_filename_noext .empty()) selection_filename_noext  .append( selection_multi_separator );
      if (!selection_filename_ext   .empty()) selection_filename_ext    .append( selection_multi_separator );
      if (!selection_drive_letter   .empty()) selection_drive_letter    .append( selection_multi_separator );
      if (!selection_drive_path     .empty()) selection_drive_path      .append( selection_multi_separator );
      if (!selection_mimetype       .empty()) selection_mimetype        .append( selection_multi_separator );
      if (!selection_description    .empty()) selection_description     .append( selection_multi_separator );
      //if (!selection_libmagic_ext .empty()) selection_libmagic_ext    .append( selection_multi_separator );
      if (!selection_charset        .empty()) selection_charset         .append( selection_multi_separator );

      // Append this specific element properties to the global property string
      selection_path           .append( element_selection_path            );
      selection_dir            .append( element_selection_dir             );
      selection_parent_path    .append( element_selection_parent_path     );
      selection_parent_filename.append( element_selection_parent_filename );
      selection_filename       .append( element_selection_filename        );
      selection_filename_noext .append( element_selection_filename_noext  );
      selection_filename_ext   .append( element_selection_filename_ext    );
      selection_drive_letter   .append( element_selection_drive_letter    );
      selection_drive_path     .append( element_selection_drive_path      );
      selection_mimetype       .append( element_selection_mimetype        );
      selection_description    .append( element_selection_description     );
      //selection_libmagic_ext .append( element_selection_libmagic_ext    );
      selection_charset        .append( element_selection_charset         );
    }
 
    // Directory based properties
    if (elements.size() == 1)
    {
      const std::string & element = elements[0];
      bool isDir = ra::filesystem::DirectoryExistsUtf8(element.c_str());
      if (isDir)
      {
        ra::strings::StringVector files;
        bool files_found = ra::filesystem::FindFilesUtf8(files, element.c_str(), 0);
        if (files_found)
        {
          selection_dir_count = ra::strings::ToString(files.size());
          selection_dir_empty = (files.size() == 0 ? "true" : "false");
        }
      }
    }

    pmgr.SetProperty("selection.path"               , selection_path           );
    pmgr.SetProperty("selection.dir"                , selection_dir            );
    pmgr.SetProperty("selection.dir.count"          , selection_dir_count      );
    pmgr.SetProperty("selection.dir.empty"          , selection_dir_empty      );
    pmgr.SetProperty("selection.parent.path"        , selection_parent_path    );
    pmgr.SetProperty("selection.parent.filename"    , selection_parent_filename);
    pmgr.SetProperty("selection.filename"           , selection_filename       );
    pmgr.SetProperty("selection.filename.noext"     , selection_filename_noext );
    pmgr.SetProperty("selection.filename.extension" , selection_filename_ext   );
    pmgr.SetProperty("selection.drive.letter"       , selection_drive_letter   );
    pmgr.SetProperty("selection.drive.path"         , selection_drive_path     );
    pmgr.SetProperty("selection.drive.path"         , selection_drive_path     );
    pmgr.SetProperty("selection.mimetype"           , selection_mimetype       );
    pmgr.SetProperty("selection.description"        , selection_description    );
    //pmgr.SetProperty("selection.libmagic_ext"     , selection_libmagic_ext   );
    pmgr.SetProperty("selection.charset"            , selection_charset        );

    selection_count             = ra::strings::ToString(elements.size());
    selection_files_count       = ra::strings::ToString(this->GetNumFiles());
    selection_directories_count = ra::strings::ToString(this->GetNumDirectories());

    pmgr.SetProperty("selection.count"             , selection_count            );
    pmgr.SetProperty("selection.files.count"       , selection_files_count      );
    pmgr.SetProperty("selection.directories.count" , selection_directories_count);
  }
 
  void Context::UnregisterProperties() const
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
    pmgr.ClearProperty("selection.path"               );
    pmgr.ClearProperty("selection.dir"                );
    pmgr.ClearProperty("selection.parent.path"        );
    pmgr.ClearProperty("selection.parent.filename"    );
    pmgr.ClearProperty("selection.filename"           );
    pmgr.ClearProperty("selection.filename.noext"     );
    pmgr.ClearProperty("selection.filename.extension" );
    pmgr.ClearProperty("selection.drive.letter"       );
    pmgr.ClearProperty("selection.drive.path"         );
    pmgr.ClearProperty("selection.mimetype"           );
    pmgr.ClearProperty("selection.description"        );
    //pmgr.ClearProperty("selection.libmagic_ext"       );
    pmgr.ClearProperty("selection.charset"            );
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