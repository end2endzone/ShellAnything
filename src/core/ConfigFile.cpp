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

#include "ConfigFile.h"
#include "SelectionContext.h"
#include "ActionProperty.h"
#include "ObjectFactory.h"
#include "LoggerHelper.h"

#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/random.h"
#include "rapidassist/environment.h"

#include "tinyxml2.h"
#include "SaUtils.h"

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
  static ConfigFile* gUpdatingConfigFile = NULL;

  std::string GetXmlEncoding(XMLDocument& doc, std::string& error)
  {
    XMLNode* first = doc.FirstChild();
    if (!first)
    {
      error = "XML declaration not found.";
      return "";
    }

    XMLDeclaration* declaration = first->ToDeclaration();
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

  ConfigFile::ConfigFile() :
    mFileModifiedDate(0),
    mDefaults(NULL)
  {
  }

  ConfigFile::~ConfigFile()
  {
    DeleteChildren();
  }

  ConfigFile* ConfigFile::GetUpdatingConfigFile()
  {
    return gUpdatingConfigFile;
  }

  void ConfigFile::SetUpdatingConfigFile(ConfigFile* config_file)
  {
    gUpdatingConfigFile = config_file;
  }

  ConfigFile* ConfigFile::LoadFile(const std::string& path, std::string& error)
  {
    SA_DECLARE_SCOPE_LOGGER_INFO(sli);
    sli.verbose = true;
    ScopeLogger logger(&sli);

    SA_VERBOSE_LOG(INFO) << "Loading configuration file '" << path << "'.";

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

    const XMLElement* xml_root = XMLHandle(&doc).FirstChildElement("root").ToElement();
    if (!xml_root)
    {
      error = "Node <root> not found";
      return NULL;
    }

    const XMLElement* xml_shell = XMLHandle(&doc).FirstChildElement("root").FirstChildElement("shell").ToElement();
    if (!xml_shell)
    {
      error = "Node <shell> not found";
      return NULL;
    }

    ConfigFile* config = new ConfigFile();
    config->SetFilePath(path);
    config->SetFileModifiedDate(file_modified_date);

    //find <default> nodes under <shell>
    const XMLElement* xml_defaults = xml_shell->FirstChildElement("default");
    while (xml_defaults)
    {
      //found a new defaults node
      DefaultSettings* defaults = ObjectFactory::GetInstance().ParseDefaults(xml_defaults, error);
      if (defaults != NULL)
      {
        //add the new menu to the current configuration
        config->SetDefaultSettings(defaults);
      }

      //next defaults node
      xml_defaults = xml_defaults->NextSiblingElement("default");
    }

    //find <plugins> nodes under <root>
    const XMLElement* xml_plugins = xml_root->FirstChildElement("plugins");
    while (xml_plugins)
    {
      //find <plugin> nodes under <plugins>
      const XMLElement* xml_plugin = xml_plugins->FirstChildElement("plugin");
      while (xml_plugin)
      {
        //found a new plugin node
        Plugin* plugin = ObjectFactory::GetInstance().ParsePlugin(xml_plugin, error);
        if (plugin != NULL)
        {
          // try to load the plugin.
          bool loaded = plugin->Load();
          if (!loaded)
          {
            SA_LOG(WARNING) << "The plugin file '" << plugin->GetPath() << "' has failed to load, the plugin is disabled.";
          }

          //add the new plugin to the current configuration (even if loading failed)
          config->AddPlugin(plugin);
        }

        //next xml_plugin node
        xml_plugin = xml_plugin->NextSiblingElement("plugin");
      }

      //next plugins node
      xml_plugins = xml_plugins->NextSiblingElement("plugins");
    }

    //set active plugins for parsing child elements
    //notify the ObjectParser about this configuration's plugins.
    const Plugin::PluginPtrList& active_plugins = config->GetPlugins();
    ObjectFactory::GetInstance().SetActivePlugins(active_plugins);

    //find <menu> nodes under <shell>
    const XMLElement* xml_menu = xml_shell->FirstChildElement("menu");
    while (xml_menu)
    {
      //found a new menu node
      Menu* menu = ObjectFactory::GetInstance().ParseMenu(xml_menu, error);
      if (menu == NULL)
      {
        delete config;
        ObjectFactory::GetInstance().ClearActivePlugins();
        return NULL;
      }

      //add the new menu to the current configuration
      config->AddMenu(menu);

      //next menu node
      xml_menu = xml_menu->NextSiblingElement("menu");
    }

    //cleanup ObjectFactory plugins.
    ObjectFactory::GetInstance().ClearActivePlugins();

    return config;
  }

  bool ConfigFile::IsValidConfigFile(const std::string& path)
  {
    std::string file_extension = ra::filesystem::GetFileExtention(path);
    file_extension = ra::strings::Uppercase(file_extension);

    if (file_extension != "XML")
      return false;

    // Peek at the file for known xml elements
    std::string data;
    bool peeked = ra::filesystem::PeekFileUtf8(path, 1024 * 1024, data);
    if (!peeked)
      return false;

    // Search for element <root>
    size_t root_element_pos = data.find("<root>");
    if (root_element_pos == std::string::npos)
      return false;

    // Search for element <shell>
    size_t shell_element_pos = data.find("<shell>", root_element_pos);
    if (root_element_pos == std::string::npos)
      return false;

    return true;
  }

  const std::string& ConfigFile::GetFilePath() const
  {
    return mFilePath;
  }

  void ConfigFile::SetFilePath(const std::string& file_path)
  {
    mFilePath = file_path;
  }

  const uint64_t& ConfigFile::GetFileModifiedDate() const
  {
    return mFileModifiedDate;
  }

  void ConfigFile::SetFileModifiedDate(const uint64_t& file_modified_date)
  {
    mFileModifiedDate = file_modified_date;
  }

  void ConfigFile::Update(const SelectionContext& context)
  {
    SA_DECLARE_SCOPE_LOGGER_INFO(sli);
    sli.verbose = true;
    sli.instance = this;
    ScopeLogger logger(&sli);

    SetUpdatingConfigFile(this);

    //run callbacks of each plugins
    //for each plugins
    for (size_t i = 0; i < mPlugins.size(); i++)
    {
      Plugin* p = mPlugins[i];
      SA_VERBOSE_LOG(INFO) << "Running update callbacks of plugin '" << p->GetPath() << "'.";
      Registry& registry = p->GetRegistry();
      size_t count = registry.GetUpdateCallbackCount();
      for (size_t j = 0; j < count; j++)
      {
        SA_VERBOSE_LOG(INFO) << "Executing update callback " << (j + 1) << " of " << count << ".";
        IUpdateCallback* callback = registry.GetUpdateCallbackFromIndex(j);
        callback->SetSelectionContext(&context);
        callback->OnNewSelection();
        callback->SetSelectionContext(NULL);
      }
    }

    //for each child
    Menu::MenuPtrList children = GetMenus();
    for (size_t i = 0; i < children.size(); i++)
    {
      Menu* child = children[i];
      child->Update(context);
    }

    SetUpdatingConfigFile(NULL);
  }

  void ConfigFile::ApplyDefaultSettings()
  {
    if (mDefaults && mDefaults->GetActions().size() > 0)
    {
      SA_DECLARE_SCOPE_LOGGER_INFO(sli);
      sli.verbose = true;
      ScopeLogger logger(&sli);

      //configuration have default properties assigned
      SA_LOG(INFO) << "Initializing default properties of configuration file '" << mFilePath.c_str() << "' started.";

      const shellanything::IAction::ActionPtrList& actions = mDefaults->GetActions();

      //convert 'actions' to a list of <const shellanything::ActionProperty *>
      typedef std::vector<const ActionProperty*> ActionPropertyPtrList;
      ActionPropertyPtrList properties;
      for (size_t i = 0; i < actions.size(); i++)
      {
        const shellanything::IAction* abstract_action = actions[i];
        const shellanything::ActionProperty* action_property = dynamic_cast<const shellanything::ActionProperty*>(abstract_action);
        if (action_property)
          properties.push_back(action_property);
        else
        {
          SA_LOG(WARNING) << "Action " << (i + 1) << " of " << actions.size() << " is not a <property> action!";
        }
      }

      //apply all ActionProperty
      SelectionContext empty_context;
      for (size_t i = 0; i < properties.size(); i++)
      {
        SA_LOG(INFO) << "Initializing property action " << (i + 1) << " of " << properties.size() << ".";
        const shellanything::ActionProperty* action_property = properties[i];
        if (action_property)
        {
          //no need to look for failures. ActionProperty never fails.
          action_property->Execute(empty_context);
        }
      }

      SA_LOG(INFO) << "Initialization of default properties of configuration file '" << mFilePath.c_str() << "' completed.";
    }
  }

  Menu* ConfigFile::FindMenuByCommandId(const uint32_t& command_id)
  {
    //for each child
    Menu::MenuPtrList children = GetMenus();
    for (size_t i = 0; i < children.size(); i++)
    {
      Menu* child = children[i];
      Menu* match = child->FindMenuByCommandId(command_id);
      if (match)
        return match;
    }

    return NULL;
  }

  Menu* ConfigFile::FindMenuByName(const std::string& name, FIND_BY_NAME_FLAGS flags)
  {
    //for each child
    Menu::MenuPtrList children = GetMenus();
    for (size_t i = 0; i < children.size(); i++)
    {
      Menu* child = children[i];
      Menu* match = child->FindMenuByName(name, flags);
      if (match)
        return match;
    }

    return NULL;
  }

  uint32_t ConfigFile::AssignCommandIds(const uint32_t& first_command_id)
  {
    uint32_t nextCommandId = first_command_id;

    //for each child
    Menu::MenuPtrList children = GetMenus();
    for (size_t i = 0; i < children.size(); i++)
    {
      Menu* child = children[i];
      nextCommandId = child->AssignCommandIds(nextCommandId);
    }

    return nextCommandId;
  }

  void ConfigFile::AddPlugin(Plugin* plugin)
  {
    mPlugins.push_back(plugin);
    plugin->SetParentConfigFile(this);
  }

  const Plugin::PluginPtrList& ConfigFile::GetPlugins() const
  {
    return mPlugins;
  }

  Menu::MenuPtrList ConfigFile::GetMenus()
  {
    return mMenus;
  }

  void ConfigFile::SetDefaultSettings(DefaultSettings* defaults)
  {
    if (mDefaults)
      delete mDefaults;

    mDefaults = defaults;
  }

  const DefaultSettings* ConfigFile::GetDefaultSettings() const
  {
    return mDefaults;
  }

  void ConfigFile::AddMenu(Menu* menu)
  {
    mMenus.push_back(menu);
    menu->SetParentConfigFile(this);
  }

  std::string ConfigFile::ToShortString() const
  {
    std::string str;
    str += "ConfigFile ";
    str += ToHexString(this);
    str += ", ";
    str += this->mFilePath;
    if (mMenus.size())
    {
      str += ", ";
      IObject::AppendObjectCount(str, "menu", mMenus.size());
    }
    return str;
  }

  void ConfigFile::ToLongString(std::string& str, int indent) const
  {
    static const char* NEW_LINE = ra::environment::GetLineSeparator();
    const bool have_children = (mMenus.size() > 0);
    const std::string indent_str = std::string(indent, ' ');

    const std::string short_string = ToShortString();
    str += indent_str + short_string;
    if (have_children)
    {
      str += " {";
      str += NEW_LINE;

      // print children
      for (size_t i = 0; i < mMenus.size(); i++)
      {
        Menu* menu = mMenus[i];
        menu->ToLongString(str, indent + 2);

        str += NEW_LINE;
      }

      str += indent_str + "}";
    }
  }

  void ConfigFile::DeleteChildren()
  {
    // Delete menus
    for (size_t i = 0; i < mMenus.size(); i++)
    {
      Menu* sub = mMenus[i];
      delete sub;
    }
    mMenus.clear();

    // Delete plugins
    // Note that plugins must be deleted after everything else.
    // This is because some object in the configuration may require code from plugins.
    // If we delete a plugin, the module is unloaded which prevent object created by this
    // plugin to be properly deleted resulting in an exception.
    for (size_t i = 0; i < mPlugins.size(); i++)
    {
      Plugin* plugin = mPlugins[i];
      delete plugin;
    }
    mPlugins.clear();
  }

  void ConfigFile::DeleteChild(Menu* menu)
  {
    delete menu;
  }

} //namespace shellanything
