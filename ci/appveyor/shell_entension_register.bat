@echo off

:: Validate appveyor's environment
if "%APPVEYOR_BUILD_FOLDER%"=="" (
  echo Please define 'APPVEYOR_BUILD_FOLDER' environment variable.
  exit /B 1
)

:: Validate administrative permissions...
:: https://stackoverflow.com/questions/4051883/batch-script-how-to-check-for-admin-rights
echo Administrative permissions required for registering the Shell Extension.
net session >nul 2>&1
if %errorLevel% == 0 (
  echo Found administrative permissions.
) else (
  echo Failure: Please open a command prompt with administrative permissions.
  pause
  exit /B 1
)

:: Registering the Shell Extension.
echo Proceeding with the Shell Extension registration.
cd /d "%APPVEYOR_BUILD_FOLDER%\build\bin\%Configuration%"
if "%Configuration%" == "Debug" (
  regsvr32 shellext-d.dll
) else (
  regsvr32 shellext.dll
)

::Return to launch folder
cd /d %~dp0
