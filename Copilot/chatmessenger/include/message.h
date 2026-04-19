/**
 * @file message.h
 * @brief Message interface and implementation for chat system
 * 
 * Defines the contract for messages and provides validation
 * and serialization utilities.
 * 
 * @author Copilot Agent
 * @version 1.0
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <vector>
#include <ctime>

/**
 * @class IMessage
 * @brief Abstract base class for messages
 * 
 * Defines the contract that any message implementation must follow.
 * Ensures consistent message format and validation across the system.
 */
class IMessage {
public:
    virtual ~IMessage() = default;
    
    /**
     * @brief Get the sender's identifier
     * @return User identifier of message sender
     * @throws std::runtime_error if sender not set
     */
    virtual std::string getSender() const = 0;
    
    /**
     * @brief Get the message content
     * @return Message text
     * @throws std::runtime_error if content not set
     */
    virtual std::string getContent() const = 0;
    
    /**
     * @brief Get the message timestamp
     * @return UNIX timestamp of message creation
     */
    virtual time_t getTimestamp() const = 0;
    
    /**
     * @brief Validate message format and content
     * @return true if valid, false otherwise
     */
    virtual bool validate() const = 0;
    
    /**
     * @brief Get last validation error
     * @return Error message string (empty if valid)
     */
    virtual std::string getValidationError() const = 0;
    
    /**
     * @brief Serialize message to network format
     * @return Binary data ready for transmission
     */
    virtual std::vector<char> serialize() const = 0;
};

/**
 * @class Message
 * @brief Concrete implementation of a chat message
 * 
 * Stores sender, content, and timestamp. Validates all input
 * for correctness and appropriate length.
 * 
 * Format: [sender_len:sender:content_len:content:timestamp]
 * All lengths are 4-byte big-endian integers
 */
class Message : public IMessage {
private:
    std::string sender_;
    std::string content_;
    time_t timestamp_;
    mutable std::string last_error_;
    
    // Validation constants
    static constexpr size_t MIN_SENDER_LENGTH = 1;
    static constexpr size_t MAX_SENDER_LENGTH = 64;
    static constexpr size_t MIN_CONTENT_LENGTH = 1;
    static constexpr size_t MAX_CONTENT_LENGTH = 4096;
    
public:
    /**
     * @brief Construct a message
     * @param sender User identifier (1-64 characters)
     * @param content Message text (1-4096 characters)
     * @param timestamp Optional UNIX timestamp (defaults to current time)
     */
    Message(const std::string& sender, const std::string& content, time_t timestamp = 0);
    
    /**
     * @brief Virtual destructor
     */
    ~Message() override = default;
    
    // IMessage interface implementation
    std::string getSender() const override;
    std::string getContent() const override;
    time_t getTimestamp() const override;
    bool validate() const override;
    std::string getValidationError() const override;
    std::vector<char> serialize() const override;
    
    /**
     * @brief Deserialize a message from network data
     * @param data Binary data from network
     * @param size Size of data buffer
     * @return Message object, or empty message if deserialization fails
     */
    static Message deserialize(const char* data, size_t size);
    
    /**
     * @brief Check if message is valid (for error checking)
     * @return true if message is empty/invalid
     */
    bool isEmpty() const;
};

#endif // MESSAGE_H
