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

#include "shellanything/ActionOpen.h"
#include "rapidassist/process.h"
#include "rapidassist/filesystem.h"
#include "PropertyManager.h"
#include "Platform.h"
#include "utf_strings.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

//#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#undef GetEnvironmentVariable
#undef DeleteFile
#undef CreateDirectory
#undef CopyFile
#undef CreateFile

#pragma comment(lib, "Urlmon.lib") //for IsValidURL()

namespace shellanything
{

  ActionOpen::ActionOpen()
  {
  }

  ActionOpen::~ActionOpen()
  {
  }

  bool OpenPathGeneric(const std::string & iPath) {
    SHELLEXECUTEINFO info = { 0 };

    info.cbSize = sizeof(SHELLEXECUTEINFO);

    info.fMask |= SEE_MASK_NOCLOSEPROCESS;
    info.fMask |= SEE_MASK_NOASYNC;
    info.fMask |= SEE_MASK_FLAG_DDEWAIT;

    info.hwnd = HWND_DESKTOP;
    info.nShow = SW_SHOWDEFAULT;
    info.lpVerb = "open";
    info.lpFile = iPath.c_str();
    info.lpParameters = NULL; //arguments
    info.lpDirectory = NULL; // default directory

    BOOL success = ShellExecuteEx(&info);
    return (success == TRUE);
  }

  bool ActionOpen::execute(const Context & iContext) const
  {
    PropertyManager & pmgr = PropertyManager::getInstance();
    std::string path = pmgr.expand(mPath);

    //is path a file?
    if (ra::filesystem::FileExists(path.c_str()))
    {
      LOG(INFO) << "Open file '" << path << "'";
      uint32_t pId = ra::process::OpenDocument(path);
      return pId != ra::process::INVALID_PROCESS_ID;
    }

    //is path a directory?
    if (ra::filesystem::DirectoryExists(path.c_str()))
    {
      LOG(INFO) << "Open directory '" << path << "'";
      bool success = OpenPathGeneric(path);
      return success;
    }

    //is path a valid url?
    std::wstring wide_path = encoding::utf::ansi_to_unicode(path);
    if (IsValidURL(NULL, wide_path.c_str(), 0) == S_OK)
    {
      LOG(INFO) << "Open url '" << path << "'";
      bool success = OpenPathGeneric(path);
      return success;
    }

    LOG(ERROR) << "Unable to open '" << path << "'";
    return false; //file not found
  }

  const std::string & ActionOpen::getPath() const
  {
    return mPath;
  }

  void ActionOpen::setPath(const std::string & iPath)
  {
    mPath = iPath;
  }

} //namespace shellanything
