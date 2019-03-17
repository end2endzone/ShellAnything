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

#include "shellanything/Item.h"

namespace shellanything
{
  Item::Item() : Node("Item"),
    mSeparator(false)
  {
  }

  Item::~Item()
  {
  }

  bool Item::isSeparator() const
  {
    return mSeparator;
  }

  void Item::setSeparator(bool iSeparator)
  {
    mSeparator = iSeparator;
  }

  bool Item::isParent() const
  {
    Item::ItemPtrList sub_items = filterNodes<Item*>(this->findChildren("Item"));
    bool parent_item = (sub_items.size() != 0);
    return parent_item;
  }

  const std::string & Item::getName() const
  {
    return mName;
  }

  void Item::setName(const std::string & iName)
  {
    mName = iName;
  }

  Icon * Item::getIcon()
  {
    Node * node = this->findFirst("Icon");
    Icon * icon = dynamic_cast<Icon *>(node);
    return icon;
  }

  void Item::setIcon(Icon * iIcon)
  {
    Icon * previous_icon = getIcon();
    if (previous_icon)
    {
      this->removeChild(previous_icon);
      delete previous_icon;
    }
    this->addChild(iIcon);
  }

  Validator * Item::getValidity()
  {
    Node * node = this->findFirst("Validity");
    Validator * validator = dynamic_cast<Validator *>(node);
    return validator;
  }

  void Item::setValidity(Validator * iValidity)
  {
    Validator * previous_validator = getValidity();
    if (previous_validator)
    {
      this->removeChild(previous_validator);
      delete previous_validator;
    }
    this->addChild(iValidity);
  }

  Validator * Item::getVisibility()
  {
    Node * node = this->findFirst("Visibility");
    Validator * validator = dynamic_cast<Validator *>(node);
    return validator;
  }

  void Item::setVisibility(Validator * iVisibility)
  {
    Validator * previous_validator = getVisibility();
    if (previous_validator)
    {
      this->removeChild(previous_validator);
      delete previous_validator;
    }
    this->addChild(iVisibility);
  }

  Item::ItemPtrList Item::getSubItems()
  {
    Item::ItemPtrList sub_items = filterNodes<Item*>(this->findChildren("Item"));
    return sub_items;
  }

  Action::ActionPtrList Item::getActions()
  {
    //cannot filter using node_type because each action classes have a different type 
    Action::ActionPtrList actions = filterNodes<Action*>(this->getChildren());
    return actions;
  }

} //namespace shellanything
