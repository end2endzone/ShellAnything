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

  /// <summary>
  /// Action class that executes a program.
  /// </summary>
  class ActionExecute : public Action
  {
  public:
    ActionExecute();
    virtual ~ActionExecute();

    /// <summary>
    /// Execute an application.
    /// </summary>
    /// <param name="iContext">The current context of execution.</param>
    /// <returns>Returns true if the execution is successful. Returns false otherwise.</returns>
    virtual bool execute(const Context & iContext) const;

    /// <summary>
    /// Getter for the 'path' parameter.
    /// </summary>
    const std::string & getPath() const;

    /// <summary>
    /// Setter for the 'path' parameter.
    /// </summary>
    void setPath(const std::string & iPath);

    /// <summary>
    /// Getter for the 'basedir' parameter.
    /// </summary>
    const std::string & getBaseDir() const;

    /// <summary>
    /// Setter for the 'basedir' parameter.
    /// </summary>
    void setBaseDir(const std::string & iBaseDir);

    /// <summary>
    /// Getter for the 'arguments' parameter.
    /// </summary>
    const std::string & getArguments() const;

    /// <summary>
    /// Setter for the 'arguments' parameter.
    /// </summary>
    void setArguments(const std::string & iArguments);

  private:
    std::string mPath;
    std::string mBaseDir;
    std::string mArguments;
  };

} //namespace shellanything

#endif //SA_ACTION_EXECUTE_H
