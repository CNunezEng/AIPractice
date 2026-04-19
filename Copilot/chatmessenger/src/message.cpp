/**
 * @file message.cpp
 * @brief Message implementation
 */

#include "message.h"
#include <cstring>
#include <cstdint>
#include <stdexcept>
#include <sstream>
#include <algorithm>

/**
 * Constructor - initialize message with sender and content
 */
Message::Message(const std::string& sender, const std::string& content, time_t timestamp)
    : sender_(sender), content_(content) {
    if (timestamp == 0) {
        timestamp_ = std::time(nullptr);
    } else {
        timestamp_ = timestamp;
    }
}

/**
 * Get sender - throws if empty
 */
std::string Message::getSender() const {
    if (sender_.empty()) {
        throw std::runtime_error("Sender is not set");
    }
    return sender_;
}

/**
 * Get content - throws if empty
 */
std::string Message::getContent() const {
    if (content_.empty()) {
        throw std::runtime_error("Content is not set");
    }
    return content_;
}

/**
 * Get timestamp
 */
time_t Message::getTimestamp() const {
    return timestamp_;
}

/**
 * Validate message - check sender and content constraints
 */
bool Message::validate() const {
    last_error_.clear();
    
    // Validate sender
    if (sender_.empty()) {
        last_error_ = "Sender cannot be empty";
        return false;
    }
    
    if (sender_.length() < MIN_SENDER_LENGTH) {
        last_error_ = "Sender must be at least 1 character";
        return false;
    }
    
    if (sender_.length() > MAX_SENDER_LENGTH) {
        last_error_ = "Sender cannot exceed 64 characters";
        return false;
    }
    
    // Check sender contains only valid characters
    for (char c : sender_) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            last_error_ = "Sender must contain only letters, numbers, underscore, or hyphen";
            return false;
        }
    }
    
    // Validate content
    if (content_.empty()) {
        last_error_ = "Content cannot be empty";
        return false;
    }
    
    if (content_.length() < MIN_CONTENT_LENGTH) {
        last_error_ = "Content must be at least 1 character";
        return false;
    }
    
    if (content_.length() > MAX_CONTENT_LENGTH) {
        last_error_ = "Content cannot exceed 4096 characters";
        return false;
    }
    
    return true;
}

/**
 * Get last validation error
 */
std::string Message::getValidationError() const {
    return last_error_;
}

/**
 * Serialize message to network format
 * Format: [4-byte sender_len][sender][4-byte content_len][content][8-byte timestamp]
 */
std::vector<char> Message::serialize() const {
    std::vector<char> data;
    
    // Sender length (4 bytes, big-endian)
    uint32_t sender_len = static_cast<uint32_t>(sender_.length());
    data.push_back((sender_len >> 24) & 0xFF);
    data.push_back((sender_len >> 16) & 0xFF);
    data.push_back((sender_len >> 8) & 0xFF);
    data.push_back(sender_len & 0xFF);
    
    // Sender
    data.insert(data.end(), sender_.begin(), sender_.end());
    
    // Content length (4 bytes, big-endian)
    uint32_t content_len = static_cast<uint32_t>(content_.length());
    data.push_back((content_len >> 24) & 0xFF);
    data.push_back((content_len >> 16) & 0xFF);
    data.push_back((content_len >> 8) & 0xFF);
    data.push_back(content_len & 0xFF);
    
    // Content
    data.insert(data.end(), content_.begin(), content_.end());
    
    // Timestamp (8 bytes, big-endian)
    uint64_t ts = static_cast<uint64_t>(timestamp_);
    data.push_back((ts >> 56) & 0xFF);
    data.push_back((ts >> 48) & 0xFF);
    data.push_back((ts >> 40) & 0xFF);
    data.push_back((ts >> 32) & 0xFF);
    data.push_back((ts >> 24) & 0xFF);
    data.push_back((ts >> 16) & 0xFF);
    data.push_back((ts >> 8) & 0xFF);
    data.push_back(ts & 0xFF);
    
    return data;
}

/**
 * Deserialize message from network data
 */
Message Message::deserialize(const char* data, size_t size) {
    const char* ptr = data;
    
    // Need at least 4 + 4 + 8 = 16 bytes for headers and timestamp
    if (size < 16) {
        return Message("", "", 0);
    }
    
    // Read sender length (4 bytes, big-endian)
    uint32_t sender_len = 0;
    sender_len |= ((unsigned char)ptr[0] << 24);
    sender_len |= ((unsigned char)ptr[1] << 16);
    sender_len |= ((unsigned char)ptr[2] << 8);
    sender_len |= (unsigned char)ptr[3];
    ptr += 4;
    
    if (sender_len < MIN_SENDER_LENGTH || sender_len > MAX_SENDER_LENGTH || ptr - data + sender_len > (int)size) {
        return Message("", "", 0);
    }
    
    // Read sender
    std::string sender(ptr, sender_len);
    ptr += sender_len;
    
    // Read content length (4 bytes, big-endian)
    uint32_t content_len = 0;
    content_len |= ((unsigned char)ptr[0] << 24);
    content_len |= ((unsigned char)ptr[1] << 16);
    content_len |= ((unsigned char)ptr[2] << 8);
    content_len |= (unsigned char)ptr[3];
    ptr += 4;
    
    if (content_len < MIN_CONTENT_LENGTH || content_len > MAX_CONTENT_LENGTH || ptr - data + content_len + 8 > (int)size) {
        return Message("", "", 0);
    }
    
    // Read content
    std::string content(ptr, content_len);
    ptr += content_len;
    
    // Read timestamp (8 bytes, big-endian)
    uint64_t ts = 0;
    ts |= ((uint64_t)(unsigned char)ptr[0] << 56);
    ts |= ((uint64_t)(unsigned char)ptr[1] << 48);
    ts |= ((uint64_t)(unsigned char)ptr[2] << 40);
    ts |= ((uint64_t)(unsigned char)ptr[3] << 32);
    ts |= ((uint64_t)(unsigned char)ptr[4] << 24);
    ts |= ((uint64_t)(unsigned char)ptr[5] << 16);
    ts |= ((uint64_t)(unsigned char)ptr[6] << 8);
    ts |= (uint64_t)(unsigned char)ptr[7];
    
    return Message(sender, content, static_cast<time_t>(ts));
}

/**
 * Check if message is empty/invalid
 */
bool Message::isEmpty() const {
    return sender_.empty() && content_.empty();
}
