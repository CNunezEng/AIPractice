# Chat Messenger - Architecture & Design

## Overview
A C++ TCP-based chat application for Windows (MinGW) with real-time message delivery following professional software engineering standards from AIinfo.txt.

## Design Patterns Used

### 1. **Dependency Injection via Interfaces**
- `IMessage` abstract base class defines message contract
- `IChat` abstract base class defines send/receive contract
- Concrete classes (`Message`, `ChatClient`) inherit from interfaces
- Allows testing and swapping implementations

### 2. **RAII (Resource Acquisition Is Initialization)**
- `Socket` class manages socket lifecycle
- Socket automatically closed in destructor
- No manual cleanup needed
- Prevents resource leaks

### 3. **Observer Pattern**
- Server listens for incoming client connections in background thread
- Client listens for incoming messages in background thread
- Main thread sends/receives without blocking

### 4. **Factory Pattern**
- `Message::deserialize()` creates Message from network data
- Encapsulates deserialization logic

### 5. **Thread Safety**
- Separate threads for send and receive operations
- Atomic flags for thread communication
- No shared mutable state between threads

## Component Diagram

```
┌──────────────────────┐         ┌──────────────────────┐
│    ChatClient        │ TCP     │    ChatServer        │
│                      ├────────►│                      │
│ - client_socket      │<────────┤ - listen_socket      │
│ - listen_socket      │         │ - Message Queue      │
│ - receive_thread     │         │ - accept_thread      │
└─────────┬────────────┘         └──────────┬───────────┘
          │                                  │
          │ uses                             │ uses
          ▼                                  ▼
    ┌──────────────┐              ┌──────────────┐
    │   IMessage   │              │    Socket    │
    │   Message    │              │  (RAII wrap) │
    │              │              │              │
    │ - sender     │              │ - send()     │
    │ - content    │              │ - receive()  │
    │ - timestamp  │              │ - connect()  │
    │              │              │ - bind()     │
    │ - serialize  │              │ - listen()   │
    │ - deserialize│              │ - accept()   │
    └──────────────┘              └──────────────┘
```

## Sequence Diagram: Send Message

```
Client Thread        Server Accept Thread      Server Handler Thread
      │                     │                         │
      ├─ Create Message ─┐  │                         │
      │                  │  │                         │
      ├─ Validate ───────┤  │                         │
      │                  │  │                         │
      ├─ Serialize ──────┤  │                         │
      │                  │  │                         │
      ├─ Send via TCP ────────────────────────────────►
      │                  │                │           │
      │                  │                │ Receive  │
      │                  │                │           │
      │                  │                │ Deserialize
      │                  │                │           │
      │                  │                │ Display  │
      │                  │                │           │
      │ <─ Acknowledge ──────────────────┤           │
      │
```

## Message Format (Network Protocol)

Each message is serialized as binary data for efficient transmission:

```
┌──────────────┬────────────────┬──────────────┬────────────────┬──────────────┐
│ Sender Len   │ Sender Data    │ Content Len  │ Content Data   │ Timestamp    │
├──────────────┼────────────────┼──────────────┼────────────────┼──────────────┤
│ 4 bytes (BE) │ Variable       │ 4 bytes (BE) │ Variable       │ 8 bytes (BE) │
└──────────────┴────────────────┴──────────────┴────────────────┴──────────────┘
```

- BE = Big-Endian format for network compatibility
- Lengths are 4-byte unsigned integers
- Timestamp is 8-byte UNIX timestamp

## Input Validation Strategy

**Philosophy:** Trust nothing. Validate everything.

### Message Validation

1. **Sender validation:**
   - Non-empty (minimum 1 character)
   - Maximum 64 characters
   - Only alphanumeric + underscore + hyphen
   - Prevents injection attacks

2. **Content validation:**
   - Non-empty (minimum 1 character)
   - Maximum 4096 characters
   - No character restrictions (allows Unicode)

3. **Deserialization validation:**
   - Check buffer size before reading
   - Validate length fields before reading data
   - Return empty message on invalid data
   - No exceptions on malformed network data

## Error Handling Strategy

