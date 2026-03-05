/**
 * @file test_message.cpp
 * @brief Unit tests for Message class using Catch2
 * 
 * Following AIinfo.txt standards for comprehensive testing.
 * Tests standard cases, edge cases, and wrong inputs.
 */

#define CATCH_CONFIG_MAIN
#include "../../../../thirdPartyIncludes/catchorg-Catch2-31b1109/src/catch2/catch_test_macros.hpp"
#include "../../../../thirdPartyIncludes/catchorg-Catch2-31b1109/src/catch2/catch_session.hpp"
#include "../src/include/interfaces.h"
#include "../src/include/message_impl.h"
#include <thread>
#include <chrono>
#include <vector>

using namespace chat;

// Standard Cases
TEST_CASE("Message standard creation", "[message]") {
    Message msg("user1", "Hello world");
    
    REQUIRE(msg.sender_id == "user1");
    REQUIRE(msg.content == "Hello world");
    REQUIRE(msg.message_type == "text");
    REQUIRE(msg.is_valid() == true);
}

TEST_CASE("Message with custom type", "[message]") {
    Message msg("user1", "Image content", "image");
    
    REQUIRE(msg.sender_id == "user1");
    REQUIRE(msg.content == "Image content");
    REQUIRE(msg.message_type == "image");
    REQUIRE(msg.is_valid() == true);
}

TEST_CASE("Message default type", "[message]") {
    Message msg("user1", "Default type message");
    
    REQUIRE(msg.message_type == "text");
    REQUIRE(msg.is_valid() == true);
}

// Edge Cases
TEST_CASE("Empty sender ID", "[message]") {
    Message msg("", "content");
    
    REQUIRE(msg.sender_id == "");
    REQUIRE(msg.content == "content");
    REQUIRE(msg.is_valid() == false);
}

TEST_CASE("Empty content", "[message]") {
    Message msg("user", "");
    
    REQUIRE(msg.sender_id == "user");
    REQUIRE(msg.content == "");
    REQUIRE(msg.is_valid() == false);
}

TEST_CASE("Maximum length content", "[message]") {
    std::string max_content(1000, 'a');
    Message msg("user", max_content);
    
    REQUIRE(msg.content.length() == 1000);
    REQUIRE(msg.is_valid() == true);
}

TEST_CASE("Special characters", "[message]") {
    std::string special_chars = "!@#$%^&*()_+-=[]{}|;':\",./<>?";
    Message msg("user", special_chars);
    
    REQUIRE(msg.content == special_chars);
    REQUIRE(msg.is_valid() == true);
}

TEST_CASE("Unicode characters", "[message]") {
    std::string unicode = "ñáéíóú 中文 العربية русский 日本語 한국어";
    Message msg("user", unicode);
    
    REQUIRE(msg.content == unicode);
    REQUIRE(msg.is_valid() == true);
}

// Wrong Inputs
TEST_CASE("Too long content", "[message]") {
    std::string too_long_content(1001, 'a');
    Message msg("user", too_long_content);
    
    REQUIRE(msg.content.length() == 1001);
    REQUIRE(msg.is_valid() == false);
}

// JSON Serialization Tests
TEST_CASE("JSON serialization", "[message][json]") {
    Message valid_message("user1", "Hello world", "text");
    std::string json_str = valid_message.to_json();
    
    REQUIRE_FALSE(json_str.empty());
    REQUIRE(json_str.find("sender_id") != std::string::npos);
    REQUIRE(json_str.find("content") != std::string::npos);
    REQUIRE(json_str.find("timestamp") != std::string::npos);
    REQUIRE(json_str.find("message_type") != std::string::npos);
}

TEST_CASE("JSON deserialization", "[message][json]") {
    Message valid_message("user1", "Hello world", "text");
    std::string json_str = valid_message.to_json();
    Message recovered = Message::from_json(json_str);
    
    REQUIRE(recovered.sender_id == valid_message.sender_id);
    REQUIRE(recovered.content == valid_message.content);
    REQUIRE(recovered.message_type == valid_message.message_type);
}

TEST_CASE("JSON roundtrip", "[message][json]") {
    std::vector<std::string> message_types = {"text", "image", "file", "system", "error"};
    
    for (const auto& type : message_types) {
        Message original("user123", "Content for " + type, type);
        std::string json_str = original.to_json();
        Message recovered = Message::from_json(json_str);
        
        REQUIRE(original.sender_id == recovered.sender_id);
        REQUIRE(original.content == recovered.content);
        REQUIRE(original.message_type == recovered.message_type);
    }
}

TEST_CASE("Invalid JSON handling", "[message][json]") {
    // Test with invalid JSON
    REQUIRE_THROWS(Message::from_json("invalid json"));
    
    // Test with incomplete JSON
    std::string incomplete_json = "{\"sender_id\": \"user\"}";
    Message msg = Message::from_json(incomplete_json);
    REQUIRE(msg.sender_id == "user");
    REQUIRE(msg.content == "");  // Default empty
}

// Concurrent Tests
TEST_CASE("Concurrent message creation", "[message][concurrent]") {
    const int num_threads = 5;
    const int messages_per_thread = 10;
    std::vector<std::thread> threads;
    std::vector<Message> messages;
    std::mutex messages_mutex;
    
    auto create_messages = [&](int thread_id) {
        for (int i = 0; i < messages_per_thread; ++i) {
            Message msg("user_" + std::to_string(thread_id), 
                       "Message " + std::to_string(i) + " from thread " + std::to_string(thread_id));
            
            std::lock_guard<std::mutex> lock(messages_mutex);
            messages.push_back(msg);
            
            // Small delay to increase chance of race conditions
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    };
    
    // Create threads
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(create_messages, i);
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify results
    REQUIRE(messages.size() == num_threads * messages_per_thread);
    
    // Verify all messages are valid
    for (const auto& msg : messages) {
        REQUIRE(msg.is_valid() == true);
    }
}

// Performance Tests
TEST_CASE("Serialization performance", "[message][performance]") {
    Message valid_message("user1", "Hello world", "text");
    const int num_iterations = 10000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_iterations; ++i) {
        std::string json_str = valid_message.to_json();
        Message recovered = Message::from_json(json_str);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Should complete within reasonable time (adjust threshold as needed)
    REQUIRE(duration.count() < 1000000);  // Less than 1 second
    
    std::cout << "Serialization performance: " << duration.count() << " microseconds for " 
              << num_iterations << " iterations" << std::endl;
}

// Message Type Validation
TEST_CASE("All message types valid", "[message][validation]") {
    std::vector<std::string> valid_types = {"text", "image", "file", "system", "error"};
    
    for (const auto& type : valid_types) {
        Message msg("user", "content", type);
        REQUIRE(msg.is_valid() == true);
    }
}

// Boundary Tests
TEST_CASE("Boundary tests", "[message][boundary]") {
    // Test exactly 1000 characters (should be valid)
    Message msg1("user", std::string(1000, 'a'));
    REQUIRE(msg1.is_valid() == true);
    
    // Test exactly 1001 characters (should be invalid)
    Message msg2("user", std::string(1001, 'a'));
    REQUIRE(msg2.is_valid() == false);
    
    // Test single character messages
    Message msg3("user", "a");
    REQUIRE(msg3.is_valid() == true);
    
    // Test very long sender ID
    Message msg4(std::string(1000, 'u'), "content");
    REQUIRE(msg4.is_valid() == true);
}
