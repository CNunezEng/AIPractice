@echo off
REM Build script for Chat Messenger on Windows with MinGW

echo Building Chat Messenger...
echo.

REM Create build directory if it doesn't exist
if not exist build (
    mkdir build
)

REM Navigate to build directory
cd build

REM Configure with CMake
echo Configuring CMake...
cmake -G "MinGW Makefiles" ..

if errorlevel 1 (
    echo CMake configuration failed!
    cd ..
    exit /b 1
)

REM Build
echo.
echo Building project...
mingw32-make

if errorlevel 1 (
    echo Build failed!
    cd ..
    exit /b 1
)

cd ..

echo.
echo Build complete! Executables in build\bin\
echo.
echo To run the server:
echo   .\build\bin\server.exe 127.0.0.1 5000
echo.
echo To run the client:
echo   .\build\bin\client.exe 127.0.0.1 5000 127.0.0.1 5001
echo.
echo To run tests:
echo   .\build\bin\test_message.exe
