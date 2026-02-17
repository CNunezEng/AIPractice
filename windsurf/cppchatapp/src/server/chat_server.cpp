/**
 * @file chat_server.cpp
 * @brief Implementation of C++ chat server
 * 
 * Following AIinfo.txt standards for comprehensive error handling
 * and maintainable code structure.
 */

#include "chat_server.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>

#ifndef _WIN32
#include <fcntl.h>
#include <unistd.h>
#endif

namespace chat {

// DefaultMessageHandler Implementation
bool DefaultMessageHandler::handle_message(const Message& message) {
    // Basic message processing - log the message
    std::cout << "[SERVER] Processing message from " << message.sender_id 
              << ": " << message.content << std::endl;
    return true;
}

bool DefaultMessageHandler::validate_message(const Message& message) {
    return message.is_valid();
}

// SocketConnectionManager Implementation
SocketConnectionManager::SocketConnectionManager() 
    : server_socket_(INVALID_SOCKET), running_(false) {
#ifdef _WIN32
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data);
#endif
}

SocketConnectionManager::~SocketConnectionManager() {
    shutdown();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool SocketConnectionManager::initialize(const std::string& host, int port) {
    // Create server socket
    server_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket_ == INVALID_SOCKET) {
        std::cerr << "[ERROR] Failed to create socket" << std::endl;
        return false;
    }
    
    // Set socket to non-blocking mode
#ifdef _WIN32
    u_long mode = 1;  // 1 = non-blocking
    ioctlsocket(server_socket_, FIONBIO, &mode);
#else
    int flags = fcntl(server_socket_, F_GETFL, 0);
    fcntl(server_socket_, F_SETFL, flags | O_NONBLOCK);
#endif
    
    // Bind socket
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(server_socket_, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "[ERROR] Failed to bind socket" << std::endl;
        closesocket(server_socket_);
        server_socket_ = INVALID_SOCKET;
        return false;
    }
    
    // Start listening
    if (listen(server_socket_, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "[ERROR] Failed to listen on socket" << std::endl;
        closesocket(server_socket_);
        server_socket_ = INVALID_SOCKET;
        return false;
    }
    
    running_ = true;
    std::cout << "[SERVER] Socket initialized on " << host << ":" << port << std::endl;
    return true;
}

void SocketConnectionManager::shutdown() {
    if (running_) {
        running_ = false;
        
        // Close all client connections
        std::lock_guard<std::mutex> lock(connections_mutex_);
        for (const auto& [conn_id, socket] : client_sockets_) {
            closesocket(socket);
        }
        client_sockets_.clear();
        
        // Close server socket
        if (server_socket_ != INVALID_SOCKET) {
            closesocket(server_socket_);
            server_socket_ = INVALID_SOCKET;
        }
    }
}

std::string SocketConnectionManager::accept_connection() {
    if (!running_ || server_socket_ == INVALID_SOCKET) {
        return "";
    }
    
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    SOCKET client_socket = accept(server_socket_, (sockaddr*)&client_addr, &client_len);
    if (client_socket == INVALID_SOCKET) {
        return "";
    }
    
    // Generate connection ID
    std::string conn_id = "conn_" + std::to_string(reinterpret_cast<uintptr_t>(client_socket));
    
    // Store client socket
    {
        std::lock_guard<std::mutex> lock(connections_mutex_);
        client_sockets_[conn_id] = client_socket;
    }
    
    // Set client socket to non-blocking
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(client_socket, FIONBIO, &mode);
#else
    int flags = fcntl(client_socket, F_GETFL, 0);
    fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);
#endif
    
    std::cout << "[SERVER] Client connected: " << conn_id << std::endl;
    return conn_id;
}

void SocketConnectionManager::close_connection(const std::string& connection_id) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    auto it = client_sockets_.find(connection_id);
    if (it != client_sockets_.end()) {
        closesocket(it->second);
        client_sockets_.erase(it);
        std::cout << "[SERVER] Client disconnected: " << connection_id << std::endl;
    }
}

bool SocketConnectionManager::send_data(const std::string& connection_id, const std::string& data) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    auto it = client_sockets_.find(connection_id);
    if (it == client_sockets_.end()) {
        return false;
    }
    
    int result = send(it->second, data.c_str(), static_cast<int>(data.length()), 0);
    return result != SOCKET_ERROR;
}

std::string SocketConnectionManager::receive_data(const std::string& connection_id) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    auto it = client_sockets_.find(connection_id);
    if (it == client_sockets_.end()) {
        return "";
    }
    
    char buffer[4096];
    int bytes_received = recv(it->second, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received <= 0) {
        return "";  // Connection closed or error
    }
    
    buffer[bytes_received] = '\0';
    return std::string(buffer);
}

bool SocketConnectionManager::is_connection_active(const std::string& connection_id) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    return client_sockets_.find(connection_id) != client_sockets_.end();
}

std::vector<std::string> SocketConnectionManager::get_active_connections() const {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    std::vector<std::string> connections;
    connections.reserve(client_sockets_.size());
    
    for (const auto& [conn_id, socket] : client_sockets_) {
        connections.push_back(conn_id);
    }
    
    return connections;
}

