# C++ Chat Application

A basic chat server and client implementation in modern C++17 following professional engineering standards.

## Project Structure

```
windsurf/cppchatapp/
├── src/
│   ├── include/
│   │   ├── interfaces.h          # Abstract interfaces and message protocol
│   │   └── message_impl.h       # Message implementation
│   ├── server/
│   │   ├── chat_server.h        # Server header
│   │   ├── chat_server.cpp      # Server implementation
│   │   └── main.cpp             # Server entry point
│   └── client/
│       ├── chat_client.h        # Client header
│       ├── chat_client.cpp      # Client implementation
│       └── main.cpp             # Client entry point
├── tests/                       # Unit tests (to be implemented)
├── docs/                        # Documentation (to be expanded)
├── build/                       # Build directory
├── CMakeLists.txt               # CMake build configuration
└── README.md                    # This file
```

## Features

- **Modern C++17**: Uses latest C++ features for clean, efficient code
- **Cross-Platform**: Works on Windows and Linux systems
- **Asynchronous Architecture**: Multi-threaded for non-blocking operations
- **Clean Interfaces**: Explicit interfaces following dependency injection patterns
- **Error Handling**: Comprehensive error handling and logging
- **Message Protocol**: JSON-based message format with validation
- **Multiple Clients**: Supports concurrent client connections
- **Graceful Disconnect**: Proper connection cleanup and resource management
- **RAII**: Automatic resource management following C++ best practices

## Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.10+
- (Windows) Visual Studio or MinGW
- (Linux) build-essential package

## Building

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Make utility
- Google Test framework (for testing)

#### Install Google Test

**Windows (vcpkg):**
```bash
vcpkg install gtest
```

**Linux (apt):**
```bash
sudo apt-get install libgtest-dev libgmock-dev
```

**Building from Source:**
```bash
git clone https://github.com/google/googletest.git
cd googletest
mkdir build && cd build
cmake ..
make
sudo make install
```

### Build Commands

#### Using Makefile (Recommended)

**Windows:**
```bash
cd windsurf/cppchatapp
build.bat              # Build everything
build.bat test          # Build and run tests
build.bat dev           # Quick development build
build.bat clean         # Clean build artifacts
build.bat help          # Show help
```

**Linux/Unix:**
```bash
cd windsurf/cppchatapp
chmod +x build.sh      # Make script executable
./build.sh             # Build everything
./build.sh test         # Build and run tests
./build.sh dev          # Quick development build
./build.sh clean        # Clean build artifacts
./build.sh help         # Show help
```

#### Using Make Directly

```bash
# Build all components
make all

# Build individual components
make server
make client
make tests

# Run tests
make test
make run-tests

# Development targets
make dev-server
make dev-client

# Performance testing
make perf-test

# Clean build
make clean

# Show help
make help
```

### Build Output

```
build/
├── bin/
│   ├── chat_server.exe    # Server executable (Windows)
│   ├── chat_client.exe    # Client executable (Windows)
│   ├── chat_server       # Server executable (Linux)
│   ├── chat_client       # Client executable (Linux)
│   └── chat_tests       # Test executable
└── obj/                # Object files
```

### Usage

#### Start the Server

**Windows:**
```bash
build\bin\chat_server.exe [--host HOST] [--port PORT]
```

**Linux:**
```bash
./build/bin/chat_server [--host HOST] [--port PORT]
```

Default: `--host localhost --port 8765`

#### Start a Client

**Windows:**
```bash
build\bin\chat_client.exe [--host HOST] [--port PORT]
```

**Linux:**
```bash
./build/bin/chat_client [--host HOST] [--port PORT]
```

#### Chat Commands

- Type your message and press Enter to send
- Type `/help` to show available commands
- Type `/quit` to exit the client

## Design Patterns Used

- **Interface Segregation**: Separate interfaces for server, client, and message handling
- **Dependency Injection**: Message handler injected into server
- **Strategy Pattern**: Pluggable message processing strategies
- **Factory Pattern**: Factory functions for object creation
- **Observer Pattern**: Server observes client connections
- **RAII**: Automatic resource management

## Architecture Standards

Following AIinfo.txt standards:
- Single responsibility principle
- Explicit interfaces for cross-module interaction
- Comprehensive error handling
- No hidden state
- Separation of configuration from logic
- Unit testable design
- RAII for resource management
- Modern C++ best practices

## Key C++ Features Used

- **Smart Pointers**: `std::unique_ptr` for automatic memory management
- **RAII**: All resources managed through object lifetime
- **Threads**: `std::thread` for concurrent operations
- **Atomics**: `std::atomic` for thread-safe operations
- **Mutexes**: `std::mutex` for thread synchronization
- **Lambdas**: For callback functions and event handling
- **String Views**: Efficient string handling where applicable

## Testing

### Test Structure

The C++ chat application includes comprehensive unit tests using Google Test framework:

```
tests/
├── test_message.cpp       # Message class tests
├── test_server.cpp        # ChatServer class tests
└── (additional test files to be added)
```

### Prerequisites

Install Google Test framework:

