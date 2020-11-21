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
#include <vector>

namespace shellanything
{

  class Validator
  {
  public:
    /// <summary>
    /// A list of Validator class pointers.
    /// </summary>
    typedef std::vector<Validator*> ValidatorPtrList;

    Validator();
    virtual ~Validator();

  private:
    // Disable copy constructor and copy operator
    Validator(const Validator&);
    Validator& operator=(const Validator&);
  public:

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

    /// <summary>
    /// Getter for the 'class' parameter.
    /// </summary>
    const std::string & GetClass() const;

    /// <summary>
    /// Setter for the 'class' parameter.
    /// </summary>
    void SetClass(const std::string & iClass);

    /// <summary>
    /// Getter for the 'pattern' parameter.
    /// </summary>
    const std::string & GetPattern() const;

    /// <summary>
    /// Setter for the 'pattern' parameter.
    /// </summary>
    void SetPattern(const std::string & iPattern);

    /// <summary>
    /// Getter for the 'exprtk' parameter.
    /// </summary>
    const std::string & GetExprtk() const;

    /// <summary>
    /// Setter for the 'exprtk' parameter.
    /// </summary>
    void SetExprtk(const std::string & iExprtk);

    /// <summary>
    /// Getter for the 'inserve' parameter.
    /// </summary>
    const std::string & GetInserve() const;

    /// <summary>
    /// Setter for the 'inserve' parameter.
    /// </summary>
    void SetInserve(const std::string & iInserve);

    /// <summary>
    /// Returns true if the given attribute name is in the list of inversed attributes.
    /// The accepted attribute names matches the list of attributes of <validity> or <visibility> elements in a Configuration File.
    /// See the 'User Manual' for details.
    /// </summary>
    /// <param name="name">The name of the attribute. The name is case sensitive.</param>
    /// <returns>Returns true if the given attribute name is in the list of inversed attributes. Returns false otherwise.</returns>
    bool IsInversed(const char * name) const;

    /// <summary>
    /// Validate the given context against a set of constraints.
    /// The possible constraints includes a minimum/maximum number of files/directories selected,
    /// a list of properties that must be defined,
    /// a list of file extensions,
    /// ...
    /// Note: this function is used to enable or disable a menu.
    /// </summary>
    /// <param name="iContext">The context used for validating.</param>
    /// <returns>Returns true if the given context is valid against the set of constraints. Returns false otherwise.</returns>
    bool Validate(const Context & iContext) const;

  private:
    bool ValidateProperties(const Context & context, const std::string & properties, bool inversed) const;
    bool ValidateFileExtensions(const Context & context, const std::string & file_extensions, bool inversed) const;
    bool ValidateExists(const Context & context, const std::string & file_exists, bool inversed) const;
    bool ValidateClass(const Context & context, const std::string & class_, bool inversed) const;
    bool ValidateClassSingle(const Context & context, const std::string & class_, bool inversed) const;
    bool ValidatePattern(const Context & context, const std::string & pattern, bool inversed) const;
    bool ValidateExprtk(const Context & context, const std::string & exprtk, bool inversed) const;

  private:
    int mMaxFiles;
    int mMaxDirectories;
    std::string mProperties;
    std::string mFileExtensions;
    std::string mFileExists;
    std::string mClass;
    std::string mPattern;
    std::string mExprtk;
    std::string mInverse;
  };

} //namespace shellanything

#endif //SA_VALIDATION_H
