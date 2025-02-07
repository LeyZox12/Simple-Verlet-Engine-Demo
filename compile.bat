@echo off
cd /d %~dp0
set "GCC_PATH=%~dp0res\MinGW\bin\gcc.exe"

echo compilation of constraints begun
"%GCC_PATH%" -c -o res/customConstraints.o res/customConstraints.cpp
"%GCC_PATH%" -o res/customConstraints.dll -s -shared res/customConstraints.o 
echo done
pause