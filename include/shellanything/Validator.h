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

#include "shellanything/Context.h"
#include <string>

namespace shellanything
{

  class Validator
  {
  public:
    Validator();
    Validator(const Validator & iValidation);
    virtual ~Validator();

    const int & getMaxFiles() const;
    void setMaxFiles(const int & iMaxFiles);

    const int & getMaxDirectories() const;
    void setMaxDirectories(const int & iMaxDirectories);

    const std::string & getProperties() const;
    void setProperties(const std::string & iProperties);

    const std::string & getFileExtensions() const;
    void setFileExtensions(const std::string & iFileExtensions);

    bool isValid(const Context & iContext) const;

    const Validator & operator =(const Validator & iValidation);

  private:
    int mMaxFiles;
    int mMaxDirectories;
    std::string mProperties;
    std::string mFileExtensions;
  };

} //namespace shellanything

#endif //SA_VALIDATION_H
