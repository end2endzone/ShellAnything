taskkill /F /IM regsvr32.exe
taskkill /F /IM explorer.exe
start C:\Windows\explorer.exe

explorer.exe "%~dp0"