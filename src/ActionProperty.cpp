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
#include "libEval.h"

#include "rapidassist/strings.h"

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

  bool ActionProperty::Execute(const Context & context) const
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();
    std::string name = pmgr.Expand(mName).c_str();
    std::string value = pmgr.Expand(mValue).c_str();
    std::string exprtk = pmgr.Expand(mExprtk).c_str();

    // If exprtk is specified, it has priority over value. This is required to allow setting a property to an empty value (a.k.a. value="").
    if (!exprtk.empty())
    {
      static const size_t ERROR_SIZE = 10480;
      char error[ERROR_SIZE];
      error[0] = '\0';

      double result = 0.0;
      bool evaluated = Evaluate(exprtk.c_str(), &result, error, ERROR_SIZE);
      if (!evaluated)
      {
        LOG(WARNING) << "Failed evaluating exprtk expression '" << exprtk << "'.";
        LOG(WARNING) << "Exprtk error: " << error << "'.";
        return false;
      }

      // Store the result in 'value' as if user set this specific value (to use the same process as a property that sets a value).
      value = ra::strings::ToString(result);
    }

    //debug
    LOG(INFO) << "Setting property '" << name << "' to value '" << value << "'.";

    pmgr.SetProperty(name.c_str(), value.c_str());

    // If a user has changed property 'selection.multi.separator', the context must rebuild selection-based properties.
    // See issue #52 for details.
    if (Context::MULTI_SELECTION_SEPARATOR_PROPERTY_NAME == name.c_str())
    {
      // Force the context to rebuild selection.* properties.
      context.RegisterProperties();
    }

    return true;
  }

  const String & ActionProperty::GetName() const
  {
    return mName;
  }

  void ActionProperty::SetName(const String & name)
  {
    mName = name;
  }

  const String & ActionProperty::GetValue() const
  {
    return mValue;
  }

  void ActionProperty::SetValue(const String & value)
  {
    mValue = value;
  }

  const String & ActionProperty::GetExprtk() const
  {
    return mExprtk;
  }

  void ActionProperty::SetExprtk(const String & exprtk)
  {
    mExprtk = exprtk;
  }

} //namespace shellanything
