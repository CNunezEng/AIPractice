/**
 * @file client.cpp
 * @brief Chat client implementation
 */

#include "client.h"
#include <iostream>
#include <thread>
#include <cstring>

/**
 * Constructor
 */
ChatClient::ChatClient(const std::string& server_ip, uint16_t server_port,
                      const std::string& local_ip, uint16_t local_port)
    : server_ip_(server_ip), server_port_(server_port),
      local_ip_(local_ip), local_port_(local_port),
      connected_(false), running_(false) {
}

/**
 * Destructor
 */
ChatClient::~ChatClient() {
    disconnect();
}

/**
 * Connect to server
 */
bool ChatClient::connect() {
    try {
        // Connect to server
        if (!client_socket_.connect(server_ip_, server_port_)) {
            std::cerr << "Failed to connect to server at " << server_ip_ << ":" << server_port_ << std::endl;
            return false;
        }
        
        // Bind listening socket to local address
        if (!listen_socket_.bind(local_ip_, local_port_)) {
            std::cerr << "Failed to bind to local address" << std::endl;
            client_socket_.close();
            return false;
        }
        
        if (!listen_socket_.listen(1)) {
            std::cerr << "Failed to listen on local socket" << std::endl;
            client_socket_.close();
            return false;
        }
        
        connected_ = true;
        running_ = true;
        
        std::cout << "Connected to server at " << server_ip_ << ":" << server_port_ << std::endl;
        std::cout << "Listening on " << local_ip_ << ":" << local_port_ << std::endl;
        
        // Start receive thread
        receive_thread_ = std::thread(&ChatClient::receiveLoop, this);
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Connection error: " << e.what() << std::endl;
        return false;
    }
}

/**
 * Send message
 */
bool ChatClient::sendMessage(const IMessage& message) {
    if (!connected_) {
        std::cerr << "Not connected to server" << std::endl;
        return false;
    }
    
    // Validate message
    if (!const_cast<IMessage&>(message).validate()) {
        std::cerr << "Invalid message: " << const_cast<IMessage&>(message).getValidationError() << std::endl;
        return false;
    }
    
    // Serialize and send
    auto data = const_cast<IMessage&>(message).serialize();
    int bytes_sent = client_socket_.send(data.data(), static_cast<int>(data.size()));
    
    if (bytes_sent < 0) {
        std::cerr << "Failed to send message" << std::endl;
        connected_ = false;
        return false;
    }
    
    return true;
}

/**
 * Check if connected
 */
bool ChatClient::isConnected() const {
    return connected_;
}

/**
 * Disconnect from server
 */
void ChatClient::disconnect() {
    running_ = false;
    connected_ = false;
    
    client_socket_.close();
    listen_socket_.close();
    
    if (receive_thread_.joinable()) {
        receive_thread_.join();
    }
    
    std::cout << "Disconnected from server" << std::endl;
}

/**
 * Receive loop thread
 */
void ChatClient::receiveLoop() {
    const int BUFFER_SIZE = 8192;
    char buffer[BUFFER_SIZE];
    
    while (running_) {
        // Try to accept incoming connection from server
        Socket accepted = listen_socket_.accept();
        
        if (accepted.isValid()) {
            // Receive messages from server
            int bytes_received = accepted.receive(buffer, BUFFER_SIZE, 100);
            
            if (bytes_received > 0) {
                // Deserialize and display message
                Message msg = Message::deserialize(buffer, bytes_received);
                
                if (!msg.isEmpty()) {
                    try {
                        std::cout << "[" << msg.getSender() << "]: " << msg.getContent() << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << "Error: " << e.what() << std::endl;
                    }
                }
            } else if (bytes_received < 0) {
                std::cerr << "Connection error receiving message" << std::endl;
                connected_ = false;
                return;
            }
        }
        
        // Sleep briefly to avoid busy-waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
