#!/bin/bash
# Build script for C++ Chat Application
# Following AIinfo.txt standards for clear, maintainable scripts

set -e  # Exit on any error

echo "C++ Chat Application Build Script"
echo "================================="
echo

# Check if required tools are available
check_dependencies() {
    echo "Checking dependencies..."
    
    if ! command -v g++ &> /dev/null; then
        echo "Error: g++ compiler not found. Please install g++."
        exit 1
    fi
    
    if ! command -v make &> /dev/null; then
        echo "Error: make not found. Please install make."
        exit 1
    fi
    
    # Check for Google Test
    if ! ldconfig -p | grep -q libgtest; then
        echo "Warning: Google Test not found. Tests may not build."
        echo "Install with: sudo apt-get install libgtest-dev libgmock-dev"
    fi
    
    echo "Dependencies check complete."
    echo
}

# Build the application
build_application() {
    echo "Building C++ Chat Application..."
    
    # Clean previous build
    if [ -d "build" ]; then
        echo "Cleaning previous build..."
        make clean
    fi
    
    # Build all components
    echo "Building server, client, and tests..."
    make all
    
    echo
    echo "Build completed successfully!"
    echo "Executables created:"
    echo "  - build/bin/chat_server"
    echo "  - build/bin/chat_client"
    echo "  - build/bin/chat_tests"
    echo
}

# Run tests
run_tests() {
    echo "Running tests..."
    
    if [ -f "build/bin/chat_tests" ]; then
        ./build/bin/chat_tests --gtest_verbose
        echo
        echo "Tests completed."
    else
        echo "Error: Test executable not found. Run 'build.sh' first."
        exit 1
    fi
}

# Quick development build and test
dev_build() {
    echo "Development build and test..."
    make quick-test
}

# Main execution
case "${1:-all}" in
    "deps"|"dependencies")
        check_dependencies
        ;;
    "build"|"all")
        check_dependencies
        build_application
        ;;
    "test")
        run_tests
        ;;
    "dev"|"quick")
        check_dependencies
        dev_build
        ;;
    "clean")
        echo "Cleaning build artifacts..."
        make clean
        echo "Clean complete."
        ;;
    "help"|"-h"|"--help")
        echo "Usage: $0 [command]"
        echo
        echo "Commands:"
        echo "  deps, dependencies  - Check build dependencies"
        echo "  build, all         - Build all components"
        echo "  test               - Run tests"
        echo "  dev, quick         - Quick build and test"
        echo "  clean              - Clean build artifacts"
        echo "  help               - Show this help"
        echo
        echo "Examples:"
        echo "  $0                # Build everything"
        echo "  $0 test           # Run tests only"
        echo "  $0 dev            # Quick development build"
        ;;
    *)
        echo "Unknown command: $1"
        echo "Use '$0 help' for usage information."
        exit 1
        ;;
esac
