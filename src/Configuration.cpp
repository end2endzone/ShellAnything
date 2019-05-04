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
#include "rapidassist/filesystem.h"
#include "Platform.h"

namespace shellanything
{

  Configuration::Configuration() : Node("Configuration"),
    mFileModifiedDate(0)
  {
  }

  Configuration::~Configuration()
  {
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

} //namespace shellanything
