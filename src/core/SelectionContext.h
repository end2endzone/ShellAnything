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

#ifndef SA_SELECTION_CONTEXT_H
#define SA_SELECTION_CONTEXT_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include "StringList.h"
#include <string>
#include <vector>

namespace shellanything
{

  /// <summary>
  /// A class that holds a list of selected files and/or directories.
  /// </summary>
  class SHELLANYTHING_EXPORT SelectionContext
  {
  public:
    /// <summary>
    /// Name of the property that contains the separator for multi-selection separator.
    /// </summary>
    static const std::string MULTI_SELECTION_SEPARATOR_PROPERTY_NAME;

    /// <summary>
    /// Default value for the property 'MULTI_SELECTION_SEPARATOR_PROPERTY_NAME'.
    /// </summary>
    static const std::string DEFAULT_MULTI_SELECTION_SEPARATOR;

    SelectionContext();
    SelectionContext(const SelectionContext& c);
    virtual ~SelectionContext();

    /// <summary>
    /// Copy operator
    /// </summary>
    const SelectionContext& operator =(const SelectionContext& c);

    /// <summary>
    /// Register a list of 'properties' based on the context elements.
    /// </summary>
    void RegisterProperties() const;

    /// <summary>
    /// Unregister the list of 'properties' created by RegisterProperties().
    /// </summary>
    void UnregisterProperties() const;

    /// <summary>
    /// Get the list of elements of the SelectionContext.
    /// </summary>
    const StringList& GetElements() const;

    /// <summary>
    /// Set the list of elements to the SelectionContext.
    /// </summary>
    void SetElements(const StringList& elements);

    /// <summary>
    /// Get the number of files in the context.
    /// </summary>
    int GetNumFiles() const;

    /// <summary>
    /// Get the number of directories in the context.
    /// </summary>
    int GetNumDirectories() const;

  private:
    StringList mElements;
    int mNumFiles;
    int mNumDirectories;
  };


} //namespace shellanything

#endif //SA_SELECTION_CONTEXT_H
