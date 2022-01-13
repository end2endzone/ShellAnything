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

#include "Action.h"

namespace shellanything
{

  /// <summary>
  /// Action class that executes a program.
  /// </summary>
  class SHELLANYTHING_EXPORT ActionExecute : public Action
  {
  public:
    ActionExecute();
    virtual ~ActionExecute();

    /// <summary>
    /// Execute an application.
    /// </summary>
    /// <param name="context">The current context of execution.</param>
    /// <returns>Returns true if the execution is successful. Returns false otherwise.</returns>
    virtual bool Execute(const Context& context) const;

    /// <summary>
    /// Getter for the 'path' parameter.
    /// </summary>
    const std::string & GetPath() const;

    /// <summary>
    /// Setter for the 'path' parameter.
    /// </summary>
    void SetPath(const std::string & path);

    /// <summary>
    /// Getter for the 'basedir' parameter.
    /// </summary>
    const std::string & GetBaseDir() const;

    /// <summary>
    /// Setter for the 'basedir' parameter.
    /// </summary>
    void SetBaseDir(const std::string & base_dir);

    /// <summary>
    /// Getter for the 'arguments' parameter.
    /// </summary>
    const std::string & GetArguments() const;

    /// <summary>
    /// Setter for the 'arguments' parameter.
    /// </summary>
    void SetArguments(const std::string & arguments);

    /// <summary>
    /// Getter for the 'verb' parameter.
    /// </summary>
    const std::string& GetVerb() const;

    /// <summary>
    /// Setter for the 'verb' parameter.
    /// </summary>
    void SetVerb(const std::string& verb);

  private:
    /// <summary>
    /// Execute an application with ShellExecuteEx method.
    /// This execute method supports verbs.
    /// </summary>
    /// <param name="context">The current context of execution.</param>
    /// <returns>Returns true if the execution is successful. Returns false otherwise.</returns>
    virtual bool ExecuteVerb(const Context & context) const;

    /// <summary>
    /// Execute an application with RapidAssist method.
    /// This execute method does not supports verbs.
    /// </summary>
    /// <param name="context">The current context of execution.</param>
    /// <returns>Returns true if the execution is successful. Returns false otherwise.</returns>
    virtual bool ExecuteProcess(const Context & context) const;

  private:
    std::string mPath;
    std::string mBaseDir;
    std::string mArguments;
    std::string mVerb;
  };

} //namespace shellanything

#endif //SA_ACTION_EXECUTE_H
