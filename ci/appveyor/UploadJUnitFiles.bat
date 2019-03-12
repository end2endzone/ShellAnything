@echo off
cd c:\temp
powershell -nologo -executionpolicy bypass -File "%~dpn0.ps1"
pause
