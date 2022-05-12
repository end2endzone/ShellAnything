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

#include "PropertyManager.h"
#include "Configuration.h"
#include "Menu.h"
#include "Validator.h"
#include "ActionClipboard.h"
#include "ActionExecute.h"
#include "ActionStop.h"
#include "ActionFile.h"
#include "ActionPrompt.h"
#include "ActionProperty.h"
#include "ActionOpen.h"
#include "ActionMessage.h"

#include "rapidassist/strings.h"
#include "rapidassist/unicode.h"

using namespace tinyxml2;

namespace shellanything
{
  static const std::string NODE_MENU = "menu";
  static const std::string NODE_ICON = "icon";
  static const std::string NODE_VALIDITY = "validity";
  static const std::string NODE_VISIBILITY = "visibility";
  static const std::string NODE_DEFAULTSETTINGS = "default";
  static const std::string& NODE_ACTION_STOP = ActionStop::XML_ELEMENT_NAME;
  static const std::string& NODE_ACTION_PROPERTY = ActionProperty::XML_ELEMENT_NAME;
  static const std::string NODE_PLUGIN = "plugin";

  ObjectFactory::ObjectFactory()
  {
    // Add IAction factories for native actions.
    registry.AddActionFactory(ActionClipboard::NewFactory());
    registry.AddActionFactory(ActionExecute::NewFactory());
    registry.AddActionFactory(ActionFile::NewFactory());
    registry.AddActionFactory(ActionMessage::NewFactory());
    registry.AddActionFactory(ActionOpen::NewFactory());
    registry.AddActionFactory(ActionPrompt::NewFactory());
    registry.AddActionFactory(ActionProperty::NewFactory());
    registry.AddActionFactory(ActionStop::NewFactory());
  }

  ObjectFactory::~ObjectFactory()
  {
  }

  ObjectFactory & ObjectFactory::GetInstance()
  {
    static ObjectFactory _instance;
    return _instance;
  }

  typedef std::vector<const XMLElement *> ElementPtrList;
  ElementPtrList GetChildNodes(const XMLElement* element, const std::string & name)
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

