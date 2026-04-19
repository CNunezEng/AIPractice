# Simple Makefile for C++ Chat Application
# Following AIinfo.txt standards for clean, maintainable build system

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Werror
CXXFLAGS += -I"src/include"
CXXFLAGS += -I"C:/Users/pixar/OneDrive/Desktop/practice program s/Showcase/thirdPartyIncludes/catchorg-Catch2-31b1109/src"
CXXFLAGS += -I"C:/Users/pixar/OneDrive/Desktop/practice program s/Showcase/thirdPartyIncludes/catchorg-Catch2-31b1109/src/catch2/internal"

# Platform-specific settings
UNAME_S := Windows
ifeq ($(OS),Windows_NT)
    # Windows
    MAKE = mingw32-make
    LDFLAGS = -lws2_32
    EXE_EXT = .exe
    RM = del /Q
    MKDIR = mkdir
else
    # Linux/Unix
    MAKE = make
    LDFLAGS = -pthread
    EXE_EXT = 
    RM = rm -f
    MKDIR = mkdir -p
endif

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin
TEST_DIR = tests

# Source files
SERVER_SOURCES = $(SRC_DIR)/server/chat_server.cpp $(SRC_DIR)/server/main.cpp
CLIENT_SOURCES = $(SRC_DIR)/client/chat_client.cpp $(SRC_DIR)/client/main.cpp
TEST_SOURCES = $(TEST_DIR)/test_message_simple.cpp

