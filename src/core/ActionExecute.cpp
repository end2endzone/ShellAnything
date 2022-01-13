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

#include "ActionExecute.h"
#include "rapidassist/process_utf8.h"
#include "rapidassist/unicode.h"
#include "rapidassist/filesystem_utf8.h"
#include "PropertyManager.h"

#include <windows.h>
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

  bool ActionExecute::Execute(const Context& context) const
  {
    PropertyManager& pmgr = PropertyManager::GetInstance();
    std::string verb = pmgr.Expand(mVerb);

    //If a verb was specified, delegate to VerbExecute(). Otherwise, use ProcessExecute().
    if (verb.empty())
      return ExecuteProcess(context);
    else
      return ExecuteVerb(context);
  }

  bool ActionExecute::ExecuteVerb(const Context& context) const
  {
    PropertyManager& pmgr = PropertyManager::GetInstance();
    std::string path      = pmgr.Expand(mPath);
    std::string basedir   = pmgr.Expand(mBaseDir);
    std::string arguments = pmgr.Expand(mArguments);
    std::string verb      = pmgr.Expand(mVerb);

    std::wstring pathW      = ra::unicode::Utf8ToUnicode(path);
    std::wstring argumentsW = ra::unicode::Utf8ToUnicode(arguments);
    std::wstring basedirW   = ra::unicode::Utf8ToUnicode(basedir);
    std::wstring verbW      = ra::unicode::Utf8ToUnicode(verb);

    SHELLEXECUTEINFOW info = { 0 };

    info.cbSize = sizeof(SHELLEXECUTEINFOW);

    info.fMask |= SEE_MASK_NOCLOSEPROCESS;
    info.fMask |= SEE_MASK_NOASYNC;
    info.fMask |= SEE_MASK_FLAG_DDEWAIT;

    info.hwnd   = HWND_DESKTOP;
    info.nShow  = SW_SHOWDEFAULT;
    info.lpFile = pathW.c_str();

    //Print execute values in the logs
    LOG(INFO) << "Exec: '" << path << "'.";
    if (!verb.empty())
    {
      info.lpVerb = verbW.c_str(); // Verb
      LOG(INFO) << "Verb: '" << verb << "'.";
    }
    if (!arguments.empty())
    {
      info.lpParameters = argumentsW.c_str(); // Arguments
      LOG(INFO) << "Arguments: '" << arguments << "'.";
    }
    if (!basedir.empty())
    {
      info.lpDirectory = basedirW.c_str(); // Default directory
      LOG(INFO) << "Basedir: '" << basedir << "'.";
    }

    //Execute and get the pid
    bool success = (ShellExecuteExW(&info) == TRUE);
    if (!success)
      return false;
    DWORD pId = GetProcessId(info.hProcess);

    success = (pId != ra::process::INVALID_PROCESS_ID);
    if (success)
    {
      LOG(INFO) << "Process created. PID=" << pId;
    }

    return success;
  }

  bool ActionExecute::ExecuteProcess(const Context & context) const
  {
    PropertyManager& pmgr = PropertyManager::GetInstance();
    std::string path      = pmgr.Expand(mPath);
    std::string basedir   = pmgr.Expand(mBaseDir);
    std::string arguments = pmgr.Expand(mArguments);

    bool basedir_missing = basedir.empty();
    bool arguments_missing = arguments.empty();

    //Note: the startProcess() function requires basedir to be valid.
    //If not specified try to fill basedir with the best option available
    if (basedir.empty())
    {
      const shellanything::Context::ElementList & elements = context.GetElements();
      size_t num_dir    = context.GetNumDirectories();
      size_t num_files  = context.GetNumFiles();
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

    //Print execute values in the logs
    LOG(INFO) << "Exec: '" << path << "'.";
    if (!arguments.empty())
    {
      LOG(INFO) << "Arguments: '" << arguments << "'.";
    }    
    if (!basedir.empty())
    {
      LOG(INFO) << "Basedir: '" << basedir << "'.";
    }
    
    //Execute and get the pid
    uint32_t pId = ra::process::INVALID_PROCESS_ID;
    if (arguments_missing)
    {
      pId = ra::process::StartProcessUtf8(path, basedir);
    }
    else
    {
      pId = ra::process::StartProcessUtf8(path, basedir, arguments);
    }

    bool success = (pId != ra::process::INVALID_PROCESS_ID);
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

  void ActionExecute::SetPath(const std::string & path)
  {
    mPath = path;
  }

  const std::string & ActionExecute::GetBaseDir() const
  {
    return mBaseDir;
  }

  void ActionExecute::SetBaseDir(const std::string & base_dir)
  {
    mBaseDir = base_dir;
  }

  const std::string & ActionExecute::GetArguments() const
  {
    return mArguments;
  }

  void ActionExecute::SetArguments(const std::string & arguments)
  {
    mArguments = arguments;
  }

  const std::string& ActionExecute::GetVerb() const
  {
    return mVerb;
  }

  void ActionExecute::SetVerb(const std::string& verb)
  {
    mVerb = verb;
  }

} //namespace shellanything
