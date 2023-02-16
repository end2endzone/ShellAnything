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

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

// Windows Header Files:
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#include <unknwn.h>
#include <Shlwapi.h>
#include <shellapi.h>
#include <atlbase.h>
#include <ShObjIdl.h>
#include <ShlObj.h>
#include <ShlGuid.h>
#include <initguid.h>
#include <CmnQuery.h>
#include <DSQuery.h>
#include <DSClient.h>

void DllAddRef();
void DllRelease();

#define INITGUID
#include <guiddef.h>

// Shell extension CLSID / GUID
// {B0D35103-86A1-471C-A653-E130E3439A3B}
DEFINE_GUID(CLSID_ShellAnythingMenu, 0xb0d35103, 0x86a1, 0x471c, 0xa6, 0x53, 0xe1, 0x30, 0xe3, 0x43, 0x9a, 0x3b);

static const char* ShellExtensionClassName = "ShellExtension.ShellAnything"; //no space in string
static const char* ShellExtensionDescription = "ShellAnything Class";

// Debugging support
//#define SA_ENABLE_ATTACH_HOOK_DEBUGGING
//#define SA_ENABLE_SCOPE_DEBUGGING