# Object files
SERVER_OBJECTS = $(SERVER_SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
CLIENT_OBJECTS = $(CLIENT_SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Executables
SERVER_EXE = $(BIN_DIR)/chat_server$(EXE_EXT)
CLIENT_EXE = $(BIN_DIR)/chat_client$(EXE_EXT)
TEST_EXE = $(BIN_DIR)/chat_tests$(EXE_EXT)

# Default target
.PHONY: all clean test run-tests help

all: $(SERVER_EXE) $(CLIENT_EXE) $(TEST_EXE)

# Create directories
$(BUILD_DIR):
ifeq ($(OS),Windows_NT)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@if not exist $(BUILD_DIR)\server mkdir $(BUILD_DIR)\server
	@if not exist $(BUILD_DIR)\client mkdir $(BUILD_DIR)\client
	@if not exist $(BUILD_DIR)\tests mkdir $(BUILD_DIR)\tests
	@if not exist $(BUILD_DIR)\bin mkdir $(BUILD_DIR)\bin
else
	$(MKDIR) $(BUILD_DIR)
	$(MKDIR) $(BUILD_DIR)/server
	$(MKDIR) $(BUILD_DIR)/client
	$(MKDIR) $(BUILD_DIR)/tests
	$(MKDIR) $(BIN_DIR)
endif

# Server executable
$(SERVER_EXE): $(SERVER_OBJECTS) | $(BUILD_DIR)
	@echo "Linking server executable..."
	$(CXX) $(SERVER_OBJECTS) -o $@ $(LDFLAGS)
	@echo "Server built: $@"

# Client executable  
$(CLIENT_EXE): $(CLIENT_OBJECTS) | $(BUILD_DIR)
	@echo "Linking client executable..."
	$(CXX) $(CLIENT_OBJECTS) -o $@ $(LDFLAGS)
	@echo "Client built: $@"

# Test executable (using Catch2)
$(TEST_EXE): $(TEST_OBJECTS) | $(BUILD_DIR)
	@echo "Linking test executable..."
	$(CXX) $(TEST_OBJECTS) -o $@ $(LDFLAGS)
	@echo "Tests built: $@"

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -DSERVER_BUILD -c $< -o $@

# Compile test files
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp | $(BUILD_DIR)
	@echo "Compiling test $<..."
	$(CXX) $(CXXFLAGS) -DTEST_BUILD -c $< -o $@

# Run tests
test: $(TEST_EXE)
	@echo "Running all tests..."
	./$(TEST_EXE) --success

run-tests: test

# Run specific test categories
test-message: $(TEST_EXE)
	@echo "Running message tests..."
	./$(TEST_EXE) "[message]"

test-server: $(TEST_EXE)
	@echo "Running server tests..."
	./$(TEST_EXE) "[server]"

test-client: $(TEST_EXE)
	@echo "Running client tests..."
	./$(TEST_EXE) "[client]"

# Performance tests
perf-test: $(TEST_EXE)
	@echo "Running performance tests..."
	./$(TEST_EXE) "[performance]"

# Quick build and test
quick-test: all test

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
ifeq ($(OS),Windows_NT)
	if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
else
	$(RM) -r $(BUILD_DIR)
endif
	@echo "Clean complete."

# Install executables
install: all
	@echo "Installing executables..."
	cp $(SERVER_EXE) /usr/local/bin/
	cp $(CLIENT_EXE) /usr/local/bin/
	@echo "Installation complete."

# Development targets
dev-server: $(SERVER_EXE)
	@echo "Starting development server..."
	./$(SERVER_EXE) --host localhost --port 8765

dev-client: $(CLIENT_EXE)
	@echo "Starting development client..."
	./$(CLIENT_EXE) --host localhost --port 8765

# Performance testing
memcheck: $(TEST_EXE)
ifeq ($(UNAME_S),Linux)
	@echo "Running memory check..."
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_EXE) --gtest_filter=MessageTest.*
else
	@echo "Memory check only available on Linux with valgrind"
endif

# Help target
help:
	@echo "C++ Chat Application Makefile"
	@echo "==============================="
	@echo ""
	@echo "Build targets:"
	@echo "  all          - Build all executables (server, client, tests)"
	@echo "  server       - Build server executable only"
	@echo "  client       - Build client executable only"
	@echo "  tests        - Build test executable only"
	@echo ""
	@echo "Test targets:"
	@echo "  test         - Run all tests"
	@echo "  run-tests    - Run tests with verbose output"
	@echo "  test-message - Run message tests only"
	@echo "  test-server  - Run server tests only"
	@echo "  quick-test   - Build and run all tests"
	@echo "  perf-test    - Run performance tests"
	@echo ""
	@echo "Development targets:"
	@echo "  dev-server   - Build and run server"
	@echo "  dev-client   - Build and run client"
	@echo "  memcheck     - Run memory leak check (Linux only)"
	@echo ""
	@echo "Utility targets:"
	@echo "  clean        - Remove build artifacts"
	@echo "  install      - Install executables to /usr/local/bin"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make all           # Build everything"
	@echo "  make test          # Run tests"
	@echo "  make dev-server    # Start development server"
	@echo "  make clean all     # Clean and rebuild"

# Phony targets for individual executables
.PHONY: server client tests dev-server dev-client memcheck perf-test

server: $(SERVER_EXE)
client: $(CLIENT_EXE)
tests: $(TEST_EXE)

# Dependencies (simplified - for complex projects, consider using makedepend)
$(BUILD_DIR)/server/chat_server.o: $(SRC_DIR)/include/interfaces.h $(SRC_DIR)/include/message_impl.h
$(BUILD_DIR)/server/main.o: $(SRC_DIR)/server/chat_server.h $(SRC_DIR)/include/interfaces.h
$(BUILD_DIR)/client/chat_client.o: $(SRC_DIR)/include/interfaces.h $(SRC_DIR)/include/message_impl.h
$(BUILD_DIR)/client/main.o: $(SRC_DIR)/client/chat_client.h $(SRC_DIR)/include/interfaces.h
$(BUILD_DIR)/tests/test_message.o: $(SRC_DIR)/include/interfaces.h $(SRC_DIR)/include/message_impl.h
$(BUILD_DIR)/tests/test_server.o: $(SRC_DIR)/include/interfaces.h $(SRC_DIR)/include/message_impl.h $(SRC_DIR)/server/chat_server.h
