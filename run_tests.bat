@echo off
make tests -j 8
if errorlevel 1 exit /b 1
echo "Testing Isaac-Object-Viewer.."
build\tests\test_runner.exe
set TEST_EXITCODE=%errorlevel%
pause
exit /b %TEST_EXITCODE%
