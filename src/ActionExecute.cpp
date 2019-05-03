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
#include "PropertyManager.h"
#include "Platform.h"

#include <glog/logging.h>

#include "rapidassist/filesystem.h"

namespace shellanything
{

  ActionExecute::ActionExecute()
  {
  }

  ActionExecute::~ActionExecute()
  {
  }

  bool ActionExecute::execute(const Context & iContext) const
  {
    PropertyManager & pmgr = PropertyManager::getInstance();
    std::string path = pmgr.expand(mPath);
    std::string basedir = pmgr.expand(mBaseDir);
    std::string arguments = pmgr.expand(mArguments);

    bool basedir_missing = basedir.empty();

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
        const std::string parent_dir = ra::filesystem::getParentPath(selected_file);
        basedir = parent_dir;
      }
      else if (num_dir == 0 && num_files >= 2 && elements.size() >= 1)
      {
        //if a multiple files was selected
        const std::string & first_selected_file = elements[0];
        const std::string first_parent_dir = ra::filesystem::getParentPath(first_selected_file);
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
    LOG(INFO) << "Running '" << path << "' from directory '" << basedir << "' with arguments '" << arguments << "'";

    uint32_t pId = startProcess(path, arguments, basedir);

    bool success = pId != 0;
    if (success)
    {
      LOG(INFO) << "Process created. PID=" << pId;
    }

    return success;
  }

  const std::string & ActionExecute::getPath() const
  {
    return mPath;
  }

  void ActionExecute::setPath(const std::string & iPath)
  {
    mPath = iPath;
  }

  const std::string & ActionExecute::getBaseDir() const
  {
    return mBaseDir;
  }

  void ActionExecute::setBaseDir(const std::string & iBaseDir)
  {
    mBaseDir = iBaseDir;
  }

  const std::string & ActionExecute::getArguments() const
  {
    return mArguments;
  }

  void ActionExecute::setArguments(const std::string & iArguments)
  {
    mArguments = iArguments;
  }

} //namespace shellanything
