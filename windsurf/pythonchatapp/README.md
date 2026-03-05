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

### Test Structure

The Python chat application includes comprehensive unit tests following AIinfo.txt standards:

```
tests/
├── test_interfaces.py      # Interface and message protocol tests
├── test_server.py          # Server implementation tests
├── test_client.py          # Client implementation tests
├── test_inputs.json        # Runtime test data
└── run_tests.py           # Automated test runner
```

### Running Tests

#### Run All Tests
```bash
cd windsurf/pythonchatapp/tests
python run_tests.py
```

#### Run Individual Test Suites
```bash
# Interface tests
python -m unittest test_interfaces.py -v

# Server tests
python -m unittest test_server.py -v

# Client tests
python -m unittest test_client.py -v
```

### Test Coverage

The test suite covers:

#### Standard Cases
- Normal message creation and validation
- Server startup and shutdown
- Client connection and disconnection
- Message sending and receiving

#### Edge Cases
- Maximum length messages (1000 characters)
- Special characters and Unicode content
- Different message types (text, image, file, system, error)
- Concurrent operations

#### Wrong Inputs
- Invalid JSON format
- Empty sender IDs and content
- Messages exceeding length limits
- Network connection failures
- Invalid host/port combinations

### Test Data

Runtime test cases are defined in `tests/test_inputs.json`:
- **standard_cases**: Normal usage scenarios
- **edge_cases**: Boundary conditions
- **wrong_inputs**: Invalid inputs that should be rejected
- **message_types**: Different message type validation
- **concurrent_test_cases**: Multi-threading scenarios

### Automated Testing Features

- **Comprehensive Reporting**: Detailed test results with success rates
- **Performance Metrics**: Timing for serialization and operations
- **Concurrent Testing**: Multi-threaded message creation tests
- **JSON Report Generation**: Automated test report creation
- **Runtime Input Testing**: Tests using external data files

### Test Standards

Following AIinfo.txt testing requirements:
- ✅ Unit test every function with standard case, edge cases, and wrong inputs
- ✅ Load at runtime file for case inputs (test_inputs.json)
- ✅ Instrument for test on commit using free tools available with GIT
- ✅ Comprehensive error handling validation
- ✅ Integration testing for component interaction

### Example Test Output

```
======================================================================
PYTHON CHAT APPLICATION - AUTOMATED TEST SUITE
======================================================================
Started at: 2026-02-18 16:15:30.123456

Running test_interfaces...
  Tests: 45, Failures: 0, Errors: 0

Running test_server...
  Tests: 32, Failures: 0, Errors: 0

Running test_client...
  Tests: 28, Failures: 0, Errors: 0

======================================================================
TEST SUMMARY
======================================================================
Total Tests: 105
Passed: 105
Failures: 0
Errors: 0
Success Rate: 100.0%
Duration: 0:00:02.345678
```

### Continuous Integration

To set up automated testing on commit:

```bash
# Add to .git/hooks/pre-commit
#!/bin/bash
cd windsurf/pythonchatapp/tests
python run_tests.py
if [ $? -ne 0 ]; then
    echo "Tests failed! Commit aborted."
    exit 1
fi
```

## Future Enhancements

- GUI client interface
- Message history persistence
- User authentication
- Private messaging
- File sharing capabilities
- Message encryption
