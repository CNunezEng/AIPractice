#pragma once

/**
 * @file chat_server.h
 * @brief C++ chat server implementation
 * 
 * Following AIinfo.txt standards for clean, maintainable code.
 * Uses modern C++17 features and RAII for resource management.
 */

#include "../include/interfaces.h"
#include "../include/message_impl.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <queue>
#include <condition_variable>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

namespace chat {

/**
 * @brief Default message handler implementation
 * 
 * Concrete implementation of IMessageHandler.
 * Follows strategy pattern for message processing.
 */
class DefaultMessageHandler : public IMessageHandler {
public:
    bool handle_message(const Message& message) override;
    bool validate_message(const Message& message) override;
};

/**
 * @brief Socket-based connection manager
 * 
 * Concrete implementation of IConnectionManager using Berkeley sockets.
 * Handles low-level socket operations and connection lifecycle.
 */
class SocketConnectionManager : public IConnectionManager {
private:
    SOCKET server_socket_;
    std::unordered_map<std::string, SOCKET> client_sockets_;
    std::mutex connections_mutex_;
    std::atomic<bool> running_;
    
public:
    SocketConnectionManager();
    ~SocketConnectionManager() override;
    
    bool initialize(const std::string& host, int port);
    void shutdown();
    
    std::string accept_connection() override;
    void close_connection(const std::string& connection_id) override;
    bool send_data(const std::string& connection_id, const std::string& data) override;
    std::string receive_data(const std::string& connection_id) override;
    bool is_connection_active(const std::string& connection_id) override;
    
    std::vector<std::string> get_active_connections() const;
};

/**
 * @brief Concrete chat server implementation
 * 
 * Uses dependency injection for message handler and connection manager.
 * Implements observer pattern for client management.
 * Follows single responsibility principle.
 */
class ChatServer : public IChatServer {
private:
    std::unique_ptr<IMessageHandler> message_handler_;
    std::unique_ptr<SocketConnectionManager> connection_manager_;
    std::atomic<bool> running_;
    std::thread server_thread_;
    std::mutex clients_mutex_;
    
    // Client management
    std::unordered_map<std::string, std::string> connection_to_client_map_;
    std::atomic<int> client_counter_;
    
    // Message queue for thread-safe message handling
    std::queue<Message> message_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_condition_;
    std::thread message_processor_thread_;
    
    void server_loop();
    void message_processor_loop();
    std::string generate_client_id();
    void handle_client_message(const std::string& connection_id, const std::string& data);
    void notify_client_joined(const std::string& client_id);
    void notify_client_left(const std::string& client_id);
    
public:
    /**
     * @brief Constructor with dependency injection
     * @param message_handler Custom message handler (optional)
     * @param connection_manager Custom connection manager (optional)
     */
    explicit ChatServer(
        std::unique_ptr<IMessageHandler> message_handler = nullptr,
        std::unique_ptr<SocketConnectionManager> connection_manager = nullptr
    );
    
    ~ChatServer() override;
    
    // IChatServer interface implementation
    bool start_server(const std::string& host, int port) override;
    void stop_server() override;
    bool broadcast_message(const Message& message) override;
    std::vector<std::string> get_connected_clients() const override;
    bool is_running() const override;
    
    // Additional utility methods
    void set_message_handler(std::unique_ptr<IMessageHandler> handler);
    void set_connection_manager(std::unique_ptr<SocketConnectionManager> manager);
};

/**
 * @brief Factory function for creating chat server
 * 
 * Follows factory pattern for object creation.
 * Provides default implementations while allowing customization.
 * 
 * @param message_handler Custom message handler (optional)
 * @return Unique pointer to configured chat server
 */
std::unique_ptr<IChatServer> create_chat_server(
    std::unique_ptr<IMessageHandler> message_handler = nullptr
);

} // namespace chat