  bool ObjectFactory::ParseAttribute(const XMLElement* element, const char * attr_name, bool is_optional, bool allow_empty_values, std::string & attr_value, std::string & error)
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
      error = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::ToString(element->GetLineNum()) + " is missing attribute '" + std::string(attr_name) + "'.";
      return false;
    }

    attr_value = attr_node->Value();

    if (!allow_empty_values && attr_value.empty())
    {
      error = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::ToString(element->GetLineNum()) + " have attribute '" + std::string(attr_name) + "' value empty.";
      return false;
    }

    return true;
  }

  bool ObjectFactory::ParseAttribute(const XMLElement* element, const char * attr_name, bool is_optional, bool allow_empty_values, int & attr_value, std::string & error)
  {
    std::string str_value;    
    if (!ParseAttribute(element, attr_name, is_optional, allow_empty_values, str_value, error))
      return false; //error is already set

    //convert string to int
    int int_value = -1;
    if (!ra::strings::Parse(str_value, int_value))
    {
      //failed parsing
      error << "Failed parsing attribute '" << attr_name << "' of node '" << element->Name() << "'.";
      return false;
    }

    //valid
    attr_value = int_value;
    return true;
  }

  void ObjectFactory::SetActivePlugins(const Plugin::PluginPtrList& plugins)
  {
    mPlugins = plugins;
  }

  void ObjectFactory::ClearActivePlugins()
  {
    mPlugins.clear();
  }

  Validator * ObjectFactory::ParseValidator(const tinyxml2::XMLElement * element, std::string & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return NULL;
    }

    if (NODE_VALIDITY != element->Name() && NODE_VISIBILITY != element->Name() && NODE_ACTION_STOP != element->Name())
    {
      error = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::ToString(element->GetLineNum()) + " is not a <validity> or <visibility> node";
      return NULL;
    }

    Validator * validator = new Validator();

    PropertyManager& pmgr = PropertyManager::GetInstance();

    //parse class
    std::string class_;
    if (ParseAttribute(element, Validator::ATTRIBUTE_CLASS.c_str(), true, true, class_, error))
    {
      if (!class_.empty())
      {
        validator->SetClass(class_);
      }
    }

    //parse pattern
    std::string pattern;
    if (ParseAttribute(element, Validator::ATTRIBUTE_PATTERN.c_str(), true, true, pattern, error))
    {
      if (!pattern.empty())
      {
        validator->SetPattern(pattern);
      }
    }

    //parse exprtk
    std::string exprtk;
    if (ParseAttribute(element, Validator::ATTRIBUTE_EXPRTK.c_str(), true, true, exprtk, error))
    {
      if (!exprtk.empty())
      {
        validator->SetExprtk(exprtk);
      }
    }

    //parse maxfiles
    int maxfiles = -1;
    if (ParseAttribute(element, Validator::ATTRIBUTE_MAXFILES.c_str(), true, true, maxfiles, error))
    {
      validator->SetMaxFiles(maxfiles);
    }

    //parse maxfolders
    int maxfolders = -1;
    if (ParseAttribute(element, Validator::ATTRIBUTE_MAXDIRECTORIES.c_str(), true, true, maxfolders, error))
    {
      validator->SetMaxDirectories(maxfolders);
    }

    //parse fileextensions
    std::string fileextensions;
    if (ParseAttribute(element, Validator::ATTRIBUTE_FILEEXTENSIONS.c_str(), true, true, fileextensions, error))
    {
      if (!fileextensions.empty())
      {
        validator->SetFileExtensions(fileextensions);
      }
    }

    //parse exists
    std::string exists;
    if (ParseAttribute(element, Validator::ATTRIBUTE_EXISTS.c_str(), true, true, exists, error))
    {
      if (!exists.empty())
      {
        validator->SetFileExists(exists);
      }
    }

    //parse properties
    std::string properties;
    if (ParseAttribute(element, Validator::ATTRIBUTE_PROPERTIES.c_str(), true, true, properties, error))
    {
      if (!properties.empty())
      {
        validator->SetProperties(properties);
      }
    }

    //parse inverse
    std::string inverse;
    if (ParseAttribute(element, Validator::ATTRIBUTE_INSERVE.c_str(), true, true, inverse, error))
    {
      if (!inverse.empty())
      {
        validator->SetInserve(inverse);
      }
    }

    //parse istrue
    std::string istrue;
    if (ParseAttribute(element, Validator::ATTRIBUTE_ISTRUE.c_str(), true, true, istrue, error))
    {
      if (!istrue.empty())
      {
        validator->SetIsTrue(istrue);
      }
    }

    //parse isfalse
    std::string isfalse;
    if (ParseAttribute(element, Validator::ATTRIBUTE_ISFALSE.c_str(), true, true, isfalse, error))
    {
      if (!isfalse.empty())
      {
        validator->SetIsFalse(isfalse);
      }
    }

    //parse isempty
    std::string isempty;
    if (ParseAttribute(element, Validator::ATTRIBUTE_ISEMPTY.c_str(), true, true, isempty, error))
    {
      if (!isempty.empty())
      {
        validator->SetIsEmpty(isempty);
      }
    }

    //parse plugin's custom conditions attributes
    PropertyStore customs_attributes;
    for (size_t i = 0; i < mPlugins.size(); i++)
    {
      Plugin* p = mPlugins[i];
      if (!p)
        continue;

      //for each condition
      const std::string conditions_str = pmgr.Expand(p->GetConditions());
      ra::strings::StringVector conditions = ra::strings::Split(conditions_str, SA_CONDITIONS_ATTR_SEPARATOR_STR);
      for (size_t j = 0; j < conditions.size(); j++)
      {
        const std::string& condition = conditions[j];

        //try to parse this condition
        std::string value;
        bool hasCondition = ParseAttribute(element, condition.c_str(), true, true, value, error);
        if (hasCondition)
        {
          customs_attributes.SetProperty(condition, value);
        }
      }
    }
    validator->SetCustomAttributes(customs_attributes);

    //success
    return validator;
  }

  IAction * ObjectFactory::ParseAction(const XMLElement* element, std::string & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return NULL;
    }

    std::string name = element->Name();

    //look for a factory in the registry for the element name
    IActionFactory* factory = registry.GetActionFactoryFromName(name);

    //or look for a factory in plugin's registry
    for (size_t i = 0; i < mPlugins.size() && factory == NULL; i++)
    {
      Plugin* p = mPlugins[i];
      if (!p)
        continue;

      Registry& plugin_registry = p->GetRegistry();
      factory = plugin_registry.GetActionFactoryFromName(name);
    }

    //if a factory was found
    if (factory)
    {
      //convert the xml element to a string
      XMLPrinter printer;
      element->Accept(&printer);
      std::string text = printer.CStr();

      //try to parse an IAction from the string
      IAction* action = factory->ParseFromXml(text, error);
      return action;
    }

    //invalid
    error = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::ToString(element->GetLineNum()) + " is an unknown type.";
    return NULL;
  }

  Menu * ObjectFactory::ParseMenu(const XMLElement* element, std::string & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return NULL;
    }

    std::string xml_name = element->Name();
    if (xml_name != NODE_MENU)
    {
      error = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::ToString(element->GetLineNum()) + " is an unknown type.";
      return NULL;
    }

    //at this step the <menu> is valid
    Menu * menu = new Menu();

    //parse separator
    std::string menu_separator;
    bool have_separator = ParseAttribute(element, "separator", true, true, menu_separator, error);
    bool separator_parsed = false;
    if (have_separator)
    {
      //try to parse this menu separator as a boolean
      bool is_horizontal_separator = false;
      separator_parsed = ra::strings::Parse(menu_separator, is_horizontal_separator);
      if (separator_parsed && is_horizontal_separator)
      {
        menu->SetSeparator(true);
        return menu;
      }

      //try to parse as a string
      menu_separator = ra::strings::Lowercase(menu_separator);
      if (menu_separator == "horizontal")
      {
        menu->SetSeparator(true);
        return menu;
      }
      else if (menu_separator == "vertical" || menu_separator == "column")
      {
        menu->SetColumnSeparator(true);
        return menu;
      }
    }

    //parse name
    std::string menu_name;
    if (!ParseAttribute(element, "name", false, false, menu_name, error))
    {
      delete menu;
      return NULL;
    }
    menu->SetName(menu_name);

    //parse description
    std::string menu_desc;
    if (!ParseAttribute(element, "description", true, true, menu_desc, error))
    {
      menu->SetDescription(menu_desc);
    }

    //parse icon
    std::string icon_path;
    if (ParseAttribute(element, "icon", true, true, icon_path, error))
    {
      Icon icon;
      icon.SetPath(icon_path);
      menu->SetIcon(icon);
    }

    //parse maxlength
    std::string maxlength_str;
    if (ParseAttribute(element, "maxlength", true, true, maxlength_str, error))
    {
      int maxlength = 0;
      if (ra::strings::Parse(maxlength_str, maxlength) && maxlength > 0)
      {
        menu->SetNameMaxLength(maxlength);
      }
    }

    ElementPtrList elements; //temporary xml element containers

    //find <validity> node under <menu>
    elements = GetChildNodes(element, NODE_VALIDITY);
    for(size_t i=0; i<elements.size(); i++)
    {
      Validator * validator = ObjectFactory::GetInstance().ParseValidator(elements[i], error);
      if (validator == NULL)
      {
        delete menu;
        return NULL;
      }

      //add the new validator
      menu->AddValidity(validator);
    }

    //find <visibility> node under <menu>
    elements = GetChildNodes(element, NODE_VISIBILITY);
    for(size_t i=0; i<elements.size(); i++)
    {
      Validator * validator = ObjectFactory::GetInstance().ParseValidator(elements[i], error);
      if (validator == NULL)
      {
        delete menu;
        return NULL;
      }

      //add the new validator
      menu->AddVisibility(validator);
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
        IAction * action = ObjectFactory::GetInstance().ParseAction(xml_action, error);
        if (action == NULL)
        {
          delete menu;
          return NULL;
        }

        //add the new action node
        menu->AddAction(action);

        //next action node
        xml_action = xml_action->NextSiblingElement();
      }
    }

    //find <menu> node under <menu>
    elements = GetChildNodes(element, NODE_MENU);
    for(size_t i=0; i<elements.size(); i++)
    {
      Menu * submenu = ObjectFactory::GetInstance().ParseMenu(elements[i], error);
      if (submenu == NULL)
      {
        delete menu;
        return NULL;
      }
      menu->AddMenu(submenu);
    }

    //find <icon> node under <menu>
    elements = GetChildNodes(element, "icon");
    for(size_t i=0; i<elements.size(); i++)
    {
      Icon icon;
      if (!ObjectFactory::GetInstance().ParseIcon(elements[i], icon, error))
      {
        //failed icon parsing
        delete menu;
        return NULL;
      }
      menu->SetIcon(icon);
    }

    return menu;
  }

  bool ObjectFactory::ParseIcon(const tinyxml2::XMLElement * element, Icon & icon, std::string & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return false;
    }

    std::string xml_name = element->Name();
    if (xml_name != NODE_ICON)
    {
      error = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::ToString(element->GetLineNum()) + " is an unknown type.";
      return NULL;
    }

    //parse path
    std::string icon_path;
    bool hasPath = ParseAttribute(element, "path", true, true, icon_path, error);

    //parse fileextension
    std::string icon_fileextension;
    bool hasFileExtension = ParseAttribute(element, "fileextension", true, true, icon_fileextension, error);
    
    if (!hasPath && !hasFileExtension)
    {
      //failed parsing
      return false;
    }

    Icon result;
    if (hasPath)
      result.SetPath(icon_path);
    if (hasFileExtension)
      result.SetFileExtension(icon_fileextension);

    //parse index
    int icon_index = -1;
    if (ParseAttribute(element, "index", true, true, icon_index, error))
    {
      result.SetIndex(icon_index);
    }

    //success
    icon = result;
    return true;
  }

  DefaultSettings * ObjectFactory::ParseDefaults(const XMLElement* element, std::string & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return NULL;
    }

    std::string xml_name = element->Name();
    if (xml_name != NODE_DEFAULTSETTINGS)
    {
      error = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::ToString(element->GetLineNum()) + " is an unknown type.";
      return NULL;
    }

    DefaultSettings * defaults = new DefaultSettings();

    ElementPtrList elements; //temporary xml element containers

    //find <property> node under <default>
    elements = GetChildNodes(element, NODE_ACTION_PROPERTY);
    for(size_t i=0; i<elements.size(); i++)
    {
      const tinyxml2::XMLElement * element = elements[i];

      //found a new action node
      IAction * abstract_action = ObjectFactory::GetInstance().ParseAction(element, error);
      if (abstract_action)
      {
        //filter out all type of actions except ActionProperty actions
        ActionProperty * property_action = dynamic_cast<ActionProperty *>(abstract_action);
        if (property_action != NULL)
        {
          //add the new action node
          defaults->AddAction(property_action);
        }
        else
        {
          delete abstract_action;
        }
      }
    }

    //do not return a DefaultSettings instance if empty.
    if (defaults->GetActions().empty())
    {
      delete defaults;
      return NULL;
    }

    return defaults;
  }

  Plugin* ObjectFactory::ParsePlugin(const tinyxml2::XMLElement* element, std::string& error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return false;
    }

    std::string xml_name = element->Name();
    if (xml_name != NODE_PLUGIN)
    {
      error = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::ToString(element->GetLineNum()) + " is an unknown type.";
      return NULL;
    }

    //parse path
    std::string path;
    bool hasPath = ParseAttribute(element, "path", true, true, path, error);
    if (!hasPath)
    {
      error = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::ToString(element->GetLineNum()) + " is missing a path attribute.";
      return NULL;
    }

    //parse description
    std::string description;
    bool hasDescription = ParseAttribute(element, "description", true, true, description, error);

    //parse conditions
    std::string conditions;
    bool hasConditions = ParseAttribute(element, "conditions", true, true, conditions, error);

    //parse actions
    std::string actions;
    bool hasActions = ParseAttribute(element, "actions", true, true, actions, error);

    Plugin* plugin = new Plugin();
    plugin->SetPath(path);
    if (hasDescription)
      plugin->SetDescription(description);
    if (hasConditions)
      plugin->SetConditions(conditions);
    if (hasActions)
      plugin->SetActions(actions);

    //success
    return plugin;
  }

} //namespace shellanything
