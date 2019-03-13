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

#ifndef SA_ITEM_H
#define SA_ITEM_H

#include "shellanything/Node.h"
#include "shellanything/Validator.h"
#include "shellanything/Action.h"
#include <string>
#include <vector>

namespace shellanything
{

  class Item : public Node
  {
  public:
    typedef std::vector<Item*> ItemPtrList;

    Item();
    Item(const std::string & name);
    virtual ~Item();

    bool isSeparator() const;
    void setSeparator(bool iSeparator);

    const std::string & getName() const;
    void setName(const std::string & iName);

    const Validator & getValidity() const;
    void setValidity(const Validator & iValidity);

    const Validator & getVisibility() const;
    void setVisibility(const Validator & iVisibility);

    const Action::ActionPtrList & getActionsList() const;
    void addAction(Action * iAction);

    ItemPtrList getChildrenItems() const;
    Item * getChildItem(size_t index) const;

  private:
    bool mSeparator;
    std::string mName;
    Validator mValidity;
    Validator mVisibility;
    Action::ActionPtrList mActions;
  };

} //namespace shellanything

#endif //SA_ITEM_H
