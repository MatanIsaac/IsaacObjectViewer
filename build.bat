@echo off
echo ====================================
echo = Creating and entering build dir =
echo ====================================
@mkdir build
@cd build

echo ====================================
echo = Configuring the project with CMake
echo ====================================
@cmake ..

echo ====================================
echo = Building the project
echo ====================================
@cmake --build .

echo ====================================
echo = Moving all files from build/src/Debug to build/src
echo ====================================
REM We’re now inside the 'build' folder, so the path is 'src\Debug'
if exist src\Debug 
(
    echo "src\Debug folder found; moving files..."
    move /Y src\Debug\*.* src\
    rmdir /S /Q src\Debug
)

if exist test\Debug 
(
    echo "test\Debug folder found; moving files..."
    @move /Y test\Debug\*.* test\
    @rmdir /S /Q test\Debug
)

echo ====================================
echo = Done!
echo ====================================
pause
