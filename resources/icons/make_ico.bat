@echo off

:: This script is designed for drag and drop
IF [%1] == [] goto :eof
echo Processing file: %~1

:: Assume that ImageMagick already installed by installation script.
set PATH=%TEMP%\imagemagick;%PATH%

echo Searching for imagemagik executables in PATH...
where magick.exe
if errorlevel 1 (
  echo File 'magick.exe' not found. Please install imagemagik on system and add the directory to PATH.
  exit /B %errorlevel%
)
echo Found imagemagik directory.
echo.

:: Notes:
::   density of 32 --> 16x16 png
::   density of 48 --> 24x24 png
::   density of 64 --> 32x32 png

echo Generate pngs...
magick -density 32  -background none "%~1" "%~dpn1-16.png"
magick -density 64  -background none "%~1" "%~dpn1-32.png"
magick -density 96  -background none "%~1" "%~dpn1-48.png"
magick -density 128 -background none "%~1" "%~dpn1-64.png"

echo Generate ico...
set png_files=
set png_files=%png_files% "%~dpn1-16.png"
set png_files=%png_files% "%~dpn1-32.png"
set png_files=%png_files% "%~dpn1-48.png"
set png_files=%png_files% "%~dpn1-64.png"
magick %png_files% "%~dpn1.ico"

echo Generated "%~dpn1.ico".

echo del %png_files%

pause
