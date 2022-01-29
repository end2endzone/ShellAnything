@echo off

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
cd /d %~dp0
if exist "sa.shellextension.dll" (
  regsvr32 sa.shellextension.dll
) else (
  regsvr32 sa.shellextension-d.dll
)

pause
