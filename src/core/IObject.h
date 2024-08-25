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

#ifndef SA_IOBJECT_H
#define SA_IOBJECT_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include <string>

namespace shellanything
{

  /// <summary>
  /// Declares the IObject interface
  /// </summary>
  class SHELLANYTHING_EXPORT IObject
  {
  public:
    IObject();
    virtual ~IObject();

    /// <summary>
    /// Get a shallow string representation of this object.
    /// </summary>
    /// <returns>Get a shallow string representation of this object.</returns>
    virtual std::string ToShortString() const = 0;

    /// <summary>
    /// Get a string representation of this object and its children.
    /// The output string is multiple lines. Each line separated by '\n' (LF) character.
    /// </summary>
    /// <param name="str">The string to append this object's description to.</param>
    /// <param name="indent">The indentation (spaces) to add before each line describing this object.</param>
    virtual void ToLongString(std::string& str, int indent) const = 0;

    /// <summary>
    /// Append an object count to an exiting string. The function automatically adds an 's' character if the given count is > 1.
    /// </summary>
    /// <param name="str">The string to append to.</param>
    /// <param name="name">The name of the object count.</param>
    /// <param name="count">The count of object named we have.</param>
    static void AppendObjectCount(std::string& str, const char* name, size_t count);

    /// <summary>
    /// Append an object count to an exiting string. The function automatically adds an 's' character if the given count is > 1.
    /// </summary>
    /// <param name="str">The string to append to.</param>
    /// <param name="name_singular">The name of singluar form (when count < 2).</param>
    /// <param name="name_plurial">The name of plurial form (when count >= 2).</param>
    /// <param name="count">The count of object named we have.</param>
    static void AppendObjectCount(std::string& str, const char* name1, const char* name2, size_t count);
  };

} //namespace shellanything

#endif //SA_IOBJECT_H
