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

#include "shellanything/ActionPrompt.h"
#include "PropertyManager.h"
#include "InputBox.h"

namespace shellanything
{

  ActionPrompt::ActionPrompt()
  {
  }

  ActionPrompt::~ActionPrompt()
  {
  }

  bool ActionPrompt::execute(const Context & iContext) const
  {
    PropertyManager & pmgr = PropertyManager::getInstance();
    std::string name = pmgr.expand(mName);
    std::string title = pmgr.expand(mTitle);

    //debug
    //std::string value = "bar";
    //printf("%s ? %s\n", title.c_str(), value.c_str());

    CInputBox box(NULL);
    bool result = box.DoModal("Question / Prompt", title);
    if (!result)
      return false;

    const std::string & value = box.getText();

    //printf("Setting property '%s' to '%s'\n", name.c_str(), value.c_str());
    pmgr.setProperty(name, value);

    return true;
  }

  const std::string & ActionPrompt::getName() const
  {
    return mName;
  }

  void ActionPrompt::setName(const std::string & iName)
  {
    mName = iName;
  }

  const std::string & ActionPrompt::getTitle() const
  {
    return mTitle;
  }

  void ActionPrompt::setTitle(const std::string & iTitle)
  {
    mTitle = iTitle;
  }

} //namespace shellanything
