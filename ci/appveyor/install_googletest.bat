@echo off

:: Validate appveyor's environment
if "%APPVEYOR_BUILD_FOLDER%"=="" (
  echo Please define 'APPVEYOR_BUILD_FOLDER' environment variable.
  exit /B 1
)

set GTEST_ROOT=%APPVEYOR_BUILD_FOLDER%\third_parties\googletest\install

echo ============================================================================
echo Cloning googletest into %APPVEYOR_BUILD_FOLDER%\third_parties\googletest
echo ============================================================================
mkdir %APPVEYOR_BUILD_FOLDER%\third_parties >NUL 2>NUL
cd %APPVEYOR_BUILD_FOLDER%\third_parties
git clone "https://github.com/google/googletest.git"
cd googletest
echo.

echo Checking out version 1.8.0...
git checkout release-1.8.0
echo.

echo ============================================================================
echo Compiling...
echo ============================================================================
mkdir build >NUL 2>NUL
cd build
cmake -DCMAKE_GENERATOR_PLATFORM=%Platform% -T %PlatformToolset% -DCMAKE_INSTALL_PREFIX=%GTEST_ROOT% -Dgtest_force_shared_crt=ON -DBUILD_GMOCK=OFF -DBUILD_GTEST=ON ..
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build . --config %Configuration%
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing into %GTEST_ROOT%
echo ============================================================================
cmake --build . --config %Configuration% --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
