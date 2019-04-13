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

#include "shellanything/ConfigManager.h"
#include "shellanything/ObjectFactory.h"
#include "shellanything/Item.h"
#include "shellanything/Platform.h"

#include "rapidassist/filesystem.h"
#include "rapidassist/strings.h"
#include "rapidassist/logger.h"
#include "tinyxml2.h"

using namespace tinyxml2;

namespace shellanything
{

  ConfigManager::ConfigManager()
  {
  }

  ConfigManager::~ConfigManager()
  {
  }

  ConfigManager & ConfigManager::getInstance()
  {
    static ConfigManager _instance;
    return _instance;
  }


  Configuration * ConfigManager::loadFile(const std::string & path, std::string & error)
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

    //find <item> nodes under <shell>
    const XMLElement* xml_item = xml_shell->FirstChildElement("item");
    while (xml_item)
    {
      //found a new item node
      Item * item = ObjectFactory::getInstance().parseItem(xml_item, error);
      if (item == NULL)
      {
        delete config;
        return NULL;
      }

      //add the new item to the current configuration
      config->addChild(item);

      //next item node
      xml_item = xml_item->NextSiblingElement("item");
    }

    return config;
  }

  void ConfigManager::refresh()
  {
    //validate existing configurations
    Configuration::ConfigurationPtrList existing = getConfigurations();
    for(size_t i=0; i<existing.size(); i++)
    {
      Configuration * config = existing[i];

      //compare the file's date at the load time and the current date
      const std::string & file_path = config->getFilePath();
      const uint64_t & old_file_date = config->getFileModifiedDate();
      const uint64_t new_file_date = ra::filesystem::getFileModifiedDate(file_path);
      if (ra::filesystem::fileExists(file_path.c_str()) && old_file_date == new_file_date)
      {
        //current configuration is up to date
      }
      else
      {
        //file is missing or current configuration is out of date
        //forget about existing config
        mConfigurations.removeChild(config);
      }
    }
   
    //search every known path
    for (size_t i=0; i<mPaths.size(); i++)
    {
      const std::string & path = mPaths[i];
 
      //search files in each directory
      ra::strings::StringVector files;
      bool dir_found = ra::filesystem::findFiles(files, path.c_str());
      if (dir_found)
      {
        //search through each files for *.xml files
        for(size_t j=0; j<files.size(); j++)
        {
          const std::string & file_path = files[j];
          if (isConfigurationFile(file_path))
          {
            //is this file already loaded ?
            if (!isLoadedConfigurationFile(file_path))
            {
              //parse the file
              std::string error;
              Configuration * config = loadFile(file_path, error);
              if (config == NULL)
              {
                //log an error message
                ra::logger::log(ra::logger::LOG_ERROR, "Failed loading configuration file '%s'. Error=%s", file_path.c_str(), error.c_str());
              }
              else
              {
                //add to current list of configurations
                mConfigurations.addChild(config);
              }
            }
          }
        }
      }
      else
      {
        //log an error message
        ra::logger::log(ra::logger::LOG_WARNING, "Failed looking for configuration files in directory '%s'.", path.c_str() );
      }
    }
  }

  Item * ConfigManager::findItemByCommandId(const uint32_t & iCommandId)
  {
    //for each child
    Configuration::ConfigurationPtrList configurations = ConfigManager::getConfigurations();
    for(size_t i=0; i<configurations.size(); i++)
    {
      Configuration * config = configurations[i];
      Item * match = config->findItemByCommandId(iCommandId);
      if (match)
        return match;
    }
 
    return NULL;
  }
 
  uint32_t ConfigManager::assignCommandIds(const uint32_t & iFirstCommandId)
  {
    uint32_t nextCommandId = iFirstCommandId;

    //for each child
    Configuration::ConfigurationPtrList configurations = ConfigManager::getConfigurations();
    for(size_t i=0; i<configurations.size(); i++)
    {
      Configuration * config = configurations[i];
      nextCommandId = config->assignCommandIds(nextCommandId);
    }
 
    return nextCommandId;
  }
 
  Configuration::ConfigurationPtrList ConfigManager::getConfigurations()
  {
    Configuration::ConfigurationPtrList configurations = filterNodes<Configuration*>(mConfigurations.findChildren("Configuration"));
    return configurations;
  }

  void ConfigManager::clearSearchPath()
  {
    mPaths.clear();
  }

  void ConfigManager::addSearchPath(const std::string & path)
  {
    mPaths.push_back(path);
  }

  bool ConfigManager::isConfigurationFile(const std::string & path)
  {
    std::string file_extension = ra::filesystem::getFileExtention(path);
    file_extension = ra::strings::uppercase(file_extension);
    if (file_extension == "XML")
    {
      //read the beginning of the file
      std::string content;
      bool readed = peekFile(path.c_str(), 512, content);
      if (readed)
      {
        //and look for special XML tags
        size_t rootPos = content.find("<root>", 0);
        size_t shellPos = content.find("<shell>", rootPos);
        size_t itemPos = content.find("<item", shellPos);
        if (rootPos != std::string::npos &&
            shellPos != std::string::npos &&
            itemPos != std::string::npos)
        {
          //found the required tags
          return true;
        }
      }
    }
    return false;
  }

  bool ConfigManager::isLoadedConfigurationFile(const std::string & path) const
  {
    for(size_t i=0; i<mConfigurations.size(); i++)
    {
      const Node * node = mConfigurations.getChild(i);
      const Configuration * config = dynamic_cast<const Configuration *>(node);
      if (config != NULL && config->getFilePath() == path)
        return true;
    }
    return false;
  }

} //namespace shellanything