// ChatServer Implementation
ChatServer::ChatServer(
    std::unique_ptr<IMessageHandler> message_handler,
    std::unique_ptr<SocketConnectionManager> connection_manager
) : message_handler_(std::move(message_handler))
  , connection_manager_(std::move(connection_manager))
  , running_(false)
  , client_counter_(0) {
    
    // Use default implementations if not provided
    if (!message_handler_) {
        message_handler_ = std::make_unique<DefaultMessageHandler>();
    }
    if (!connection_manager_) {
        connection_manager_ = std::make_unique<SocketConnectionManager>();
    }
}

ChatServer::~ChatServer() {
    stop_server();
}

bool ChatServer::start_server(const std::string& host, int port) {
    if (running_) {
        return false;
    }
    
    if (!connection_manager_->initialize(host, port)) {
        return false;
    }
    
    running_ = true;
    
    // Start server thread
    server_thread_ = std::thread(&ChatServer::server_loop, this);
    
    // Start message processor thread
    message_processor_thread_ = std::thread(&ChatServer::message_processor_loop, this);
    
    std::cout << "[SERVER] Chat server started on " << host << ":" << port << std::endl;
    return true;
}

void ChatServer::stop_server() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    
    // Stop connection manager
    connection_manager_->shutdown();
    
    // Wake up message processor
    queue_condition_.notify_all();
    
    // Wait for threads to finish
    if (server_thread_.joinable()) {
        server_thread_.join();
    }
    if (message_processor_thread_.joinable()) {
        message_processor_thread_.join();
    }
    
    std::cout << "[SERVER] Chat server stopped" << std::endl;
}

bool ChatServer::broadcast_message(const Message& message) {
    if (!running_ || !message_handler_->validate_message(message)) {
        return false;
    }
    
    // Add message to queue for processing
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        message_queue_.push(message);
    }
    
    queue_condition_.notify_one();
    return true;
}

std::vector<std::string> ChatServer::get_connected_clients() const {
    std::vector<std::string> clients;
    
    std::lock_guard<std::mutex> lock(clients_mutex_);
    clients.reserve(connection_to_client_map_.size());
    
    for (const auto& [conn_id, client_id] : connection_to_client_map_) {
        if (connection_manager_->is_connection_active(conn_id)) {
            clients.push_back(client_id);
        }
    }
    
    return clients;
}

bool ChatServer::is_running() const {
    return running_;
}

void ChatServer::server_loop() {
    while (running_) {
        // Accept new connections
        std::string connection_id = connection_manager_->accept_connection();
        if (!connection_id.empty()) {
            std::string client_id = generate_client_id();
            
            {
                std::lock_guard<std::mutex> lock(clients_mutex_);
                connection_to_client_map_[connection_id] = client_id;
            }
            
            notify_client_joined(client_id);
        }
        
        // Handle existing connections
        auto active_connections = connection_manager_->get_active_connections();
        for (const auto& conn_id : active_connections) {
            std::string data = connection_manager_->receive_data(conn_id);
            if (!data.empty()) {
                handle_client_message(conn_id, data);
            }
        }
        
        // Small delay to prevent busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void ChatServer::message_processor_loop() {
    while (running_) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        queue_condition_.wait(lock, [this] { return !message_queue_.empty() || !running_; });
        
        while (!message_queue_.empty()) {
            Message message = message_queue_.front();
            message_queue_.pop();
            lock.unlock();
            
            // Process message
            message_handler_->handle_message(message);
            
            // Broadcast to all clients
            std::string json_data = message.to_json();
            auto active_connections = connection_manager_->get_active_connections();
            for (const auto& conn_id : active_connections) {
                connection_manager_->send_data(conn_id, json_data);
            }
            
            lock.lock();
        }
    }
}

std::string ChatServer::generate_client_id() {
    return "client_" + std::to_string(++client_counter_);
}

void ChatServer::handle_client_message(const std::string& connection_id, const std::string& data) {
    try {
        Message message = Message::from_json(data);
        
        // Set correct sender ID
        std::lock_guard<std::mutex> lock(clients_mutex_);
        auto it = connection_to_client_map_.find(connection_id);
        if (it != connection_to_client_map_.end()) {
            message.sender_id = it->second;
            broadcast_message(message);
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to parse message: " << e.what() << std::endl;
        
        // Send error message to client
        Message error_msg("server", "Invalid message format", "error");
        connection_manager_->send_data(connection_id, error_msg.to_json());
    }
}

void ChatServer::notify_client_joined(const std::string& client_id) {
    Message join_msg("server", client_id + " has joined the chat", "system");
    broadcast_message(join_msg);
}

void ChatServer::notify_client_left(const std::string& client_id) {
    Message leave_msg("server", client_id + " has left the chat", "system");
    broadcast_message(leave_msg);
}

void ChatServer::set_message_handler(std::unique_ptr<IMessageHandler> handler) {
    message_handler_ = std::move(handler);
}

void ChatServer::set_connection_manager(std::unique_ptr<SocketConnectionManager> manager) {
    connection_manager_ = std::move(manager);
}

// Factory function implementation
std::unique_ptr<IChatServer> create_chat_server(
    std::unique_ptr<IMessageHandler> message_handler
) {
    return std::make_unique<ChatServer>(std::move(message_handler));
}

} // namespace chat
