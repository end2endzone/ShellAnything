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

#ifndef SA_ACTION_PROPERTY_H
#define SA_ACTION_PROPERTY_H

#include "Action.h"

namespace shellanything
{

  /// <summary>
  /// Action class that changes the value of a property.
  /// </summary>
  class SHELLANYTHING_EXPORT ActionProperty : public Action
  {
  public:
    ActionProperty();
    virtual ~ActionProperty();

    /// <summary>
    /// Changes the value of a property.
    /// </summary>
    /// <param name="context">The current context of execution.</param>
    /// <returns>Returns true if the execution is successful. Returns false otherwise.</returns>
    virtual bool Execute(const Context & context) const;

    /// <summary>
    /// Getter for the 'name' parameter.
    /// </summary>
    const std::string & GetName() const;

    /// <summary>
    /// Setter for the 'name' parameter.
    /// </summary>
    void SetName(const std::string & name);

    /// <summary>
    /// Getter for the 'value' parameter.
    /// </summary>
    const std::string & GetValue() const;

    /// <summary>
    /// Setter for the 'value' parameter.
    /// </summary>
    void SetValue(const std::string & value);

    /// <summary>
    /// Getter for the 'exprtk' parameter.
    /// </summary>
    const std::string & GetExprtk() const;

    /// <summary>
    /// Setter for the 'exprtk' parameter.
    /// </summary>
    void SetExprtk(const std::string & exprtk);

  private:
    std::string mName;
    std::string mValue;
    std::string mExprtk;
  };


} //namespace shellanything

#endif //SA_ACTION_PROPERTY_H
