@echo off

:: Install ImageMagik.
:: https://github.com/ImageMagick/ImageMagick
:: https://imagemagick.org/script/download.php#windows

echo Installing ImageMagick...
set IMAGEMAGICK_URL=https://imagemagick.org/script/download.php
set IMAGEMAGICK_FILE=%TEMP%\imagemagick.zip
set IMAGEMAGICK_INSTALL_DIR=%TEMP%\imagemagick
set IMAGEMAGICK_TEMP_FILE=%TEMP%\imagemagick.temp.txt

echo Searching for a ImageMagick download link from %IMAGEMAGICK_URL%.
powershell -command "(Invoke-WebRequest -Uri $Env:IMAGEMAGICK_URL).links.href | where {$_ -match '-portable-Q16-x64.zip'}">"%IMAGEMAGICK_TEMP_FILE%"
if errorlevel 1 (
  echo Command failed. Unable to check for ImageMagick latest portable version binaries.
  exit /B %errorlevel%
)
set /p IMAGEMAGICK_URL=<"%IMAGEMAGICK_TEMP_FILE%"
echo Found download url: %IMAGEMAGICK_URL%
if [%IMAGEMAGICK_URL%] == [] exit /B 1
echo.

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
echo Downloading %IMAGEMAGICK_URL%...
python -c "import urllib.request; import os; urllib.request.urlretrieve(os.getenv('IMAGEMAGICK_URL'),os.getenv('IMAGEMAGICK_FILE'));"
if errorlevel 1 (
  echo Command failed. Unable to download ImageMagick binaries.
  exit /B %errorlevel%
)
if NOT EXIST "%IMAGEMAGICK_FILE%" (
  echo Download failed. The file '%IMAGEMAGICK_FILE%' is not found.
  exit /B %errorlevel%
)
echo done.
echo.

:: Extracting
mkdir %TEMP%\imagemagick 1>NUL 2>NUL
echo Extracting content to installation directory...
REM WARNING. THE FOLLOWING COMMAND EXACT ALL FILES IN THE ARCHIVE TO THE SAME LOCATION DISREGARDING INTERNAL DIRECTORY STRUCTURE!!
7z e %IMAGEMAGICK_FILE% -o%IMAGEMAGICK_INSTALL_DIR% -aoa
if errorlevel 1 (
  echo Failed to decompress ImageMagick binary installer.
  exit /B %errorlevel%
)
echo done.
echo.

:: Searching for imagemagick executable
echo Searching for imagemagick executable...
set PATH=%IMAGEMAGICK_INSTALL_DIR%;%PATH%
where magick.exe 1>NUL 2>NUL
if errorlevel 1 (
  echo Error: ImageMagick not installed on system.
  exit /B %errorlevel%
)
echo ImageMagick found.
echo.

echo ImageMagick was installed on the system without error.
echo.
