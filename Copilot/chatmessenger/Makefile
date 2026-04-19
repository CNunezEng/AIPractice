# Makefile for Chat Messenger
# Builds server, client, and tests with Release and Debug configurations

CXX = g++
CXXFLAGS_BASE = -std=c++17 -Wall -Wextra -I./include

# Release configuration (optimized, symbols stripped)
CXXFLAGS_RELEASE = $(CXXFLAGS_BASE) -O2
LDFLAGS_RELEASE = -lws2_32 -s

# Debug configuration (no optimization, debug symbols included)
CXXFLAGS_DEBUG = $(CXXFLAGS_BASE) -g -O0
LDFLAGS_DEBUG = -lws2_32

# Default to Release unless DEBUG=1 is specified
ifdef DEBUG
CXXFLAGS = $(CXXFLAGS_DEBUG)
LDFLAGS = $(LDFLAGS_DEBUG)
BUILD_SUFFIX = _debug
else
CXXFLAGS = $(CXXFLAGS_RELEASE)
LDFLAGS = $(LDFLAGS_RELEASE)
BUILD_SUFFIX = 
endif

# Paths
BINDIR = bin
SRCDIR = src
TESTDIR = tests
INCDIR = include
CATCH2_PATH = thirdPartyIncludes/catchorg-Catch2-31b1109/extras
CATCH2_SOURCES = $(CATCH2_PATH)/catch_amalgamated.cpp

# Common source files
COMMON_SOURCES = $(SRCDIR)/message.cpp $(SRCDIR)/socket_util.cpp

# Targets
TARGETS = $(BINDIR)/server $(BINDIR)/client $(BINDIR)/test_message

# Default target
all: $(TARGETS)

# Create bin directory
$(BINDIR):
	@if not exist $(BINDIR) mkdir $(BINDIR)

# Server executable
$(BINDIR)/server: $(BINDIR) $(SRCDIR)/main_server.cpp $(SRCDIR)/server.cpp $(COMMON_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCDIR)/main_server.cpp $(SRCDIR)/server.cpp $(COMMON_SOURCES) $(LDFLAGS)

# Client executable
$(BINDIR)/client: $(BINDIR) $(SRCDIR)/main_client.cpp $(SRCDIR)/client.cpp $(COMMON_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCDIR)/main_client.cpp $(SRCDIR)/client.cpp $(COMMON_SOURCES) $(LDFLAGS)

# Test executable with Catch2 (loads test cases from test_cases.ini at runtime)
$(BINDIR)/test_message: $(BINDIR) $(TESTDIR)/test_message.cpp $(TESTDIR)/test_loader.h $(SRCDIR)/message.cpp $(TESTDIR)/test_cases.ini
	$(CXX) $(CXXFLAGS) -I$(CATCH2_PATH) -o $@ $(TESTDIR)/test_message.cpp $(SRCDIR)/message.cpp $(CATCH2_SOURCES) $(LDFLAGS)

# Run all targets
run: all
	@echo "Build complete!"
	@echo "Server: $(BINDIR)/server.exe 127.0.0.1 5000"
	@echo "Client: $(BINDIR)/client.exe 127.0.0.1 5000 127.0.0.1 5001"
	@echo "Tests:  $(BINDIR)/test_message.exe"

# Help - show available targets
help:
	@echo "Chat Messenger Build System"
	@echo "=============================="
	@echo ""
	@echo "Release Targets (optimized, minimal size):"
	@echo "  make all              - Build server, client, and tests"
	@echo "  make server           - Build and run server"
	@echo "  make client           - Build and run client"
	@echo "  make test             - Build and run tests"
	@echo "  make run              - Show build completion and usage"
	@echo "  make clean            - Remove all built binaries"
	@echo ""
	@echo "Debug Targets (includes debug symbols for troubleshooting):"
	@echo "  make debug            - Build all with debug symbols"
	@echo "  make debug-server     - Build and run server with debug symbols"
	@echo "  make debug-client     - Build and run client with debug symbols"
	@echo "  make debug-test       - Build and run tests with debug symbols"
	@echo "  make debug-clean      - Clean debug builds"
	@echo ""
	@echo "Examples:"
	@echo "  make test             - Run tests (release, optimized)"
	@echo "  make DEBUG=1 test     - Run tests (debug, with symbols)"
	@echo "  make server           - Start server (release)"

# Run tests (must run from root directory so test_cases.ini is found)
test: $(BINDIR)/test_message
	@echo Running tests with configuration from tests/test_cases.ini...
	$(BINDIR)/test_message.exe

# Run server
server: $(BINDIR)/server
	$(BINDIR)/server.exe 127.0.0.1 5000

# Run client
client: $(BINDIR)/client
	$(BINDIR)/client.exe 127.0.0.1 5000 127.0.0.1 5001

# Clean build artifacts (Windows compatible)
clean:
	@if exist $(BINDIR) rmdir /s /q $(BINDIR)
	@echo Clean complete

# Debug targets (build with debug symbols for troubleshooting)
debug:
	$(MAKE) DEBUG=1 all

debug-test:
	$(MAKE) DEBUG=1 test

debug-server:
	$(MAKE) DEBUG=1 server

debug-client:
	$(MAKE) DEBUG=1 client

debug-clean:
	$(MAKE) DEBUG=1 clean

# Phony targets
.PHONY: all run test server client clean debug debug-test debug-server debug-client debug-clean help
