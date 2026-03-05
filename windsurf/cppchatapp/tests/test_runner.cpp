/**
 * @file test_runner.cpp
 * @brief Configurable test runner using INI file and Catch2
 * 
 * Following AIinfo.txt standards for configurable testing framework.
 */

#include "catch2_simple.hpp"
#include "../src/include/interfaces.h"
#include "../src/include/message_impl.h"
#include "../src/server/chat_server.h"
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <thread>
#include <chrono>
#include <memory>
#include <sstream>

using namespace chat;

// Simple INI parser
class TestConfig {
private:
    std::map<std::string, std::map<std::string, std::string>> config;
    
public:
    bool load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return false;
        
        std::string line;
        std::string current_section;
        
        while (std::getline(file, line)) {
            // Remove comments and trim
            size_t comment_pos = line.find('#');
            if (comment_pos != std::string::npos) {
                line = line.substr(0, comment_pos);
            }
            
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty()) continue;
            
            // Section header
            if (line[0] == '[' && line.back() == ']') {
                current_section = line.substr(1, line.length() - 2);
                continue;
            }
            
            // Key=value pair
            size_t equal_pos = line.find('=');
            if (equal_pos != std::string::npos) {
                std::string key = line.substr(0, equal_pos);
                std::string value = line.substr(equal_pos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                config[current_section][key] = value;
            }
        }
        return true;
    }
    
    bool getBool(const std::string& section, const std::string& key, bool default_val = false) {
        auto section_it = config.find(section);
        if (section_it == config.end()) return default_val;
        
        auto key_it = section_it->second.find(key);
        if (key_it == section_it->second.end()) return default_val;
        
        return key_it->second == "true" || key_it->second == "1";
    }
    
    int getInt(const std::string& section, const std::string& key, int default_val = 0) {
        auto section_it = config.find(section);
        if (section_it == config.end()) return default_val;
        
        auto key_it = section_it->second.find(key);
        if (key_it == section_it->second.end()) return default_val;
        
        return std::stoi(key_it->second);
    }
    
    std::vector<int> getIntList(const std::string& section, const std::string& key) {
        std::vector<int> result;
        auto section_it = config.find(section);
        if (section_it == config.end()) return result;
        
        auto key_it = section_it->second.find(key);
        if (key_it == section_it->second.end()) return result;
        
        std::stringstream ss(key_it->second);
        std::string item;
        while (std::getline(ss, item, ',')) {
            result.push_back(std::stoi(item));
        }
        return result;
    }
};

// Global config
TestConfig test_config;

// Message Tests
TEST_CASE("Message standard creation", "[message]") {
    if (!test_config.getBool("message_tests", "standard_creation", true)) SKIP();
    
    Message msg("user1", "Hello world");
    REQUIRE(msg.sender_id == "user1");
    REQUIRE(msg.content == "Hello world");
    REQUIRE(msg.message_type == "text");
    REQUIRE(msg.is_valid() == true);
}

TEST_CASE("Message with custom type", "[message]") {
    if (!test_config.getBool("message_tests", "custom_type", true)) SKIP();
    
    Message msg("user1", "Image content", "image");
    REQUIRE(msg.sender_id == "user1");
    REQUIRE(msg.content == "Image content");
    REQUIRE(msg.message_type == "image");
    REQUIRE(msg.is_valid() == true);
}

TEST_CASE("Empty sender ID", "[message]") {
    if (!test_config.getBool("message_tests", "empty_sender", true)) SKIP();
    
    Message msg("", "content");
    REQUIRE(msg.sender_id == "");
    REQUIRE(msg.content == "content");
    REQUIRE(msg.is_valid() == false);
}

TEST_CASE("Empty content", "[message]") {
    if (!test_config.getBool("message_tests", "empty_content", true)) SKIP();
    
    Message msg("user", "");
    REQUIRE(msg.sender_id == "user");
    REQUIRE(msg.content == "");
    REQUIRE(msg.is_valid() == false);
}

TEST_CASE("JSON serialization", "[message][json]") {
    if (!test_config.getBool("message_tests", "json_serialization", true)) SKIP();
    
    Message valid_message("user1", "Hello world", "text");
    std::string json_str = valid_message.to_json();
    
    REQUIRE_FALSE(json_str.empty());
    REQUIRE(json_str.find("sender_id") != std::string::npos);
    REQUIRE(json_str.find("content") != std::string::npos);
    REQUIRE(json_str.find("timestamp") != std::string::npos);
    REQUIRE(json_str.find("message_type") != std::string::npos);
}

