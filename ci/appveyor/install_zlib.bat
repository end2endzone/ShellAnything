@echo off

:: Validate appveyor's environment
if "%APPVEYOR_BUILD_FOLDER%"=="" (
  echo Please define 'APPVEYOR_BUILD_FOLDER' environment variable.
  exit /B 1
)

:: Call matching script for windows
call "%APPVEYOR_BUILD_FOLDER%\ci\windows\%~n0.bat"
if %errorlevel% neq 0 exit /b %errorlevel%
