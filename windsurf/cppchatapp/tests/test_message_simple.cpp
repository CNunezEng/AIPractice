/**
 * @file test_message_simple.cpp
 * @brief Unit tests for Message class using Catch2
 */

#define CATCH_CONFIG_MAIN
#include "C:/Users/pixar/OneDrive/Desktop/practice program s/Showcase/thirdPartyIncludes/catchorg-Catch2-31b1109/src/catch2/catch_test_macros.hpp"
#include "C:/Users/pixar/OneDrive/Desktop/practice program s/Showcase/thirdPartyIncludes/catchorg-Catch2-31b1109/src/catch2/catch_session.hpp"
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

TEST_CASE("Maximum length content", "[message]") {
    std::string max_content(1000, 'a');
    Message msg("user", max_content);
    
    REQUIRE(msg.content.length() == 1000);
    REQUIRE(msg.is_valid() == true);
}

TEST_CASE("Too long content", "[message]") {
    std::string too_long_content(1001, 'a');
    Message msg("user", too_long_content);
    
    REQUIRE(msg.content.length() == 1001);
    REQUIRE(msg.is_valid() == false);
}
