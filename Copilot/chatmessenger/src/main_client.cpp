/**
 * @file main_client.cpp
 * @brief Client entry point
 */

#include "client.h"
#include <iostream>
#include <string>

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name 
              << " <server_ip> <server_port> <local_ip> <local_port>" << std::endl;
    std::cout << "\nExample: " << program_name << " 127.0.0.1 5000 127.0.0.1 5001" << std::endl;
}

int main(int argc, char* argv[]) {
    // Initialize Winsock (Windows)
    if (!Socket::initWinsock()) {
        std::cerr << "Failed to initialize network" << std::endl;
        return 1;
    }
    
    // Validate command line arguments
    if (argc != 5) {
        printUsage(argv[0]);
        Socket::cleanupWinsock();
        return 1;
    }
    
    // Parse arguments
    std::string server_ip = argv[1];
    uint16_t server_port = 0;
    std::string local_ip = argv[3];
    uint16_t local_port = 0;
    
    try {
        server_port = static_cast<uint16_t>(std::stoi(argv[2]));
        local_port = static_cast<uint16_t>(std::stoi(argv[4]));
    } catch (const std::exception& e) {
        std::cerr << "Invalid port number: " << e.what() << std::endl;
        Socket::cleanupWinsock();
        return 1;
    }
    
    // Create and connect client
    ChatClient client(server_ip, server_port, local_ip, local_port);
    
    if (!client.connect()) {
        std::cerr << "Failed to connect to server" << std::endl;
        Socket::cleanupWinsock();
        return 1;
    }
    
    // Interactive message loop
    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);
    
    if (username.empty()) {
        username = "user";
    }
    
    std::cout << "Connected! Type messages below (type 'quit' to exit):" << std::endl;
    
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "quit") {
            break;
        }
        
        if (line.empty()) {
            continue;
        }
        
        // Create and send message
        Message msg(username, line);
        
        if (!msg.validate()) {
            std::cerr << "Invalid message: " << msg.getValidationError() << std::endl;
            continue;
        }
        
        if (!client.sendMessage(msg)) {
            std::cerr << "Failed to send message" << std::endl;
            break;
        }
    }
    
    // Cleanup
    client.disconnect();
    Socket::cleanupWinsock();
    
    return 0;
}
