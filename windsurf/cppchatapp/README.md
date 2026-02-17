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

### Windows (Visual Studio)

```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release

# Run executables
bin\Release\chat_server.exe
bin\Release\chat_client.exe
```

### Windows (MinGW)

```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake .. -G "MinGW Makefiles"
cmake --build .

# Run executables
bin\chat_server.exe
bin\chat_client.exe
```

### Linux

```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
make

# Run executables
./bin/chat_server
./bin/chat_client
```

## Usage

### Start the Server

```bash
chat_server [--host HOST] [--port PORT]
```

Default: `chat_server --host localhost --port 8765`

Options:
- `--host`: Host to bind to (default: localhost)
- `--port`: Port to bind to (default: 8765)
- `--help`: Show help message

### Start a Client

```bash
chat_client [--host HOST] [--port PORT]
```

Default: `chat_client --host localhost --port 8765`

### Chat Commands

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
- **Chrono**: `std::chrono` for time handling
- **String Views**: Efficient string handling where applicable

## Testing

Unit tests to be implemented in the `tests/` directory following the workflow.

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
