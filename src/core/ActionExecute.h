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

#include "IAction.h"
#include "BaseAction.h"
#include "IActionFactory.h"

namespace shellanything
{

  /// <summary>
  /// Action class that executes a program.
  /// </summary>
  class SHELLANYTHING_EXPORT ActionExecute : public BaseAction
  {
  public:
    ActionExecute();
    virtual ~ActionExecute();

    /// <summary>
    /// Name of the xml element for this action.
    /// </summary>
    static const std::string XML_ELEMENT_NAME;

    /// <summary>
    /// Instanciate an IActionFactory that is able to parse this action.
    /// </summary>
    /// <returns>Returns a IActionFactory to parse this action.</returns>
    static IActionFactory* NewFactory();

    /// <summary>
    /// Execute an application.
    /// </summary>
    /// <param name="context">The current context of execution.</param>
    /// <returns>Returns true if the execution is successful. Returns false otherwise.</returns>
    virtual bool Execute(const SelectionContext& context) const;

    /// <summary>
    /// Getter for the 'path' parameter.
    /// </summary>
    const std::string& GetPath() const;

    /// <summary>
    /// Setter for the 'path' parameter.
    /// </summary>
    void SetPath(const std::string& path);

    /// <summary>
    /// Getter for the 'basedir' parameter.
    /// </summary>
    const std::string& GetBaseDir() const;

    /// <summary>
    /// Setter for the 'basedir' parameter.
    /// </summary>
    void SetBaseDir(const std::string& base_dir);

    /// <summary>
    /// Getter for the 'arguments' parameter.
    /// </summary>
    const std::string& GetArguments() const;

    /// <summary>
    /// Setter for the 'arguments' parameter.
    /// </summary>
    void SetArguments(const std::string& arguments);

    /// <summary>
    /// Getter for the 'verb' parameter.
    /// </summary>
    const std::string& GetVerb() const;

    /// <summary>
    /// Setter for the 'verb' parameter.
    /// </summary>
    void SetVerb(const std::string& verb);

    /// <summary>
    /// Getter for the 'wait' parameter.
    /// </summary>
    const std::string& GetWait() const;

    /// <summary>
    /// Setter for the 'wait' parameter.
    /// </summary>
    void SetWait(const std::string& value);

    /// <summary>
    /// Getter for the 'timeout' parameter.
    /// </summary>
    const std::string& GetTimeout() const;

    /// <summary>
    /// Setter for the 'timeout' parameter.
    /// </summary>
    void SetTimeout(const std::string& value);

    /// <summary>
    /// Getter for the 'console' parameter.
    /// </summary>
    const std::string& GetConsole() const;

    /// <summary>
    /// Setter for the 'console' parameter.
    /// </summary>
    void SetConsole(const std::string& value);

    /// <summary>
    /// Getter for the 'pid' parameter.
    /// </summary>
    const std::string& GetPid() const;

    /// <summary>
    /// Setter for the 'pid' parameter.
    /// </summary>
    void SetPid(const std::string& value);

  private:

    /// <summary>
    /// Wait for the process to exit, if required.
    /// </summary> 
    /// <param name="pId">The process id created from ExecuteVerb() or ExecuteProcess().</param>
    /// <returns>Returns true if the wait process exit was not required or if the wait was required and succeed. Returns false otherwise.</returns>
    virtual bool WaitForExit(uint32_t pId) const;

  private:
    std::string mPath;
    std::string mBaseDir;
    std::string mArguments;
    std::string mVerb;
    std::string mWait;
    std::string mTimeout;
    std::string mConsole;
    std::string mPid;
  };

} //namespace shellanything

#endif //SA_ACTION_EXECUTE_H
