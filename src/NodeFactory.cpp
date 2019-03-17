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

#include "rapidassist/filesystem.h"
#include "rapidassist/strings.h"

using namespace tinyxml2;

namespace shellanything
{
  static const std::string ROOT_NODE = "root";
  static const std::string SHELL_NODE = "shell";
  static const std::string ITEM_NODE = "item";
  static const std::string VALIDITY_NODE = "validity";
  static const std::string VISIBILITY_NODE = "visibility";
  static const std::string CLIPBOARD_ACTION_NODE = "clipboard";
  static const std::string EXEC_ACTION_NODE = "exec";
  static const std::string PROMPT_ACTION_NODE = "prompt";
  static const std::string PROPERTY_ACTION_NODE = "property";
  static const std::string OPEN_ACTION_NODE = "open";

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
    if (VALIDITY_NODE == element->Name())
    {
      validator = new Validator("Validity");
    }
    else if (VISIBILITY_NODE == element->Name())
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

    if (CLIPBOARD_ACTION_NODE == element->Name())
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
    else if (EXEC_ACTION_NODE == element->Name())
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
    else if (PROMPT_ACTION_NODE == element->Name())
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
    else if (PROPERTY_ACTION_NODE == element->Name())
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

    //find <validity> node under <item>
    const XMLElement* xml_validity = element->FirstChildElement(VALIDITY_NODE.c_str());
    while (xml_validity)
    {
      //found a new validity node
      Validator * validity = parseValidator(xml_validity, error);
      if (validity == NULL)
      {
        delete item;
        return NULL;
      }

      //add the new validity node
      item->setValidity(validity);

      //next validity node
      xml_validity = xml_validity->NextSiblingElement(VALIDITY_NODE.c_str());
    }

    //find <visibility> node under <item>
    const XMLElement* xml_visibility = element->FirstChildElement(VISIBILITY_NODE.c_str());
    while (xml_visibility)
    {
      //found a new visibility node
      Validator * visibility = parseValidator(xml_visibility, error);
      if (visibility == NULL)
      {
        delete item;
        return NULL;
      }

      //add the new visibility node
      item->setVisibility(visibility);

      //next visibility node
      xml_visibility = xml_visibility->NextSiblingElement(VISIBILITY_NODE.c_str());
    }

    //find <actions> node under <item>
    const XMLElement* xml_actions = element->FirstChildElement("actions");
    if (xml_actions)
    {
      //find <clipboard>, <exec>, <prompt>, <property> or <open> nodes under <actions>
      const XMLElement* xml_action = xml_actions->FirstChildElement();
      while (xml_action)
      {
        //found a new clipboard node
        Action * action = parseAction(xml_action, error);
        if (action == NULL)
        {
          delete item;
          return NULL;
        }

        //add the new action node
        item->addChild(action);

        //next visibility node
        xml_action = xml_action->NextSiblingElement();
      }
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

    if (xml_name == ITEM_NODE)
      return parseItem(element, error);
    else if (xml_name == VALIDITY_NODE)
      return parseValidator(element, error);
    else if (xml_name == VISIBILITY_NODE)
      return parseValidator(element, error);
    else if (xml_name == CLIPBOARD_ACTION_NODE || xml_name == EXEC_ACTION_NODE || xml_name == PROMPT_ACTION_NODE || xml_name == PROPERTY_ACTION_NODE || xml_name == OPEN_ACTION_NODE )
      return parseAction(element, error);
    else
    {
      error = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::toString(element->GetLineNum()) + " is an unknown type.";
      return NULL;
    }
  }

} //namespace shellanything
