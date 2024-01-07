#pragma once

#include <string>
#include <vector>

typedef std::vector<std::string> Utf8FileList;

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
