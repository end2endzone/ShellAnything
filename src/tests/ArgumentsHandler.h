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

#ifndef SA_ARGUMENT_HANDLER_H
#define SA_ARGUMENT_HANDLER_H

#include <string>

namespace shellanything
{
  /// <summary>
  /// Function definition for command line entry points.
  /// </summary>
  typedef int (*CommandLineEntryPoint)(int /*argc*/, char** /*argv*/);

  struct COMMAND_LINE_ENTRY_POINT
  {
    std::string name;
    CommandLineEntryPoint func;
  };

  /// <summary>
  /// Print statistics about this program settings.
  /// </summary>
  /// <param name="argc">The number of elements in the argv array.</param>
  /// <param name="argv">The array of argument names.</param>
  /// <returns>Returns 0 when the function succeed. Returns a non zero values otherwise.</returns>
  int PrintProcessSettings(int argc, char** argv);

  /// <summary>
  /// Register a function for a given command line option name.
  /// </summary>
  /// <param name="argc">The number of elements in the argv array.</param>
  /// <param name="argv">The array of argument names.</param>
  /// <returns>Returns a valid pointer to a COMMAND_LINE_ENTRY_POINT structure when registration succeed. Returns NULL otherwise.</returns>
  COMMAND_LINE_ENTRY_POINT* RegisterCommandLineEntryPoint(const char* name, CommandLineEntryPoint func);

  /// <summary>
  /// Check if the current command line arguments matches any registered entry points.
  /// </summary>
  /// <param name="argc">The number of elements in the argv array.</param>
  /// <param name="argv">The array of argument names.</param>
  /// <returns>Returns true when at least one entry points matches. Returns false otherwise.</returns>
  bool HasCommandLineEntryPoints(int argc, char** argv);

  /// <summary>
  /// Execute all command line entry points that are specified in command line arguments.
  /// </summary>
  /// <param name="argc">The number of elements in the argv array.</param>
  /// <param name="argv">The array of argument names.</param>
  /// <returns>Returns 0 when the entry point execution succeed. Returns a non zero values otherwise.</returns>
  int ProcessCommandLineEntryPoints(int argc, char** argv);

  /// <summary>
  /// Find an entry point by name.
  /// </summary>
  /// <param name="name">The registered name of the entry point.</param>
  /// <returns>Returns a pointer to a COMMAND_LINE_ENTRY_POINT structure when found. Returns NULL otherwise.</returns>
  COMMAND_LINE_ENTRY_POINT* FindCommandLineEntryPointByName(const char* name);

  /// <summary>
  /// Invoke a new copy of current process adding a command line argument to execute the associated entry point.
  /// </summary>
  /// <param name="argc">The number of elements in the argv array.</param>
  /// <param name="argv">The array of argument names.</param>
  /// <param name="name">The registered name of the entry point.</param>
  /// <param name="entry">The entry point to invoke.</param>
  /// <returns>Returns 0 when the entry point execution succeed. Returns a non zero values otherwise.</returns>
  int InvokeCommandLineEntryPoint(const char * name, int argc, char** argv);
  int InvokeCommandLineEntryPoint(const COMMAND_LINE_ENTRY_POINT* entry, int argc, char** argv);

} //namespace shellanything

#endif //SA_ARGUMENT_HANDLER_H
