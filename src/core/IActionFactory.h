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

#ifndef SA_IACTIONFACTORY_H
#define SA_IACTIONFACTORY_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include "IAction.h"
#include <vector>

namespace shellanything
{

  /// <summary>
  /// Abstract action class.
  /// </summary>
  class SHELLANYTHING_EXPORT IActionFactory
  {
  public:
    /// <summary>
    /// A list of IAction class pointers.
    /// </summary>
    typedef std::vector<IActionFactory*> IActionFactoryPtrList;

    IActionFactory();
    virtual ~IActionFactory();

  private:
    // Disable copy constructor and copy operator
    IActionFactory(const IActionFactory&);
    IActionFactory& operator=(const IActionFactory&);
  public:

    /// <summary>
    /// Get the name of the action. This name should exactly match the name of the xml element it can support.
    /// </summary>
    /// <returns>Returns the name of the action.</returns>
    virtual const std::string& GetName() const = 0;

    /// <summary>
    /// Parse an Action from a xml.
    /// </summary>
    /// <param name="xml">A string that contains the xml element (including child nodes) to parse.</param>
    /// <param name="error">Provides an error description in case the parsing fails.</param>
    /// <returns>Returns a pointer to a valid IAction if the parsing is successful. Returns NULL otherwise.</returns>
    virtual IAction* ParseFromXml(const std::string& xml, std::string& error) const = 0;

  };


} //namespace shellanything

#endif //SA_IACTIONFACTORY_H
