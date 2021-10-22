@echo off

echo Emulating appveyor's environment.

cd /d %~dp0
cd ..\..
set APPVEYOR=True
set APPVEYOR_BUILD_FOLDER=%cd%
set Configuration=Debug
set Platform=x64
set PlatformToolset=""

echo APPVEYOR_BUILD_FOLDER set to '%PRODUCT_SOURCE_DIR%'
echo.

:: Return back to original folder
cd /d %~dp0

:: Leave the command prompt open
cmd.exe /k echo AppVeyor ready...
