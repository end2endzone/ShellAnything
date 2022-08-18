@echo off

set SHELLANYTHING_INSTALL_DIR=C:\Program Files\ShellAnything-0.8.0-win64
set PLUGIN_BUILD_DIR=%USERPROFILE%\Documents\sa_plugin_demo_build
set PLUGIN_INSTALL_DIRECTORY=%USERPROFILE%\ShellAnything\plugins\sa_plugin_demo
set PLUGIN_CONFIGURATION=Release

echo Create build directory...
mkdir "%PLUGIN_BUILD_DIR%" >NUL 2>NUL
cd /d "%PLUGIN_BUILD_DIR%"
if %errorlevel% neq 0 echo Failed creating build directory "%PLUGIN_BUILD_DIR%" && exit /b %errorlevel%
echo done.
echo.

echo Create project files...
cmake -DCMAKE_INSTALL_PREFIX="%PLUGIN_INSTALL_DIRECTORY%" -DCMAKE_PREFIX_PATH="%SHELLANYTHING_INSTALL_DIR%" "%~dp0"
if %errorlevel% neq 0 exit /b %errorlevel%
echo done.
echo.

echo Building plugin...
cmake --build . --config %PLUGIN_CONFIGURATION% -- -maxcpucount /m
if %errorlevel% neq 0 echo Failed to build source code && exit /b %errorlevel%
echo done.
echo.

echo Installing plugin...
cmake --build . --config %PLUGIN_CONFIGURATION% --target INSTALL
if %errorlevel% neq 0 echo Failed to install plugin in directory "%PLUGIN_INSTALL_DIRECTORY%" && exit /b %errorlevel%
echo done.
echo.

cd /d "%~dp0"

pause
