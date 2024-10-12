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

:: For each svg icons
echo Converting flat-color-icons svg files to ico format.
echo Searching directory: "%IMAGE_INPUT_DIR%".
setlocal EnableDelayedExpansion
for /f %%f in ('dir /b "%IMAGE_INPUT_DIR%"') do (
  REM echo %%f
  call :process_file "%%f"
  if !errorlevel! neq 0 echo Failed to process file "%%f". ERRORLEVEL=!errorlevel! && exit /b !errorlevel!
)
endlocal
echo done.
echo.

echo All flat-color-icons icons converted to ico format.
echo Output directory: "%IMAGE_OUTPUT_DIR%".
echo.
goto :eof




:process_file
@echo off

:: echo FILENAME IS %~1
:: echo INPUT_FILE_PATH IS %IMAGE_INPUT_DIR%\%~1
:: echo OUTPUT_FILE_PATH IS %IMAGE_OUTPUT_DIR%\%~n1-32.png

echo Processing file: %~1
magick -size 16x16 -background none "%IMAGE_INPUT_DIR%\%~1" "%TEMP_DIR%\%~n1-16.png"
if %errorlevel% neq 0 echo Failure to convert svg file to png. ERRORLEVEL=%errorlevel% && exit /b %errorlevel%
magick -size 32x32 -background none "%IMAGE_INPUT_DIR%\%~1" "%TEMP_DIR%\%~n1-32.png"
if %errorlevel% neq 0 echo Failure to convert svg file to png. ERRORLEVEL=%errorlevel% && exit /b %errorlevel%
magick -size 48x48 -background none "%IMAGE_INPUT_DIR%\%~1" "%TEMP_DIR%\%~n1-48.png"
if %errorlevel% neq 0 echo Failure to convert svg file to png. ERRORLEVEL=%errorlevel% && exit /b %errorlevel%
magick -size 64x64 -background none "%IMAGE_INPUT_DIR%\%~1" "%TEMP_DIR%\%~n1-64.png"
if %errorlevel% neq 0 echo Failure to convert svg file to png. ERRORLEVEL=%errorlevel% && exit /b %errorlevel%

set png_files=
set png_files=%png_files% "%TEMP_DIR%\%~n1-16.png"
set png_files=%png_files% "%TEMP_DIR%\%~n1-32.png"
set png_files=%png_files% "%TEMP_DIR%\%~n1-48.png"
set png_files=%png_files% "%TEMP_DIR%\%~n1-64.png"

magick %png_files% "%IMAGE_OUTPUT_DIR%\%~n1.ico"
if %errorlevel% neq 0 echo Failure to convert png files to ico. ERRORLEVEL=%errorlevel% && exit /b %errorlevel%

goto :eof


:EOF
