@echo off

:: Validate appveyor's environment
if "%APPVEYOR_BUILD_FOLDER%"=="" (
  echo Please define 'APPVEYOR_BUILD_FOLDER' environment variable.
  exit /B 1
)

set glog_DIR=%APPVEYOR_BUILD_FOLDER%\third_parties\glog\install_dir
echo glog_DIR=%glog_DIR%

echo ============================================================================
echo Cloning GLOG into %APPVEYOR_BUILD_FOLDER%\third_parties\glog
echo ============================================================================
mkdir %APPVEYOR_BUILD_FOLDER%\third_parties >NUL 2>NUL
cd %APPVEYOR_BUILD_FOLDER%\third_parties
git clone "https://github.com/google/glog.git"
cd glog
echo.

echo Checking out version v0.4.0...
git checkout v0.4.0
echo.

echo ============================================================================
echo Fixing code based on pull request 197...
echo ============================================================================
echo cscript //nologo "%~dpn0.vbs" "base_filename_+filename_extension_+" "base_filename_+ time_pid_string+" "%APPVEYOR_BUILD_FOLDER%\third_parties\glog\src\logging.cc"
echo cscript //nologo "%~dpn0.vbs" "                   time_pid_string;" "            filename_extension_;" "%APPVEYOR_BUILD_FOLDER%\third_parties\glog\src\logging.cc"
cscript //nologo "%~dpn0.vbs" "base_filename_+filename_extension_+" "base_filename_+ time_pid_string+" "%APPVEYOR_BUILD_FOLDER%\third_parties\glog\src\logging.cc"
cscript //nologo "%~dpn0.vbs" "                   time_pid_string;" "            filename_extension_;" "%APPVEYOR_BUILD_FOLDER%\third_parties\glog\src\logging.cc"
echo.

echo ============================================================================
echo Compiling...
echo ============================================================================
mkdir build_dir >NUL 2>NUL
cd build_dir
cmake -DCMAKE_INSTALL_PREFIX=%glog_DIR% -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_STATIC_LIBS:BOOL=ON ..
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build . --config %Configuration%
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing into %glog_DIR%
echo ============================================================================
cmake --build . --config %Configuration% --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