- **Network errors:** Logged and connection closed gracefully
- **Validation errors:** Specific error messages returned
- **No exceptions for control flow:** Use return values and error codes
- **Graceful degradation:** Continue running despite single message errors

## Thread Architecture

### Server
```
Main Thread                Accept Thread              Handler Threads (one per client)
│                          │                          │
├─ Initialize socket  ─────┼─ Accept connections     │
│                          │ (non-blocking)           │
├─ Start accept thread ────┤                          │
│                          ├─ Create handler thread──┬┴─ Receive messages
│                          │                          │   Deserialize
├─ Signal handler ─────────┼─ Yield                  │   Display
│ (SIGINT = shutdown)      │                          │   Continue
│                          └──────────────────────────┘
└─ Wait for shutdown
```

### Client
```
Main Thread              Receive Thread
│                        │
├─ Parse args  ─┐        │
│               │        │
├─ Connect ────┼────────┼─ Accept connection from server
│               │        │   (non-blocking)
├─ Create      │        │
│  recv thread─┼────────┤
│               │        ├─ Receive messages
├─ Input loop ─┤        │   Deserialize
│   Read line   │        │   Display
│   Create msg │        │
│   Send to    │        Let │
│   server  ───┼───────────►
│               │        │
│               │        └─ Continue listening
│
└─ Cleanup
```

## Windows Network Programming

### Winsock Initialization
- `Socket::initWinsock()` must be called before using sockets
- `Socket::cleanupWinsock()` called on exit
- Automatic setup in main_server.cpp and main_client.cpp

### Socket Modes
- Non-blocking sockets for timeout support
- `select()` used for connection timeouts and receive timeouts
- Prevents hanging on slow networks

## Separation of Concerns

**Message Format:** Message class handles serialization/deserialization
**Network I/O:** Socket class handles TCP communication
**Application Logic:** Client/Server classes coordinate message flow
**User Interface:** main_*.cpp files handle interactions

Each component has one reason to change:
- Message changes if message format requirements change
- Socket changes if network protocol changes
- Client changes if client logic changes
- Server changes if server logic changes

## Testing Strategy

### Unit Tests (test_message.cpp)
- Message creation with valid/invalid inputs
- Validation rules (length, characters, format)
- Serialization/deserialization roundtrips
- Error cases (empty data, invalid format)
- Large messages (4000+ bytes)
- Special characters in content

### Manual Integration Tests
1. Start server: `server.exe 127.0.0.1 5000`
2. Start client: `client.exe 127.0.0.1 5000 127.0.0.1 5001`
3. Type messages and verify they appear immediately
4. Verify network bandwidth is efficient (binary format)
5. Test with special characters and Unicode

## Performance Characteristics

- **Message latency:** ~1-10ms over local TCP
- **Memory usage:** ~100KB per connected client (single message buffer)
- **Message size:** Up to 4096 bytes content
- **Serialization overhead:** 16 bytes per message (headers)
- **Network protocol:** Efficient binary format, not text-based

## Future Improvements

1. **Authentication** - User login and password verification
2. **Message history** - Database backend for persistent storage
3. **Encryption** - TLS/SSL for secure communication
4. **Multiple clients** - Client broadcasting to multiple receivers
5. **Message types** - Support different message types (text, file, etc.)
6. **Compression** - zlib compression for large messages
7. **Async I/O** - io_uring on Linux for better scalability
8. **Structured logging** - Write to file instead of console
9. **Configuration file** - Load ports/addresses from config
10. **NAT traversal** - STUN/TURN for cross-network communication

## Troubleshooting

### Build Issues
- **"cmake not found"**: Install CMake from cmake.org
- **"mingw32-make not found"**: Install MinGW from MinGW-W64
- **Winsock errors**: Ensure Windows SDK is installed

### Runtime Issues
- **"Connection refused"**: Server not running on specified address/port
- **"Address already in use"**: Another process using the port, wait 60 seconds or use different port
- **Garbled messages**: Endianness mismatch (shouldn't happen - code uses big-endian)
- **Slow messages**: Network congestion or firewall delays

### Debugging
- Add verbose logging in main_*.cpp
- Use Wireshark to inspect network packets
- Check Windows Firewall allows connections
- Use netstat to verify port is listening
