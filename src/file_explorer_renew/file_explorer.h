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

#pragma once

#include "user_feedback.h"

#include <string>
#include <vector>

typedef std::vector<std::string> Utf8FileList;

/// <summary>
/// Search a given path within a file list.
/// </summary>
/// <param name="list">The list of path to search into.</param>
/// <param name="path">The value to seach for.</param>
/// <returns>Returns true when given path was found. Returns false otherwise.</returns>
bool FindPath(const Utf8FileList& list, const std::string& path);

/// <summary>
/// Get the list of paths for all opened File Explorer windows.
/// The paths are returned in utf8 format.
/// </summary>
/// <param name="files">The output file path list in utf8 format.</param>
/// <returns>Returns true when the function has succeeded. Returns false otherwise.</returns>
bool GetFileExplorerWindowPaths(Utf8FileList& files);

/// <summary>
/// Open File Explorer to the given location.
/// </summary>
/// <param name="path">The path location in utf8 format.</param>
/// <returns>Returns true when the function has succeeded. Returns false otherwise.</returns>
bool OpenFileExplorerWindow(const std::string& path);

/// <summary>
/// Test to open directory unicode directory `D:\Ψ`.
/// </summary>
void TestOpenFolderUnicode();

/// <summary>
/// Print the list of given paths to a string
/// </summary>
/// <param name="paths">The list of paths to print.</param>
/// <param name="str">The output string.</param>
void PrintPathsToString(const Utf8FileList& paths, tstring_t& str);

/// <summary>
/// Get the path (utf8) of File Explorer executable.
/// </summary>
/// <returns>Returns a valid string when the operation is succesful. Returns an empty string on error.</returns>
std::string GetFileExplorerExecPath();

/// <summary>
/// Show a MessageBox centered on parent window.
/// </summary>
/// <returns>Returns true when the operation is succesful. Returns false otherwise.</returns>
bool KillFileExplorerProcesses();

/// <summary>
/// Find elements from a previous call to GetFileExplorerWindowPaths() that are missing from a recent call to GetFileExplorerWindowPaths().
/// </summary>
/// <param name="previous">The paths found from a previous call to GetFileExplorerWindowPaths().</param>
/// <param name="output">The out paths that are missing.</param>
/// <returns>Returns true when the operation is succesful. Returns false otherwise.</returns>
bool FindMissingElements(const Utf8FileList& previous, Utf8FileList & output);
