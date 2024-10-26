@echo off

:: Validate administrative permissions...
:: https://stackoverflow.com/questions/4051883/batch-script-how-to-check-for-admin-rights
echo Administrative permissions required for unregistering the Shell Extension.
net session >nul 2>&1
if %errorLevel% == 0 (
  echo Found administrative permissions.
) else (
  echo Failure: Please open a command prompt with administrative permissions.
  pause
  exit /B 1
)

:: Unregistering the Shell Extension.
echo Proceeding with the Shell Extension unregistration.
cd /d %~dp0
if exist "sa.shellextension.dll" (
  regsvr32 /u sa.shellextension.dll
) else (
  regsvr32 /u sa.shellextension-d.dll
)

pause
