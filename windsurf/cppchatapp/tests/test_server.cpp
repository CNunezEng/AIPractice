/**
 * @file test_server.cpp
 * @brief Unit tests for ChatServer class
 * 
 * Following AIinfo.txt standards for comprehensive testing.
 * Tests standard cases, edge cases, and wrong inputs.
 */

#include <gtest/gtest.h>
#include "../src/include/interfaces.h"
#include "../src/include/message_impl.h"
#include "../src/server/chat_server.h"
#include <thread>
#include <chrono>
#include <memory>

using namespace chat;

class ServerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test fixtures
        server = std::make_unique<ChatServer>();
    }
    
    void TearDown() override {
        // Clean up
        if (server && server->is_running()) {
            server->stop_server();
        }
    }
    
    std::unique_ptr<ChatServer> server;
};

// Standard Cases
TEST_F(ServerTest, ServerCreation) {
    EXPECT_NE(server, nullptr);
    EXPECT_FALSE(server->is_running());
    EXPECT_EQ(server->get_connected_clients().size(), 0);
}

TEST_F(ServerTest, ServerWithCustomHandler) {
    auto custom_handler = std::make_unique<DefaultMessageHandler>();
    auto custom_server = std::make_unique<ChatServer>(std::move(custom_handler));
    
    EXPECT_NE(custom_server, nullptr);
    EXPECT_FALSE(custom_server->is_running());
}

TEST_F(ServerTest, FactoryFunction) {
    auto factory_server = create_chat_server();
    
    EXPECT_NE(factory_server, nullptr);
    EXPECT_FALSE(factory_server->is_running());
}

// Edge Cases
TEST_F(ServerTest, StartStopServer) {
    // Test starting server
    EXPECT_TRUE(server->start_server("localhost", 8766));
    EXPECT_TRUE(server->is_running());
    
    // Test stopping server
    server->stop_server();
    EXPECT_FALSE(server->is_running());
}

TEST_F(ServerTest, BroadcastMessageWhileRunning) {
    server->start_server("localhost", 8767);
    
    Message msg("user1", "Hello everyone");
    EXPECT_TRUE(server->broadcast_message(msg));
    
    server->stop_server();
}

TEST_F(ServerTest, BroadcastMessageWhileNotRunning) {
    Message msg("user1", "Hello everyone");
    EXPECT_FALSE(server->broadcast_message(msg));
}

// Wrong Inputs
TEST_F(ServerTest, InvalidPort) {
    // Test with invalid port (should fail gracefully)
    EXPECT_FALSE(server->start_server("localhost", -1));
    EXPECT_FALSE(server->is_running());
}

TEST_F(ServerTest, InvalidHost) {
    // Test with invalid host (should fail gracefully)
    EXPECT_FALSE(server->start_server("invalid_host", 8765));
    EXPECT_FALSE(server->is_running());
}

TEST_F(ServerTest, BroadcastInvalidMessage) {
    server->start_server("localhost", 8768);
    
    Message invalid_msg("", "");  // Invalid message
    EXPECT_FALSE(server->broadcast_message(invalid_msg));
    
    server->stop_server();
}

// Client Management Tests
TEST_F(ServerTest, ClientManagement) {
    server->start_server("localhost", 8769);
    
    // Initially no clients
    EXPECT_EQ(server->get_connected_clients().size(), 0);
    
    server->stop_server();
}

// Message Handler Tests
class MockMessageHandler : public IMessageHandler {
public:
    bool handle_message(const Message& message) override {
        processed_messages.push_back(message);
        return true;
    }
    
    bool validate_message(const Message& message) override {
        validated_messages.push_back(message);
        return message.is_valid();
    }
    
    std::vector<Message> processed_messages;
    std::vector<Message> validated_messages;
};

TEST_F(ServerTest, CustomMessageHandler) {
    auto mock_handler = std::make_unique<MockMessageHandler>();
    auto test_server = std::make_unique<ChatServer>(std::move(mock_handler));
    
    test_server->start_server("localhost", 8770);
    
    Message msg("user1", "Test message");
    EXPECT_TRUE(test_server->broadcast_message(msg));
    
    test_server->stop_server();
}