#### Windows (vcpkg)
```bash
vcpkg install gtest
```

#### Linux (apt)
```bash
sudo apt-get install libgtest-dev libgmock-dev
```

#### Building from Source
```bash
git clone https://github.com/google/googletest.git
cd googletest
mkdir build && cd build
cmake ..
make
sudo make install
```

### Running Tests

#### Quick Test Run
```bash
# Windows
build.bat test

# Linux
./build.sh test
```

#### Using Make Directly
```bash
# Run all tests
make test

# Run with verbose output
make run-tests

# Run specific test categories
make test-message    # Message tests only
make test-server     # Server tests only

# Performance tests
make perf-test
```

#### Test Output
```
[==========] Running 15 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 8 tests from MessageTest
[ RUN      ] MessageTest.StandardMessageCreation
[       OK ] MessageTest.StandardMessageCreation (1 ms)
...
[----------] 8 tests from MessageTest (5 ms total)
[----------] 7 tests from ServerTest
[ RUN      ] ServerTest.ServerCreation
[       OK ] ServerTest.ServerCreation (0 ms)
...
[----------] 7 tests from ServerTest (10 ms total)

[----------] Global test environment tear-down
[==========] 15 tests from 2 test suites ran. (15 ms total)
[  PASSED  ] 15 tests.
```

### Test Coverage

The test suite covers:

#### Standard Cases
- Message creation and validation
- JSON serialization/deserialization
- Server startup and shutdown
- Client connection management

#### Edge Cases
- Boundary conditions (1000 character limit)
- Special characters and Unicode handling
- Multiple message types
- Concurrent operations

#### Wrong Inputs
- Invalid JSON format
- Empty/null values
- Exceeding size limits
- Network error conditions

#### Performance Tests
- Serialization performance benchmarks
- Concurrent message handling
- Stress testing with high load

### Test Categories

#### Message Tests (`test_message.cpp`)
- **Standard Cases**: Normal message creation and validation
- **Edge Cases**: Boundary conditions, special characters, Unicode
- **JSON Tests**: Serialization/deserialization accuracy
- **Concurrent Tests**: Multi-threaded message creation
- **Performance Tests**: Timing benchmarks

#### Server Tests (`test_server.cpp`)
- **Lifecycle Tests**: Server start/stop operations
- **Message Broadcasting**: Valid and invalid message handling
- **Client Management**: Connection simulation
- **Error Handling**: Invalid ports, hosts, network failures
- **Stress Tests**: High-load concurrent operations

### Test Standards

Following AIinfo.txt testing requirements:
- Unit test every function with standard case, edge cases, and wrong inputs
- Load at runtime file for case inputs (JSON-based test data)
- Instrument for test on commit using free tools available with GIT
- Comprehensive error handling validation
- Integration testing for component interaction

### Example Test Output

```
[==========] Running 15 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 8 tests from MessageTest
[ RUN      ] MessageTest.StandardMessageCreation
[       OK ] MessageTest.StandardMessageCreation (1 ms)
[ RUN      ] MessageTest.JsonSerialization
[       OK ] MessageTest.JsonSerialization (0 ms)
...
[----------] 8 tests from MessageTest (5 ms total)

[----------] 7 tests from ServerTest
[ RUN      ] ServerTest.ServerCreation
[       OK ] ServerTest.ServerCreation (0 ms)
...
[----------] 7 tests from ServerTest (10 ms total)

[----------] Global test environment tear-down
[==========] 15 tests from 2 test suites ran. (15 ms total)
[  PASSED  ] 15 tests.
```

### Continuous Integration

Set up automated testing with Make:

```bash
# Add to CI/CD pipeline
cd windsurf/cppchatapp
make clean && make all
make test
```

Git pre-commit hook:
```bash
#!/bin/bash
# .git/hooks/pre-commit
cd windsurf/cppchatapp
make clean && make quick-test
if [ $? -ne 0 ]; then
    echo "Tests failed! Commit aborted."
    exit 1
fi
```

### Test Configuration

The Makefile includes:
- **GTest Integration**: Automatic test discovery and execution
- **Test Filtering**: Run specific test categories
- **Verbose Output**: Detailed failure information
- **Performance Benchmarks**: Timing measurements for critical operations
- **Cross-Platform Support**: Works on Windows and Linux

## Performance Considerations

- Non-blocking socket operations
- Thread-safe message queues
- Efficient JSON serialization
- Minimal memory allocations in hot paths
- Connection pooling for scalability

## Future Enhancements

- GUI client interface (Qt/ImGui)
- Message history persistence
- User authentication
- Private messaging
- File sharing capabilities
- Message encryption (SSL/TLS)
- IPv6 support
- Configuration file support
- Logging to file
- Performance metrics and monitoring

## Troubleshooting

### Windows Issues
- Ensure Winsock2.h is included before windows.h
- Link with ws2_32.lib
- Use proper socket cleanup

### Linux Issues
- Check firewall settings
- Ensure proper permissions for binding ports
- Use valgrind for memory leak detection

### Build Issues
- Verify C++17 compiler support
- Check CMake version compatibility
- Ensure all dependencies are installed
