#ifndef CHATMESSENGER_CHATCONFIG_H
#define CHATMESSENGER_CHATCONFIG_H

#include <cstdint>
#include <string>

namespace chatmessenger {

/**
 * @brief Network endpoint made from an IPv4 address and UDP port.
 */
struct Endpoint {
    std::string ip;
    uint16_t port;
};

/**
 * @brief Runtime configuration for one chat peer.
 */
struct ChatConfig {
    Endpoint source;
    Endpoint destination;
};

/**
 * @brief Returns true when text is a dotted IPv4 address accepted by inet_pton.
 */
bool isValidIpv4(const std::string& text);

/**
 * @brief Parses a decimal TCP/UDP port in the range 1 through 65535.
 * @param text Port text supplied by the user.
 * @param port Receives the parsed port on success.
 * @param error Receives a user-facing error on failure.
 * @return True when parsing succeeds.
 */
bool parsePort(const std::string& text, uint16_t& port, std::string& error);

/**
 * @brief Parses an endpoint from "ip:port" text.
 * @param text Endpoint text supplied by the user.
 * @param endpoint Receives the parsed endpoint on success.
 * @param error Receives a user-facing error on failure.
 * @return True when parsing succeeds.
 */
bool parseEndpoint(const std::string& text, Endpoint& endpoint, std::string& error);

/**
 * @brief Parses the command line expected by Client and Server.
 *
 * Expected form:
 * Client source-ip:source-port destination-ip:destination-port
 * Server source-ip:source-port destination-ip:destination-port
 */
bool parseChatConfig(int argc, char* argv[], ChatConfig& config, std::string& error);

/**
 * @brief Formats the usage string for the named executable.
 */
std::string usageFor(const std::string& programName);

} // namespace chatmessenger

#endif // CHATMESSENGER_CHATCONFIG_H
