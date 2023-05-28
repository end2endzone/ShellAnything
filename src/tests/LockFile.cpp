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
#include <stdlib.h>
#include <share.h>

#ifdef _WIN32
#include <Windows.h>
#include "rapidassist/undef_windows_macros.h"
#else
 //TODO: complete with a valid implementation
#endif

namespace shellanything
{
  struct FILE_LOCK_HANDLE
  {
#ifdef _WIN32
    HANDLE hFile;
    BOOL bLocked;
#else
#endif
  };

  typedef FILE_LOCK_HANDLE* FILE_LOCK_HANDLE_PTR;


  FILE* fopen_lock(const char* path, const char* mode)
  {
#ifdef _WIN32
    // lock the file with _fsopen()
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fsopen-wfsopen?redirectedfrom=MSDN&view=msvc-170
    FILE* f = _fsopen(path, mode, _SH_DENYRW);
    return f;
#else
    return false;
#endif
  }

  void fclose_lock(FILE* file)
  {
    fclose(file);
  }

  bool file_lock(const char* path, FILE_LOCK_HANDLE** handle)
  {
    if (handle == NULL)
      return false;

#ifdef _WIN32
    FILE_LOCK_HANDLE_PTR & pHandle = *handle;

    // allocate a new handle
    pHandle = new shellanything::FILE_LOCK_HANDLE;
    if (pHandle == NULL)
      return false;

    // initialize
    memset(pHandle, 0, sizeof(*pHandle));

    // lock the file
    // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-lockfile
    pHandle->hFile = CreateFileA(path, GENERIC_READ | GENERIC_WRITE | FILE_APPEND_DATA, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (pHandle->hFile == INVALID_HANDLE_VALUE)
      return false;
    pHandle->bLocked = LockFile(pHandle->hFile, 0, 0, 0, MAXDWORD);
    if (!pHandle->bLocked)
    {
      //std::string err_desc = ra::errors::GetLastErrorDescription();
      CloseHandle(pHandle->hFile);
      memset(pHandle, 0, sizeof(*pHandle));
    }
    return true;
#else
    return false;
#endif
  }

  bool file_unlock(FILE_LOCK_HANDLE** handle)
  {
    if (handle == NULL)
      return false;

#ifdef _WIN32
    FILE_LOCK_HANDLE_PTR& pHandle = *handle;

    // unlock the file
    BOOL bUnlocked = UnlockFile(pHandle->hFile, 0, 0, 0, MAXDWORD);
    if (!bUnlocked)
      return false;
    CloseHandle(pHandle->hFile);

    memset(pHandle, 0, sizeof(*pHandle));

    delete pHandle;
    pHandle = NULL;

    return true;
#else
    return false;
#endif
  }

} //namespace shellanything
