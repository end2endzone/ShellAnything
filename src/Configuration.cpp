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

#include "shellanything/Configuration.h"
#include "shellanything/Context.h"
#include "rapidassist/filesystem.h"
#include "Platform.h"
#include "ObjectFactory.h"

#include "tinyxml2.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <Windows.h>

using namespace tinyxml2;

namespace shellanything
{

  Configuration::Configuration() : Node("Configuration"),
    mFileModifiedDate(0),
    mDefaults(NULL)
  {
  }

  Configuration::~Configuration()
  {
  }

  Configuration * Configuration::loadFile(const std::string & path, std::string & error)
  {
    error = "";

    if (!ra::filesystem::fileExists(path.c_str()))
    {
      error = "File '" + path + "' not found.";
      return NULL;
    }

    uint64_t file_modified_date = ra::filesystem::getFileModifiedDate(path.c_str());

    //Parse the xml file
    //http://leethomason.github.io/tinyxml2/
    
    XMLDocument doc;
    XMLError result = doc.LoadFile(path.c_str());
    if (result != XML_SUCCESS && doc.ErrorStr())
    {
      error = doc.ErrorStr();
      return NULL;
    }

    const XMLElement * xml_shell = XMLHandle(&doc).FirstChildElement("root").FirstChildElement("shell").ToElement();
    if (!xml_shell)
    {
      error = "Node <shell> not found";
      return NULL;
    }

    Configuration * config = new Configuration();
    config->setFilePath(path);
    config->setFileModifiedDate(file_modified_date);

    //find <defaults> nodes under <shell>
    const XMLElement* xml_defaults = xml_shell->FirstChildElement("defaults");
    while (xml_defaults)
    {
      //found a new menu node
      Defaults * defaults = ObjectFactory::getInstance().parseDefaults(xml_defaults, error);
      if (defaults != NULL)
      {
        //add the new menu to the current configuration
        config->setDefaults(defaults);
      }

      //next defaults node
      xml_defaults = xml_defaults->NextSiblingElement("defaults");
    }

    //find <menu> nodes under <shell>
    const XMLElement* xml_menu = xml_shell->FirstChildElement("menu");
    while (xml_menu)
    {
      //found a new menu node
      Menu * menu = ObjectFactory::getInstance().parseMenu(xml_menu, error);
      if (menu == NULL)
      {
        delete config;
        return NULL;
      }

      //add the new menu to the current configuration
      config->addChild(menu);

      //next menu node
      xml_menu = xml_menu->NextSiblingElement("menu");
    }

    return config;
  }

  bool Configuration::isValidConfigFile(const std::string & path)
  {
    std::string file_extension = ra::filesystem::getFileExtention(path);
    file_extension = ra::strings::uppercase(file_extension);
    if (file_extension == "XML")
    {
      //read the beginning of the file
      std::string content;
      bool readed = peekFile(path.c_str(), 1024, content);
      if (readed)
      {
        //and look for special XML tags
        size_t rootPos = content.find("<root>", 0);
        size_t shellPos = content.find("<shell>", rootPos);
        size_t menuPos = content.find("<menu", shellPos);
        if (rootPos != std::string::npos &&
            shellPos != std::string::npos &&
            menuPos != std::string::npos)
        {
          //found the required tags
          return true;
        }
      }
    }
    return false;
  }

  const std::string & Configuration::getFilePath() const
  {
    return mFilePath;
  }

  void Configuration::setFilePath(const std::string & iFilePath)
  {
    mFilePath = iFilePath;
  }

  const uint64_t & Configuration::getFileModifiedDate() const
  {
    return mFileModifiedDate;
  }

  void Configuration::setFileModifiedDate(const uint64_t & iFileModifiedDate)
  {
    mFileModifiedDate = iFileModifiedDate;
  }

  void Configuration::update(const Context & c)
  {
    //for each child
    Menu::MenuPtrList children = getMenus();
    for(size_t i=0; i<children.size(); i++)
    {
      Menu * child = children[i];
      child->update(c);
    }
  }

  void Configuration::applyDefaults()
  {
    Context empty_context;

    if (mDefaults && mDefaults->getActions().size() > 0)
    {
      //configuration have default properties assigned
      LOG(INFO) << __FUNCTION__ << "(), initializing default properties of configuration file '" << mFilePath.c_str() << "'...";

      //execute actions
      const shellanything::Action::ActionPtrList & actions = mDefaults->getActions();
      for(size_t i=0; i<actions.size(); i++)
      {
        LOG(INFO) << __FUNCTION__ << "(), executing action " << (i+1) << " of " << actions.size() << ".";
        const shellanything::Action * action = actions[i];
        if (action)
        {
          SetLastError(0); //reset win32 error code in case the action fails.
          bool success = action->execute(empty_context);

          if (!success)
          {
            //try to get an error mesage from win32
            uint32_t dwError = shellanything::GetSystemErrorCode();
            if (dwError)
            {
              std::string error_message = shellanything::GetSystemErrorDescription(dwError);
              LOG(ERROR) << __FUNCTION__ << "(), action #" << (i+1) << " has failed: " << error_message;
            }
            else
            {
              //simply log an error
              LOG(ERROR) << __FUNCTION__ << "(), action #" << (i+1) << " has failed.";
            }

            //stop executing the next actions
            i = actions.size();
          }
        }
      }

      LOG(INFO) << __FUNCTION__ << "(), executing default properties of configuration file '" << mFilePath.c_str() << "' completed.";
    }
  }

  Menu * Configuration::findMenuByCommandId(const uint32_t & iCommandId)
  {
    //for each child
    Menu::MenuPtrList children = getMenus();
    for(size_t i=0; i<children.size(); i++)
    {
      Menu * child = children[i];
      Menu * match = child->findMenuByCommandId(iCommandId);
      if (match)
        return match;
    }
 
    return NULL;
  }
 
  uint32_t Configuration::assignCommandIds(const uint32_t & iFirstCommandId)
  {
    uint32_t nextCommandId = iFirstCommandId;

    //for each child
    Menu::MenuPtrList children = getMenus();
    for(size_t i=0; i<children.size(); i++)
    {
      Menu * child = children[i];
      nextCommandId = child->assignCommandIds(nextCommandId);
    }
 
    return nextCommandId;
  }
 
  Menu::MenuPtrList Configuration::getMenus()
  {
    Menu::MenuPtrList sub_menus = filterNodes<Menu*>(this->findChildren("Menu"));
    return sub_menus;
  }

  void Configuration::setDefaults(Defaults * defaults)
  {
    if (mDefaults)
      delete mDefaults;

    mDefaults = defaults;
  }

  const Defaults * Configuration::getDefaults() const
  {
    return mDefaults;
  }

} //namespace shellanything
