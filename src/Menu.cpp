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

#include "shellanything/Menu.h"
#include "win32_registry.h"

#include <glog/logging.h>

namespace shellanything
{
  const uint32_t Menu::INVALID_COMMAND_ID = 0;

  Menu::Menu() : Node("Menu"),
    mSeparator(false),
    mCommandId(INVALID_COMMAND_ID),
    mVisible(true),
    mEnabled(true)
  {
  }

  Menu::~Menu()
  {
    for(size_t i=0; i<mActions.size(); i++)
    {
      Action * action = mActions[i];
      delete action;
    }
    mActions.clear();
  }

  bool Menu::isSeparator() const
  {
    return mSeparator;
  }

  void Menu::setSeparator(bool iSeparator)
  {
    mSeparator = iSeparator;
  }

  bool Menu::isParentMenu() const
  {
    Menu::MenuPtrList sub_menus = filterNodes<Menu*>(this->findChildren("Menu"));
    bool parent_menu = (sub_menus.size() != 0);
    return parent_menu;
  }

  const std::string & Menu::getName() const
  {
    return mName;
  }

  void Menu::setName(const std::string & iName)
  {
    mName = iName;
  }

  const std::string & Menu::getDescription() const
  {
    return mDescription;
  }

  void Menu::setDescription(const std::string & iDescription)
  {
    mDescription = iDescription;
  }

  const Icon & Menu::getIcon() const
  {
    return mIcon;
  }

  void Menu::setIcon(const Icon & icon)
  {
    mIcon = icon;
  }

  void Menu::update(const Context & c)
  {
    //update current menu
    bool visible = mVisibility.validate(c);
    bool enabled = mValidity.validate(c);
    setVisible(visible);
    setEnabled(enabled);

    //update children
    bool all_invisible_children = true;

    //for each child
    Menu::MenuPtrList children = getSubMenus();
    for(size_t i=0; i<children.size(); i++)
    {
      Menu * child = children[i];
      child->update(c);

      //refresh the flag
      all_invisible_children = all_invisible_children && !child->isVisible();
    }

    //Issue #4 - Parent menu with no children.
    //if all the direct children of this menu are invisible
    if (isParentMenu() && visible && all_invisible_children)
    {
      //force this node as invisible.
      setVisible(false);
    }
  }

  void Menu::resolveFileExtensionIcons()
  {
    //resolve current menu
    const std::string & file_extension = mIcon.getFileExtension();
    if (!file_extension.empty())
    {
      //try to find the path to the icon module for the given file extension.
      win32_registry::REGISTRY_ICON resolved_icon = win32_registry::getFileTypeIcon(file_extension.c_str());
      if (!resolved_icon.path.empty() && resolved_icon.index != win32_registry::INVALID_ICON_INDEX)
      {
        //found the icon for the file extension
        //replace this menu's icon with the new information
        LOG(INFO) << "Resolving icon for file extension '" << file_extension << "' to file '" << resolved_icon.path << "' with index '" << resolved_icon.index << "'";
        mIcon.setPath(resolved_icon.path);
        mIcon.setIndex(resolved_icon.index);
        mIcon.setFileExtension("");
      }
      else
      {
        //failed to find a valid icon.
        //using the default "unknown" icon
        win32_registry::REGISTRY_ICON unknown_file_icon = win32_registry::getUnknownFileTypeIcon();
        LOG(WARNING) << "Failed to find icon for file extension '" << file_extension << "'. Resolving icon with default icon for unknown file type '" << unknown_file_icon.path << "' with index '" << unknown_file_icon.index << "'";
        mIcon.setPath(unknown_file_icon.path);
        mIcon.setIndex(unknown_file_icon.index);
        mIcon.setFileExtension("");
      }
    }

    //for each child
    Menu::MenuPtrList children = getSubMenus();
    for(size_t i=0; i<children.size(); i++)
    {
      Menu * child = children[i];
      child->resolveFileExtensionIcons();
    }
  }

  Menu * Menu::findMenuByCommandId(const uint32_t & iCommandId)
  {
    if (mCommandId == iCommandId)
      return this;
 
    //for each child
    Menu::MenuPtrList children = getSubMenus();
    for(size_t i=0; i<children.size(); i++)
    {
      Menu * child = children[i];
      Menu * match = child->findMenuByCommandId(iCommandId);
      if (match)
        return match;
    }
 
    return NULL;
  }
 
  uint32_t Menu::assignCommandIds(const uint32_t & iFirstCommandId)
  {
    uint32_t nextCommandId = iFirstCommandId;

    //Issue #5 - ConfigManager::assignCommandIds() should skip invisible menus
    if (!mVisible || iFirstCommandId == INVALID_COMMAND_ID)
    {
      this->setCommandId(INVALID_COMMAND_ID); //invalidate this menu's command id
    }
    else
    {
      //assign a command id to this menu
      this->setCommandId(nextCommandId);
      nextCommandId++;
    }

    //for each child
    Menu::MenuPtrList children = getSubMenus();
    for(size_t i=0; i<children.size(); i++)
    {
      Menu * child = children[i];

      if (mCommandId == INVALID_COMMAND_ID)
        child->assignCommandIds(INVALID_COMMAND_ID); //also assign invalid ids to sub menus
      else
        nextCommandId = child->assignCommandIds(nextCommandId); //assign the next command ids to sub menus
    }
 
    return nextCommandId;
  }
 
  const uint32_t & Menu::getCommandId() const
  {
    return mCommandId;
  }
 
  void Menu::setCommandId(const uint32_t & iCommandId)
  {
    mCommandId = iCommandId;
  }
 
  bool Menu::isVisible() const
  {
    return mVisible;
  }

  bool Menu::isEnabled() const
  {
    return mEnabled;
  }

  void Menu::setVisible(bool visible)
  {
    mVisible = visible;
  }

  void Menu::setEnabled(bool enabled)
  {
    mEnabled = enabled;
  }

  const Validator & Menu::getValidity()
  {
    return mValidity;
  }

  void Menu::setValidity(const Validator & iValidity)
  {
    mValidity = iValidity;
  }

  const Validator & Menu::getVisibility()
  {
    return mVisibility;
  }

  void Menu::setVisibility(const Validator & iVisibility)
  {
    mVisibility = iVisibility;
  }

  Menu::MenuPtrList Menu::getSubMenus()
  {
    Menu::MenuPtrList sub_menus = filterNodes<Menu*>(this->findChildren("Menu"));
    return sub_menus;
  }

  void Menu::addAction(Action * action)
  {
    mActions.push_back(action);
  }

  const Action::ActionPtrList & Menu::getActions() const
  {
    return mActions;
  }

} //namespace shellanything
