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

#include "ArgumentsHandler.h"
#include "rapidassist/process.h"
#include "rapidassist/environment.h"
#include "rapidassist/filesystem.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <Windows.h>
#undef GetEnvironmentVariable
#undef DeleteFile
#undef CreateDirectory
#undef CopyFile
#undef CreateFile
#undef GetCurrentDirectory


namespace shellanything
{

  //https://stackoverflow.com/questions/8046097/how-to-check-if-a-process-has-the-administrative-rights
  bool IsProcessElevated()
  {
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if( OpenProcessToken( GetCurrentProcess( ),TOKEN_QUERY,&hToken ) )
    {
      TOKEN_ELEVATION Elevation;
      DWORD cbSize = sizeof( TOKEN_ELEVATION );
      if( GetTokenInformation( hToken, TokenElevation, &Elevation, sizeof( Elevation ), &cbSize ) )
      {
        fRet = Elevation.TokenIsElevated;
      }
    }
    if( hToken )
    {
      CloseHandle( hToken );
    }
    return (fRet == TRUE);
  }

  //http://blog.aaronballman.com/2011/08/how-to-check-access-rights/
  bool CanAccessFolder( LPCTSTR folderName, DWORD genericAccessRights )
  {
    bool bRet = false;
    DWORD length = 0;
    if (!::GetFileSecurity( folderName, OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, NULL, NULL, &length ) && ERROR_INSUFFICIENT_BUFFER == ::GetLastError())
    {
      PSECURITY_DESCRIPTOR security = static_cast< PSECURITY_DESCRIPTOR >( ::malloc( length ) );
      if (security && ::GetFileSecurity( folderName, OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, security, length, &length ))
      {
        HANDLE hToken = NULL;
        if (::OpenProcessToken( ::GetCurrentProcess(), TOKEN_IMPERSONATE | TOKEN_QUERY | TOKEN_DUPLICATE | STANDARD_RIGHTS_READ, &hToken ))
        {
          HANDLE hImpersonatedToken = NULL;
          if (::DuplicateToken( hToken, SecurityImpersonation, &hImpersonatedToken ))
          {
            GENERIC_MAPPING mapping = { 0xFFFFFFFF };
            PRIVILEGE_SET privileges = { 0 };
            DWORD grantedAccess = 0, privilegesLength = sizeof( privileges );
            BOOL result = FALSE;

            mapping.GenericRead = FILE_GENERIC_READ;
            mapping.GenericWrite = FILE_GENERIC_WRITE;
            mapping.GenericExecute = FILE_GENERIC_EXECUTE;
            mapping.GenericAll = FILE_ALL_ACCESS;

            ::MapGenericMask( &genericAccessRights, &mapping );
            if (::AccessCheck( security, hImpersonatedToken, genericAccessRights, &mapping, &privileges, &privilegesLength, &grantedAccess, &result ))
            {
              bRet = (result == TRUE);
            }
            ::CloseHandle( hImpersonatedToken );
          }
          ::CloseHandle( hToken );
        }
        ::free( security );
      }
    }

    return bRet;
  }

  int PrintProcessSettings(int argc, char **argv)
  {
    std::string process_path = ra::process::GetCurrentProcessPath();
    ra::process::processid_t process_id = ra::process::GetCurrentProcessId();
    std::string process_current_directory = ra::filesystem::GetCurrentDirectory();
    int process_architecture = (ra::environment::IsProcess64Bit() ? 64 : 86);
    int process_elevated = (IsProcessElevated() ? 1 : 0);
    const char * build_configuration = (ra::environment::IsConfigurationDebug() ? "debug" : "release");
    std::string env_temp = ra::environment::GetEnvironmentVariable("TEMP");
    bool has_windows_write_access = CanAccessFolder("C:\\Windows", GENERIC_WRITE);

    // print to the console
    printf("process.arguments.argc=%d\n", argc);
    for(int i=0; i<argc; i++)
      printf("process.arguments.argv[%d]=%s\n", i, argv[i]);
    printf("process.path=%s\n", process_path.c_str());
    printf("process.pid=%d\n", process_id);
    printf("process.current_dir=%s\n", process_current_directory.c_str());
    printf("process.architecture=x%d\n", process_architecture);
    printf("process.elevated=%d\n", process_elevated);
    printf("build.configuration=%s\n", build_configuration);
    printf("env.temp=%s\n", env_temp.c_str());
    printf("has_windows_write_access=%d\n", (int)has_windows_write_access);

    // print to a file
    FILE * f = fopen("sa.tests.ProcessSettings.txt", "w");
    if (!f)
      return 1;

    printf("process.arguments.argc=%d\n", argc);
    for(int i=0; i<argc; i++)
      fprintf(f, "process.arguments.argv[%d]=%s\n", i, argv[i]);
    fprintf(f, "process.path=%s\n", process_path.c_str());
    fprintf(f, "process.pid=%d\n", process_id);
    fprintf(f, "process.current_dir=%s\n", process_current_directory.c_str());
    fprintf(f, "process.architecture=x%d\n", process_architecture);
    fprintf(f, "process.elevated=%d\n", process_elevated);
    fprintf(f, "build.configuration=%s\n", build_configuration);
    fprintf(f, "env.temp=%s\n", env_temp.c_str());
    fprintf(f, "has_windows_write_access=%d\n", (int)has_windows_write_access);

    fclose(f);

    return 0;
  }

} //namespace shellanything
