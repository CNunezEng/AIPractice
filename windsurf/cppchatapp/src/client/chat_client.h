#pragma once

/**
 * @file chat_client.h
 * @brief C++ chat client implementation
 * 
 * Following AIinfo.txt standards for clean, maintainable code.
 * Uses modern C++17 features and RAII for resource management.
 */

#include "../include/interfaces.h"
#include "../include/message_impl.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
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
 * @brief Concrete chat client implementation
 * 
 * Uses RAII for connection management.
 * Follows single responsibility principle - only handles chat functionality.
 * Implements observer pattern for message reception.
 */
class ChatClient : public IChatClient {
private:
    SOCKET client_socket_;
    std::atomic<bool> connected_;
    std::atomic<bool> receiving_;
    std::string connection_status_;
    std::mutex status_mutex_;
    
    // Message receiving
    std::thread receive_thread_;
    std::function<void(const Message&)> message_handler_;
    
    void receive_loop();
    void update_status(const std::string& status);
    bool initialize_socket();
    void cleanup_socket();
    
public:
    /**
     * @brief Constructor
     */
    ChatClient();
    
    /**
     * @brief Destructor - ensures proper cleanup
     */
    ~ChatClient() override;
    
    // IChatClient interface implementation
    bool connect(const std::string& host, int port) override;
    void disconnect() override;
    bool send_message(const std::string& content) override;
    void start_receiving(std::function<void(const Message&)> message_handler) override;
    bool is_connected() const override;
    std::string get_connection_status() const override;
    
    // Additional utility methods
    void set_message_handler(std::function<void(const Message&)> handler);
};

/**
 * @brief Interactive chat client with command line interface
 * 
 * Extends base client with user interaction capabilities.
 * Provides a simple console-based chat interface.
 */
class InteractiveChatClient : public ChatClient {
private:
    std::atomic<bool> running_;
    
    void display_help();
    void handle_user_input();
    
public:
    /**
     * @brief Constructor
     */
    InteractiveChatClient();
    
    /**
     * @brief Start interactive chat session
     * @param host Server host address
     * @param port Server port number
     */
    void start_interactive_session(const std::string& host, int port);
    
    /**
     * @brief Stop interactive session
     */
    void stop_interactive_session();
};

/**
 * @brief Factory functions for creating chat clients
 * 
 * Follows factory pattern for object creation.
 * Provides convenient creation methods with default configurations.
 */

/**
 * @brief Create basic chat client
 * @return Unique pointer to chat client
 */
std::unique_ptr<IChatClient> create_chat_client();

/**
 * @brief Create interactive chat client
 * @return Unique pointer to interactive chat client
 */
std::unique_ptr<InteractiveChatClient> create_interactive_client();

} // namespace chat
