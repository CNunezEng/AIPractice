/**
 * @file client.h
 * @brief Chat client interface
 * 
 * Provides client-side chat functionality with connection
 * to server and message send/receive capabilities.
 * 
 * @author Copilot Agent
 * @version 1.0
 */

#ifndef CLIENT_H
#define CLIENT_H

#include "message.h"
#include "socket_util.h"
#include <string>
#include <thread>
#include <atomic>

/**
 * @class IChat
 * @brief Abstract interface for chat operations
 */
class IChat {
public:
    virtual ~IChat() = default;
    
    /**
     * @brief Send a message
     * @param message Message to send
     * @return true if sent successfully, false otherwise
     */
    virtual bool sendMessage(const IMessage& message) = 0;
    
    /**
     * @brief Check if connected
     * @return true if connected to remote, false otherwise
     */
    virtual bool isConnected() const = 0;
};

/**
 * @class ChatClient
 * @brief Client-side chat implementation
 * 
 * Connects to a server on a specified address/port.
 * Listens for incoming connections from server.
 * Manages bidirectional communication.
 * 
 * Design Pattern: Observer (receive thread)
 */
class ChatClient : public IChat {
private:
    Socket client_socket_;           ///< Connection to server
    Socket listen_socket_;           ///< Socket listening for incoming from server
    std::string server_ip_;
    uint16_t server_port_;
    std::string local_ip_;
    uint16_t local_port_;
    std::atomic<bool> connected_;
    std::atomic<bool> running_;
    std::thread receive_thread_;
    
public:
    /**
     * @brief Construct a chat client
     * @param server_ip Server IP address
     * @param server_port Server port
     * @param local_ip Local IP to listen on
     * @param local_port Local port to listen on
     */
    ChatClient(const std::string& server_ip, uint16_t server_port,
               const std::string& local_ip, uint16_t local_port);
    
    /**
     * @brief Destructor - closes connections
     */
    ~ChatClient() override;
    
    /**
     * @brief Connect to server
     * @return true if successful, false otherwise
     */
    bool connect();
    
    /**
     * @brief IChat interface implementation
     */
    bool sendMessage(const IMessage& message) override;
    bool isConnected() const override;
    
    /**
     * @brief Disconnect from server
     */
    void disconnect();
    
private:
    /**
     * @brief Thread function for receiving messages
     */
    void receiveLoop();
};

#endif // CLIENT_H
