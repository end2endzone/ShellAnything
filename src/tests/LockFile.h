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

#ifndef SA_TEST_LOCKFILE_H
#define SA_TEST_LOCKFILE_H

#include <stdio.h>

namespace shellanything
{

  /// <summary>
  /// Open and lock a file for exclusive access.
  /// </summary>
  /// <param name="path">The path to the file to open and lock.</param>
  /// <param name="mode">The file open mode. Follows fopen() standard.</param>
  /// <returns>Returns a valid FILE pointer when successful. Returns a NULL pointer otherwise.</returns>
  FILE * fopen_lock(const char* path, const char* mode);

  /// <summary>
  /// Close a locked for exclusive access a file.
  /// </summary>
  /// <param name="file">The file handle.</param>
  void fclose_lock(FILE* file);

  /// <summary>
  /// An opaque file lock handle
  /// </summary>
  struct FILE_LOCK_HANDLE;

  /// <summary>
  /// Open and lock a file for exclusive access.
  /// </summary>
  /// <param name="handle">A opaque file handle.</param>
  /// <returns>Returns true when file lock is successful. Returns a false otherwise.</returns>
  bool file_lock(const char* path, FILE_LOCK_HANDLE** handle);

  /// <summary>
  /// Close a locked file.
  /// </summary>
  /// <param name="handle">A opaque file handle.</param>
  /// <returns>Returns true when file lock is successful. Returns a false otherwise.</returns>
  bool file_unlock(FILE_LOCK_HANDLE** handle);

} //namespace shellanything

#endif //SA_TEST_LOCKFILE_H
