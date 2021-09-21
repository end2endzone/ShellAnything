@echo off

:: Validate appveyor's environment
if "%APPVEYOR_BUILD_FOLDER%"=="" (
  echo Please define 'APPVEYOR_BUILD_FOLDER' environment variable.
  exit /B 1
)

set GTEST_ROOT=%APPVEYOR_BUILD_FOLDER%\third_parties\googletest\install
set rapidassist_DIR=%APPVEYOR_BUILD_FOLDER%\third_parties\RapidAssist\install
set tinyxml2_DIR=%APPVEYOR_BUILD_FOLDER%\third_parties\tinyxml2\install
set glog_DIR=%APPVEYOR_BUILD_FOLDER%\third_parties\glog\install_dir
set libmagic_DIR=%APPVEYOR_BUILD_FOLDER%\third_parties\libmagic\install
set zlib_DIR=%APPVEYOR_BUILD_FOLDER%\third_parties\zlib\install
set INSTALL_LOCATION=%APPVEYOR_BUILD_FOLDER%\install

echo ============================================================================
echo Generating ShellAnything...
echo ============================================================================
cd /d %APPVEYOR_BUILD_FOLDER%
mkdir build >NUL 2>NUL
cd build
cmake -Wno-dev -DCMAKE_GENERATOR_PLATFORM=%Platform% -T %PlatformToolset% -DCMAKE_INSTALL_PREFIX=%INSTALL_LOCATION% -DSHELLANYTHING_BUILD_TEST=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_PREFIX_PATH="%zlib_DIR%" ..
if %errorlevel% neq 0 exit /b %errorlevel%

echo ============================================================================
echo Compiling ShellAnything...
echo ============================================================================
cmake --build . --config %Configuration%
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing ShellAnything into %INSTALL_LOCATION%
echo ============================================================================
cmake --build . --config %Configuration% --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Creating ShellAnything install package
echo ============================================================================
cmake --build . --config %Configuration% --target PACKAGE
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

::Delete all temporary environment variable created
set GTEST_ROOT=
set rapidassist_DIR=
set tinyxml2_DIR=
set glog_DIR=
set LIBMAGIC_DIR=
set INSTALL_LOCATION=

::Return to launch folder
cd /d %~dp0
