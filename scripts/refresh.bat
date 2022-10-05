@echo off

set SECONDARY_SCRIPT=refresh.tmp.bat

:: Delete secondary script, if any
if exist "%SECONDARY_SCRIPT%" (
	del "%SECONDARY_SCRIPT%"
)

:: List directories opened by explorer.exe and
:: prefix all elements with `@start ""` to convert each element to a command.
:: Appends the output to a secondary batch file that will be called later.
powershell "$directories = @((New-Object -com shell.application).Windows()).Document.Folder.Self.Path; $directories = $directories -replace '^', '@start \""\"" \""'; $directories -replace '$', '\""';"> "%SECONDARY_SCRIPT%"
if %errorlevel% neq 0 echo Failed getting extract directories from explorer.exe && exit /b %errorlevel%

:: Kill all explorer.exe instances...
taskkill /F /IM explorer.exe

:: Run explorer.exe again. This will create the taskbar.
start C:\Windows\explorer.exe

:: Call the generated secondary script to reopen the same previous directories...
call "%SECONDARY_SCRIPT%"
