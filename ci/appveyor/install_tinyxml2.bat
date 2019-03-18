@echo off

:: Validate appveyor's environment
if "%APPVEYOR_BUILD_FOLDER%"=="" (
  echo Please define 'APPVEYOR_BUILD_FOLDER' environment variable.
  exit /B 1
)

set tinyxml2_DIR=%APPVEYOR_BUILD_FOLDER%\third_parties\tinyxml2\install
echo tinyxml2_DIR=%tinyxml2_DIR%

echo ============================================================================
echo Cloning TinyXml-2 into %APPVEYOR_BUILD_FOLDER%\third_parties\tinyxml2
echo ============================================================================
mkdir %APPVEYOR_BUILD_FOLDER%\third_parties >NUL 2>NUL
cd %APPVEYOR_BUILD_FOLDER%\third_parties
git clone "https://github.com/leethomason/tinyxml2.git"
cd tinyxml2
echo.

echo Checking out version 6.2.0...
git checkout 6.2.0
echo.

echo ============================================================================
echo Compiling...
echo ============================================================================
mkdir build >NUL 2>NUL
cd build
cmake -DCMAKE_INSTALL_PREFIX=%tinyxml2_DIR% -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_STATIC_LIBS:BOOL=ON -DBUILD_TESTS:BOOL=OFF ..
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build . --config %Configuration%
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing into %tinyxml2_DIR%
echo ============================================================================
cmake --build . --config %Configuration% --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
