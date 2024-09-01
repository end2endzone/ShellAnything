@echo off

:: Assume that ImageMagick already installed by installation script.
set PATH=%TEMP%\imagemagick;%PATH%

echo Searching for imagemagick executables in PATH...
where magick.exe
if errorlevel 1 (
  echo File 'magick.exe' not found. Please install imagemagick on system and add the directory to PATH.
  exit /B %errorlevel%
)
echo Found imagemagick directory.
echo.

set TEMP_DIR=%TEMP%\flat-color-icons
set IMAGE_INPUT_DIR=%~dp0..\..\third_parties\flat-color-icons\svg
set IMAGE_OUTPUT_DIR=%~dp0..\..\build\third_parties\flat-color-icons

:: Create output directory
mkdir "%IMAGE_OUTPUT_DIR%" 1>NUL 2>NUL
mkdir "%TEMP_DIR%" 1>NUL 2>NUL

echo Deleting previous resource file %IMAGE_OUTPUT_DIR%\resource.rc...
del %IMAGE_OUTPUT_DIR%\resource.rc 1>NUL 2>NUL
echo done.
echo.

:: For each svg icons
echo Converting each svg file as ico.
echo Searching for svg files in directory "%IMAGE_INPUT_DIR%"...
for /f %%f in ('dir /b "%IMAGE_INPUT_DIR%"') do (
  REM echo %%f
  call :process_file "%%f"
)
echo done.
echo.

pause


goto :eof

:process_file
@echo off

:: echo FILENAME IS %~1
:: echo INPUT_FILE_PATH IS %IMAGE_INPUT_DIR%\%~1
:: echo OUTPUT_FILE_PATH IS %IMAGE_OUTPUT_DIR%\%~n1-32.png

echo Processing file: %~1
magick -size 16x16 -background none "%IMAGE_INPUT_DIR%\%~1" "%TEMP_DIR%\%~n1-16.png"
magick -size 32x32 -background none "%IMAGE_INPUT_DIR%\%~1" "%TEMP_DIR%\%~n1-32.png"
magick -size 48x48 -background none "%IMAGE_INPUT_DIR%\%~1" "%TEMP_DIR%\%~n1-48.png"
magick -size 64x64 -background none "%IMAGE_INPUT_DIR%\%~1" "%TEMP_DIR%\%~n1-64.png"

set png_files=
set png_files=%png_files% "%TEMP_DIR%\%~n1-16.png"
set png_files=%png_files% "%TEMP_DIR%\%~n1-32.png"
set png_files=%png_files% "%TEMP_DIR%\%~n1-48.png"
set png_files=%png_files% "%TEMP_DIR%\%~n1-64.png"

magick %png_files% "%IMAGE_OUTPUT_DIR%\%~n1.ico"

goto :eof


:EOF
