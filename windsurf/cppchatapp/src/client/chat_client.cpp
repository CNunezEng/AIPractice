/**
 * @file chat_client.cpp
 * @brief Implementation of C++ chat client
 * 
 * Following AIinfo.txt standards for comprehensive error handling
 * and maintainable code structure.
 */

#include "chat_client.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>

#ifndef _WIN32
#include <fcntl.h>
#include <unistd.h>
#endif

namespace chat {

// ChatClient Implementation
ChatClient::ChatClient() 
    : client_socket_(INVALID_SOCKET), connected_(false), receiving_(false)
    , connection_status_("Disconnected") {
    
#ifdef _WIN32
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data);
#endif
}

ChatClient::~ChatClient() {
    disconnect();
    
#ifdef _WIN32
    WSACleanup();
#endif
}

bool ChatClient::connect(const std::string& host, int port) {
    if (connected_) {
        update_status("Already connected");
        return false;
    }
    
    if (!initialize_socket()) {
        update_status("Failed to initialize socket");
        return false;
    }
    
    // Set up server address
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    // Convert host to IP address
    if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0) {
        update_status("Invalid host address");
        cleanup_socket();
        return false;
    }
    
    // Connect to server
    if (::connect(client_socket_, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        update_status("Connection failed");
        cleanup_socket();
        return false;
    }
    
    connected_ = true;
    update_status("Connected to " + host + ":" + std::to_string(port));
    
    std::cout << "[CLIENT] Connected to chat server at " << host << ":" << port << std::endl;
    return true;
}

void ChatClient::disconnect() {
    if (receiving_) {
        receiving_ = false;
        if (receive_thread_.joinable()) {
            receive_thread_.join();
        }
    }
    
    if (connected_) {
        cleanup_socket();
        connected_ = false;
        update_status("Disconnected");
        std::cout << "[CLIENT] Disconnected from server" << std::endl;
    }
}

bool ChatClient::send_message(const std::string& content) {
    if (!connected_) {
        update_status("Not connected to server");
        return false;
    }
    
    Message message("client", content);
    std::string json_data = message.to_json();
    
    int result = send(client_socket_, json_data.c_str(), static_cast<int>(json_data.length()), 0);
    if (result == SOCKET_ERROR) {
        update_status("Failed to send message");
        return false;
    }
    
    return true;
}

void ChatClient::start_receiving(std::function<void(const Message&)> message_handler) {
    if (!connected_) {
        update_status("Not connected to server");
        return;
    }
    
    message_handler_ = message_handler;
    receiving_ = true;
    
    receive_thread_ = std::thread(&ChatClient::receive_loop, this);
}

bool ChatClient::is_connected() const {
    return connected_;
}

std::string ChatClient::get_connection_status() const {
    return connection_status_;
}

void ChatClient::set_message_handler(std::function<void(const Message&)> handler) {
    message_handler_ = handler;
}

void ChatClient::receive_loop() {
    char buffer[4096];
    
    while (receiving_ && connected_) {
        int bytes_received = recv(client_socket_, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received <= 0) {
            // Connection closed or error
            update_status("Connection lost");
            connected_ = false;
            break;
        }
        
        buffer[bytes_received] = '\0';
        
        try {
            Message message = Message::from_json(std::string(buffer));
            
            if (message_handler_) {
                message_handler_(message);
            }
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Failed to parse received message: " << e.what() << std::endl;
        }
    }
}

void ChatClient::update_status(const std::string& status) {
    std::lock_guard<std::mutex> lock(status_mutex_);
    connection_status_ = status;
}

bool ChatClient::initialize_socket() {
    client_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket_ == INVALID_SOCKET) {
        return false;
    }
    
    // Set socket to non-blocking mode
#ifdef _WIN32
    u_long mode = 1;  // 1 = non-blocking
    ioctlsocket(client_socket_, FIONBIO, &mode);
#else
    int flags = fcntl(client_socket_, F_GETFL, 0);
    fcntl(client_socket_, F_SETFL, flags | O_NONBLOCK);
#endif
    
    return true;
}

void ChatClient::cleanup_socket() {
    if (client_socket_ != INVALID_SOCKET) {
        closesocket(client_socket_);
        client_socket_ = INVALID_SOCKET;
    }
}

// InteractiveChatClient Implementation
InteractiveChatClient::InteractiveChatClient() : running_(false) {
}

void InteractiveChatClient::start_interactive_session(const std::string& host, int port) {
    if (!connect(host, port)) {
        std::cerr << "[ERROR] Failed to connect to server" << std::endl;
        return;
    }
    
    running_ = true;
    
    // Set up message handler to display received messages
    start_receiving([this](const Message& message) {
        auto timestamp = std::chrono::system_clock::to_time_t(message.timestamp);
        std::cout << "[" << std::put_time(std::localtime(&timestamp), "%H:%M:%S") 
                  << "] " << message.sender_id << ": " << message.content << std::endl;
    });
    
    std::cout << "\nConnected to chat server! Commands:" << std::endl;
    std::cout << "  /help  - Show this help message" << std::endl;
    std::cout << "  /quit  - Exit the chat" << std::endl;
    std::cout << "  Type your message and press Enter to send\n" << std::endl;
    
    // Main input loop
    handle_user_input();
    
    stop_interactive_session();
}

void InteractiveChatClient::stop_interactive_session() {
    running_ = false;
    disconnect();
    std::cout << "Chat session ended." << std::endl;
}

void InteractiveChatClient::display_help() {
    std::cout << "\nAvailable commands:" << std::endl;
    std::cout << "  /help  - Show this help message" << std::endl;
    std::cout << "  /quit  - Exit the chat" << std::endl;
    std::cout << "  Just type your message and press Enter to send\n" << std::endl;
}

void InteractiveChatClient::handle_user_input() {
    std::string input;
    
    while (running_ && is_connected()) {
        std::cout << "> ";
        std::getline(std::cin, input);
        
        if (!running_) {
            break;
        }
        
        if (input.empty()) {
            continue;
        }
        
        if (input == "/quit") {
            break;
        } else if (input == "/help") {
            display_help();
        } else {
            if (!send_message(input)) {
                std::cerr << "[ERROR] Failed to send message" << std::endl;
            }
        }
    }
}

// Factory function implementations
std::unique_ptr<IChatClient> create_chat_client() {
    return std::make_unique<ChatClient>();
}

std::unique_ptr<InteractiveChatClient> create_interactive_client() {
    return std::make_unique<InteractiveChatClient>();
}

} // namespace chat
