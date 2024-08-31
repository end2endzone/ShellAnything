@echo off

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

set TEMP_DIR=%TEMP%\flat-color-icons
set IMAGE_INPUT_DIR=%~dp0..\..\third_parties\flat-color-icons\svg
set IMAGE_OUTPUT_DIR=%~dp0..\..\build\third_parties\flat-color-icons

:: Create output directory
mkdir "%IMAGE_OUTPUT_DIR%" 1>NUL 2>NUL
mkdir "%TEMP_DIR%" 1>NUL 2>NUL

echo Deleting previous resource file...
del %IMAGE_OUTPUT_DIR%\resource.rc
echo done.
echo.

:: For each svg icons
echo Converting each svg file as ico.
for /f %%f in ('dir /b "%IMAGE_SRC_DIR%"') do (
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

:: Notes:
::   density of 32 --> 16x16 png
::   density of 48 --> 24x24 png
::   density of 64 --> 32x32 png

echo Processing file: %~1
magick -density 32  -background none "%IMAGE_INPUT_DIR%\%~1" "%TEMP_DIR%\%~n1-16.png"
magick -density 64  -background none "%IMAGE_INPUT_DIR%\%~1" "%TEMP_DIR%\%~n1-32.png"
magick -density 96  -background none "%IMAGE_INPUT_DIR%\%~1" "%TEMP_DIR%\%~n1-48.png"
magick -density 128 -background none "%IMAGE_INPUT_DIR%\%~1" "%TEMP_DIR%\%~n1-64.png"
REM magick -density 192 -background none "%IMAGE_INPUT_DIR%\%~1" "%TEMP_DIR%\%~n1-96.png"
REM magick -density 256 -background none "%IMAGE_INPUT_DIR%\%~1" "%TEMP_DIR%\%~n1-128.png"
magick "%TEMP_DIR%\%~n1-16.png" "%TEMP_DIR%\%~n1-32.png" "%TEMP_DIR%\%~n1-48.png" "%TEMP_DIR%\%~n1-64.png" "%IMAGE_OUTPUT_DIR%\%~n1.ico"

goto :eof


:EOF
