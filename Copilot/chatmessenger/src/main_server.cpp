/**
 * @file main_server.cpp
 * @brief Server entry point
 */

#include "server.h"
#include <iostream>
#include <string>
#include <csignal>

ChatServer* g_server = nullptr;

// Signal handler for graceful shutdown
void signalHandler([[maybe_unused]] int signal) {
    if (g_server != nullptr && g_server->isRunning()) {
        std::cout << "\nShutting down server..." << std::endl;
        g_server->stop();
    }
}

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " <bind_ip> <bind_port>" << std::endl;
    std::cout << "\nExample: " << program_name << " 127.0.0.1 5000" << std::endl;
}

int main(int argc, char* argv[]) {
    // Initialize Winsock (Windows)
    if (!Socket::initWinsock()) {
        std::cerr << "Failed to initialize network" << std::endl;
        return 1;
    }
    
    // Validate command line arguments
    if (argc != 3) {
        printUsage(argv[0]);
        Socket::cleanupWinsock();
        return 1;
    }
    
    // Parse arguments
    std::string bind_ip = argv[1];
    uint16_t bind_port = 0;
    
    try {
        bind_port = static_cast<uint16_t>(std::stoi(argv[2]));
    } catch (const std::exception& e) {
        std::cerr << "Invalid port number: " << e.what() << std::endl;
        Socket::cleanupWinsock();
        return 1;
    }
    
    // Set signal handler for graceful shutdown
    std::signal(SIGINT, signalHandler);
    
    // Create and start server
    ChatServer server(bind_ip, bind_port);
    g_server = &server;
    
    if (!server.start()) {
        std::cerr << "Failed to start server" << std::endl;
        Socket::cleanupWinsock();
        return 1;
    }
    
    std::cout << "\nPress Ctrl+C to shut down the server" << std::endl;
    
    // Keep server running
    while (server.isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Cleanup
    Socket::cleanupWinsock();
    
    return 0;
}
