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

#ifndef SA_ACTION_FILE_H
#define SA_ACTION_FILE_H

#include "shellanything/Action.h"

namespace shellanything
{

  /// <summary>
  /// Action class that opens a documents using default application.
  /// </summary>
  class ActionFile : public Action
  {
  public:
    ActionFile();
    virtual ~ActionFile();

    /// <summary>
    /// Create a file.
    /// </summary>
    /// <param name="context">The current context of execution.</param>
    /// <returns>Returns true if the execution is successful. Returns false otherwise.</returns>
    virtual bool Execute(const Context & context) const;

    /// <summary>
    /// Getter for the 'path' parameter.
    /// </summary>
    const String & GetPath() const;

    /// <summary>
    /// Setter for the 'path' parameter.
    /// </summary>
    void SetPath(const String & path);

    /// <summary>
    /// Getter for the 'content' parameter.
    /// </summary>
    const String & GetText() const;

    /// <summary>
    /// Setter for the 'content' parameter.
    /// </summary>
    void SetText(const String & text);

    /// <summary>
    /// Getter for the 'encoding' parameter.
    /// </summary>
    const String & GetEncoding() const;

    /// <summary>
    /// Setter for the 'encoding' parameter.
    /// </summary>
    void SetEncoding(const String & encoding);

  private:
    String mPath;
    String mText;
    String mEncoding;
  };

} //namespace shellanything

#endif //SA_ACTION_FILE_H
