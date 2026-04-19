/**
 * @file socket_util.h
 * @brief Socket networking utilities for Windows
 * 
 * Provides cross-platform socket wrappers and utilities
 * for TCP communication.
 * 
 * @author Copilot Agent
 * @version 1.0
 */

#ifndef SOCKET_UTIL_H
#define SOCKET_UTIL_H

#include <string>
#include <cstdint>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    typedef int SOCKET;
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

/**
 * @class Socket
 * @brief RAII wrapper for socket resource management
 * 
 * Automatically handles socket creation and cleanup.
 * Prevents resource leaks and double-closes.
 */
class Socket {
private:
    SOCKET socket_;
    
public:
    /**
     * @brief Construct a TCP socket
     * @throws std::runtime_error if socket creation fails
     */
    Socket();
    
    /**
     * @brief Destructor - closes socket if open
     */
    ~Socket();
    
    /**
     * @brief Bind socket to address and port
     * @param ip IP address to bind to
     * @param port Port number to bind to
     * @return true if successful, false otherwise
     */
    bool bind(const std::string& ip, uint16_t port);
    
    /**
     * @brief Listen for incoming connections
     * @param backlog Maximum pending connections
     * @return true if successful, false otherwise
     */
    bool listen(int backlog = 5);
    
    /**
     * @brief Accept an incoming connection
     * @return Valid socket on success, invalid socket on failure
     */
    Socket accept();
    
    /**
     * @brief Connect to remote address
     * @param ip IP address to connect to
     * @param port Port number to connect to
     * @return true if successful, false otherwise
     */
    bool connect(const std::string& ip, uint16_t port);
    
    /**
     * @brief Send data through socket
     * @param data Pointer to data buffer
     * @param size Number of bytes to send
     * @return Number of bytes sent, -1 on error
     */
    int send(const char* data, int size);
    
    /**
     * @brief Receive data from socket with timeout
     * @param buffer Pointer to receive buffer
     * @param size Maximum bytes to receive
     * @param timeout_ms Timeout in milliseconds (0 = blocking)
     * @return Number of bytes received, 0 if timeout, -1 on error
     */
    int receive(char* buffer, int size, int timeout_ms = 0);
    
    /**
     * @brief Close the socket
     */
    void close();
    
    /**
     * @brief Check if socket is valid
     * @return true if socket is open, false otherwise
     */
    bool isValid() const;
    
    /**
     * @brief Get the underlying socket handle
     * @return SOCKET handle
     */
    SOCKET getHandle() const;
    
    /**
     * @brief Get the last socket error message
     * @return Error description string
     */
    static std::string getLastError();
    
    /**
     * @brief Initialize Winsock (Windows only)
     * @return true if successful, false otherwise
     */
    static bool initWinsock();
    
    /**
     * @brief Cleanup Winsock (Windows only)
     */
    static void cleanupWinsock();
};

#endif // SOCKET_UTIL_H
