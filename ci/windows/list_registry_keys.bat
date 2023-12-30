@echo off
echo ==========================================================================================================
echo   WindowsUpdate registry
echo ==========================================================================================================
reg QUERY "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\WindowsUpdate" /s
echo.
echo.
echo.

echo ==========================================================================================================
echo   Windows Defender registry
echo ==========================================================================================================
reg QUERY "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows Defender" /s
echo.
echo.
echo.
