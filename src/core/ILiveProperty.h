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

#ifndef SA_ILIVE_PROPERTY_H
#define SA_ILIVE_PROPERTY_H

#include "shellanything/export.h"
#include "shellanything/config.h"

#include <string>

namespace shellanything
{
  /// <summary>
  /// Abstract interface for defining a Property which value is resolved as needed.
  /// </summary>
  class SHELLANYTHING_EXPORT ILiveProperty
  {
  public:
    ILiveProperty();
    virtual ~ILiveProperty();

  private:
    // Disable and copy constructor, dtor and copy operator
    ILiveProperty(const ILiveProperty&);
    ILiveProperty& operator=(const ILiveProperty&);
  public:

    /// <summary>
    /// Get the name of the live property.
    /// </summary>
    /// <returns>Returns the name of the dynamic property.</returns>
    virtual const std::string& GetName() const = 0;

    /// <summary>
    /// Get the value of the property.
    /// </summary>
    /// <returns>Returns value of the property if the property is set. Returns an empty string otherwise.</returns>
    virtual std::string GetProperty() const = 0;

  };


} //namespace shellanything

#endif //SA_ILIVE_PROPERTY_H
