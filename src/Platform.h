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

  /// <summary>
  /// Parse the given string as a boolean value.
  /// </summary>
  /// <param name="str">The input string which contains a boolean string representation.</param>
  /// <returns>Returns true when the given string is logically 'true'. Returns false otherwise.</returns>
  bool parseBoolean(const std::string & value);

  /// <summary>
  /// Expand a file path by replacing environment variable reference by the actual variable's value.
  /// Unix    environment variables syntax must in the following form:  $name where  'name' is an environment variable.
  /// Windows environment variables syntax must in the following form:  %name% where 'name' is an environment variable.
  /// </summary>
  /// <param name="iValue">The path that must be expanded.</param>
  /// <returns>Returns a new string with the expanded strings.</returns>
  std::string expand(const std::string & iValue);

  /// <summary>
  /// Returns a list of all environment variables defined by the current process.
  /// </summary>
  /// <returns>Returns a list of all environment variables.</returns>
  ra::strings::StringVector getEnvironmentVariables();

  /// <summary>
  /// Returns the current user 'home' directory.
  /// For unix    systems, the returned path is "~".
  /// For windows systems, the returned path is "%USERPROFILE%" (ie "C:\Users\JohnSmith").
  /// </summary>
  /// <returns>Returns the current user's home directory.</returns>
  std::string getHomeDirectory();

  /// <summary>
  /// Returns the current user 'application data' directory.
  /// For unix    systems, the returned path is "/usr/share".
  /// For windows systems, the returned path is "%USERPROFILE%\AppData\Local" (ie "C:\Users\JohnSmith\AppData\Local").
  /// </summary>
  /// <returns>Returns the current user's application data directory.</returns>
  std::string getApplicationsDataDirectory();

  /// <summary>
  /// Returns the current user 'documents' directory.
  /// For unix    systems, the returned path is "~/Documents".
  /// For windows systems, the returned path is "%USERPROFILE%\Documents" (ie "C:\Users\JohnSmith\Documents").
  /// </summary>
  /// <returns>Returns the current user's documents directory.</returns>
  std::string getDocumentsDirectory();

  /// <summary>
  /// Returns the current user 'desktop' directory.
  /// For unix    systems, the returned path is "~/Desktop".
  /// For windows systems, the returned path is "%USERPROFILE%\Desktop" (ie "C:\Users\JohnSmith\Desktop").
  /// </summary>
  /// <returns>Returns the current user's desktop directory.</returns>
  std::string getDesktopDirectory();

  /// <summary>
  /// Reads the first 'size' bytes of file 'path' and copy the content to 'content' variable.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="size">The number of bytes to peek into the file.</param>
  /// <param name="content">The variable that will contains the readed bytes.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool peekFile(const std::string & path, size_t size, std::string & content);

  /// <summary>
  /// Reads the content of the given file path copy the content to 'content' variable.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="content">The variable that will contains the readed bytes.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool readFile(const std::string & path, std::string & content);

  /// <summary>
  /// Writes the given content to a file.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="content">The content to write to the file.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool writeFile(const std::string & path, std::string & content);

  //https://github.com/end2endzone/RapidAssist/issues/27
  bool createFolder(const char * iPath);

  /// <summary>
  /// copyFile() callback interface
  /// </summary>
  class IProgressReport
  {
  public:
    /// <summary>
    /// CopyFile() callback function.
    /// </summary>
    /// <param name="progress">The progress of the file copy. Ranges [0, 1] inclusive.</param>
    virtual void onProgressReport(double progress) = 0;
  };

  /// <summary>
  /// CopyFile() callback function.
  /// </summary>
  /// <param name="progress">The progress of the file copy. Ranges [0, 1] inclusive.</param>
  typedef void (*ProgressReportCallback)(double);

  /// <summary>
  /// Copy a file to another destination.
  /// </summary>
  /// <param name="source_path">The source file path to copy.</param>
  /// <param name="destination_path">The destination file path.</param>
  /// <returns>Returns true if file copy is successful. Returns false otherwise.</returns>
  bool copyFile(const std::string & source_path, const std::string & destination_path);

  /// <summary>
  /// Copy a file to another destination.
  /// </summary>
  /// <param name="source_path">The source file path to copy.</param>
  /// <param name="destination_path">The destination file path.</param>
  /// <param name="progress_functor">A valid IProgressReport pointer to handle the copy callback.</param>
  /// <returns>Returns true if file copy is successful. Returns false otherwise.</returns>
  bool copyFile(const std::string & source_path, const std::string & destination_path, IProgressReport * progress_functor);

  /// <summary>
  /// Copy a file to another destination.
  /// </summary>
  /// <param name="source_path">The source file path to copy.</param>
  /// <param name="destination_path">The destination file path.</param>
  /// <param name="progress_function">A valid ProgressReportCallback function pointer to handle the copy callback.</param>
  /// <returns>Returns true if file copy is successful. Returns false otherwise.</returns>
  bool copyFile(const std::string & source_path, const std::string & destination_path, ProgressReportCallback progress_function);

  /// <summary>
  /// Get the file name of a path without the extension.
  /// </summary>
  /// <param name="iPath">The file path of a file.</param>
  /// <returns>Returns the file name without the extension.</returns>
  std::string getFilenameWithoutExtension(const char * iPath);

  std::string makeRelativePath(const std::string & base_path, const std::string & test_path);

  /// <summary>
  /// Start the given process from the given directory.
  /// </summary>
  /// <param name="iCommand">The command to start. Can include a process path and/or arguments.</param>
  /// <param name="iDefaultDirectory">The directory to run the command from.</param>
  /// <returns>Returns the process id when successful. Returns 0 otherwise.</returns>
  uint32_t startProcess(const std::string & iCommand, const std::string & iDefaultDirectory);

  /// <summary>
  /// Start the given process with the given arguments from the given directory.
  /// </summary>
  /// <param name="iExecPath">The path to the executable to start.</param>
  /// <param name="iArguments">The arguments to send to the process.</param>
  /// <param name="iDefaultDirectory">The directory to run the command from.</param>
  /// <returns>Returns the process id when successful. Returns 0 otherwise.</returns>
  uint32_t startProcess(const std::string & iExecPath, const std::string & iArguments, const std::string & iDefaultDirectory);

  /// <summary>
  /// Open a document with the default system application.
  /// </summary>
  /// <param name="iPath">The path to the document to open.</param>
  /// <returns>Returns the process id when successful. Returns 0 otherwise.</returns>
  uint32_t openDocument(const std::string & iPath);

  /// <summary>
  /// Get the latest system error code.
  /// </summary>
  /// <returns>Returns the latest system error code.</returns>
  uint32_t GetSystemErrorCode();

  /// <summary>
  /// Get a system error description from the given error code.
  /// </summary>
  /// <param name="dwError">The system error code.</param>
  /// <returns>Returns the system error description from the given error code.</returns>
  std::string GetSystemErrorDescription(uint32_t dwError);

  /// <summary>
  /// Show an error message to the user.
  /// </summary>
  /// <param name="title">The caption title of the window.</param>
  /// <param name="message">The message to display to the user.</param>
  void ShowErrorMessage(const std::string & title, const std::string & message);

  /// <summary>
  /// Finds a file using the PATH environment variable.
  /// </summary>
  /// <param name="filename">The filename that we are searching for.</param>
  /// <param name="locations">The path locations where the file was found.</param>
  /// <returns>Returns true if the filename was found at least once. Returns false otherwise.</returns>
  bool findFileFromPaths(const std::string & filename, ra::strings::StringVector & locations);

  /// <summary>
  /// Finds a file using the PATH environment variable.
  /// </summary>
  /// <param name="filename">The filename that we are searching for.</param>
  /// <returns>Returns the first location where the file was found. Returns empty string otherwise.</returns>
  std::string findFileFromPaths(const std::string & filename);

} //namespace shellanything

#endif //SA_ACTION_H
