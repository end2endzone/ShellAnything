@echo off

:: Validate appveyor's environment
if "%APPVEYOR_BUILD_FOLDER%"=="" (
  echo Please define 'APPVEYOR_BUILD_FOLDER' environment variable.
  exit /B 1
)

set LIBMAGIC_ROOT=%APPVEYOR_BUILD_FOLDER%\third_parties\libmagic
set LIBMAGIC_DIR=%LIBMAGIC_ROOT%\install

set PCRE2_ROOT=%LIBMAGIC_ROOT%\pcre2
set PCRE2_INSTALL_DIR=%PCRE2_ROOT%\install_dir

echo ============================================================================
echo Cloning libmagic into %APPVEYOR_BUILD_FOLDER%\third_parties\libmagic
echo ============================================================================
mkdir %APPVEYOR_BUILD_FOLDER%\third_parties >NUL 2>NUL
cd %APPVEYOR_BUILD_FOLDER%\third_parties
@REM git clone --recurse-submodules "https://github.com/julian-r/file-windows" libmagic
git clone --recurse-submodules "https://github.com/Cirn09/file-windows" libmagic

cd libmagic
echo.

@REM echo Checking out version 5.38...
@REM git -c advice.detachedHead=false checkout v5.38
@REM echo.

echo ============================================================================
echo Preparing...
echo ============================================================================
cd /d %PCRE2_ROOT%
mkdir build >NUL 2>NUL
cd build
cmake -Wno-dev -DCMAKE_GENERATOR_PLATFORM=%Platform% -T %PlatformToolset% -DCMAKE_INSTALL_PREFIX="%PCRE2_INSTALL_DIR%" ..
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build . --config %Configuration%
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --install . --config %CONFIGURATION%
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Compiling...
echo ============================================================================

cd /d %LIBMAGIC_ROOT%
mkdir build >NUL 2>NUL
cd build
cmake -Wno-dev -DCMAKE_GENERATOR_PLATFORM=%Platform% -T %PlatformToolset% -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=%CONFIGURATION% -DCMAKE_INSTALL_PREFIX="%libmagic_DIR%" ..
@REM cmake -G "%GENERATOR%" -DCMAKE_BUILD_TYPE=%CONFIGURATION% ..
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build . --config %Configuration%
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing into %libmagic_DIR%
echo ============================================================================
cmake --build . --config %Configuration% --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
