@echo off

:: Validate appveyor's environment
if "%APPVEYOR_BUILD_FOLDER%"=="" (
  echo Please define 'APPVEYOR_BUILD_FOLDER' environment variable.
  exit /B 1
)

echo =======================================================================
echo Testing project
echo =======================================================================
cd /d %APPVEYOR_BUILD_FOLDER%\build\bin\%Configuration%
if "%Configuration%" == "Debug" (
  shellanything_unittest-d.exe
) else (
  shellanything_unittest.exe
)

::reset error in case of test case fail
exit /b 0
