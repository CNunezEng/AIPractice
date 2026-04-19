/**
 * @file socket_util.cpp
 * @brief Socket utilities implementation
 */

#include "socket_util.h"
#include <iostream>
#include <cstring>

#ifdef _WIN32
    #ifdef _MSC_VER
        #pragma comment(lib, "ws2_32.lib")
    #endif
#endif

// Static initialization for Winsock
static bool winsock_initialized = false;

/**
 * Initialize Winsock (Windows only)
 */
bool Socket::initWinsock() {
#ifdef _WIN32
    if (winsock_initialized) {
        return true;
    }
    
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return false;
    }
    
    winsock_initialized = true;
    return true;
#else
    return true;
#endif
}

/**
 * Cleanup Winsock (Windows only)
 */
void Socket::cleanupWinsock() {
#ifdef _WIN32
    if (winsock_initialized) {
        WSACleanup();
        winsock_initialized = false;
    }
#endif
}

/**
 * Constructor - create a new socket
 */
Socket::Socket() : socket_(INVALID_SOCKET) {
    socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if (socket_ == INVALID_SOCKET) {
        throw std::runtime_error("Failed to create socket: " + getLastError());
    }
    
    // Set socket to non-blocking mode for select() with timeout
    #ifdef _WIN32
        unsigned long mode = 1;
        if (ioctlsocket(socket_, FIONBIO, &mode) != 0) {
            closesocket(socket_);
            throw std::runtime_error("Failed to set non-blocking mode");
        }
    #else
        int flags = fcntl(socket_, F_GETFL, 0);
        if (fcntl(socket_, F_SETFL, flags | O_NONBLOCK) < 0) {
            closesocket(socket_);
            throw std::runtime_error("Failed to set non-blocking mode");
        }
    #endif
}

/**
 * Destructor - close socket if open
 */
Socket::~Socket() {
    close();
}

/**
 * Bind socket to address and port
 */
bool Socket::bind(const std::string& ip, uint16_t port) {
    if (socket_ == INVALID_SOCKET) {
        return false;
    }
    
    struct sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address: " << ip << std::endl;
        return false;
    }
    
    if (::bind(socket_, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "bind() failed: " << getLastError() << std::endl;
        return false;
    }
    
    return true;
}

/**
 * Listen for incoming connections
 */
bool Socket::listen(int backlog) {
    if (socket_ == INVALID_SOCKET) {
        return false;
    }
    
    if (::listen(socket_, backlog) == SOCKET_ERROR) {
        std::cerr << "listen() failed: " << getLastError() << std::endl;
        return false;
    }
    
    return true;
}

/**
 * Accept an incoming connection
 */
Socket Socket::accept() {
    Socket new_socket;
    
    if (socket_ == INVALID_SOCKET) {
        return new_socket;  // Invalid socket
    }
    
    struct sockaddr_in client_addr = {};
    socklen_t client_addr_len = sizeof(client_addr);
    
    SOCKET accepted = ::accept(socket_, (struct sockaddr*)&client_addr, &client_addr_len);
    
    if (accepted == INVALID_SOCKET) {
        // Not an error for non-blocking socket - just no connection ready
        return new_socket;
    }
    
    new_socket.socket_ = accepted;
    return new_socket;
}

/**
 * Connect to remote address
 */
bool Socket::connect(const std::string& ip, uint16_t port) {
    if (socket_ == INVALID_SOCKET) {
        return false;
    }
    
    struct sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address: " << ip << std::endl;
        return false;
    }
    
    int result = ::connect(socket_, (struct sockaddr*)&server_addr, sizeof(server_addr));
    
#ifdef _WIN32
    if (result == SOCKET_ERROR) {
        int error = WSAGetLastError();
        // WSAEWOULDBLOCK is expected for non-blocking sockets
        if (error != WSAEWOULDBLOCK) {
            std::cerr << "connect() failed: " << error << std::endl;
            return false;
        }
    }
#else
    if (result == SOCKET_ERROR) {
        if (errno != EINPROGRESS) {
            std::cerr << "connect() failed: " << strerror(errno) << std::endl;
            return false;
        }
    }
#endif
    
    // For non-blocking socket, wait for connection to complete
    fd_set write_set;
    FD_ZERO(&write_set);
    FD_SET(socket_, &write_set);
    
    struct timeval timeout = {};
    timeout.tv_sec = 5;  // 5 second timeout
    timeout.tv_usec = 0;
    
    int select_result = select((int)socket_ + 1, nullptr, &write_set, nullptr, &timeout);
    
    if (select_result <= 0) {
        std::cerr << "connect() timeout or error" << std::endl;
        return false;
    }
    
    return true;
}

/**
 * Send data through socket
 */
int Socket::send(const char* data, int size) {
    if (socket_ == INVALID_SOCKET) {
        return -1;
    }
    
    int bytes_sent = ::send(socket_, data, size, 0);
    
    if (bytes_sent == SOCKET_ERROR) {
        std::cerr << "send() failed: " << getLastError() << std::endl;
        return -1;
    }
    
    return bytes_sent;
}

/**
 * Receive data from socket with timeout
 */
int Socket::receive(char* buffer, int size, int timeout_ms) {
    if (socket_ == INVALID_SOCKET) {
        return -1;
    }
    
    if (timeout_ms > 0) {
        fd_set read_set;
        FD_ZERO(&read_set);
        FD_SET(socket_, &read_set);
        
        struct timeval timeout = {};
        timeout.tv_sec = timeout_ms / 1000;
        timeout.tv_usec = (timeout_ms % 1000) * 1000;
        
        int select_result = select((int)socket_ + 1, &read_set, nullptr, nullptr, &timeout);
        
        if (select_result == 0) {
            return 0;  // Timeout
        }
        
        if (select_result < 0) {
            std::cerr << "select() failed: " << getLastError() << std::endl;
            return -1;
        }
    }
    
    int bytes_received = ::recv(socket_, buffer, size, 0);
    
    if (bytes_received == SOCKET_ERROR) {
        std::cerr << "recv() failed: " << getLastError() << std::endl;
        return -1;
    }
    
    if (bytes_received == 0) {
        // Connection closed
        return 0;
    }
    
    return bytes_received;
}

/**
 * Close socket
 */
void Socket::close() {
    if (socket_ != INVALID_SOCKET) {
        closesocket(socket_);
        socket_ = INVALID_SOCKET;
    }
}

/**
 * Check if socket is valid
 */
bool Socket::isValid() const {
    return socket_ != INVALID_SOCKET;
}

/**
 * Get socket handle
 */
SOCKET Socket::getHandle() const {
    return socket_;
}

/**
 * Get last socket error
 */
std::string Socket::getLastError() {
#ifdef _WIN32
    int error = WSAGetLastError();
    char buffer[256];
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, error, 0, buffer, sizeof(buffer), nullptr);
    return std::string(buffer);
#else
    return std::string(strerror(errno));
#endif
}
