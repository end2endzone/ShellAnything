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

#ifndef SA_CONFIGURATION_H
#define SA_CONFIGURATION_H

#include "shellanything/Node.h"
#include <stdint.h>

namespace shellanything
{

  class Configuration : public Node
  {
  public:
    typedef std::vector<Configuration*> ConfigurationPtrList;

    Configuration();
    virtual ~Configuration();

    const std::string & getFilePath() const;
    void setFilePath(const std::string & iFilePath);

    const uint64_t & getFileModifiedDate() const;
    void setFileModifiedDate(const uint64_t & iFileModifiedDate);

  private:
    uint64_t mFileModifiedDate;
    std::string mFilePath;
  };

} //namespace shellanything

#endif //SA_CONFIGURATION_H
