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

#include "shellanything/NodeFactory.h"

#include "shellanything/Configuration.h"
#include "shellanything/Item.h"
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
  static const std::string NODE_ITEM = "item";
  static const std::string NODE_VALIDITY = "validity";
  static const std::string NODE_VISIBILITY = "visibility";
  static const std::string NODE_ACTION_CLIPBOARD = "clipboard";
  static const std::string NODE_ACTION_EXEC = "exec";
  static const std::string NODE_ACTION_PROMPT = "prompt";
  static const std::string NODE_ACTION_PROPERTY = "property";
  static const std::string NODE_ACTION_OPEN = "open";

  NodeFactory::NodeFactory()
  {
  }

  NodeFactory::~NodeFactory()
  {
  }

  NodeFactory & NodeFactory::getInstance()
  {
    static NodeFactory _instance;
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

  Validator * parseValidator(const XMLElement* element, std::string & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return NULL;
    }

    Validator * validator = NULL;
    if (NODE_VALIDITY == element->Name())
    {
      validator = new Validator("Validity");
    }
    else if (NODE_VISIBILITY == element->Name())
    {
      validator = new Validator("Visibility");
    }
    else
    {
      error = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::toString(element->GetLineNum()) + " is not a <validity> or <visibility> node";
      return NULL;
    }

    //temporary parsed attribute values
    std::string tmp_str;
    int tmp_int = -1;

    //parse maxfiles
    tmp_str = "";
    tmp_int = -1;
    if (parseAttribute(element, "maxfiles", true, true, tmp_str, error))
    {
      if (ra::strings::parse(tmp_str, tmp_int))
      {
        validator->setMaxFiles(tmp_int);
      }
    }

    //parse maxfolders
    tmp_str = "";
    tmp_int = -1;
    if (parseAttribute(element, "maxfolders", true, true, tmp_str, error))
    {
      if (ra::strings::parse(tmp_str, tmp_int))
      {
        validator->setMaxDirectories(tmp_int);
      }
    }

    //parse fileextensions
    tmp_str = "";
    tmp_int = -1;
    if (parseAttribute(element, "fileextensions", true, true, tmp_str, error))
    {
      if (!tmp_str.empty())
      {
        validator->setFileExtensions(tmp_str);
      }
    }

    return validator;
  }

  Action * parseAction(const XMLElement* element, std::string & error)
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

  Item * parseItem(const XMLElement* element, std::string & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return NULL;
    }

    //at this step the <item> is valid
    Item * item = new Item();

    //parse separator
    std::string item_separator;
    bool have_separetor = parseAttribute(element, "separator", true, true, item_separator, error);
    bool is_separator = false;
    if (have_separetor)
    {
      is_separator = parseBoolean(item_separator);
      if (is_separator)
      {
        item->setSeparator(true);
        return item;
      }
    }

    //parse name
    std::string item_name;
    if (!parseAttribute(element, "name", false, false, item_name, error))
    {
      delete item;
      return NULL;
    }
    item->setName(item_name);

    //parse icon
    std::string icon_path;
    if (parseAttribute(element, "icon", true, true, icon_path, error))
    {
      Icon * icon = new Icon();
      icon->setPath(icon_path);
      item->setIcon(icon);
    }

    ElementPtrList elements; //temporary xml element containers

    //find <validity> node under <item>
    elements = getChildNodes(element, NODE_VALIDITY);
    for(size_t i=0; i<elements.size(); i++)
    {
      Node * parsed = NodeFactory::getInstance().parseNode(elements[i], error);
      Validator * validity = dynamic_cast<Validator *>(parsed);
      if (validity == NULL)
      {
        delete item;
        return NULL;
      }
      item->setValidity(validity);
    }

    //find <visibility> node under <item>
    elements = getChildNodes(element, NODE_VISIBILITY);
    for(size_t i=0; i<elements.size(); i++)
    {
      Node * parsed = NodeFactory::getInstance().parseNode(elements[i], error);
      Validator * visibility = dynamic_cast<Validator *>(parsed);
      if (visibility == NULL)
      {
        delete item;
        return NULL;
      }
      item->setVisibility(visibility);
    }

    //find <actions> node under <item>
    const XMLElement* xml_actions = element->FirstChildElement("actions");
    if (xml_actions)
    {
      //actions must be read in order.

      //find <clipboard>, <exec>, <prompt>, <property> or <open> nodes under <actions>
      const XMLElement* xml_action = xml_actions->FirstChildElement();
      while (xml_action)
      {
        //found a new action node
        Node * parsed = NodeFactory::getInstance().parseNode(xml_action, error);
        Action * action = dynamic_cast<Action*>(parsed);
        if (action == NULL)
        {
          delete item;
          return NULL;
        }

        //add the new action node
        item->addChild(action);

        //next action node
        xml_action = xml_action->NextSiblingElement();
      }
    }

    //find <item> node under <item>
    elements = getChildNodes(element, NODE_ITEM);
    for(size_t i=0; i<elements.size(); i++)
    {
      Node * parsed = NodeFactory::getInstance().parseNode(elements[i], error);
      Item * subitem = dynamic_cast<Item *>(parsed);
      if (subitem == NULL)
      {
        delete item;
        return NULL;
      }
      item->addChild(subitem);
    }

    return item;
  }

  Node * NodeFactory::parseNode(const tinyxml2::XMLElement * element, std::string & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return NULL;
    }

    std::string xml_name = element->Name();

    if (xml_name == NODE_ITEM)
      return parseItem(element, error);
    else if (xml_name == NODE_VALIDITY)
      return parseValidator(element, error);
    else if (xml_name == NODE_VISIBILITY)
      return parseValidator(element, error);
    else if (xml_name == NODE_ACTION_CLIPBOARD || xml_name == NODE_ACTION_EXEC || xml_name == NODE_ACTION_PROMPT || xml_name == NODE_ACTION_PROPERTY || xml_name == NODE_ACTION_OPEN )
      return parseAction(element, error);
    else
    {
      error = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::toString(element->GetLineNum()) + " is an unknown type.";
      return NULL;
    }
  }

} //namespace shellanything
