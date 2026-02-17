#pragma once

/**
 * @file message_impl.h
 * @brief Implementation of Message class JSON serialization
 */

#include "interfaces.h"
#include <sstream>
#include <iomanip>

namespace chat {

inline std::string Message::to_json() const {
    std::ostringstream oss;
    auto timestamp_t = std::chrono::system_clock::to_time_t(timestamp);
    
    oss << "{"
        << "\"sender_id\":\"" << sender_id << "\","
        << "\"content\":\"" << content << "\","
        << "\"timestamp\":\"" << std::put_time(std::gmtime(&timestamp_t), "%Y-%m-%dT%H:%M:%SZ") << "\","
        << "\"message_type\":\"" << message_type << "\""
        << "}";
    
    return oss.str();
}

inline Message Message::from_json(const std::string& json_str) {
    // Simple JSON parsing - in production, use a proper JSON library
    Message msg;
    
    // Extract sender_id
    size_t sender_start = json_str.find("\"sender_id\":\"") + 13;
    size_t sender_end = json_str.find("\"", sender_start);
    if (sender_start != std::string::npos && sender_end != std::string::npos) {
        msg.sender_id = json_str.substr(sender_start, sender_end - sender_start);
    }
    
    // Extract content
    size_t content_start = json_str.find("\"content\":\"") + 11;
    size_t content_end = json_str.find("\"", content_start);
    if (content_start != std::string::npos && content_end != std::string::npos) {
        msg.content = json_str.substr(content_start, content_end - content_start);
    }
    
    // Extract message_type
    size_t type_start = json_str.find("\"message_type\":\"") + 16;
    size_t type_end = json_str.find("\"", type_start);
    if (type_start != std::string::npos && type_end != std::string::npos) {
        msg.message_type = json_str.substr(type_start, type_end - type_start);
    }
    
    return msg;
}

inline bool Message::is_valid() const {
    return !sender_id.empty() && !content.empty() && content.length() <= 1000;
}

} // namespace chat
