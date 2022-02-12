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

#include "ActionMessage.h"
#include "PropertyManager.h"
#include "rapidassist/strings.h"
#include "rapidassist/unicode.h"
#include "ObjectFactory.h"

#include "Windows.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

#include "tinyxml2.h"
using namespace tinyxml2;

namespace shellanything
{
  const std::string ActionMessage::XML_ELEMENT_NAME = "message";

  class ActionMessageFactory : public virtual IActionFactory
  {
  public:
    ActionMessageFactory() {}
    virtual ~ActionMessageFactory() {}

    virtual const std::string& GetName() const { return ActionMessage::XML_ELEMENT_NAME; }

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

      ActionMessage* action = new ActionMessage();
      std::string tmp_str;

      //parse title
      tmp_str = "";
      if (ObjectFactory::ParseAttribute(element, "title", false, true, tmp_str, error))
      {
        action->SetTitle(tmp_str);
      }

      //parse caption
      tmp_str = "";
      if (ObjectFactory::ParseAttribute(element, "caption", false, true, tmp_str, error))
      {
        action->SetCaption(tmp_str);
      }

      //parse icon
      tmp_str = "";
      if (ObjectFactory::ParseAttribute(element, "icon", true, true, tmp_str, error))
      {
        action->SetIcon(tmp_str);
      }

      //done parsing
      return action;
    }

  };

  IActionFactory* ActionMessage::NewFactory()
  {
    return new ActionMessageFactory();
  }

  ActionMessage::ActionMessage()
  {
  }

  ActionMessage::~ActionMessage()
  {
  }

  bool ActionMessage::Execute(const Context & context) const
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
    const std::string title = pmgr.Expand(mTitle);
    const std::string caption = pmgr.Expand(mCaption);
    const std::string icon = pmgr.Expand(mIcon);

    //convert to windows unicode...
    std::wstring title_utf16   = ra::unicode::Utf8ToUnicode(title);
    std::wstring caption_utf16 = ra::unicode::Utf8ToUnicode(caption);

    //debug
    LOG(INFO) << "Message, caption: '" << caption << "', title: '" << title << "'.";

    //validate icon
    UINT icon_flags = MB_OK;
    const std::string tmp = ra::strings::Uppercase(icon);
    if (tmp == "" || tmp == "INFORMATION")
      icon_flags = icon_flags | MB_ICONINFORMATION;
    else if (tmp == "ASTERISK")
      icon_flags = icon_flags | MB_ICONASTERISK;
    else if (tmp == "EXCLAMATION")
      icon_flags = icon_flags | MB_ICONEXCLAMATION;
    else if (tmp == "HAND")
      icon_flags = icon_flags | MB_ICONHAND;
    else if (tmp == "WARNING")
      icon_flags = icon_flags | MB_ICONWARNING;
    else if (tmp == "NONE")
    {
    }
    else
    {
      LOG(ERROR) << "Unknown icon attribute '" << icon << "'.";
      return false;
    }

    int result = MessageBoxW(NULL, title_utf16.c_str(), caption_utf16.c_str(), icon_flags);

    return true;
  }

  const std::string & ActionMessage::GetTitle() const
  {
    return mTitle;
  }

  void ActionMessage::SetTitle(const std::string & title)
  {
    mTitle = title;
  }

  const std::string & ActionMessage::GetCaption() const
  {
    return mCaption;
  }

  void ActionMessage::SetCaption(const std::string & caption)
  {
    mCaption = caption;
  }

  const std::string & ActionMessage::GetIcon() const
  {
    return mIcon;
  }

  void ActionMessage::SetIcon(const std::string & icon)
  {
    mIcon = icon;
  }

} //namespace shellanything
