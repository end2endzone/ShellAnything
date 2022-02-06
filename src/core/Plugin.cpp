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

#include "Plugin.h"

namespace shellanything
{
  Plugin::Plugin()
  {
  }

  Plugin::Plugin(const Plugin & p)
  {
    (*this) = p;
  }

  Plugin::~Plugin()
  {
  }

  const Plugin & Plugin::operator =(const Plugin & p)
  {
    if (this != &p)
    {
      mPath         = p.mPath;
      mDescription  = p.mDescription;
      mConditions   = p.mConditions;
      mActions      = p.mActions;
    }
    return (*this);
  }

  const std::string& Plugin::GetPath() const
  {
    return mPath;
  }

  void Plugin::SetPath(const std::string& path)
  {
    mPath = path;
  }

  const std::string& Plugin::GetDescription() const
  {
    return mDescription;
  }

  void Plugin::SetDescription(const std::string& description)
  {
    mDescription = description;
  }

  const std::string& Plugin::GetConditions() const
  {
    return mConditions;
  }

  void Plugin::SetConditions(const std::string& conditions)
  {
    mConditions = conditions;
  }

  const std::string& Plugin::GetActions() const
  {
    return mActions;
  }

  void Plugin::SetActions(const std::string& actions)
  {
    mActions = actions;
  }

} //namespace shellanything