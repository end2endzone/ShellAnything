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

#ifndef SA_MIMETYPE_H
#define SA_MIMETYPE_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include <string>
#include "magic.h"

namespace shellanything
{
  class SHELLANYTHING_EXPORT FileMagicManager
  {
  private:
    FileMagicManager();
    ~FileMagicManager();

  private:
    // Disable copy constructor and copy operator
    FileMagicManager(const FileMagicManager&);
    FileMagicManager& operator=(const FileMagicManager&);
  public:

    static FileMagicManager & GetInstance();

    std::string GetMIMEType(const std::string & path) const;
    std::string GetDescription(const std::string & path) const;
    std::string GetExtension(const std::string & path) const;
    std::string GetCharset(const std::string & path) const;

  private:
    magic_t magic_cookie;
  };


} //namespace shellanything

#endif //SA_MIMETYPE_H