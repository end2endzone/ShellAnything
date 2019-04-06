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

#ifndef SA_PLATFORM_H
#define SA_PLATFORM_H

#include "rapidassist/strings.h"

namespace shellanything
{

  std::string expand(const std::string & iValue);
  ra::strings::StringVector getEnvironmentVariables();
  std::string getHomeDirectory();
  std::string getApplicationsDataDirectory();
  std::string getDocumentsDirectory();
  std::string getDesktopDirectory();

  bool peekFile(const std::string & path, size_t size, std::string & content);
  bool readFile(const std::string & path, std::string & content);
  bool writeFile(const std::string & path, std::string & content);

  // copyFile() callback interfaces
  class IProgressReport
  {
  public:
    virtual void onProgressReport(double progress) = 0;
  };
  typedef void (*ProgressReportCallback)(double);
  bool copyFile(const std::string & source_path, const std::string & destination_path);
  bool copyFile(const std::string & source_path, const std::string & destination_path, IProgressReport * progress_functor);
  bool copyFile(const std::string & source_path, const std::string & destination_path, ProgressReportCallback progress_function);

  std::string getFilenameWithoutExtension(const char * iPath);
  std::string makeRelativePath(const std::string & base_path, const std::string & test_path);

  uint32_t startProcess(const std::string & iCommand, const std::string & iDefaultDirectory);
  uint32_t startProcess(const std::string & iExecPath, const std::string & iArguments, const std::string & iDefaultDirectory);
  uint32_t openDocument(const std::string & iPath);

} //namespace shellanything

#endif //SA_ACTION_H
