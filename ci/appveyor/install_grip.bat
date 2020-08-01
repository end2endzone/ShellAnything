@echo off

:: Install grip.
:: https://github.com/joeyespo/grip
:: https://pypi.org/project/grip/

:: Validate if python launcher is installed
where py.exe >NUL 2>NUL
if errorlevel 1 (
  echo Command failed. Please install python and python launcher to continue.
  exit /B %errorlevel%
)

:: Validate if pip is installed
where pip.exe >NUL 2>NUL
if errorlevel 1 (
  echo Command failed. Please install pip ^(Python package installer^) to continue.
  exit /B %errorlevel%
)

:: Installing grip.
echo Proceeding with instaling grip...
pip install grip
if errorlevel 1 (
  echo Command failed. An error was found while installing grip.
  exit /B %errorlevel%
)
