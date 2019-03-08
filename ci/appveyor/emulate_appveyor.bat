@echo off

cd /d %~dp0
cd ..\..
set APPVEYOR_BUILD_FOLDER=%cd%
echo Emulating appveyor's environment.
echo APPVEYOR_BUILD_FOLDER set to '%APPVEYOR_BUILD_FOLDER%'
echo.

:: Return back to original folder
cd /d %~dp0

:: Leave the command prompt open
cmd.exe /k echo AppVeyor ready...
