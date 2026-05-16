#ifndef CHATMESSENGER_UDPSOCKET_H
#define CHATMESSENGER_UDPSOCKET_H

#include "ChatConfig.h"

#include <cstddef>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#else
using SOCKET = int;
#endif

namespace chatmessenger {

/**
 * @brief Explicit interface for datagram transport.
 *
 * This uses the Adapter pattern: application code depends on this interface,
 * while UdpSocket adapts the operating system socket API.
 */
class IUdpTransport {
public:
    virtual ~IUdpTransport() = default;
    virtual bool open(const Endpoint& source, std::string& error) = 0;
    virtual bool sendTo(const Endpoint& destination, const std::string& message, std::string& error) = 0;
    virtual bool receive(std::string& message, Endpoint& sender, std::string& error) = 0;
    virtual void close() = 0;
};

/**
 * @brief UDP transport backed by Winsock on Windows.
 */
class UdpSocket final : public IUdpTransport {
public:
    UdpSocket();
    ~UdpSocket() override;

    UdpSocket(const UdpSocket&) = delete;
    UdpSocket& operator=(const UdpSocket&) = delete;

    bool open(const Endpoint& source, std::string& error) override;
    bool sendTo(const Endpoint& destination, const std::string& message, std::string& error) override;
    bool receive(std::string& message, Endpoint& sender, std::string& error) override;
    void close() override;

private:
    SOCKET socketHandle_;
    bool winsockStarted_;
};

} // namespace chatmessenger

#endif // CHATMESSENGER_UDPSOCKET_H
