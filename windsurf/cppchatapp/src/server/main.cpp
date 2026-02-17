/**
 * @file main.cpp
 * @brief Entry point for C++ chat server
 * 
 * Following AIinfo.txt standards for clear entry points and error handling.
 * Provides command-line interface for server configuration.
 */

#include "chat_server.h"
#include <iostream>
#include <string>
#include <signal.h>

using namespace chat;

std::unique_ptr<ChatServer> g_server = nullptr;

void signal_handler(int signal) {
    std::cout << "\n[SERVER] Received signal " << signal << ", shutting down..." << std::endl;
    if (g_server) {
        g_server->stop_server();
    }
}

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --host <hostname>    Host to bind to (default: localhost)" << std::endl;
    std::cout << "  --port <port>        Port to bind to (default: 8765)" << std::endl;
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
    
    // Set up signal handlers for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    std::cout << "[SERVER] Starting C++ Chat Server" << std::endl;
    std::cout << "[SERVER] Host: " << host << ", Port: " << port << std::endl;
    
    try {
        // Create and configure server
        g_server = std::make_unique<ChatServer>();
        
        // Start the server
        if (!g_server->start_server(host, port)) {
            std::cerr << "[ERROR] Failed to start server" << std::endl;
            return 1;
        }
        
        std::cout << "[SERVER] Server running. Press Ctrl+C to stop." << std::endl;
        
        // Keep server running
        while (g_server->is_running()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            // Print server status periodically
            auto clients = g_server->get_connected_clients();
            std::cout << "[SERVER] Status: " << clients.size() << " clients connected" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Server error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "[SERVER] Server stopped successfully" << std::endl;
    return 0;
}
