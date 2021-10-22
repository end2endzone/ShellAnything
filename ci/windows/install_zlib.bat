@echo off

:: Validate mandatory environment variables
if "%CONFIGURATION%"=="" (
  echo Please define 'Configuration' environment variable.
  exit /B 1
)
if "%PLATFORM%"=="" (
  echo Please define 'Platform' environment variable.
  exit /B 1
)

:: Set PRODUCT_SOURCE_DIR root directory
setlocal enabledelayedexpansion
if "%PRODUCT_SOURCE_DIR%"=="" (
  :: Delayed expansion is required within parentheses https://superuser.com/questions/78496/variables-in-batch-file-not-being-set-when-inside-if
  cd /d "%~dp0"
  cd ..\..
  set PRODUCT_SOURCE_DIR=!CD!
  cd ..\..
  echo PRODUCT_SOURCE_DIR set to '!PRODUCT_SOURCE_DIR!'.
)
endlocal & set PRODUCT_SOURCE_DIR=%PRODUCT_SOURCE_DIR%
echo.

:: zlib is a C library with a C api.
:: We do not have any plans on debugging the library,
:: so compiling in Debug mode is not required for this project.
:: Because the C api provides binary compatibility, this library
:: can be safely compiled in Release mode while ShellAnything
:: and other modules are compiled in Debug mode.
::
:: Forcing Configuration=Release for this build.
::

:: Prepare CMAKE parameters
set CMAKE_INSTALL_PREFIX=%PRODUCT_SOURCE_DIR%\third_parties\zlib\install
set CMAKE_PREFIX_PATH=
set CMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%;

echo ============================================================================
echo Cloning zlib into %PRODUCT_SOURCE_DIR%\third_parties\zlib
echo ============================================================================
mkdir %PRODUCT_SOURCE_DIR%\third_parties >NUL 2>NUL
cd %PRODUCT_SOURCE_DIR%\third_parties
git clone "https://github.com/madler/zlib.git"
cd zlib
echo.

echo Checking out version v1.2.11...
git -c advice.detachedHead=false checkout v1.2.11
echo.

echo ============================================================================
echo Generating zlib...
echo ============================================================================
mkdir build >NUL 2>NUL
cd build
cmake -Wno-dev -DCMAKE_GENERATOR_PLATFORM=%PLATFORM% -T %PLATFORMTOOLSET% -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX="%CMAKE_INSTALL_PREFIX%" -DCMAKE_PREFIX_PATH="%CMAKE_PREFIX_PATH%" ..
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Compiling zlib...
echo ============================================================================
cmake --build . --config Release -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing zlib into %CMAKE_INSTALL_PREFIX%
echo ============================================================================
cmake --build . --config Release --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

::Return to launch folder
cd /d "%~dp0"
