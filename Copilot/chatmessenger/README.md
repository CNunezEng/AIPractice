# Chat Messenger - C++ Edition

A lightweight C++ chat application for Windows (MinGW) demonstrating professional software engineering practices.

## Purpose
A simple client-server chat messenger for testing message delivery and real-time communication over TCP sockets.

## Project Structure
```
chatmessenger/
├── src/
│   ├── message.cpp          # Message implementation
│   ├── socket_util.cpp      # Socket networking utilities
│   ├── client.cpp           # Client implementation
│   └── server.cpp           # Server implementation
├── include/
│   ├── message.h            # Message interface
│   ├── socket_util.h        # Socket utilities interface
│   ├── client.h             # Client interface
│   └── server.h             # Server interface
├── tests/
│   └── test_message.cpp     # Unit tests for Message
├── CMakeLists.txt           # Build configuration
└── docs/
    └── DESIGN.md            # Architecture and design decisions
```

## Build Instructions

### Prerequisites
- MinGW (g++ compiler)
- CMake 3.10+
- Windows OS

### Build Steps
```bash
cd chatmessenger
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```

### Output
- `build/client.exe` - Client executable
- `build/server.exe` - Server executable

## Run Instructions

### Start the Server
```bash
.\server.exe 127.0.0.1 5000
```
Listens on 127.0.0.1:5000 and waits for client connection.

### Start the Client
```bash
.\client.exe 127.0.0.1 5000 127.0.0.1 5001
```
Connects to server at 127.0.0.1:5000, listens on local 127.0.0.1:5001.

### Usage
- Type messages and press Enter to send
- Incoming messages appear immediately with sender identifier
- Type "quit" to exit

## Command Line Arguments

### Server
```
.\server.exe <bind_ip> <bind_port>

bind_ip   = IP address to listen on (e.g., 127.0.0.1)
bind_port = Port to listen on (e.g., 5000)
```

### Client
```
.\client.exe <server_ip> <server_port> <local_ip> <local_port>

server_ip   = Server IP address (e.g., 127.0.0.1)
server_port = Server port (e.g., 5000)
local_ip    = Local IP to bind on (e.g., 127.0.0.1)
local_port  = Local port to listen on (e.g., 5001)
```

## Design Principles
- **Interface-based design**: Abstract base classes define contracts
- **Input validation**: All network input verified before processing
- **Thread safety**: Separate threads for send/receive operations
- **Single responsibility**: Each class has one reason to change
- **Error handling**: Explicit error codes and messages
- **Resource management**: RAII pattern for socket cleanup
- **Readability**: Clear, self-documenting code

## Testing
Unit tests for message parsing and validation:
```bash
.\build\test_message.exe
```

## Design Patterns Used
1. **Dependency Injection**: Components receive dependencies through constructors
2. **Observer**: Receive thread monitors server for incoming messages
3. **Factory**: Message.parse() creates messages from network data
4. **RAII**: Socket objects manage resource lifecycle automatically
5. **Strategy**: Different message types handled polymorphically
