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

#include "shellanything/ActionProperty.h"
#include "PropertyManager.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

namespace shellanything
{

  ActionProperty::ActionProperty()
  {
  }

  ActionProperty::~ActionProperty()
  {
  }

  bool ActionProperty::Execute(const Context & iContext) const
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
    std::string name = pmgr.expand(mName);
    std::string value = pmgr.expand(mValue);

    //debug
    LOG(INFO) << "Setting property '" << name << "' to value '" << value << "'.";
    pmgr.setProperty(name, value);

    return true;
  }

  const std::string & ActionProperty::GetName() const
  {
    return mName;
  }

  void ActionProperty::SetName(const std::string & iName)
  {
    mName = iName;
  }

  const std::string & ActionProperty::GetValue() const
  {
    return mValue;
  }

  void ActionProperty::SetValue(const std::string & iValue)
  {
    mValue = iValue;
  }

} //namespace shellanything
