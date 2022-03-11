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

#ifndef SA_ACTION_OPEN_H
#define SA_ACTION_OPEN_H

#include "IAction.h"
#include "BaseAction.h"
#include "IActionFactory.h"

namespace shellanything
{

  /// <summary>
  /// Action class that opens a documents using default application.
  /// </summary>
  class SHELLANYTHING_EXPORT ActionOpen : public BaseAction
  {
  public:
    ActionOpen();
    virtual ~ActionOpen();

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
    /// Open a document with the default application.
    /// </summary>
    /// <param name="context">The current context of execution.</param>
    /// <returns>Returns true if the execution is successful. Returns false otherwise.</returns>
    virtual bool Execute(const SelectionContext & context) const;

    /// <summary>
    /// Getter for the 'path' parameter.
    /// </summary>
    const std::string & GetPath() const;

    /// <summary>
    /// Setter for the 'path' parameter.
    /// </summary>
    void SetPath(const std::string & path);

  private:
    std::string mPath;
  };

} //namespace shellanything

#endif //SA_ACTION_OPEN_H
