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

#ifndef SA_VALIDATION_H
#define SA_VALIDATION_H

#include "shellanything/Node.h"
#include "shellanything/Context.h"
#include <string>

namespace shellanything
{

  class Validator
  {
  public:
    Validator();
    Validator(const Validator & validator);
    virtual ~Validator();

    /// <summary>
    /// Copy operator
    /// </summary>
    const Validator & operator =(const Validator & validator);

    /// <summary>
    /// Getter for the 'maxfiles' parameter.
    /// </summary>
    const int & GetMaxFiles() const;

    /// <summary>
    /// Setter for the 'maxfiles' parameter.
    /// </summary>
    void SetMaxFiles(const int & iMaxFiles);

    /// <summary>
    /// Getter for the 'maxdirectories' parameter.
    /// </summary>
    const int & GetMaxDirectories() const;

    /// <summary>
    /// Setter for the 'maxdirectories' parameter.
    /// </summary>
    void SetMaxDirectories(const int & iMaxDirectories);

    /// <summary>
    /// Getter for the 'properties' parameter.
    /// </summary>
    const std::string & GetProperties() const;

    /// <summary>
    /// Setter for the 'properties' parameter.
    /// </summary>
    void SetProperties(const std::string & iProperties);

    /// <summary>
    /// Getter for the 'fileextensions' parameter.
    /// </summary>
    const std::string & GetFileExtensions() const;

    /// <summary>
    /// Setter for the 'fileextensions' parameter.
    /// </summary>
    void SetFileExtensions(const std::string & iFileExtensions);

    /// <summary>
    /// Getter for the 'fileexists' parameter.
    /// </summary>
    const std::string & GetFileExists() const;

    /// <summary>
    /// Setter for the 'fileexists' parameter.
    /// </summary>
    void SetFileExists(const std::string & iFileExists);

    bool Validate(const Context & iContext) const;

  private:
    int mMaxFiles;
    int mMaxDirectories;
    std::string mProperties;
    std::string mFileExtensions;
    std::string mFileExists;
  };

} //namespace shellanything

#endif //SA_VALIDATION_H
