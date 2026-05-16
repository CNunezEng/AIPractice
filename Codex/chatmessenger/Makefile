CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -Iinclude -IthirdPartyIncludes/Catch2
LDFLAGS := -lws2_32
BUILD_DIR := build

COMMON_SRC := src/ChatConfig.cpp src/UdpSocket.cpp src/ChatApp.cpp
CLIENT_SRC := $(COMMON_SRC) src/Client.cpp
SERVER_SRC := $(COMMON_SRC) src/Server.cpp
TEST_SRC := src/ChatConfig.cpp tests/TestRunner.cpp thirdPartyIncludes/Catch2/catch_amalgamated.cpp

.PHONY: all clean test dirs

all: dirs $(BUILD_DIR)/Client.exe $(BUILD_DIR)/Server.exe $(BUILD_DIR)/TestRunner.exe

dirs:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"

$(BUILD_DIR)/Client.exe: $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/Server.exe: $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/TestRunner.exe: $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

test: all
	$(BUILD_DIR)/TestRunner.exe

clean:
	@if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"