// Concurrent Tests
TEST_F(ServerTest, ConcurrentClientSimulation) {
    server->start_server("localhost", 8771);
    
    const int num_threads = 5;
    std::vector<std::thread> threads;
    std::vector<Message> messages;
    std::mutex messages_mutex;
    
    // Simulate concurrent message broadcasting
    auto broadcast_messages = [&](int thread_id) {
        for (int i = 0; i < 10; ++i) {
            Message msg(f"user_{thread_id}", f"Message {i} from thread {thread_id}");
            
            if (server->broadcast_message(msg)) {
                std::lock_guard<std::mutex> lock(messages_mutex);
                messages.push_back(msg);
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    };
    
    // Create threads
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(broadcast_messages, i);
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    server->stop_server();
    
    // Verify messages were processed
    EXPECT_GT(messages.size(), 0);
}

// Performance Tests
TEST_F(ServerTest, MessageBroadcastPerformance) {
    server->start_server("localhost", 8772);
    
    const int num_messages = 1000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_messages; ++i) {
        Message msg("user1", f"Message {i}");
        server->broadcast_message(msg);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    server->stop_server();
    
    // Should complete within reasonable time
    EXPECT_LT(duration.count(), 1000000);  // Less than 1 second
    std::cout << "Broadcast performance: " << duration.count() << " microseconds for " 
              << num_messages << " messages" << std::endl;
}

// Error Handling Tests
TEST_F(ServerTest, MultipleStartAttempts) {
    // Start server
    EXPECT_TRUE(server->start_server("localhost", 8773));
    EXPECT_TRUE(server->is_running());
    
    // Try to start again (should fail gracefully)
    EXPECT_FALSE(server->start_server("localhost", 8774));
    
    server->stop_server();
}

TEST_F(ServerTest, MultipleStopAttempts) {
    // Start and stop server normally
    server->start_server("localhost", 8775);
    server->stop_server();
    
    // Try to stop again (should be safe)
    server->stop_server();
    EXPECT_FALSE(server->is_running());
}

// Integration Tests
TEST_F(ServerTest, MessageValidationIntegration) {
    server->start_server("localhost", 8776);
    
    // Test valid message
    Message valid_msg("user1", "Valid message");
    EXPECT_TRUE(server->broadcast_message(valid_msg));
    
    // Test invalid message
    Message invalid_msg("", "Invalid message");
    EXPECT_FALSE(server->broadcast_message(invalid_msg));
    
    server->stop_server();
}

TEST_F(ServerTest, MultipleMessageTypes) {
    server->start_server("localhost", 8777);
    
    std::vector<std::string> message_types = {"text", "image", "file", "system", "error"};
    
    for (const auto& type : message_types) {
        Message msg("user1", f"Content for {type}", type);
        EXPECT_TRUE(server->broadcast_message(msg));
    }
    
    server->stop_server();
}

// Resource Management Tests
TEST_F(ServerTest, ResourceCleanup) {
    {
        auto temp_server = std::make_unique<ChatServer>();
        temp_server->start_server("localhost", 8778);
        temp_server->stop_server();
        // temp_server goes out of scope here
    }
    
    // Should not crash and resources should be cleaned up
    SUCCEED();
}

// Connection Manager Tests
TEST_F(ServerTest, ConnectionManagerIntegration) {
    auto custom_manager = std::make_unique<SocketConnectionManager>();
    auto test_server = std::make_unique<ChatServer>(
        nullptr,  // Use default message handler
        std::move(custom_manager)
    );
    
    EXPECT_TRUE(test_server->start_server("localhost", 8779));
    EXPECT_TRUE(test_server->is_running());
    
    test_server->stop_server();
}

// Stress Tests
TEST_F(ServerTest, StressTest) {
    server->start_server("localhost", 8780);
    
    const int num_threads = 10;
    const int messages_per_thread = 100;
    std::vector<std::thread> threads;
    
    auto stress_broadcast = [&](int thread_id) {
        for (int i = 0; i < messages_per_thread; ++i) {
            Message msg(f"user_{thread_id}", f"Stress message {i}");
            server->broadcast_message(msg);
        }
    };
    
    // Create stress threads
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(stress_broadcast, i);
    }
    
    // Wait for completion
    for (auto& thread : threads) {
        thread.join();
    }
    
    server->stop_server();
    
    // Should complete without crashing
    SUCCEED();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
