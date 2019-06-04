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
    const int & getMaxFiles() const;

    /// <summary>
    /// Setter for the 'maxfiles' parameter.
    /// </summary>
    void setMaxFiles(const int & iMaxFiles);

    /// <summary>
    /// Getter for the 'maxdirectories' parameter.
    /// </summary>
    const int & getMaxDirectories() const;

    /// <summary>
    /// Setter for the 'maxdirectories' parameter.
    /// </summary>
    void setMaxDirectories(const int & iMaxDirectories);

    /// <summary>
    /// Getter for the 'properties' parameter.
    /// </summary>
    const std::string & getProperties() const;

    /// <summary>
    /// Setter for the 'properties' parameter.
    /// </summary>
    void setProperties(const std::string & iProperties);

    /// <summary>
    /// Getter for the 'fileextensions' parameter.
    /// </summary>
    const std::string & getFileExtensions() const;

    /// <summary>
    /// Setter for the 'fileextensions' parameter.
    /// </summary>
    void setFileExtensions(const std::string & iFileExtensions);

    /// <summary>
    /// Getter for the 'fileexists' parameter.
    /// </summary>
    const std::string & getFileExists() const;

    /// <summary>
    /// Setter for the 'fileexists' parameter.
    /// </summary>
    void setFileExists(const std::string & iFileExists);

    bool validate(const Context & iContext) const;

  private:
    int mMaxFiles;
    int mMaxDirectories;
    std::string mProperties;
    std::string mFileExtensions;
    std::string mFileExists;
  };

} //namespace shellanything

#endif //SA_VALIDATION_H
