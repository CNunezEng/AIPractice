@echo off
echo Building C++ Chat Application...

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake...
cmake .. -G "MinGW Makefiles" 2>nul
if errorlevel 1 (
    echo CMake configuration failed. Trying Visual Studio generator...
    cmake .. -G "Visual Studio 16 2019" 2>nul
    if errorlevel 1 (
        echo ERROR: CMake configuration failed
        pause
        exit /b 1
    )
    echo Building with Visual Studio...
    cmake --build . --config Release
) else (
    echo Building with MinGW...
    cmake --build .
)

if errorlevel 1 (
    echo ERROR: Build failed
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo Executables are in: build\bin\
echo.
echo To run the server:
echo   build\bin\chat_server.exe
echo.
echo To run a client:
echo   build\bin\chat_client.exe
echo.

cd ..
pause
