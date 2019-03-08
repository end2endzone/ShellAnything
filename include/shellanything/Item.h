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

#ifndef SA_CONTEXT_H
#define SA_CONTEXT_H

#include "shellanything/Validation.h"
#include "shellanything/Action.h"
#include <string>
#include <vector>

namespace shellanything
{

  class Item;
  typedef std::vector<Item*> ItemPtrList;

  class Item
  {
  public:
    Item();
    virtual ~Item();

    bool isSeparator() const;
    void setSeparator(bool iSeparator);

    const std::string & getName() const;
    void setName(const std::string & iName);

    const Validation & getValidity() const;
    void setValidity(const Validation & iValidity);

    const Validation & getVisibility() const;
    void setVisibility(const Validation & iVisibility);

    const ActionPtrList & getActionPtrList() const;
    void setActionPtrList(const ActionPtrList & iActionPtrList);

    const ItemPtrList & getItemPtrList() const;
    void setItemPtrList(const ItemPtrList & iItemPtrList);

  private:
    bool mSeparator;
    std::string mName;
    Validation mValidity;
    Validation mVisibility;
    ActionPtrList mActionPtrList;
    ItemPtrList mItemPtrList;
  };

  typedef std::vector<Item> ItemList;


} //namespace shellanything

#endif //SA_CONTEXT_H
