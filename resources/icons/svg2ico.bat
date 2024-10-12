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

echo Generate pngs...
magick -background none -size 16x16 "%~1" "%~dpn1-16.png"
magick -background none -size 32x32 "%~1" "%~dpn1-32.png"
magick -background none -size 48x48 "%~1" "%~dpn1-48.png"
magick -background none -size 64x64 "%~1" "%~dpn1-64.png"

echo Generate ico...
set png_files=
set png_files=%png_files% "%~dpn1-16.png"
set png_files=%png_files% "%~dpn1-32.png"
set png_files=%png_files% "%~dpn1-48.png"
set png_files=%png_files% "%~dpn1-64.png"
magick %png_files% "%~dpn1.ico"

echo Generated "%~dpn1.ico".
echo.

echo Deleting png (intermediate files).
del %png_files%

pause
