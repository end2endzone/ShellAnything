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

#include "ActionOpen.h"
#include "rapidassist/process_utf8.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/unicode.h"
#include "PropertyManager.h"
#include "ObjectFactory.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

 //#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#include "rapidassist/undef_windows_macros.h"

#include "tinyxml2.h"
using namespace tinyxml2;

#pragma comment(lib, "Urlmon.lib") //for IsValidURL()

namespace shellanything
{
  const std::string ActionOpen::XML_ELEMENT_NAME = "open";

  class ActionOpenFactory : public virtual IActionFactory
  {
  public:
    ActionOpenFactory()
    {
    }
    virtual ~ActionOpenFactory()
    {
    }

    virtual const std::string& GetName() const
    {
      return ActionOpen::XML_ELEMENT_NAME;
    }

    virtual IAction* ParseFromXml(const std::string& xml, std::string& error) const
    {
      tinyxml2::XMLDocument doc;
      XMLError result = doc.Parse(xml.c_str());
      if (result != XML_SUCCESS)
      {
        if (doc.ErrorStr())
        {
          error = doc.ErrorStr();
          return NULL;
        }
        else
        {
          error = "Unknown error reported by XML library.";
          return NULL;
        }
      }
      XMLElement* element = doc.FirstChildElement(GetName().c_str());

      ActionOpen* action = new ActionOpen();
      std::string tmp_str;

      //parse path
      tmp_str = "";
      if (ObjectFactory::ParseAttribute(element, "path", false, true, tmp_str, error))
      {
        action->SetPath(tmp_str);
      }

      //done parsing
      return action;
    }

  };

  IActionFactory* ActionOpen::NewFactory()
  {
    return new ActionOpenFactory();
  }

  ActionOpen::ActionOpen()
  {
  }

  ActionOpen::~ActionOpen()
  {
  }

  bool OpenPathGeneric(const std::string& path)
  {
    std::wstring pathW = ra::unicode::Utf8ToUnicode(path);

    SHELLEXECUTEINFOW info = { 0 };

    info.cbSize = sizeof(SHELLEXECUTEINFOW);

    info.fMask |= SEE_MASK_NOCLOSEPROCESS;
    info.fMask |= SEE_MASK_NOASYNC;
    info.fMask |= SEE_MASK_FLAG_DDEWAIT;

    info.hwnd = HWND_DESKTOP;
    info.nShow = SW_SHOWDEFAULT;
    info.lpVerb = L"open";
    info.lpFile = pathW.c_str();
    info.lpParameters = NULL; // arguments
    info.lpDirectory = NULL;  // Default directory

    BOOL success = ShellExecuteExW(&info);
    return (success == TRUE);
  }

  bool ActionOpen::Execute(const SelectionContext& context) const
  {
    PropertyManager& pmgr = PropertyManager::GetInstance();
    std::string path = pmgr.Expand(mPath);

    //is path a file?
    if (ra::filesystem::FileExistsUtf8(path.c_str()))
    {
      LOG(INFO) << "Open file '" << path << "'.";
      uint32_t pId = ra::process::OpenDocumentUtf8(path);
      return pId != ra::process::INVALID_PROCESS_ID;
    }

    //is path a directory?
    if (ra::filesystem::DirectoryExistsUtf8(path.c_str()))
    {
      LOG(INFO) << "Open directory '" << path << "'.";
      bool success = OpenPathGeneric(path);
      return success;
    }

    //is path a valid url?
    std::wstring wide_path = ra::unicode::Utf8ToUnicode(path);
    if (IsValidURL(NULL, wide_path.c_str(), 0) == S_OK)
    {
      LOG(INFO) << "Open url '" << path << "'.";
      bool success = OpenPathGeneric(path);
      return success;
    }

    LOG(ERROR) << "Unable to open '" << path << "'.";
    return false; //file not found
  }

  const std::string& ActionOpen::GetPath() const
  {
    return mPath;
  }

  void ActionOpen::SetPath(const std::string& path)
  {
    mPath = path;
  }

} //namespace shellanything
