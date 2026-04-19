/**
 * @file server.cpp
 * @brief Chat server implementation
 */

#include "server.h"
#include <iostream>
#include <thread>
#include <cstring>

/**
 * Constructor
 */
ChatServer::ChatServer(const std::string& bind_ip, uint16_t bind_port)
    : bind_ip_(bind_ip), bind_port_(bind_port), running_(false) {
}

/**
 * Destructor
 */
ChatServer::~ChatServer() {
    stop();
}

/**
 * Start the server
 */
bool ChatServer::start() {
    try {
        // Bind socket
        if (!listen_socket_.bind(bind_ip_, bind_port_)) {
            std::cerr << "Failed to bind to " << bind_ip_ << ":" << bind_port_ << std::endl;
            return false;
        }
        
        // Listen for connections
        if (!listen_socket_.listen(5)) {
            std::cerr << "Failed to listen" << std::endl;
            return false;
        }
        
        running_ = true;
        
        std::cout << "Server started on " << bind_ip_ << ":" << bind_port_ << std::endl;
        std::cout << "Waiting for client connections..." << std::endl;
        
        // Start accept thread
        accept_thread_ = std::thread(&ChatServer::acceptLoop, this);
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return false;
    }
}

/**
 * Stop the server
 */
void ChatServer::stop() {
    running_ = false;
    listen_socket_.close();
    
    if (accept_thread_.joinable()) {
        accept_thread_.join();
    }
    
    std::cout << "Server stopped" << std::endl;
}

/**
 * Check if server is running
 */
bool ChatServer::isRunning() const {
    return running_;
}

/**
 * Accept loop thread
 */
void ChatServer::acceptLoop() {
    while (running_) {
        // Accept incoming connection
        Socket client = listen_socket_.accept();
        
        if (client.isValid()) {
            std::cout << "Client connected" << std::endl;
            
            // Handle client in separate thread
            std::thread(&ChatServer::handleClient, this, std::move(client)).detach();
        }
        
        // Sleep briefly to avoid busy-waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

/**
 * Handle a client connection
 */
void ChatServer::handleClient(Socket client) {
    const int BUFFER_SIZE = 8192;
    char buffer[BUFFER_SIZE];
    
    try {
        while (running_) {
            // Receive message
            int bytes_received = client.receive(buffer, BUFFER_SIZE, 1000);
            
            if (bytes_received <= 0) {
                if (bytes_received < 0) {
                    std::cerr << "Error receiving from client" << std::endl;
                }
                break;  // Connection closed or error
            }
            
            // Deserialize message
            Message msg = Message::deserialize(buffer, bytes_received);
            
            if (!msg.isEmpty() && msg.validate()) {
                try {
                    std::cout << "[" << msg.getSender() << "]: " << msg.getContent() << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Client handler error: " << e.what() << std::endl;
    }
    
    std::cout << "Client disconnected" << std::endl;
}
