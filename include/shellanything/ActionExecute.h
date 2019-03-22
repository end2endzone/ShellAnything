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

#ifndef SA_ACTION_EXECUTE_H
#define SA_ACTION_EXECUTE_H

#include "shellanything/Action.h"

namespace shellanything
{

  class ActionExecute : public Action
  {
  public:
    ActionExecute();
    virtual ~ActionExecute();

    virtual bool execute(const Context & iContext) const;

    const std::string & getPath() const;
    void setPath(const std::string & iPath);

    const std::string & getBaseDir() const;
    void setBaseDir(const std::string & iBaseDir);

    const std::string & getArguments() const;
    void setArguments(const std::string & iArguments);

  private:
    std::string mPath;
    std::string mBaseDir;
    std::string mArguments;
  };

} //namespace shellanything

#endif //SA_ACTION_EXECUTE_H
