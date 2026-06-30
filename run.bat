@echo off
make -j 8
if errorlevel 1 exit /b 1
echo "Running Isaac-Object-Viewer.."
build\iov.exe
