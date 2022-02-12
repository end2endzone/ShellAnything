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

namespace shellanything
{

  Registry::Registry()
  {
  }

  Registry::~Registry()
  {
    // cleanup action factories
    for (ActionFactoryMap::iterator it = mActionFactories.begin(); it != mActionFactories.end(); ++it)
    {
      const std::string& key = (it->first);
      IActionFactory* factory = (it->second);
      delete factory;
    }
    mActionFactories.clear();

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
    mActionFactories[name] = factory;
  }

} //namespace shellanything
