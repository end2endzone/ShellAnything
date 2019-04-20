@echo off
set path=%PATH%;F:\Projets\Programmation\Cpp\bin2cpp\git\master\install\bin
cd /d %~dp0

bin2cpp-d --override --file=shellext.rgs --output=. --headerfile=shellext_registry_script.h --identifier=RegistryScript

pause
