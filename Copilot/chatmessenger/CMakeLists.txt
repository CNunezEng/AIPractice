cmake_minimum_required(VERSION 3.10)
project(ChatMessenger CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files for library
set(COMMON_SOURCES
    src/message.cpp
    src/socket_util.cpp
)

# Server executable
add_executable(server
    src/main_server.cpp
    src/server.cpp
    ${COMMON_SOURCES}
)

# Client executable
add_executable(client
    src/main_client.cpp
    src/client.cpp
    ${COMMON_SOURCES}
)

# Platform-specific linking
if(WIN32)
    target_link_libraries(server ws2_32)
    target_link_libraries(client ws2_32)
    target_compile_definitions(server PRIVATE _WIN32_WINNT=0x0601)
    target_compile_definitions(client PRIVATE _WIN32_WINNT=0x0601)
else()
    target_link_libraries(server pthread)
    target_link_libraries(client pthread)
endif()

# Test executable
add_executable(test_message
    tests/test_message.cpp
    src/message.cpp
)

if(WIN32)
    target_link_libraries(test_message ws2_32)
else()
    target_link_libraries(test_message pthread)
endif()

# Enable testing
enable_testing()
add_test(NAME MessageTest COMMAND test_message)
