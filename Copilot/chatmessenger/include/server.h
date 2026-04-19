/**
 * @file server.h
 * @brief Chat server interface
 * 
 * Provides server-side chat functionality with client
 * connection management.
 * 
 * @author Copilot Agent
 * @version 1.0
 */

#ifndef SERVER_H
#define SERVER_H

#include "message.h"
#include "socket_util.h"
#include <string>
#include <memory>
#include <thread>
#include <atomic>

/**
 * @class ChatServer
 * @brief Server-side chat implementation
 * 
 * Accepts client connections and relays messages between clients.
 * Listens on a specified address/port for incoming connections.
 * 
 * Design Pattern: Observer (connection listener thread)
 */
class ChatServer {
private:
    Socket listen_socket_;
    std::string bind_ip_;
    uint16_t bind_port_;
    std::atomic<bool> running_;
    std::thread accept_thread_;
    
public:
    /**
     * @brief Construct a chat server
     * @param bind_ip IP address to listen on
     * @param bind_port Port to listen on
     */
    ChatServer(const std::string& bind_ip, uint16_t bind_port);
    
    /**
     * @brief Destructor - stops server
     */
    ~ChatServer();
    
    /**
     * @brief Start the server and wait for connections
     * @return true if started successfully, false otherwise
     */
    bool start();
    
    /**
     * @brief Stop the server
     */
    void stop();
    
    /**
     * @brief Check if server is running
     * @return true if running, false otherwise
     */
    bool isRunning() const;
    
private:
    /**
     * @brief Thread function that accepts connections
     */
    void acceptLoop();
    
    /**
     * @brief Handle a client connection
     * @param client Connected socket
     */
    void handleClient(Socket client);
};

#endif // SERVER_H
