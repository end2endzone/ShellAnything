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

#include "ObjectFactory.h"

#include "shellanything/Configuration.h"
#include "shellanything/Menu.h"
#include "shellanything/Validator.h"
#include "shellanything/ActionClipboard.h"
#include "shellanything/ActionExecute.h"
#include "shellanything/ActionPrompt.h"
#include "shellanything/ActionProperty.h"
#include "shellanything/ActionOpen.h"

#include "rapidassist/filesystem.h"
#include "rapidassist/strings.h"

using namespace tinyxml2;

namespace shellanything
{
  static const std::string NODE_MENU = "menu";
  static const std::string NODE_ICON = "icon";
  static const std::string NODE_VALIDITY = "validity";
  static const std::string NODE_VISIBILITY = "visibility";
  static const std::string NODE_ACTION_CLIPBOARD = "clipboard";
  static const std::string NODE_ACTION_EXEC = "exec";
  static const std::string NODE_ACTION_PROMPT = "prompt";
  static const std::string NODE_ACTION_PROPERTY = "property";
  static const std::string NODE_ACTION_OPEN = "open";

  ObjectFactory::ObjectFactory()
  {
  }

  ObjectFactory::~ObjectFactory()
  {
  }

  ObjectFactory & ObjectFactory::getInstance()
  {
    static ObjectFactory _instance;
    return _instance;
  }

  typedef std::vector<const XMLElement *> ElementPtrList;
  ElementPtrList getChildNodes(const XMLElement* element, const std::string & name)
  {
    ElementPtrList elements;

    if (!element)
      return elements;

    const XMLElement* current = element->FirstChildElement(name.c_str());
    while (current)
    {
      //found a new node
      elements.push_back(current);

      //next node
      current = current->NextSiblingElement(name.c_str());
    }

    return elements;
  }

  bool parseAttribute(const XMLElement* element, const char * attr_name, bool is_optional, bool allow_empty_values, std::string & attr_value, std::string & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return false;
    }

    attr_value = "";

    const XMLAttribute * attr_node = element->FindAttribute(attr_name);
    if (is_optional && !attr_node)
    {
      //failed parsing but its not an error
      return false;
    }
    else if (!attr_node)
    {
      error = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::toString(element->GetLineNum()) + " is missing attribute '" + std::string(attr_name) + "'.";
      return false;
    }

    attr_value = attr_node->Value();

    if (!allow_empty_values && attr_value.empty())
    {
      error = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::toString(element->GetLineNum()) + " have attribute '" + std::string(attr_name) + "' value empty.";
      return false;
    }

