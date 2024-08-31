@echo off

:: Install ImageMagik.
:: https://github.com/ImageMagick/ImageMagick
:: https://imagemagick.org/script/download.php#windows

echo Installing ImageMagick...
set IKMAGEMAGICK_URL=https://imagemagick.org/archive/binaries/ImageMagick-7.1.1-37-portable-Q16-x64.zip
set IKMAGEMAGICK_FILE=%TEMP%\imagemagick.zip
set IKMAGEMAGICK_INSTALL_DIR=%TEMP%\imagemagick

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
echo Downloading %IKMAGEMAGICK_URL%...
python -c "import urllib.request; import os; urllib.request.urlretrieve(os.getenv('IKMAGEMAGICK_URL'),os.getenv('IKMAGEMAGICK_FILE'));"
if errorlevel 1 (
  echo Command failed. Unable to download ImageMagick binaries.
  exit /B %errorlevel%
)
if NOT EXIST "%IKMAGEMAGICK_FILE%" (
  echo Download failed. The file '%IKMAGEMAGICK_FILE%' is not found.
  exit /B %errorlevel%
)
echo done.
echo.

:: Extracting
mkdir %TEMP%\imagemagick 1>NUL 2>NUL
echo Extracting content to installation directory...
REM WARNING. THE FOLLOWING COMMAND EXACT ALL FILES IN THE ARCHIVE TO THE SAME LOCATION DISREGARDING INTERNAL DIRECTORY STRUCTURE!!
7z e %IKMAGEMAGICK_FILE% -o%IKMAGEMAGICK_INSTALL_DIR% -aoa
if errorlevel 1 (
  echo Failed to decompress ImageMagick binary installer.
  exit /B %errorlevel%
)
echo done.
echo.

:: Searching for imagemagick executable
echo Searching for imagemagick executable...
set PATH=%IKMAGEMAGICK_INSTALL_DIR%;%PATH%
where magick.exe 1>NUL 2>NUL
if errorlevel 1 (
  echo Error: ImageMagick not installed on system.
  exit /B %errorlevel%
)
echo ImageMagick found.
echo.

echo ImageMagick was installed on the system without error.
echo.
