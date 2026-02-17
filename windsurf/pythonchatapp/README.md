# Simple Chat Application

A basic chat server and client implementation following professional engineering standards.

## Project Structure

```
windsurf/chat-app/
├── src/
│   ├── interfaces.py          # Abstract interfaces and message protocol
│   ├── server/
│   │   ├── chat_server.py     # Server implementation
│   │   └── run_server.py      # Server entry point
│   └── client/
│       ├── chat_client.py     # Client implementation
│       └── run_client.py      # Client entry point
├── tests/                     # Unit tests (to be implemented)
├── docs/                      # Documentation (to be expanded)
└── README.md                  # This file
```

## Features

- **Asynchronous Architecture**: Uses async/await for non-blocking operations
- **Clean Interfaces**: Explicit interfaces following dependency injection patterns
- **Error Handling**: Comprehensive error handling and logging
- **Message Protocol**: JSON-based message format with validation
- **Multiple Clients**: Supports concurrent client connections
- **Graceful Disconnect**: Proper connection cleanup

## Requirements

- Python 3.8+
- websockets library

## Installation

1. Install dependencies:
```bash
pip install websockets
```

2. Navigate to the chat-app directory:
```bash
cd windsurf/chat-app/src
```

## Usage

### Start the Server

```bash
python server/run_server.py [--host HOST] [--port PORT] [--log-level LEVEL]
```

Default: `python server/run_server.py --host localhost --port 8765`

Options:
- `--host`: Host to bind to (default: localhost)
- `--port`: Port to bind to (default: 8765)
- `--log-level`: Logging level (DEBUG, INFO, WARNING, ERROR)

### Start a Client

```bash
python client/run_client.py [--host HOST] [--port PORT] [--log-level LEVEL]
```

Default: `python client/run_client.py --host localhost --port 8765`

### Chat Commands

- Type your message and press Enter to send
- Type `/quit` to exit the client

## Design Patterns Used

- **Interface Segregation**: Separate interfaces for server, client, and message handling
- **Dependency Injection**: Message handler injected into server
- **Strategy Pattern**: Pluggable message processing strategies
- **Factory Pattern**: Factory functions for object creation
- **Observer Pattern**: Server observes client connections

## Architecture Standards

Following AIinfo.txt standards:
- Single responsibility principle
- Explicit interfaces for cross-module interaction
- Comprehensive error handling
- No hidden state
- Separation of configuration from logic
- Unit testable design

## Testing

Unit tests to be implemented in the `tests/` directory following the workflow.

## Future Enhancements

- GUI client interface
- Message history persistence
- User authentication
- Private messaging
- File sharing capabilities
- Message encryption
