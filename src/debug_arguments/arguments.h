#pragma once

#include <string>
#include <tchar.h>

static const std::wstring EMPTY_WIDE_STRING;
static const std::string  EMPTY_ANSI_STRING;

#ifdef UNICODE
typedef std::wstring tstring_t;
#else
typedef std::string tstring_t;
#endif

void ReadCommandLineArguments(tstring_t & str);
