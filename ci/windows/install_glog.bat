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

:: Prepare CMAKE parameters
set CMAKE_INSTALL_PREFIX=%PRODUCT_SOURCE_DIR%\third_parties\glog\install_dir
set CMAKE_PREFIX_PATH=
set CMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%;

echo ============================================================================
echo Cloning GLOG into %PRODUCT_SOURCE_DIR%\third_parties\glog
echo ============================================================================
mkdir %PRODUCT_SOURCE_DIR%\third_parties >NUL 2>NUL
cd %PRODUCT_SOURCE_DIR%\third_parties
git clone "https://github.com/google/glog.git"
cd glog
echo.

echo Checking out version v0.4.0...
git -c advice.detachedHead=false checkout v0.4.0
echo.

echo ============================================================================
echo Fixing code based on pull request 197...
echo ============================================================================
@echo on
cscript //nologo "%~dpn0.vbs" "base_filename_+filename_extension_+" "base_filename_+ time_pid_string+" "%PRODUCT_SOURCE_DIR%\third_parties\glog\src\logging.cc"
cscript //nologo "%~dpn0.vbs" "                   time_pid_string;" "            filename_extension_;" "%PRODUCT_SOURCE_DIR%\third_parties\glog\src\logging.cc"
@echo off
echo.

echo ============================================================================
echo Generating GLOG...
echo ============================================================================
mkdir build_dir >NUL 2>NUL
cd build_dir
cmake -Wno-dev -DCMAKE_GENERATOR_PLATFORM=%PLATFORM% -T %PLATFORMTOOLSET% -DCMAKE_INSTALL_PREFIX="%CMAKE_INSTALL_PREFIX%" -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_STATIC_LIBS:BOOL=OFF -DCMAKE_PREFIX_PATH="%CMAKE_PREFIX_PATH%" ..
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Compiling GLOG...
echo ============================================================================
cmake --build . --config %CONFIGURATION% -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing GLOG into %CMAKE_INSTALL_PREFIX%
echo ============================================================================
cmake --build . --config %CONFIGURATION% --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

::Return to launch folder
cd /d "%~dp0"
