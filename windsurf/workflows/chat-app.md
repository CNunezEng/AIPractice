---
description: Create a simple chat server and client application
---

# Chat Application Workflow

This workflow creates a simple chat server and client following the standards in AIinfo.txt.

## Steps

1. Set up project structure
   - Create chat-app/ directory under windsurf/
   - Set up src/, tests/, docs/ subdirectories
   - Create separate folders for server and client code

2. Design interfaces
   - Define IChatServer interface
   - Define IChatClient interface
   - Define IMessage protocol/interface

3. Implement chat server
   - Create TCP server with socket handling
   - Implement client connection management
   - Add message broadcasting to all connected clients
   - Include error handling and logging

4. Implement chat client
   - Create TCP client with connection management
   - Add user interface for sending/receiving messages
   - Implement connection status monitoring
   - Include graceful disconnect handling

5. Add testing
   - Unit test server connection handling
   - Unit test client message sending/receiving
   - Integration test with multiple clients
   - Edge case testing (disconnections, malformed messages)

6. Documentation
   - Update README with build/run instructions
   - Document API interfaces
   - Create user guide for running server and clients
   - Set up user input log template

7. Review and validate
   - Check error handling for network failures
   - Verify message format consistency
   - Ensure no blocking operations in UI
   - Test concurrent client connections
