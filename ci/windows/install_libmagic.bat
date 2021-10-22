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

:: libmagic is a C library with a C api.
:: We do not have any plans on debugging the library,
:: so compiling in Debug mode is not required for this project.
:: Because the C api provides binary compatibility, this library
:: can be safely compiled in Release mode while ShellAnything
:: and other modules are compiled in Debug mode.
::
:: Forcing Configuration=Release for this build.
::

set LIBMAGIC_ROOT=%PRODUCT_SOURCE_DIR%\third_parties\libmagic
set LIBMAGIC_DIR=%LIBMAGIC_ROOT%\install

set PCRE2_ROOT=%LIBMAGIC_ROOT%\pcre2
set PCRE2_INSTALL_DIR=%PCRE2_ROOT%\install_dir

:: Prepare CMAKE parameters
set CMAKE_INSTALL_PREFIX=%PRODUCT_SOURCE_DIR%\third_parties\libmagic\install
set CMAKE_PREFIX_PATH=
set CMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%;%PRODUCT_SOURCE_DIR%\third_parties\zlib\install;

echo ============================================================================
echo Cloning libmagic into %PRODUCT_SOURCE_DIR%\third_parties\libmagic
echo ============================================================================
mkdir %PRODUCT_SOURCE_DIR%\third_parties >NUL 2>NUL
cd %PRODUCT_SOURCE_DIR%\third_parties
@REM git clone --recurse-submodules "https://github.com/julian-r/file-windows" libmagic
git clone --recurse-submodules "https://github.com/Cirn09/file-windows" libmagic
cd libmagic
echo.

@REM echo Checking out version 5.38...
@REM git -c advice.detachedHead=false checkout v5.38
@REM echo.

echo ============================================================================
echo Generating PCRE2...
echo ============================================================================
cd /d %PCRE2_ROOT%
mkdir build >NUL 2>NUL
cd build
cmake -Wno-dev -DCMAKE_GENERATOR_PLATFORM=%PLATFORM% -T %PLATFORMTOOLSET% -DCMAKE_INSTALL_PREFIX="%PCRE2_INSTALL_DIR%" -DCMAKE_PREFIX_PATH="%PRODUCT_SOURCE_DIR%\third_parties\zlib\install" ..
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Compiling PCRE2...
echo ============================================================================
cd /d %PCRE2_ROOT%\build
cmake --build . --config Release -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing PCRE2 into %PCRE2_INSTALL_DIR%
echo ============================================================================
cd /d %PCRE2_ROOT%\build
cmake --install . --config Release
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Generating libmagic...
echo ============================================================================
cd /d %LIBMAGIC_ROOT%
mkdir build >NUL 2>NUL
cd build
cmake -Wno-dev -DCMAKE_GENERATOR_PLATFORM=%PLATFORM% -T %PLATFORMTOOLSET% -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=%CONFIGURATION% -DCMAKE_INSTALL_PREFIX="%libmagic_DIR%" -DCMAKE_PREFIX_PATH="%PCRE2_INSTALL_DIR%;%PRODUCT_SOURCE_DIR%\third_parties\zlib\install" ..
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Compiling libmagic...
echo ============================================================================
cmake --build . --config Release -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing libmagic into %CMAKE_INSTALL_PREFIX%
echo ============================================================================
cmake --install . --config Release
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

::Return to launch folder
cd /d "%~dp0"
