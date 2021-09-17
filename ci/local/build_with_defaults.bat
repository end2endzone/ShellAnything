@echo off

echo Emulating appveyor's environment.

cd /d %~dp0
cd ..\..
set APPVEYOR=True
set APPVEYOR_BUILD_FOLDER=%cd%
set Configuration=Release
set Platform=x64
set PlatformToolset=""

echo APPVEYOR_BUILD_FOLDER set to '%APPVEYOR_BUILD_FOLDER%'
echo.

:: Return back to original folder
cd /d %~dp0

:: Call appveyor's scripts one by one.
call %APPVEYOR_BUILD_FOLDER%\ci\appveyor\install_googletest.bat
if %errorlevel% neq 0 exit /b %errorlevel%
call %APPVEYOR_BUILD_FOLDER%\ci\appveyor\install_tinyxml2.bat
if %errorlevel% neq 0 exit /b %errorlevel%
call %APPVEYOR_BUILD_FOLDER%\ci\appveyor\install_rapidassist.bat
if %errorlevel% neq 0 exit /b %errorlevel%
call %APPVEYOR_BUILD_FOLDER%\ci\appveyor\install_glog.bat
if %errorlevel% neq 0 exit /b %errorlevel%
call %APPVEYOR_BUILD_FOLDER%\ci\appveyor\install_grip.bat
if %errorlevel% neq 0 exit /b %errorlevel%
call %APPVEYOR_BUILD_FOLDER%\ci\appveyor\install_libmagic.bat

call %APPVEYOR_BUILD_FOLDER%\ci\appveyor\build_library.bat
if %errorlevel% neq 0 exit /b %errorlevel%

:: Press a key to continue
pause
