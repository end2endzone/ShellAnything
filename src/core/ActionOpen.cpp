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
#include "LoggerHelper.h"

#include "tinyxml2.h"
using namespace tinyxml2;

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

  bool ActionOpen::Execute(const SelectionContext& context) const
  {
    PropertyManager& pmgr = PropertyManager::GetInstance();
    std::string path = pmgr.Expand(mPath);

    IProcessLauncherService* process_launcher_service = App::GetInstance().GetProcessLauncherService();
    if (process_launcher_service == NULL)
    {
      SA_LOG(ERROR) << "No Process Launcher service configured for creating process.";
      return false;
    }

    //is path a file?
    if (ra::filesystem::FileExistsUtf8(path.c_str()))
    {
      SA_LOG(INFO) << "Open file '" << path << "'.";
      bool success = process_launcher_service->OpenDocument(path);
      if (!success)
        SA_LOG(ERROR) << "Failed opening file '" << path << "'.";
      return success;
    }

    //is path a directory?
    if (ra::filesystem::DirectoryExistsUtf8(path.c_str()))
    {
      SA_LOG(INFO) << "Open directory '" << path << "'.";
      bool success = process_launcher_service->OpenPath(path);
      if (!success)
        SA_LOG(ERROR) << "Failed opening directory '" << path << "'.";
      return success;
    }

    //is path a valid url?
    if (process_launcher_service->IsValidUrl(path))
    {
      SA_LOG(INFO) << "Open url '" << path << "'.";
      bool success = process_launcher_service->OpenUrl(path);
      if (!success)
        SA_LOG(ERROR) << "Failed opening URL '" << path << "'.";
      return success;
    }

    SA_LOG(ERROR) << "Unable to open '" << path << "'.";
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
