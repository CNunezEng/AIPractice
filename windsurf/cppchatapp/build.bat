@echo off
REM Build script for C++ Chat Application (Windows)
REM Following AIinfo.txt standards for clear, maintainable scripts

echo C++ Chat Application Build Script (Windows)
echo =========================================
echo.

REM Check if required tools are available
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: g++ compiler not found. Please install MinGW or MSYS2.
    echo Download from: https://www.mingw-w64.org/
    pause
    exit /b 1
)

where mingw32-make >nul 2>nul
if %errorlevel% neq 0 (
    where make >nul 2>nul
    if %errorlevel% neq 0 (
        echo Error: make not found. Please install MinGW or MSYS2.
        pause
        exit /b 1
    )
    set MAKE=make
) else (
    set MAKE=mingw32-make
)

echo Dependencies check complete.
echo.

REM Build the application
if "%1"=="clean" goto clean
if "%1"=="test" goto test
if "%1"=="dev" goto dev
if "%1"=="help" goto help

:build
echo Building C++ Chat Application...

REM Clean previous build
if exist build (
    echo Cleaning previous build...
    %MAKE% clean
)

REM Build all components
echo Building server, client, and tests...
%MAKE% all

echo.
echo Build completed successfully!
echo Executables created:
echo   - build\bin\chat_server.exe
echo   - build\bin\chat_client.exe
echo   - build\bin\chat_tests.exe
echo.
goto end

:test
echo Running tests...
if exist build\bin\chat_tests.exe (
    build\bin\chat_tests.exe --gtest_verbose
    echo.
    echo Tests completed.
) else (
    echo Error: Test executable not found. Run 'build.bat' first.
    pause
    exit /b 1
)
goto end

:dev
echo Development build and test...
%MAKE% quick-test
goto end

:clean
echo Cleaning build artifacts...
if exist build (
    rmdir /s /q build
    echo Clean complete.
) else (
    echo No build directory found.
)
goto end

:help
echo Usage: %0 [command]
echo.
echo Commands:
echo   (none)  - Build all components
echo   test     - Run tests
echo   dev      - Quick development build and test
echo   clean    - Clean build artifacts
echo   help     - Show this help
echo.
echo Examples:
echo   %0           # Build everything
echo   %0 test      # Run tests only
echo   %0 dev       # Quick development build
goto end

:end
if "%1"=="" pause
