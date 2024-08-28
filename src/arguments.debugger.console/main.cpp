#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <string>
#include "framework.h"
#include "../arguments.debugger.window/arguments.h"

#include "rapidassist/console.h"

// https://stackoverflow.com/questions/33836706/what-are-tchar-strings-and-the-a-or-w-version-of-win32-api-functions

#if defined(UNICODE)
std::wistream& tcin = std::wcin;
std::wostream& tcout = std::wcout;
#else
std::istream& tcin = std::cin;
std::ostream& tcout = std::cout;
#endif

// void ReadCommandLineArguments(tstring_t & str);

int _tmain(int argc, _TCHAR* argv[])
{
  _setmode(_fileno(stdout), _O_U16TEXT);

  tstring_t arguments_desc;
  ReadCommandLineArguments(arguments_desc);

  tcout << arguments_desc << "\n";

  tcout << "Press any key to continue . . .";
  int tmp = ra::console::WaitKeyPress();
  tcout << "\n";

  return 0;
}
