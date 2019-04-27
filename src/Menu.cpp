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

namespace shellanything
{
  Menu::Menu() : Node("Menu"),
    mSeparator(false),
    mCommandId(0)
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
    this->setCommandId(nextCommandId);
    nextCommandId++;
 
    //for each child
    Menu::MenuPtrList children = getSubMenus();
    for(size_t i=0; i<children.size(); i++)
    {
      Menu * child = children[i];
      nextCommandId = child->assignCommandIds(nextCommandId);
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
 
  bool Menu::isVisible(const Context & c)
  {
    bool validated = mVisibility.validate(c);
    return validated;
  }

  bool Menu::isEnabled(const Context & c)
  {
    bool validated = mValidity.validate(c);
    return validated;
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