    return true;
  }

  bool parseAttribute(const XMLElement* element, const char * attr_name, bool is_optional, bool allow_empty_values, int & attr_value, std::string & error)
  {
    std::string str_value;    
    if (!parseAttribute(element, attr_name, is_optional, allow_empty_values, str_value, error))
      return false; //error is already set

    //convert string to int
    int int_value = -1;
    if (!ra::strings::parse(str_value, int_value))
    {
      //failed parsing
      error << "Failed parsing attribute '" << attr_name << "' of node '" << element->Name() << "'.";
      return false;
    }

    //valid
    attr_value = int_value;
    return true;
  }

  bool ObjectFactory::parseValidator(const tinyxml2::XMLElement * element, Validator & validator, std::string & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return NULL;
    }

    if (NODE_VALIDITY != element->Name() && NODE_VISIBILITY != element->Name())
    {
      error = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::toString(element->GetLineNum()) + " is not a <validity> or <visibility> node";
      return false;
    }

    Validator result;

    //parse maxfiles
    int maxfiles = -1;
    if (parseAttribute(element, "maxfiles", true, true, maxfiles, error))
    {
      result.setMaxFiles(maxfiles);
    }

    //parse maxfolders
    int maxfolders = -1;
    if (parseAttribute(element, "maxfolders", true, true, maxfolders, error))
    {
      result.setMaxDirectories(maxfolders);
    }

    //parse fileextensions
    std::string fileextensions;
    if (parseAttribute(element, "fileextensions", true, true, fileextensions, error))
    {
      if (!fileextensions.empty())
      {
        result.setFileExtensions(fileextensions);
      }
    }

    //parse properties
    std::string properties;
    if (parseAttribute(element, "properties", true, true, properties, error))
    {
      if (!fileextensions.empty())
      {
        result.setProperties(properties);
      }
    }

    //success
    validator = result;
    return true;
  }

  Action * ObjectFactory::parseAction(const XMLElement* element, std::string & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return NULL;
    }

    //temporary parsed attribute values
    std::string tmp_str;
    int tmp_int = -1;

    if (NODE_ACTION_CLIPBOARD == element->Name())
    {
      ActionClipboard * action = new ActionClipboard();

      //parse value
      tmp_str = "";
      tmp_int = -1;
      if (parseAttribute(element, "value", false, true, tmp_str, error))
      {
        action->setValue(tmp_str);
      }

      //done parsing
      return action;
    }
    else if (NODE_ACTION_EXEC == element->Name())
    {
      ActionExecute * action = new ActionExecute();

      //parse path
      tmp_str = "";
      tmp_int = -1;
      if (parseAttribute(element, "path", false, true, tmp_str, error))
      {
        action->setPath(tmp_str);
      }

      //parse arguments
      tmp_str = "";
      tmp_int = -1;
      if (parseAttribute(element, "arguments", true, true, tmp_str, error))
      {
        action->setArguments(tmp_str);
      }

      //done parsing
      return action;
    }
    else if (NODE_ACTION_PROMPT == element->Name())
    {
      ActionPrompt * action = new ActionPrompt();

      //parse name
      tmp_str = "";
      tmp_int = -1;
      if (parseAttribute(element, "name", false, true, tmp_str, error))
      {
        action->setName(tmp_str);
      }

      //parse title
      tmp_str = "";
      tmp_int = -1;
      if (parseAttribute(element, "title", false, true, tmp_str, error))
      {
        action->setTitle(tmp_str);
      }

      //done parsing
      return action;
    }
    else if (NODE_ACTION_PROPERTY == element->Name())
    {
      ActionProperty * action = new ActionProperty();

      //parse name
      tmp_str = "";
      tmp_int = -1;
      if (parseAttribute(element, "name", false, true, tmp_str, error))
      {
        action->setName(tmp_str);
      }

      //parse value
      tmp_str = "";
      tmp_int = -1;
      if (parseAttribute(element, "value", false, true, tmp_str, error))
      {
        action->setValue(tmp_str);
      }

      //done parsing
      return action;
    }
    else if (NODE_ACTION_OPEN == element->Name())
    {
      ActionOpen * action = new ActionOpen();

      //parse path
      tmp_str = "";
      tmp_int = -1;
      if (parseAttribute(element, "path", false, true, tmp_str, error))
      {
        action->setPath(tmp_str);
      }

      //done parsing
      return action;
    }
    else
    {
      error = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::toString(element->GetLineNum()) + " is an unknown type.";
      return NULL;
    }

    //invalid
    return NULL;
  }

  bool parseBoolean(const std::string & value)
  {
    if (value == "1")
      return true;
    else if (ra::strings::uppercase(value) == "TRUE")
      return true;
    else if (ra::strings::uppercase(value) == "YES")
      return true;
    else if (ra::strings::uppercase(value) == "ON")
      return true;
    return false;
  }

  Menu * ObjectFactory::parseMenu(const XMLElement* element, std::string & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return NULL;
    }

    std::string xml_name = element->Name();
    if (xml_name != NODE_MENU)
    {
      error = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::toString(element->GetLineNum()) + " is an unknown type.";
      return NULL;
    }

    //at this step the <menu> is valid
    Menu * menu = new Menu();

    //parse separator
    std::string menu_separator;
    bool have_separetor = parseAttribute(element, "separator", true, true, menu_separator, error);
    bool is_separator = false;
    if (have_separetor)
    {
      is_separator = parseBoolean(menu_separator);
      if (is_separator)
      {
        menu->setSeparator(true);
        return menu;
      }
    }

    //parse name
    std::string menu_name;
    if (!parseAttribute(element, "name", false, false, menu_name, error))
    {
      delete menu;
      return NULL;
    }
    menu->setName(menu_name);

    //parse description
    std::string menu_desc;
    if (!parseAttribute(element, "description", true, true, menu_desc, error))
    {
      menu->setDescription(menu_desc);
    }

    //parse icon
    std::string icon_path;
    if (parseAttribute(element, "icon", true, true, icon_path, error))
    {
      Icon icon;
      icon.setPath(icon_path);
      menu->setIcon(icon);
    }

    ElementPtrList elements; //temporary xml element containers

    //find <validity> node under <menu>
    elements = getChildNodes(element, NODE_VALIDITY);
    for(size_t i=0; i<elements.size(); i++)
    {
      Validator validity;
      if (!ObjectFactory::getInstance().parseValidator(elements[i], validity, error))
      {
        delete menu;
        return NULL;
      }
      menu->setValidity(validity);
    }

    //find <visibility> node under <menu>
    elements = getChildNodes(element, NODE_VISIBILITY);
    for(size_t i=0; i<elements.size(); i++)
    {
      Validator visibility;
      if (!ObjectFactory::getInstance().parseValidator(elements[i], visibility, error))
      {
        delete menu;
        return NULL;
      }
      menu->setVisibility(visibility);
    }

    //find <actions> node under <menu>
    const XMLElement* xml_actions = element->FirstChildElement("actions");
    if (xml_actions)
    {
      //actions must be read in order.

      //find <clipboard>, <exec>, <prompt>, <property> or <open> nodes under <actions>
      const XMLElement* xml_action = xml_actions->FirstChildElement();
      while (xml_action)
      {
        //found a new action node
        Action * action = ObjectFactory::getInstance().parseAction(xml_action, error);
        if (action == NULL)
        {
          delete menu;
          return NULL;
        }

        //add the new action node
        menu->addAction(action);

        //next action node
        xml_action = xml_action->NextSiblingElement();
      }
    }

    //find <menu> node under <menu>
    elements = getChildNodes(element, NODE_MENU);
    for(size_t i=0; i<elements.size(); i++)
    {
      Menu * submenu = ObjectFactory::getInstance().parseMenu(elements[i], error);
      if (submenu == NULL)
      {
        delete menu;
        return NULL;
      }
      menu->addChild(submenu);
    }

    //find <icon> node under <menu>
    elements = getChildNodes(element, "icon");
    for(size_t i=0; i<elements.size(); i++)
    {
      Icon icon;
      if (!ObjectFactory::getInstance().parseIcon(elements[i], icon, error))
      {
        //failed icon parsing
        delete menu;
        return NULL;
      }
      menu->setIcon(icon);
    }

    return menu;
  }

  bool ObjectFactory::parseIcon(const tinyxml2::XMLElement * element, Icon & icon, std::string & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return false;
    }

    std::string xml_name = element->Name();
    if (xml_name != NODE_ICON)
    {
      error = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::toString(element->GetLineNum()) + " is an unknown type.";
      return NULL;
    }

    //parse path
    std::string icon_path;
    if (!parseAttribute(element, "path", false, false, icon_path, error))
    {
      //failed parsing
      return false;
    }

    Icon result;
    result.setPath(icon_path);

    //parse index
    int icon_index = -1;
    if (parseAttribute(element, "index", true, true, icon_index, error))
    {
      result.setIndex(icon_index);
    }

    //success
    icon = result;
    return true;
  }

} //namespace shellanything
