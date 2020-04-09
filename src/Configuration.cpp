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
#include "shellanything/ActionProperty.h"

#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/random.h"
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
#undef GetEnvironmentVariable
#undef DeleteFile
#undef CreateDirectory
#undef CopyFile
#undef CreateFile

using namespace tinyxml2;

namespace shellanything
{
  std::string GetXmlEncoding(XMLDocument & doc, std::string & error)
  {
    XMLNode * first = doc.FirstChild();
    if (!first)
    {
      error = "XML declaration not found.";
      return "";
    }

    XMLDeclaration * declaration = first->ToDeclaration();
    if (!declaration)
    {
      error = "XML declaration not found.";
      return "";
    }

    if (declaration->Value() == NULL)
    {
      error = "XML declaration is empty.";
      return "";
    }

    //expecting: xml version="1.0" encoding="utf-8"
    static const std::string ENCODING_IDENTIFIER = "ENCODING=\"";
    const std::string str = declaration->Value();
    size_t pos_start = ra::strings::Uppercase(str).find(ENCODING_IDENTIFIER);
    if (pos_start == std::string::npos)
    {
      error = "XML encoding not specified.";
      return "";
    }
    pos_start += ENCODING_IDENTIFIER.size();

    size_t pos_end = str.find("\"", pos_start);
    if (pos_end == std::string::npos)
    {
      error = "XML declaration is malformed. End of encoding attribute is not found.";
      return "";
    }
    size_t length = pos_end - pos_start;
    
    //extract encoding
    std::string encoding = str.substr(pos_start, length);

    return encoding;
  }

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

    if (!ra::filesystem::FileExistsUtf8(path.c_str()))
    {
      error = "File '" + path + "' not found.";
      return NULL;
    }

    uint64_t file_modified_date = ra::filesystem::GetFileModifiedDateUtf8(path.c_str());

    //Parse the xml file
    //http://leethomason.github.io/tinyxml2/
    
    XMLDocument doc;
    XMLError result = doc.LoadFile(path.c_str());
    if (result == XML_ERROR_FILE_NOT_FOUND)
    {
      //We already validated that file exists. This is probably because the file path is utf-8 encoded.
      //This is not supported by TinyXML.

      //As a workaround, copy the file to a temp directory with a non utf-8 path
      std::string temp_filename;
      ra::random::GetRandomString(temp_filename, 10);
      temp_filename.append(".xml");
      std::string temp_path = ra::filesystem::GetTemporaryDirectory() + ra::filesystem::GetPathSeparatorStr() + temp_filename;
      bool copied = ra::filesystem::CopyFileUtf8(path, temp_path);
      if (!copied)
      {
        error.clear();
        error << "Failed to copy file '" << path << "' to '" << temp_path << "'.";
        return NULL;
      }

      //and load the new temp file
      result = doc.LoadFile(temp_path.c_str());

      //cleanup
      ra::filesystem::DeleteFile(temp_path.c_str());
    }
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

    //get xml encoding
    std::string encoding = GetXmlEncoding(doc, error);
    if (encoding.empty())
    {
      //Not an utf-8 encoded file.
      error.clear();
      error << "File is not encoded in UTF-8.";
      return NULL;
    }

    //validate utf-8 encoding
    std::string tmp_encoding = ra::strings::Uppercase(encoding);
    ra::strings::Replace(tmp_encoding, "-", "");
    if (tmp_encoding != "UTF8")
    {
      //Not an utf-8 encoded file.
      error.clear();
      error << "File is not encoded in UTF-8. Encoding found is '" << encoding << "'.";
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

    //find <default> nodes under <shell>
    const XMLElement* xml_defaults = xml_shell->FirstChildElement("default");
    while (xml_defaults)
    {
      //found a new menu node
      DefaultSettings * defaults = ObjectFactory::getInstance().parseDefaults(xml_defaults, error);
      if (defaults != NULL)
      {
        //add the new menu to the current configuration
        config->setDefaultSettings(defaults);
      }

      //next defaults node
      xml_defaults = xml_defaults->NextSiblingElement("default");
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
    std::string file_extension = ra::filesystem::GetFileExtention(path);
    file_extension = ra::strings::Uppercase(file_extension);
    if (file_extension == "XML")
    {
      //read the beginning of the file
      std::string content;
      bool readed = ra::filesystem::PeekFileUtf8(path.c_str(), 2048, content);
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

  void Configuration::applyDefaultSettings()
  {
    if (mDefaults && mDefaults->getActions().size() > 0)
    {
      //configuration have default properties assigned
      LOG(INFO) << __FUNCTION__ << "(), initializing default properties of configuration file '" << mFilePath.c_str() << "'...";

      const shellanything::Action::ActionPtrList & actions = mDefaults->getActions();

      //convert 'actions' to a list of <const shellanything::ActionProperty *>
      typedef std::vector<const ActionProperty *> ActionPropertyPtrList;
      ActionPropertyPtrList properties;
      for(size_t i=0; i<actions.size(); i++)
      {
        const shellanything::Action * abstract_action = actions[i];
        const shellanything::ActionProperty * action_property = dynamic_cast<const shellanything::ActionProperty *>(abstract_action);
        if (action_property)
          properties.push_back(action_property);
      }

      //apply all ActionProperty
      Context empty_context;
      for(size_t i=0; i<properties.size(); i++)
      {
        LOG(INFO) << __FUNCTION__ << "(), executing property " << (i+1) << " of " << properties.size() << ".";
        const shellanything::ActionProperty * action_property = properties[i];
        if (action_property)
        {
          //no need to look for failures. ActionProperty never fails.
          action_property->execute(empty_context);
        }
      }

      LOG(INFO) << __FUNCTION__ << "(), execution of default properties of configuration file '" << mFilePath.c_str() << "' completed.";
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

  void Configuration::setDefaultSettings(DefaultSettings * defaults)
  {
    if (mDefaults)
      delete mDefaults;

    mDefaults = defaults;
  }

  const DefaultSettings * Configuration::getDefaultSettings() const
  {
    return mDefaults;
  }

} //namespace shellanything
