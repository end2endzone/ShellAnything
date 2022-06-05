@echo off

:: Install docygen.
:: https://github.com/doxygen/doxygen
:: https://www.doxygen.nl/download.html

echo Installing doxygen...
set DOXYGEN_URL=https://www.doxygen.nl/files/doxygen-1.9.4.windows.x64.bin.zip
set DOXYGEN_FILE=%TEMP%\doxygen.zip
set DOXYGEN_INSTALL_DIR=%TEMP%\doxygen

:: Check that 7zip is installed
echo Searching for 7zip executable...
set PATH=C:\Program Files\7-Zip;%PATH%
where 7z.exe 1>NUL 2>NUL
if errorlevel 1 (
  echo Error: 7zip not installed on system.
  exit /B %errorlevel%
)
echo 7zip found.
echo.

:: Download installer
echo Downloading %DOXYGEN_URL%...

REM Note:
REM Running `powershell -nologo -executionpolicy bypass -command "(New-Object System.Net.WebClient).DownloadFile('%DOXYGEN_URL%', '%DOXYGEN_FILE%')"`
REM result in the following exception on AppVeyor:
REM   Exception calling "DownloadFile" with "2" argument(s): "The request was 
REM   aborted: Could not create SSL/TLS secure channel."
REM The solution is explained here: https://stackoverflow.com/questions/41618766/powershell-invoke-webrequest-fails-with-ssl-tls-secure-channel

REM OLD COMMAND: powershell -nologo -executionpolicy bypass -command "wget '%DOXYGEN_URL%' -outfile '%DOXYGEN_FILE%'
powershell -nologo -executionpolicy bypass -command "[Net.ServicePointManager]::SecurityProtocol = 'tls12, tls11, tls'; (New-Object System.Net.WebClient).DownloadFile('%DOXYGEN_URL%', '%DOXYGEN_FILE%')"
if errorlevel 1 (
  echo Command failed. Unable to download doxygen binaries.
  exit /B %errorlevel%
)
if NOT EXIST "%DOXYGEN_FILE%" (
  echo Download failed. The file '%DOXYGEN_FILE%' is not found.
  exit /B %errorlevel%
)
echo done.
echo.

:: Extracting
mkdir %TEMP%\doxygen 1>NUL 2>NUL
echo Extracting content to installation directory...
7z x %DOXYGEN_FILE% -o%DOXYGEN_INSTALL_DIR% -aoa
if errorlevel 1 (
  echo Failed to decompress doxygen binary installer.
  exit /B %errorlevel%
)
echo done.
echo.

:: Searching for doxygen executable
echo Searching for doxygen executable...
set PATH=%DOXYGEN_INSTALL_DIR%;%PATH%
where doxygen.exe 1>NUL 2>NUL
if errorlevel 1 (
  echo Error: Doxygen not installed on system.
  exit /B %errorlevel%
)
echo Doxygen found.
echo.

echo doxygen was installed on the system without error.
echo.
