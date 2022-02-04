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

#include "ActionStop.h"
#include "PropertyManager.h"
#include "rapidassist/strings.h"
#include "rapidassist/unicode.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

namespace shellanything
{

  ActionStop::ActionStop() :
    mValidator(NULL)
  {
  }

  ActionStop::~ActionStop()
  {
    if (mValidator)
      delete mValidator;
    mValidator = NULL;
  }

  bool ActionStop::Execute(const Context & context) const
  {
    PropertyManager & pmgr = PropertyManager::GetInstance();

    if (!mValidator)
      return true;

    bool validated = mValidator->Validate(context);

    //update the property
    if (validated)
      LOG(INFO) << "ActionStop: Validation is successful.";
    else
      LOG(INFO) << "ActionStop: Validation has failed.";

    return validated;
  }

  Validator* ActionStop::GetValidator()
  {
    return mValidator;
  }

  void ActionStop::SetValidator(Validator* validator)
  {
    mValidator = validator;
  }

} //namespace shellanything
