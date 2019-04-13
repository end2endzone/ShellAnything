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
#include "shellanything/Icon.h"
#include "shellanything/Validator.h"
#include "shellanything/Action.h"
#include <string>
#include <vector>
#include <stdint.h>

namespace shellanything
{

  class Item : public Node
  {
  public:
    typedef std::vector<Item*> ItemPtrList;

    Item();
    virtual ~Item();

    bool isSeparator() const;
    void setSeparator(bool iSeparator);

    bool isParentItem() const;

    const std::string & getName() const;
    void setName(const std::string & iName);

    const Icon & getIcon() const;
    void setIcon(const Icon & iIcon);

    Item * findItemByCommandId(const uint32_t & iCommandId);
    uint32_t assignCommandIds(const uint32_t & iFirstCommandId);

    const uint32_t & getCommandId() const;
    void setCommandId(const uint32_t & iCommandId);

    bool isVisible(const Context & c);
    bool isEnabled(const Context & c);

    const Validator & getValidity();
    void setValidity(const Validator & iValidity);

    const Validator & getVisibility();
    void setVisibility(const Validator & iVisibility);

    void addAction(Action * action);
    const Action::ActionPtrList & getActions() const;

    ItemPtrList getSubItems();

  private:
    Icon mIcon;
    Validator mValidity;
    Validator mVisibility;
    bool mSeparator;
    uint32_t mCommandId;
    std::string mName;
    Action::ActionPtrList mActions;
  };

} //namespace shellanything

#endif //SA_ITEM_H
