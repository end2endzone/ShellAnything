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

#include "shellanything/ActionOpen.h"
#include "shellanything/PropertyManager.h"
#include "shellanything/Platform.h"

namespace shellanything
{

  ActionOpen::ActionOpen()
  {
  }

  ActionOpen::~ActionOpen()
  {
  }

  bool ActionOpen::execute(const Context & iContext) const
  {
    PropertyManager & pmgr = PropertyManager::getInstance();
    std::string path = pmgr.expand(mPath);

    //debug
    //printf("Open file '%s'.\n", path.c_str());

    uint32_t pId = openDocument(path);

    return pId != 0;
  }

  const std::string & ActionOpen::getPath() const
  {
    return mPath;
  }

  void ActionOpen::setPath(const std::string & iPath)
  {
    mPath = iPath;
  }

} //namespace shellanything
