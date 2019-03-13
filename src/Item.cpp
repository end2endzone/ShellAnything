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
  Item::Item()
  {
  }

  Item::Item(const std::string & name) : Node(name)
  {
  }

  Item::~Item()
  {
    //delete actions
    for(size_t i=0; i<mActions.size(); i++) 
    {
      Action * action = mActions[i];
      if (action)
        delete action;
    }
    mActions.clear();
  }

  bool Item::isSeparator() const
  {
    return mSeparator;
  }

  void Item::setSeparator(bool iSeparator)
  {
    mSeparator = iSeparator;
  }

  const std::string & Item::getName() const
  {
    return mName;
  }

  void Item::setName(const std::string & iName)
  {
    mName = iName;
  }

  const Validator & Item::getValidity() const
  {
    return mValidity;
  }

  void Item::setValidity(const Validator & iValidity)
  {
    mValidity = iValidity;
  }

  const Validator & Item::getVisibility() const
  {
    return mVisibility;
  }

  void Item::setVisibility(const Validator & iVisibility)
  {
    mVisibility = iVisibility;
  }

  const Action::ActionPtrList & Item::getActionsList() const
  {
    return mActions;
  }

  void Item::addAction(Action * iAction)
  {
    mActions.push_back(iAction);
  }

  Item::ItemPtrList Item::getChildrenItems() const
  {
    Item::ItemPtrList items;
    NodePtrList nodes = getChildren();
    for(size_t i=0; i<nodes.size(); i++)
    {
      Node * node = nodes[i];
      Item * item = dynamic_cast<Item*>(node);
      if (item)
        items.push_back(item);
    }
    return items;
  }

  Item * Item::getChildItem(size_t index) const
  {
    Node * node = this->getChild(index);
    if (!node)
      return NULL;

    Item * item = dynamic_cast<Item*>(node);
    return item;
  }

} //namespace shellanything
