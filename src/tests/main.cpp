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

#include <stdio.h>
#include <iostream>
#include <stdio.h>

#include <gtest/gtest.h>

#pragma warning( push )
#pragma warning( disable: 4355 ) // glog\install_dir\include\glog/logging.h(1167): warning C4355: 'this' : used in base member initializer list
#include <glog/logging.h>
#pragma warning( pop )

#include "rapidassist/testing.h"
#include "rapidassist/environment.h"
#include "rapidassist/cli.h"

#include "ArgumentsHandler.h"
#include "GlogUtils.h"

using namespace ra;

int main(int argc, char **argv)
{
  // Look for custom command line arguments
  bool has_PrintProcessSettings = ra::cli::ParseArgument("PrintProcessSettings", std::string(), argc, argv);
  if (has_PrintProcessSettings)
    return shellanything::PrintProcessSettings(argc, argv);

  // Initialize Google's logging library.
  shellanything::InitLogger();

  //Issue #60 - Unit tests cannot execute from installation directory.
  //Create log directory under the current executable.
  //When running tests from a developer environment, the log directory is expected to have write access.
  //If unit tests are executed from the installation directory, the log directory under the current executable is denied write access.
  std::string log_dir = fLS::FLAGS_log_dir;
  printf("Using log directory: '%s'.\n", log_dir.c_str());

  LOG(INFO) << "Starting unit tests";
  LOG(INFO) << __FUNCTION__ << "() - BEGIN";

  //define path for xml output report
  std::string outputXml = "xml:";
  if (!shellanything::HasDirectoryWriteAccess("."))
  {
    //Issue #60 - Unit tests cannot execute from installation directory.
    //Output report in same directory as the log files
    outputXml.append(log_dir);
    outputXml.append("\\");
  }
  outputXml.append("sa.tests");
  outputXml += (ra::environment::IsProcess32Bit() ? ".x86" : ".x64");
  outputXml += (ra::environment::IsConfigurationDebug() ? ".debug" : ".release");
  outputXml += ".xml";
  ::testing::GTEST_FLAG(output) = outputXml;
  printf("Using xml report: '%s'.\n", outputXml.c_str());

  ::testing::GTEST_FLAG(filter) = "*";
  ::testing::InitGoogleTest(&argc, argv);

  int wResult = RUN_ALL_TESTS(); //Find and run all tests

  LOG(INFO) << __FUNCTION__ << "() - END";

  // Shutdown Google's logging library.
  google::ShutdownGoogleLogging();

  return wResult; // returns 0 if all the tests are successful, or 1 otherwise
}
