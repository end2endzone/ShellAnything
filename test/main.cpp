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

#include "Platform.h"

#include "rapidassist/testing.h"
#include "rapidassist/environment.h"

using namespace ra;

int main(int argc, char **argv)
{
  // Prepare Google's logging library.
  fLB::FLAGS_logtostderr = false; //on error, print to stdout instead of stderr
  fLB::FLAGS_log_prefix = 1; //prefix each message in file/console with 'E0405 19:13:07.577863  6652 shellext.cpp:847]'
  fLI::FLAGS_stderrthreshold = INT_MAX; //disable console output
  fLI::FLAGS_logbufsecs = 0; //flush logs after each LOG() calls
  fLI::FLAGS_minloglevel = google::GLOG_INFO;
  fLI::FLAGS_v = 4; //disables vlog(2), vlog(3), vlog(4)

  google::SetLogFilenameExtension(".log");

  //define log directory to be the current directory
  //which (in most cases) should be next to the unit test executable
  fLS::FLAGS_log_dir = ".";

  // Initialize Google's logging library.
  google::InitGoogleLogging(argv[0]);

  LOG(INFO) << "Starting unit tests";
  LOG(INFO) << __FUNCTION__ << "() - BEGIN";

  //define default values for xml output report
  std::string outputXml = "xml:" "shellanything_unittest";
  outputXml += (ra::environment::IsProcess32Bit() ? ".x86" : ".x64");
  outputXml += (ra::environment::IsConfigurationDebug() ? ".debug" : ".release");
  outputXml += ".xml";
  ::testing::GTEST_FLAG(output) = outputXml;

  ::testing::GTEST_FLAG(filter) = "*";
  ::testing::InitGoogleTest(&argc, argv);

  int wResult = RUN_ALL_TESTS(); //Find and run all tests

  LOG(INFO) << __FUNCTION__ << "() - END";
  return wResult; // returns 0 if all the tests are successful, or 1 otherwise
}
