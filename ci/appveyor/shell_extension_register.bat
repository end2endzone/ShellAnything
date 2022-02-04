@echo off

:: Validate mandatory environment variables
if "%CONFIGURATION%"=="" (
  echo Please define 'Configuration' environment variable.
  exit /B 1
)
if "%PLATFORM%"=="" (
  echo Please define 'Platform' environment variable.
  exit /B 1
)

:: Set PRODUCT_SOURCE_DIR root directory
setlocal enabledelayedexpansion
if "%PRODUCT_SOURCE_DIR%"=="" (
  :: Delayed expansion is required within parentheses https://superuser.com/questions/78496/variables-in-batch-file-not-being-set-when-inside-if
  cd /d "%~dp0"
  cd ..\..
  set PRODUCT_SOURCE_DIR=!CD!
  cd ..\..
  echo PRODUCT_SOURCE_DIR set to '!PRODUCT_SOURCE_DIR!'.
)
endlocal & set PRODUCT_SOURCE_DIR=%PRODUCT_SOURCE_DIR%
echo.

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
cd /d "%PRODUCT_SOURCE_DIR%\build\bin\%CONFIGURATION%"
if "%CONFIGURATION%" == "Debug" (
  regsvr32 sa.shellextension-d.dll
) else (
  regsvr32 sa.shellextension.dll
)

::Return to launch folder
cd /d %~dp0
