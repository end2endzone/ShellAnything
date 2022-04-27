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

#ifndef SA_IUPDATECALLBACK_H
#define SA_IUPDATECALLBACK_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include "SelectionContext.h"

namespace shellanything
{

  /// <summary>
  /// Event callback interface.
  /// </summary>
  class SHELLANYTHING_EXPORT IUpdateCallback
  {
  public:
    IUpdateCallback();
    virtual ~IUpdateCallback();

  private:
    // Disable copy constructor and copy operator
    IUpdateCallback(const IUpdateCallback&);
    IUpdateCallback& operator=(const IUpdateCallback&);
  public:

    /// <summary>
    /// Set a pointer to a SelectionContext that contains the list of files and directory that are selected.
    /// </summary>
    /// <param name="context">The selection context used for validating.</param>
    virtual void SetSelectionContext(const SelectionContext* context) = 0;

    /// <summary>
    /// Get a pointer to a PropertyStore that contains the custom attribute names and values of the validator.
    /// </summary>
    /// <returns>Returns a pointer to a SelectionContext. Returns NULL if no selection context is set.</returns>
    virtual const SelectionContext* GetSelectionContext() const = 0;

    /// <summary>
    /// Callback function when a user make a new selection.
    /// </summary>
    virtual void OnNewSelection() const = 0;

  };


} //namespace shellanything

#endif //SA_IATTRIBUTEVALIDATOR_H
