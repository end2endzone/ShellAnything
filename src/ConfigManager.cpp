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
#include "shellanything/NodeFactory.h"
#include "shellanything/Item.h"
#include "shellanything/Platform.h"
#include "shellanything/PropertyManager.h"

#include "rapidassist/filesystem.h"
#include "rapidassist/strings.h"
#include "rapidassist/logger.h"
#include "rapidassist/environment.h"
#include "tinyxml2.h"

using namespace tinyxml2;

namespace shellanything
{

  std::string getFilenameWithoutExtension(const char * iPath)
  {
    if (iPath == NULL)
      return "";

    std::string filename = ra::filesystem::getFilename(iPath);
    std::string extension = ra::filesystem::getFileExtention(iPath);
  
    //extract filename without extension
    std::string filenameWE = filename.substr(0, filename.size() - extension.size());
  
    //remove last dot of the filename if required
    filenameWE = ra::strings::trimRight(filenameWE, '.');

    return filenameWE;
  }



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
      Node * node = NodeFactory::getInstance().parseNode(xml_item, error);
      Item * item = dynamic_cast<Item*>(node);
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
      const uint64_t & file_date = config->getFileModifiedDate();
      if (ra::filesystem::fileExists(file_path.c_str()) && ra::filesystem::getFileModifiedDate(file_path) == file_date)
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

  void ConfigManager::registerProperties(const Context & c) const
  {
    PropertyManager & pmgr = PropertyManager::getInstance();
   
    const Context::ElementList & elements = c.getElements();
 
    if (elements.empty())
      return; //nothing to register
 
    std::string selection_path           ;
    std::string selection_parent_path    ;
    std::string selection_parent_filename;
    std::string selection_filename       ;
    std::string selection_filename_noext ;
 
    //for each element
    for(size_t i=0; i<elements.size(); i++)
    {
      const std::string & element = elements[i];
 
      //${selection.path} is the full path of the clicked element
      //${selection.parent.path} is the full path of the parent element
      //${selection.parent.filename} is the filename of the parent element
      //${selection.filename} is selection.filename (including file extension)
      //${selection.filename_noext} is selection.filename without file extension
 
      //build properties for this specific element
      std::string element_selection_path            = element;
      std::string element_selection_parent_path     = ra::filesystem::getParentPath(element_selection_path);
      std::string element_selection_parent_filename = ra::filesystem::getFilename(element_selection_parent_path.c_str());
      std::string element_selection_filename        = ra::filesystem::getFilename(element_selection_path.c_str());
      std::string element_selection_filename_noext  = getFilenameWithoutExtension(element_selection_path.c_str());
 
      //append this specific element properties to the global property string
 
      //add a newline if the property value is not empty. This allows printing all file path on individual lines
      static const char * line_separator = ra::environment::getLineSeparator();
      if (!selection_path           .empty()) selection_path           .append( line_separator );
      if (!selection_parent_path    .empty()) selection_parent_path    .append( line_separator );
      if (!selection_parent_filename.empty()) selection_parent_filename.append( line_separator );
      if (!selection_filename       .empty()) selection_filename       .append( line_separator );
      if (!selection_filename_noext .empty()) selection_filename_noext .append( line_separator );
 
      selection_path           .append( element_selection_path            );
      selection_parent_path    .append( element_selection_parent_path     );
      selection_parent_filename.append( element_selection_parent_filename );
      selection_filename       .append( element_selection_filename        );
      selection_filename_noext .append( element_selection_filename_noext  );
    }
 
    pmgr.setProperty("selection.path"           , selection_path           );
    pmgr.setProperty("selection.parent.path"    , selection_parent_path    );
    pmgr.setProperty("selection.parent.filename", selection_parent_filename);
    pmgr.setProperty("selection.filename"       , selection_filename       );
    pmgr.setProperty("selection.filename.noext" , selection_filename_noext );
  }
 
  void ConfigManager::unregisterProperties(const Context & c) const
  {
    PropertyManager & pmgr = PropertyManager::getInstance();
    pmgr.clearProperty("selection.path"           );
    pmgr.clearProperty("selection.parent.path"    );
    pmgr.clearProperty("selection.parent.filename");
    pmgr.clearProperty("selection.filename"       );
    pmgr.clearProperty("selection.filename.noext" );
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
