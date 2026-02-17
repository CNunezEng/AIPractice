#pragma once

/**
 * @file interfaces.h
 * @brief Chat application interfaces and message protocol
 * 
 * Following AIinfo.txt standards for explicit interfaces and design patterns.
 * Uses modern C++17 features for clean, maintainable code.
 */

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <functional>

namespace chat {

/**
 * @brief Message data structure for chat communication
 * 
 * Follows single responsibility principle - only contains message data.
 * Uses struct for aggregate initialization and clear data representation.
 */
struct Message {
    std::string sender_id;
    std::string content;
    std::chrono::system_clock::time_point timestamp;
    std::string message_type = "text";
    
    /**
     * @brief Default constructor
     */
    Message() : timestamp(std::chrono::system_clock::now()) {}
    
    /**
     * @brief Constructor with parameters
     * @param sender ID of the message sender
     * @param content Message content
     * @param type Message type (default: "text")
     */
    Message(const std::string& sender, const std::string& content, 
            const std::string& type = "text")
        : sender_id(sender), content(content), message_type(type)
        , timestamp(std::chrono::system_clock::now()) {}
    
    /**
     * @brief Convert message to JSON string
     * @return JSON representation of the message
     */
    std::string to_json() const;
    
    /**
     * @brief Create message from JSON string
     * @param json_str JSON string to parse
     * @return Message object
     */
    static Message from_json(const std::string& json_str);
    
    /**
     * @brief Validate message format and content
     * @return true if message is valid, false otherwise
     */
    bool is_valid() const;
};

/**
 * @brief Interface for chat server implementation
 * 
 * Uses dependency injection pattern for client handlers.
 * Follows interface segregation principle with pure virtual methods.
 */
class IChatServer {
public:
    virtual ~IChatServer() = default;
    
    /**
     * @brief Start the chat server on specified host and port
     * @param host Host address to bind to
     * @param port Port number to listen on
     * @return true if server started successfully, false otherwise
     */
    virtual bool start_server(const std::string& host, int port) = 0;
    
    /**
     * @brief Stop the chat server gracefully
     */
    virtual void stop_server() = 0;
    
    /**
     * @brief Broadcast message to all connected clients
     * @param message Message to broadcast
     * @return true if message broadcast successfully, false otherwise
     */
    virtual bool broadcast_message(const Message& message) = 0;
    
    /**
     * @brief Get list of currently connected client IDs
     * @return Vector of client IDs
     */
    virtual std::vector<std::string> get_connected_clients() const = 0;
    
    /**
     * @brief Check if server is running
     * @return true if server is running, false otherwise
     */
    virtual bool is_running() const = 0;
};

/**
 * @brief Interface for chat client implementation
 * 
 * Follows interface segregation principle - only chat-related methods.
 * Uses RAII for connection management.
 */
class IChatClient {
public:
    virtual ~IChatClient() = default;
    
    /**
     * @brief Connect to chat server
     * @param host Server host address
     * @param port Server port number
     * @return true if connection successful, false otherwise
     */
    virtual bool connect(const std::string& host, int port) = 0;
    
    /**
     * @brief Disconnect from chat server
     */
    virtual void disconnect() = 0;
    
    /**
     * @brief Send message to server
     * @param content Message content to send
     * @return true if message sent successfully, false otherwise
     */
    virtual bool send_message(const std::string& content) = 0;
    
    /**
     * @brief Start receiving messages from server
     * @param message_handler Callback function for received messages
     */
    virtual void start_receiving(std::function<void(const Message&)> message_handler) = 0;
    
    /**
     * @brief Check if client is connected to server
     * @return true if connected, false otherwise
     */
    virtual bool is_connected() const = 0;
    
    /**
     * @brief Get detailed connection status
     * @return Connection status string
     */
    virtual std::string get_connection_status() const = 0;
};

/**
 * @brief Interface for message handling strategies
 * 
 * Allows for different message processing implementations.
 * Follows strategy pattern for pluggable message processing.
 */
class IMessageHandler {
public:
    virtual ~IMessageHandler() = default;
    
    /**
     * @brief Process incoming message
     * @param message Message to process
     * @return true if message processed successfully, false otherwise
     */
    virtual bool handle_message(const Message& message) = 0;
    
    /**
     * @brief Validate message format and content
     * @param message Message to validate
     * @return true if message is valid, false otherwise
     */
    virtual bool validate_message(const Message& message) = 0;
};

/**
 * @brief Interface for connection management
 * 
 * Handles low-level socket operations and connection lifecycle.
 * Separates connection concerns from chat logic.
 */
class IConnectionManager {
public:
    virtual ~IConnectionManager() = default;
    
    /**
     * @brief Accept new client connection
     * @return Connection ID for the new client
     */
    virtual std::string accept_connection() = 0;
    
    /**
     * @brief Close client connection
     * @param connection_id Connection ID to close
     */
    virtual void close_connection(const std::string& connection_id) = 0;
    
    /**
     * @brief Send data to specific connection
     * @param connection_id Target connection ID
     * @param data Data to send
     * @return true if data sent successfully, false otherwise
     */
    virtual bool send_data(const std::string& connection_id, const std::string& data) = 0;
    
    /**
     * @brief Receive data from connection
     * @param connection_id Source connection ID
     * @return Received data string
     */
    virtual std::string receive_data(const std::string& connection_id) = 0;
    
    /**
     * @brief Check if connection is active
     * @param connection_id Connection ID to check
     * @return true if connection is active, false otherwise
     */
    virtual bool is_connection_active(const std::string& connection_id) = 0;
};

} // namespace chat
