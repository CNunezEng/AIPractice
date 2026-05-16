#include "ChatConfig.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <sstream>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif

namespace chatmessenger {
namespace {

bool containsOnlyDigits(const std::string& text) {
    return !text.empty() && std::all_of(text.begin(), text.end(), [](unsigned char ch) {
        return std::isdigit(ch) != 0;
    });
}

} // namespace

bool isValidIpv4(const std::string& text) {
    sockaddr_in address{};
    return inet_pton(AF_INET, text.c_str(), &address.sin_addr) == 1;
}

bool parsePort(const std::string& text, uint16_t& port, std::string& error) {
    if (!containsOnlyDigits(text)) {
        error = "Port must contain only digits.";
        return false;
    }

    char* end = nullptr;
    const long value = std::strtol(text.c_str(), &end, 10);
    if (*end != '\0' || value < 1 || value > 65535) {
        error = "Port must be between 1 and 65535.";
        return false;
    }

    port = static_cast<uint16_t>(value);
    return true;
}

bool parseEndpoint(const std::string& text, Endpoint& endpoint, std::string& error) {
    const std::size_t separator = text.find(':');
    if (separator == std::string::npos || separator == 0 || separator == text.size() - 1) {
        error = "Endpoint must use ip:port format.";
        return false;
    }
    if (text.find(':', separator + 1) != std::string::npos) {
        error = "Endpoint must contain exactly one ':' separator.";
        return false;
    }

    const std::string ip = text.substr(0, separator);
    const std::string portText = text.substr(separator + 1);
    if (!isValidIpv4(ip)) {
        error = "IP address must be a valid IPv4 address.";
        return false;
    }

    uint16_t port = 0;
    if (!parsePort(portText, port, error)) {
        return false;
    }

    endpoint = Endpoint{ip, port};
    return true;
}

bool parseChatConfig(int argc, char* argv[], ChatConfig& config, std::string& error) {
    if (argc != 3) {
        error = "Expected exactly two arguments.";
        return false;
    }

    Endpoint source{};
    Endpoint destination{};
    if (!parseEndpoint(argv[1], source, error)) {
        error = "Invalid source endpoint: " + error;
        return false;
    }
    if (!parseEndpoint(argv[2], destination, error)) {
        error = "Invalid destination endpoint: " + error;
        return false;
    }

    config = ChatConfig{source, destination};
    return true;
}

std::string usageFor(const std::string& programName) {
    std::ostringstream usage;
    usage << "Usage: " << programName << " source-ip:source-port destination-ip:destination-port\n"
          << "Example: " << programName << " 127.0.0.1:5000 127.0.0.1:5001";
    return usage.str();
}

} // namespace chatmessenger
