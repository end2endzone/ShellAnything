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

#include "Registry.h"
#include <set>

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

namespace shellanything
{
  typedef std::set<IAttributeValidator* /*validator*/> AttributeValidatorSet;

  Registry::Registry()
  {
  }

  Registry::~Registry()
  {
    Clear();
  }

  void Registry::Clear()
  {
    // cleanup action factories
    for (ActionFactoryMap::iterator it = mActionFactories.begin(); it != mActionFactories.end(); ++it)
    {
      const std::string& key = (it->first);
      IActionFactory* factory = (it->second);
      delete factory;
    }
    mActionFactories.clear();

    // cleanup attribute validators
    // the same IAttributeValidator instance can be assigned to multiple names.
    // we must first get a list of unique instances.
    AttributeValidatorSet tmp;
    for (AttributeValidatorMap::iterator it = mAttributeValidators.begin(); it != mAttributeValidators.end(); ++it)
    {
      const std::string& key = (it->first);
      IAttributeValidator* validator = (it->second);
      tmp.insert(validator);
    }
    for (AttributeValidatorSet::iterator it = tmp.begin(); it != tmp.end(); ++it)
    {
      IAttributeValidator* validator = (*it);
      delete validator;
    }
    mAttributeValidators.clear();

  }

  IActionFactory* Registry::GetActionFactoryFromName(const std::string& name) const
  {
    ActionFactoryMap::const_iterator it = mActionFactories.find(name);
    bool found = (it != mActionFactories.end());
    if (found)
    {
      IActionFactory* factory = it->second;
      return factory;
    }
    return NULL;
  }

  void Registry::AddActionFactory(IActionFactory* factory)
  {
    const std::string& name = factory->GetName();
    if (GetActionFactoryFromName(name))
    {
      LOG(WARNING) << "An action factory already exists for the action named '" << name << "', this factory will be ignored.";
    }
    else
      mActionFactories[name] = factory;
  }

  IAttributeValidator* Registry::GetAttributeValidatorFromName(const std::string& name) const
  {
    AttributeValidatorMap::const_iterator it = mAttributeValidators.find(name);
    bool found = (it != mAttributeValidators.end());
    if (found)
    {
      IAttributeValidator* validator = it->second;
      return validator;
    }
    return NULL;
  }

  void Registry::AddAttributeValidator(IAttributeValidator* validator)
  {
    // register the attribute validator to each attributes names
    const StringList& names = validator->GetAttributeNames();
    for (size_t i = 0; i < names.size(); i++)
    {
      const std::string& name = names[i];
      if (GetAttributeValidatorFromName(name))
      {
        LOG(WARNING) << "An attribute validator already exists for the attribute '" << name << "', this validator will be ignored.";
      }
      else
        mAttributeValidators[name] = validator;
    }
  }

  size_t Registry::GetUpdateCallbackCount() const
  {
    return mUpdateCallbacks.size();
  }

  IUpdateCallback* Registry::GetUpdateCallbackFromIndex(size_t index) const
  {
    if (index >= mUpdateCallbacks.size())
      return NULL;
    IUpdateCallback* callback = mUpdateCallbacks[index];
    return callback;
  }

  void Registry::AddUpdateCallback(IUpdateCallback* callback)
  {
    mUpdateCallbacks.push_back(callback);
  }

} //namespace shellanything