TEST_CASE("JSON deserialization", "[message][json]") {
    if (!test_config.getBool("message_tests", "json_deserialization", true)) SKIP();
    
    Message valid_message("user1", "Hello world", "text");
    std::string json_str = valid_message.to_json();
    Message recovered = Message::from_json(json_str);
    
    REQUIRE(recovered.sender_id == valid_message.sender_id);
    REQUIRE(recovered.content == valid_message.content);
    REQUIRE(recovered.message_type == valid_message.message_type);
}

TEST_CASE("Maximum length content", "[message]") {
    if (!test_config.getBool("message_tests", "max_length", true)) SKIP();
    
    std::string max_content(1000, 'a');
    Message msg("user", max_content);
    
    REQUIRE(msg.content.length() == 1000);
    REQUIRE(msg.is_valid() == true);
}

TEST_CASE("Too long content", "[message]") {
    if (!test_config.getBool("message_tests", "too_long", true)) SKIP();
    
    std::string too_long_content(1001, 'a');
    Message msg("user", too_long_content);
    
    REQUIRE(msg.content.length() == 1001);
    REQUIRE(msg.is_valid() == false);
}

// Server Tests
TEST_CASE("Server creation", "[server]") {
    if (!test_config.getBool("server_tests", "creation", true)) SKIP();
    
    auto server = std::make_unique<ChatServer>();
    REQUIRE(server != nullptr);
    REQUIRE_FALSE(server->is_running());
    REQUIRE(server->get_connected_clients().size() == 0);
}

TEST_CASE("Start/stop server", "[server]") {
    if (!test_config.getBool("server_tests", "start_stop", true)) SKIP();
    
    auto server = std::make_unique<ChatServer>();
    auto ports = test_config.getIntList("test_settings", "server_ports");
    int port = ports.empty() ? 8766 : ports[0];
    
    REQUIRE(server->start_server("localhost", port));
    REQUIRE(server->is_running());
    
    server->stop_server();
    REQUIRE_FALSE(server->is_running());
}

TEST_CASE("Broadcast while running", "[server]") {
    if (!test_config.getBool("server_tests", "broadcast_running", true)) SKIP();
    
    auto server = std::make_unique<ChatServer>();
    auto ports = test_config.getIntList("test_settings", "server_ports");
    int port = ports.size() > 1 ? ports[1] : 8767;
    
    server->start_server("localhost", port);
    
    Message msg("user1", "Hello everyone");
    REQUIRE(server->broadcast_message(msg));
    
    server->stop_server();
}

TEST_CASE("Broadcast while not running", "[server]") {
    if (!test_config.getBool("server_tests", "broadcast_not_running", true)) SKIP();
    
    auto server = std::make_unique<ChatServer>();
    Message msg("user1", "Hello everyone");
    REQUIRE_FALSE(server->broadcast_message(msg));
}

TEST_CASE("Invalid port", "[server][edge]") {
    if (!test_config.getBool("server_tests", "invalid_port", true)) SKIP();
    if (!test_config.getBool("test_settings", "enable_edge_case_tests", true)) SKIP();
    
    auto server = std::make_unique<ChatServer>();
    REQUIRE_FALSE(server->start_server("localhost", -1));
    REQUIRE_FALSE(server->is_running());
}

TEST_CASE("Message broadcast performance", "[server][performance]") {
    if (!test_config.getBool("server_tests", "performance", true)) SKIP();
    if (!test_config.getBool("test_settings", "enable_performance_tests", true)) SKIP();
    
    auto server = std::make_unique<ChatServer>();
    auto ports = test_config.getIntList("test_settings", "server_ports");
    int port = ports.size() > 9 ? ports[9] : 8775;
    
    server->start_server("localhost", port);
    
    int num_iterations = test_config.getInt("test_settings", "performance_iterations", 1000);
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_iterations; ++i) {
        Message msg("user1", "Performance message " + std::to_string(i));
        server->broadcast_message(msg);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    server->stop_server();
    
    int timeout_ms = test_config.getInt("test_settings", "performance_timeout_ms", 1000000);
    REQUIRE(duration.count() < timeout_ms);
    
    if (test_config.getBool("output", "show_performance", true)) {
        std::cout << "Broadcast performance: " << duration.count() << " microseconds for " 
                  << num_iterations << " messages" << std::endl;
    }
}

// Test configuration loader
static void loadTestConfig() {
    if (!test_config.load("test_config.ini")) {
        std::cout << "Warning: Could not load test_config.ini, using defaults" << std::endl;
    }
}

// Register configuration loader
struct ConfigLoader {
    ConfigLoader() {
        loadTestConfig();
    }
};
static ConfigLoader configLoader;
