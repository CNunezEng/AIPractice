cmake_minimum_required(VERSION 3.10)
project(CppChatApp VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4 /WX)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Include directories
include_directories(src/include)

# Source files
set(SERVER_SOURCES
    src/server/chat_server.cpp
    src/server/main.cpp
)

set(CLIENT_SOURCES
    src/client/chat_client.cpp
    src/client/main.cpp
)

# Create server executable
add_executable(chat_server ${SERVER_SOURCES})

# Create client executable
add_executable(chat_client ${CLIENT_SOURCES})

# Platform-specific linking
if(WIN32)
    target_link_libraries(chat_server ws2_32)
    target_link_libraries(chat_client ws2_32)
endif()

# Set output directory
set_target_properties(chat_server chat_client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Installation
install(TARGETS chat_server chat_client
    RUNTIME DESTINATION bin
)

# Custom targets for convenience
target_compile_definitions(chat_server PRIVATE SERVER_BUILD)
target_compile_definitions(chat_client PRIVATE CLIENT_BUILD)

# Add custom clean target
add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/bin
    COMMENT "Cleaning all build artifacts"
)
