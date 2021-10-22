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
set CMAKE_INSTALL_PREFIX=%PRODUCT_SOURCE_DIR%\third_parties\googletest\install
set CMAKE_PREFIX_PATH=
set CMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%;

echo ============================================================================
echo Cloning googletest into %PRODUCT_SOURCE_DIR%\third_parties\googletest
echo ============================================================================
mkdir "%PRODUCT_SOURCE_DIR%\third_parties" >NUL 2>NUL
cd "%PRODUCT_SOURCE_DIR%\third_parties"
git clone "https://github.com/google/googletest.git"
cd googletest
echo.

echo Checking out version 1.8.0...
git -c advice.detachedHead=false checkout release-1.8.0
echo.

echo ============================================================================
echo Generating googletest...
echo ============================================================================
mkdir build >NUL 2>NUL
cd build
cmake -Wno-dev -DCMAKE_GENERATOR_PLATFORM=%PLATFORM% -T %PLATFORMTOOLSET% -Dgtest_force_shared_crt=ON -DBUILD_GMOCK=OFF -DBUILD_GTEST=ON -DCMAKE_INSTALL_PREFIX="%CMAKE_INSTALL_PREFIX%" -DCMAKE_PREFIX_PATH="%CMAKE_PREFIX_PATH%" ..
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo.
echo.
echo ==================================================================================
echo Patching googletest to silence MSVC warning C4996 about deprecated 'std::tr1' namespace
echo ==================================================================================
echo add_definitions(-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)>>..\googletest\CMakeLists.txt
echo add_definitions(-D_CRT_SECURE_NO_WARNINGS)>>..\googletest\CMakeLists.txt
cmake .. 1>NUL 2>NUL
echo Done patching.
echo.
echo.

:: Continue with compilation
echo ============================================================================
echo Compiling googletest...
echo ============================================================================
cmake --build . --config %CONFIGURATION% -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing googletest into %CMAKE_INSTALL_PREFIX%
echo ============================================================================
cmake --build . --config %CONFIGURATION% --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

::Return to launch folder
cd /d "%~dp0"
