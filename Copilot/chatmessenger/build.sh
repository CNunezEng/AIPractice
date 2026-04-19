#!/bin/bash
# Build script for Chat Messenger on Linux/Mac

echo "Building Chat Messenger..."
echo

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

# Navigate to build directory
cd build

# Configure with CMake
echo "Configuring CMake..."
cmake -DCMAKE_BUILD_TYPE=Release ..

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    cd ..
    exit 1
fi

# Build
echo
echo "Building project..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "Build failed!"
    cd ..
    exit 1
fi

cd ..

echo
echo "Build complete! Executables in build/bin/"
echo
echo "To run the server:"
echo "  ./build/bin/server 127.0.0.1 5000"
echo
echo "To run the client:"
echo "  ./build/bin/client 127.0.0.1 5000 127.0.0.1 5001"
echo
echo "To run tests:"
echo "  ./build/bin/test_message"
