@echo off

:: Validate appveyor's environment
if "%APPVEYOR_BUILD_FOLDER%"=="" (
  echo Please define 'APPVEYOR_BUILD_FOLDER' environment variable.
  exit /B 1
)

:: zlib is a C library with a C api.
:: We do not have any plans on debugging the library,
:: so compiling in Debug mode is not required for this project.
:: Because the C api provides binary compatibility, this library
:: can be safely compiled in Release mode while ShellAnything
:: and other modules are compiled in Debug mode.
::
:: Forcing Configuration=Release for this build.
::

set zlib_DIR=%APPVEYOR_BUILD_FOLDER%\third_parties\zlib\install
echo zlib_DIR=%zlib_DIR%

echo ============================================================================
echo Cloning zlib into %APPVEYOR_BUILD_FOLDER%\third_parties\zlib
echo ============================================================================
mkdir %APPVEYOR_BUILD_FOLDER%\third_parties >NUL 2>NUL
cd %APPVEYOR_BUILD_FOLDER%\third_parties
git clone "https://github.com/madler/zlib.git"
cd zlib
echo.

echo Checking out version v1.2.11...
git -c advice.detachedHead=false checkout v1.2.11
echo.

echo ============================================================================
echo Compiling zlib...
echo ============================================================================
mkdir build >NUL 2>NUL
cd build
cmake -Wno-dev -DCMAKE_GENERATOR_PLATFORM=%Platform% -T %PlatformToolset% -DCMAKE_INSTALL_PREFIX=%zlib_DIR% ..
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build . --config Release
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing zlib into %zlib_DIR%
echo ============================================================================
cmake --build . --config Release --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
