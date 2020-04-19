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

#include "shellanything/ActionExecute.h"
#include "rapidassist/process_utf8.h"
#include "rapidassist/unicode.h"
#include "rapidassist/filesystem_utf8.h"
#include "PropertyManager.h"
#include "Platform.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )


namespace shellanything
{

  ActionExecute::ActionExecute()
  {
  }

  ActionExecute::~ActionExecute()
  {
  }

  bool ActionExecute::Execute(const Context & iContext) const
  {
    PropertyManager & pmgr = PropertyManager::getInstance();
    std::string path = pmgr.expand(mPath);
    std::string basedir = pmgr.expand(mBaseDir);
    std::string arguments = pmgr.expand(mArguments);

    bool basedir_missing = basedir.empty();
    bool arguments_missing = arguments.empty();

    //Note: the startProcess() function requires basedir to be valid.
    //If not specified try to fill basedir with the best option available
    if (basedir.empty())
    {
      const shellanything::Context::ElementList & elements = iContext.getElements();
      size_t num_dir    = iContext.getNumDirectories();
      size_t num_files  = iContext.getNumFiles();
      if (num_dir == 1 && elements.size() == 1)
      {
        //if a single directory was selected
        basedir = elements[0];
      }
      else if (num_files == 1 && elements.size() == 1)
      {
        //if a single file was selected
        const std::string & selected_file = elements[0];
        const std::string parent_dir = ra::filesystem::GetParentPath(selected_file);
        basedir = parent_dir;
      }
      else if (num_dir == 0 && num_files >= 2 && elements.size() >= 1)
      {
        //if a multiple files was selected
        const std::string & first_selected_file = elements[0];
        const std::string first_parent_dir = ra::filesystem::GetParentPath(first_selected_file);
        basedir = first_parent_dir;
      }
    }

    //warn the user if basedir was modified
    if (basedir_missing && !basedir.empty())
    {
      LOG(INFO) << "Warning: 'basedir' not specified. Setting basedir to '" << basedir << "'.";
    }

    if (basedir.empty())
    {
      LOG(WARNING) << "attribute 'basedir' not specified.";
    }

    //debug
    uint32_t pId = ra::process::INVALID_PROCESS_ID;
    if (arguments_missing)
    {
      LOG(INFO) << "Running '" << path << "' from directory '" << basedir << "'.";
      pId = ra::process::StartProcessUtf8(path, basedir);
    }
    else
    {
      LOG(INFO) << "Running '" << path << "' from directory '" << basedir << "' with arguments '" << arguments << "'.";
      pId = ra::process::StartProcessUtf8(path, basedir, arguments);
    }

    bool success = pId != ra::process::INVALID_PROCESS_ID;
    if (success)
    {
      LOG(INFO) << "Process created. PID=" << pId;
    }

    return success;
  }

  const std::string & ActionExecute::GetPath() const
  {
    return mPath;
  }

  void ActionExecute::SetPath(const std::string & iPath)
  {
    mPath = iPath;
  }

  const std::string & ActionExecute::GetBaseDir() const
  {
    return mBaseDir;
  }

  void ActionExecute::SetBaseDir(const std::string & iBaseDir)
  {
    mBaseDir = iBaseDir;
  }

  const std::string & ActionExecute::GetArguments() const
  {
    return mArguments;
  }

  void ActionExecute::SetArguments(const std::string & iArguments)
  {
    mArguments = iArguments;
  }

} //namespace shellanything
