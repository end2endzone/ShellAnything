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

#include "shellanything/ActionExecute.h"
#include "shellanything/PropertyManager.h"

namespace shellanything
{

  ActionExecute::ActionExecute()
  {
  }

  ActionExecute::~ActionExecute()
  {
  }

  bool ActionExecute::execute(const Context & iContext) const
  {
    PropertyManager & pmgr = PropertyManager::getInstance();
    std::string path = pmgr.expand(mPath);
    std::string basedir = pmgr.expand(mBaseDir);
    std::string arguments = pmgr.expand(mArguments);

    //debug
    printf("Running '%s' from directory '%s' with arguments '%s'.\n", path.c_str(), basedir.c_str(), arguments.c_str());

    return true;
  }

  const std::string & ActionExecute::getPath() const
  {
    return mPath;
  }

  void ActionExecute::setPath(const std::string & iPath)
  {
    mPath = iPath;
  }

  const std::string & ActionExecute::getBaseDir() const
  {
    return mBaseDir;
  }

  void ActionExecute::setBaseDir(const std::string & iBaseDir)
  {
    mBaseDir = iBaseDir;
  }

  const std::string & ActionExecute::getArguments() const
  {
    return mArguments;
  }

  void ActionExecute::setArguments(const std::string & iArguments)
  {
    mArguments = iArguments;
  }

} //namespace shellanything
