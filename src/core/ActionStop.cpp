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
#include "ObjectFactory.h"

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

#include "tinyxml2.h"
using namespace tinyxml2;

namespace shellanything
{
  const std::string ActionStop::XML_ELEMENT_NAME = "stop";

  class ActionStopFactory : public virtual IActionFactory
  {
  public:
    ActionStopFactory()
    {
    }
    virtual ~ActionStopFactory()
    {
    }

    virtual const std::string& GetName() const
    {
      return ActionStop::XML_ELEMENT_NAME;
    }

    virtual IAction* ParseFromXml(const std::string& xml, std::string& error) const
    {
      tinyxml2::XMLDocument doc;
      XMLError result = doc.Parse(xml.c_str());
      if (result != XML_SUCCESS)
      {
        if (doc.ErrorStr())
        {
          error = doc.ErrorStr();
          return NULL;
        }
        else
        {
          error = "Unknown error reported by XML library.";
          return NULL;
        }
      }
      XMLElement* element = doc.FirstChildElement(GetName().c_str());

      ActionStop* action = new ActionStop();
      std::string tmp_str;

      //parse like a Validator
      Validator* validator = ObjectFactory::GetInstance().ParseValidator(element, error);
      if (validator == NULL)
      {
        delete action;
        return NULL;
      }

      action->SetValidator(validator);

      //done parsing
      return action;
    }

  };

  IActionFactory* ActionStop::NewFactory()
  {
    return new ActionStopFactory();
  }

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

  bool ActionStop::Execute(const SelectionContext& context) const
  {
    PropertyManager& pmgr = PropertyManager::GetInstance();

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
