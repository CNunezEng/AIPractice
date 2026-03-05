/**
 * @file test_server_simple.cpp
 * @brief Unit tests for ChatServer class using Catch2
 */

#include "../../../../thirdPartyIncludes/catchorg-Catch2-31b1109/src/catch2/catch_test_macros.hpp"
#include "../src/include/interfaces.h"
#include "../src/include/message_impl.h"
#include "../src/server/chat_server.h"
#include <thread>
#include <chrono>
#include <memory>

using namespace chat;

// Standard Cases
TEST_CASE("Server creation", "[server]") {
    auto server = std::make_unique<ChatServer>();
    REQUIRE(server != nullptr);
    REQUIRE_FALSE(server->is_running());
    REQUIRE(server->get_connected_clients().size() == 0);
}

TEST_CASE("Server with custom handler", "[server]") {
    auto custom_handler = std::make_unique<DefaultMessageHandler>();
    auto custom_server = std::make_unique<ChatServer>(std::move(custom_handler));
    
    REQUIRE(custom_server != nullptr);
    REQUIRE_FALSE(custom_server->is_running());
}

TEST_CASE("Start/stop server", "[server]") {
    auto server = std::make_unique<ChatServer>();
    REQUIRE(server->start_server("localhost", 8766));
    REQUIRE(server->is_running());
    
    server->stop_server();
    REQUIRE_FALSE(server->is_running());
}

TEST_CASE("Broadcast while running", "[server]") {
    auto server = std::make_unique<ChatServer>();
    server->start_server("localhost", 8767);
    
    Message msg("user1", "Hello everyone");
    REQUIRE(server->broadcast_message(msg));
    
    server->stop_server();
}

TEST_CASE("Broadcast while not running", "[server]") {
    auto server = std::make_unique<ChatServer>();
    Message msg("user1", "Hello everyone");
    REQUIRE_FALSE(server->broadcast_message(msg));
}

// Edge Cases
TEST_CASE("Invalid port", "[server][edge]") {
    auto server = std::make_unique<ChatServer>();
    REQUIRE_FALSE(server->start_server("localhost", -1));
    REQUIRE_FALSE(server->is_running());
}

TEST_CASE("Invalid host", "[server][edge]") {
    auto server = std::make_unique<ChatServer>();
    REQUIRE_FALSE(server->start_server("invalid_host", 8765));
    REQUIRE_FALSE(server->is_running());
}

TEST_CASE("Broadcast invalid message", "[server][edge]") {
    auto server = std::make_unique<ChatServer>();
    server->start_server("localhost", 8768);
    
    Message invalid_msg("", "");  // Invalid message
    REQUIRE_FALSE(server->broadcast_message(invalid_msg));
    
    server->stop_server();
}

// Client Management Tests
TEST_CASE("Client management", "[server]") {
    auto server = std::make_unique<ChatServer>();
    server->start_server("localhost", 8769);
    
    // Initially no clients
    REQUIRE(server->get_connected_clients().size() == 0);
    
    server->stop_server();
}

// Error Handling Tests
TEST_CASE("Multiple start attempts", "[server][error]") {
    auto server = std::make_unique<ChatServer>();
    REQUIRE(server->start_server("localhost", 8770));
    REQUIRE(server->is_running());
    
    // Try to start again (should fail gracefully)
    REQUIRE_FALSE(server->start_server("localhost", 8771));
    
    server->stop_server();
}

TEST_CASE("Multiple stop attempts", "[server][error]") {
    auto server = std::make_unique<ChatServer>();
    server->start_server("localhost", 8772);
    server->stop_server();
    
    // Try to stop again (should be safe)
    server->stop_server();
    REQUIRE_FALSE(server->is_running());
}

// Integration Tests
TEST_CASE("Message validation integration", "[server][integration]") {
    auto server = std::make_unique<ChatServer>();
    server->start_server("localhost", 8773);
    
    // Test valid message
    Message valid_msg("user1", "Valid message");
    REQUIRE(server->broadcast_message(valid_msg));
    
    // Test invalid message
    Message invalid_msg("", "Invalid message");
    REQUIRE_FALSE(server->broadcast_message(invalid_msg));
    
    server->stop_server();
}

TEST_CASE("Multiple message types", "[server][integration]") {
    auto server = std::make_unique<ChatServer>();
    server->start_server("localhost", 8774);
    
    std::vector<std::string> message_types = {"text", "image", "file", "system", "error"};
    
    for (const auto& type : message_types) {
        Message msg("user1", "Content for " + type, type);
        REQUIRE(server->broadcast_message(msg));
    }
    
    server->stop_server();
}

// Performance Tests
TEST_CASE("Message broadcast performance", "[server][performance]") {
    auto server = std::make_unique<ChatServer>();
    server->start_server("localhost", 8775);
    
    const int num_messages = 1000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_messages; ++i) {
        Message msg("user1", "Performance message " + std::to_string(i));
        server->broadcast_message(msg);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    server->stop_server();
    
    // Should complete within reasonable time
    REQUIRE(duration.count() < 1000000);  // Less than 1 second
    
    std::cout << "Broadcast performance: " << duration.count() << " microseconds for " 
              << num_messages << " messages" << std::endl;
}
