@echo off

:: Install CMake.
:: https://cmake.org/download/
:: https://github.com/Kitware/CMake/releases/latest

echo Installing CMake...
set CMAKE_MSI_URL=https://github.com/Kitware/CMake/releases/download/v3.29.7/cmake-3.29.7-windows-x86_64.msi
set CMAKE_MSI_FILE=%TEMP%\cmake-3.29.7-windows-x86_64.msi
set CMAKE_MSI_INSTALL_DIR=C:\Program Files\CMake

:: Validate if python launcher is installed
where py.exe >NUL 2>NUL
if errorlevel 1 (
  echo Command failed. Please install python and python launcher to continue.
  exit /B %errorlevel%
)
echo Found python interpreter
python --version

:: Validate if pip is installed
where pip.exe >NUL 2>NUL
if errorlevel 1 (
  echo Command failed. Please install pip ^(Package Installer Python^) to continue.
  exit /B %errorlevel%
)

:: Download installer
echo Downloading %CMAKE_MSI_URL%...
python -c "import urllib.request; import os; urllib.request.urlretrieve(os.getenv('CMAKE_MSI_URL'),os.getenv('CMAKE_MSI_FILE'));"
if errorlevel 1 (
  echo Command failed. Unable to download cmake installer.
  exit /B %errorlevel%
)
if NOT EXIST "%CMAKE_MSI_FILE%" (
  echo Download failed. The file '%CMAKE_MSI_FILE%' is not found.
  exit /B %errorlevel%
)
echo done.
echo.

:: Validate administrative permissions...
:: https://stackoverflow.com/questions/4051883/batch-script-how-to-check-for-admin-rights
echo Checking administrative permissions...
net session >nul 2>&1
if %errorLevel% == 0 (
  echo Found administrative permissions. Installation in quiet mode enabled.
  set QUIET_MODE_ARGS=/quiet /qn
) else (
  echo Administrative permissions not found. Installation in interractive mode will be used.
)

:: Installing MSI
REM Note: `/quiet /qn` is not available when launched in user mode.
echo Running CMake installer...
echo QUIET_MODE_ARGS=%QUIET_MODE_ARGS%
msiexec /i "%CMAKE_MSI_FILE%" %QUIET_MODE_ARGS% /norestart /log "%TEMP%\cmake_insall.log"
if errorlevel 1 (
  echo Failed to install MSI package "%CMAKE_MSI_FILE%".
  exit /B %errorlevel%
)
echo done.
echo.

:: Searching for cmake executable
echo Searching for cmake executable...
set PATH=%CMAKE_MSI_INSTALL_DIR%\bin;%PATH%
where cmake.exe 1>NUL 2>NUL
if errorlevel 1 (
  echo Error: CMake not installed on system.
  exit /B %errorlevel%
)
echo CMake found.
cmake --version
echo.

echo CMake was installed on the system without error.
echo.
