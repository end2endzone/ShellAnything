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
set CMAKE_INSTALL_PREFIX=%PRODUCT_SOURCE_DIR%\third_parties\tinyxml2\install
set CMAKE_PREFIX_PATH=
set CMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%;

echo ============================================================================
echo Cloning tinyxml2 into %PRODUCT_SOURCE_DIR%\third_parties\tinyxml2
echo ============================================================================
mkdir %PRODUCT_SOURCE_DIR%\third_parties >NUL 2>NUL
cd %PRODUCT_SOURCE_DIR%\third_parties
git clone "https://github.com/leethomason/tinyxml2.git"
cd tinyxml2
echo.

echo Checking out version 6.2.0...
git -c advice.detachedHead=false checkout 6.2.0
echo.

echo ============================================================================
echo Generating tinyxml2...
echo ============================================================================
mkdir build >NUL 2>NUL
cd build
cmake -Wno-dev -DCMAKE_GENERATOR_PLATFORM=%PLATFORM% -T %PLATFORMTOOLSET% -DCMAKE_INSTALL_PREFIX="%CMAKE_INSTALL_PREFIX%" -DCMAKE_PREFIX_PATH="%CMAKE_PREFIX_PATH%" -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_STATIC_LIBS:BOOL=ON -DBUILD_TESTS:BOOL=OFF ..
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Compiling tinyxml2...
echo ============================================================================
cmake --build . --config %CONFIGURATION% -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing tinyxml2 into %CMAKE_INSTALL_PREFIX%
echo ============================================================================
cmake --build . --config %CONFIGURATION% --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

::Return to launch folder
cd /d "%~dp0"
