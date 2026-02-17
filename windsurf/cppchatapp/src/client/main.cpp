/**
 * @file main.cpp
 * @brief Entry point for C++ chat client
 * 
 * Following AIinfo.txt standards for clear entry points and error handling.
 * Provides command-line interface for client configuration.
 */

#include "chat_client.h"
#include <iostream>
#include <string>

using namespace chat;

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --host <hostname>    Server host to connect to (default: localhost)" << std::endl;
    std::cout << "  --port <port>        Server port to connect to (default: 8765)" << std::endl;
    std::cout << "  --help               Show this help message" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string host = "localhost";
    int port = 8765;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            print_usage(argv[0]);
            return 0;
        } else if (arg == "--host" && i + 1 < argc) {
            host = argv[++i];
        } else if (arg == "--port" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        } else {
            std::cerr << "[ERROR] Unknown argument: " << arg << std::endl;
            print_usage(argv[0]);
            return 1;
        }
    }
    
    std::cout << "[CLIENT] Starting C++ Chat Client" << std::endl;
    std::cout << "[CLIENT] Connecting to: " << host << ":" << port << std::endl;
    
    try {
        // Create interactive client
        auto client = create_interactive_client();
        
        // Start interactive session
        client->start_interactive_session(host, port);
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Client error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
