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

#ifndef SA_SHELLEXTENSION_H
#define SA_SHELLEXTENSION_H

#include "stdafx.h"

#include <string>

#include "shellanything/version.h"
#include "shellanything/config.h"

#ifdef SA_ENABLE_ATTACH_HOOK_DEBUGGING
#define ATTACH_HOOK_DEBUGGING DebugHook(__FUNCTION__);
#else
#define ATTACH_HOOK_DEBUGGING ;
#endif // #ifdef SA_ENABLE_ATTACH_HOOK_DEBUGGING

std::string GuidToString(GUID guid);
std::string GuidToInterfaceName(GUID guid);
std::string GetProcessContextDesc();

std::string GetQueryContextMenuFlags(UINT flags);
std::string GetGetCommandStringFlags(UINT flags);

void LogEnvironment();

void DebugHook(const char* fname);

/// <summary>
/// Detect if the process loading the shell extension is Windows File Explorer.
/// </summary>
/// <returns>Returns true if the loading process is Windows File Explorer. Returns false otherwise.</returns>
bool IsFileExplorerProcess();

/// <summary>
/// Detect if the process loading the shell extension is Windows regsvr32 application.
/// </summary>
/// <returns>Returns true if the loading process is Windows regsvr32 application. Returns false otherwise.</returns>
bool IsRegsvr32Process();

#endif //SA_SHELLEXTENSION_H
