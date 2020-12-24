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

#ifndef SA_TEST_WORKSPACE_H
#define SA_TEST_WORKSPACE_H

#include <string>

namespace shellanything
{

  /// <summary>
  /// Create a temporary workspace directory for storing test files.
  /// The workspace directory is deleted when the class is destroyed.
  /// </summary>
  /// <returns>Returns 0 when the function succeed. Returns a non zero values otherwise.</returns>
  class Workspace
  {
  public:
    /// <summary>
    /// Create a temporary workspace directory based on the TEMP environment variable and current test name.
    /// </summary>
    Workspace();

    /// <summary>
    /// Create a temporary workspace directory located in the 'workspace' directory.
    /// </summary>
    /// <param name="workspace">The path to a directory that is used for workspace.</param>
    Workspace(const char * workspace);

    virtual ~Workspace();

    /// <summary>
    /// Delete the workspace directory including all files in the workspace.
    /// </summary>
    /// <returns>Returns true if the workspace is completely deleted. Returns false otherwise.</returns>
    bool Cleanup();

    /// <summary>
    /// Get the base directory of the current workspace.
    /// </summary>
    /// <returns>Returns the base directory of the current workspace. Returns an empty string if the workspace was not properly created.</returns>
    std::string GetBaseDirectory() const;

    /// <summary>
    /// Import a directory into the workspace.
    /// </summary>
    /// <param name="source">The path to a directory to import into the workspace.</param>
    /// <param name="target_dir">A relative directory path in the workspace to copy the source directory to. Can be NULL to copy the source directory in the root of the workspace.</param>
    /// <returns>Returns true when the directory was successfully imported. Returns false otherwise.</returns>
    bool ImportDirectoryUtf8(const char * source, const char * target_dir = NULL);

    /// <summary>
    /// Import a file into the workspace.
    /// </summary>
    /// <param name="source">The path to a file to import into the workspace.</param>
    /// <param name="target_dir">A relative directory path in the workspace to copy the source file to. Can be NULL to copy the source file in the root of the workspace.</param>
    /// <returns>Returns true when the file was successfully imported. Returns false otherwise.</returns>
    bool ImportFileUtf8(const char * source, const char * target_dir = NULL);

    /// <summary>
    /// Import and rename a file into the workspace.
    /// </summary>
    /// <param name="source">The path to a file to import into the workspace.</param>
    /// <param name="target">A relative file path in the workspace to copy the source file to. Can be NULL to copy the source file in the root of the workspace.</param>
    /// <returns>Returns true when the file was successfully imported. Returns false otherwise.</returns>
    bool ImportAndRenameFileUtf8(const char * source, const char * target = NULL);

    /// <summary>
    /// Converts a relative path from the workspace into an absolute path.
    /// The given path does not have to be an existing file or directory within the workspace.
    /// </summary>
    /// <param name="path">A relative path in the workspace. The given path does not have to be an existing file or directory within the workspace.</param>
    /// <returns>Returns the absolute path from the given relative path.</returns>
    std::string GetFullPathUtf8(const char * path) const;

  private:
    void Init(const char * workspace);

  private:
    std::string mWorkspace;
  };

} //namespace shellanything

#endif //SA_TEST_WORKSPACE_H
