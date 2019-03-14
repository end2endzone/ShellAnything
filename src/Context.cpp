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

#include "shellanything/Context.h"
#include "rapidassist/filesystem.h"

namespace shellanything
{

  Context::Context() :
    mNumFiles(0),
    mNumDirectories(0)
  {
  }

  Context::~Context()
  {
  }

  const Context::ElementList & Context::getElements() const
  {
    return mElements;
  }

  void Context::setElements(const Context::ElementList & elements)
  {
    mElements = elements;

    mNumFiles = 0;
    mNumDirectories = 0;

    //get elements stats
    for(size_t i=0; i<elements.size(); i++)
    {
      const std::string & element = elements[i];
      bool isFile = ra::filesystem::fileExists(element.c_str());
      bool isDir  = ra::filesystem::folderExists(element.c_str());
      if (isFile)
        mNumFiles++;
      if (isDir)
        mNumDirectories++;
    }
  }

  int Context::getNumFiles() const
  {
    return mNumFiles;
  }

  int Context::getNumDirectories() const
  {
    return mNumDirectories;
  }

} //namespace shellanything
