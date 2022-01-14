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

#include "Workspace.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/testing.h"
#include "rapidassist/user.h"
#include "rapidassist/environment.h"

namespace shellanything
{

  Workspace::Workspace()
  {
    std::string temp_dir = ra::filesystem::GetTemporaryDirectoryUtf8();
    std::string test_name = ra::testing::GetTestQualifiedName();
    std::string workspace_dir = temp_dir + "\\" + test_name;

    Init(workspace_dir.c_str());
  }

  Workspace::Workspace(const char * workspace)
  {
    Init(workspace);
  }

  Workspace::~Workspace()
  {
    Cleanup();
  }

  bool Workspace::Cleanup()
  {
    if (mWorkspace.empty())
      return true;
    if (!ra::filesystem::DirectoryExistsUtf8(mWorkspace.c_str()))
      return true;

    //printf("Deleting workspace directory: %s\n", mWorkspace.c_str());
    bool deleted = ra::filesystem::DeleteDirectoryUtf8(mWorkspace.c_str());
    if (!deleted)
    {
      printf("*** Failed deleting directory '%s'\n", mWorkspace.c_str());
      return false;
    }

    mWorkspace = "";
    return true;
  }

  void Workspace::Init(const char * workspace)
  {
    if (workspace == NULL || workspace[0] == '\0')
      return;

    //printf("Creating workspace directory: %s\n", workspace);
    bool created = ra::filesystem::CreateDirectoryUtf8(workspace);
    if (!created)
    {
      printf("*** Failed creating directory '%s'.\n", workspace);
      return;
    }

    mWorkspace = workspace;
  }

  std::string Workspace::GetBaseDirectory() const
  {
    return mWorkspace;
  }

  bool Workspace::ImportDirectoryUtf8(const char * source, const char * target_dir)
  {
    if (source == NULL)
      return false;
    if (target_dir == NULL)
      target_dir = "";
    size_t source_length = strlen(source);
    if (source_length == 0)
      return false;
    if (!ra::filesystem::DirectoryExistsUtf8(source))
      return false;

    //printf("Importing directory into workspace: %s\n", source);
    
    //Define source parent directory
    std::string parent_dir = ra::filesystem::GetParentPath(source);

    //Searching all files in the directory
    ra::strings::StringVector files;
    bool found = ra::filesystem::FindFilesUtf8(files, source);
    if (!found)
    {
      printf("*** Failed to get files in directory '%s'.\n", source);
      return false;
    }

    //Copying each files individually.
    for(size_t i=0; i<files.size(); i++)
    {
      const std::string & source_file = files[i];

      //Remove the parent path of the source directory to make the file paths relative to the parent directory.
      std::string target_relative = source_file;
      target_relative.replace(0, parent_dir.size(), ""); //make the source relative
      while (!target_relative.empty() && target_relative[0] == '\\')
        target_relative.erase(0, 1);
      if (target_dir[0] != '\0')
      {
        target_relative.insert(0, 1, '\\');
        target_relative.insert(0, target_dir);
      }

      bool imported = ImportAndRenameFileUtf8(source_file.c_str(), target_relative.c_str());
      if (!imported)
      {
        printf("*** Failed to import file '%s' in workspace.\n", source_file.c_str());
        return false;
      }
    }

    //printf("Directory imported into workspace.\n");
    return true;
  }

  bool Workspace::ImportFileUtf8(const char * source, const char * target_dir)
  {
    if (source == NULL || source[0] == '\0')
      return false;
    if (target_dir == NULL)
      target_dir = "";
    size_t source_length = strlen(source);
    size_t target_length = strlen(target_dir);
    if (!ra::filesystem::FileExistsUtf8(source))
    {
      printf("*** Source file not found: %s\n", source);
      return false;
    }

    //Make the path absolute
    std::string target_dir_abs = this->GetFullPathUtf8(target_dir);
    std::string filename = ra::filesystem::GetFilename(source);
    std::string target_file = target_dir_abs + "\\" + filename;

    //Make sure destination directory exists.
    bool created = ra::filesystem::CreateDirectoryUtf8(target_dir_abs.c_str());
    if (!created)
    {
      printf("*** Failed to create destination directory: %s\n", target_dir_abs.c_str());
      return false;
    }

    //Copy the file
    bool copied = ra::filesystem::CopyFileUtf8(source, target_file);
    if (!copied)
    {
      printf("*** Failed to copy source file '%s' to destination '%s'.\n", source, target_file.c_str());
      return false;
    }

    return true;
  }

  bool Workspace::ImportAndRenameFileUtf8(const char * source, const char * target_relative)
  {
    if (source == NULL || source[0] == '\0')
      return false;
    if (target_relative == NULL)
      target_relative = "";
    size_t source_length = strlen(source);
    size_t target_length = strlen(target_relative);
    if (!ra::filesystem::FileExistsUtf8(source))
    {
      printf("*** Source file not found: %s\n", source);
      return false;
    }

    //Make the path absolute
    std::string target_file = this->GetFullPathUtf8(target_relative);

    //Make sure destination directory exists.
    std::string parent_directory = ra::filesystem::GetParentPath(target_file);
    bool created = ra::filesystem::CreateDirectoryUtf8(parent_directory.c_str());
    if (!created)
    {
      printf("*** Failed to create destination directory: %s\n", parent_directory.c_str());
      return false;
    }

    //Copy the file
    bool copied = ra::filesystem::CopyFileUtf8(source, target_file);
    if (!copied)
    {
      printf("*** Failed to copy source file '%s' to destination '%s'.\n", source, target_file.c_str());
      return false;
    }

    return true;
  }

  std::string Workspace::GetFullPathUtf8(const char * path) const
  {
    if (path == NULL)
      return std::string();
    
    if (path[0] == '\0')
      return mWorkspace;

    std::string source = path;
    while (source[0] == '\\')
    {
      source.erase(0, 1);
    }

    source.insert(0, 1, '\\');
    source.insert(0, mWorkspace);

    return source;
  }

} //namespace shellanything
