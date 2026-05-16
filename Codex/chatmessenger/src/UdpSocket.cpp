#include "UdpSocket.h"

#include <array>
#include <sstream>

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#endif

namespace chatmessenger {
namespace {

constexpr std::size_t kMaxDatagramBytes = 4096;

std::string lastSocketError() {
#ifdef _WIN32
    std::ostringstream text;
    text << "socket error " << WSAGetLastError();
    return text.str();
#else
    return std::strerror(errno);
#endif
}

sockaddr_in toSockaddr(const Endpoint& endpoint) {
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(endpoint.port);
    inet_pton(AF_INET, endpoint.ip.c_str(), &address.sin_addr);
    return address;
}

Endpoint fromSockaddr(const sockaddr_in& address) {
    std::array<char, INET_ADDRSTRLEN> buffer{};
    inet_ntop(AF_INET, &address.sin_addr, buffer.data(), static_cast<socklen_t>(buffer.size()));
    return Endpoint{buffer.data(), ntohs(address.sin_port)};
}

} // namespace

UdpSocket::UdpSocket() : socketHandle_(INVALID_SOCKET), winsockStarted_(false) {}

UdpSocket::~UdpSocket() {
    close();
}

bool UdpSocket::open(const Endpoint& source, std::string& error) {
#ifdef _WIN32
    WSADATA data{};
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        error = "Failed to start Winsock.";
        return false;
    }
    winsockStarted_ = true;
#endif

    socketHandle_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socketHandle_ == INVALID_SOCKET) {
        error = "Failed to create UDP socket: " + lastSocketError();
        close();
        return false;
    }

    const sockaddr_in localAddress = toSockaddr(source);
    if (bind(socketHandle_, reinterpret_cast<const sockaddr*>(&localAddress), sizeof(localAddress)) == SOCKET_ERROR) {
        error = "Failed to bind " + source.ip + ":" + std::to_string(source.port) + ": " + lastSocketError();
        close();
        return false;
    }

    return true;
}

bool UdpSocket::sendTo(const Endpoint& destination, const std::string& message, std::string& error) {
    const sockaddr_in remoteAddress = toSockaddr(destination);
    const int sent = sendto(socketHandle_,
                            message.c_str(),
                            static_cast<int>(message.size()),
                            0,
                            reinterpret_cast<const sockaddr*>(&remoteAddress),
                            sizeof(remoteAddress));
    if (sent == SOCKET_ERROR || sent != static_cast<int>(message.size())) {
        error = "Failed to send message: " + lastSocketError();
        return false;
    }
    return true;
}

bool UdpSocket::receive(std::string& message, Endpoint& sender, std::string& error) {
    std::array<char, kMaxDatagramBytes> buffer{};
    sockaddr_in remoteAddress{};
    socklen_t addressLength = sizeof(remoteAddress);
    const int received = recvfrom(socketHandle_,
                                  buffer.data(),
                                  static_cast<int>(buffer.size() - 1),
                                  0,
                                  reinterpret_cast<sockaddr*>(&remoteAddress),
                                  &addressLength);
    if (received == SOCKET_ERROR) {
        error = "Failed to receive message: " + lastSocketError();
        return false;
    }

    message.assign(buffer.data(), static_cast<std::size_t>(received));
    sender = fromSockaddr(remoteAddress);
    return true;
}

void UdpSocket::close() {
    if (socketHandle_ != INVALID_SOCKET) {
#ifdef _WIN32
        closesocket(socketHandle_);
#else
        ::close(socketHandle_);
#endif
        socketHandle_ = INVALID_SOCKET;
    }

#ifdef _WIN32
    if (winsockStarted_) {
        WSACleanup();
        winsockStarted_ = false;
    }
#endif
}

} // namespace chatmessenger
