/**
 * @file test_message.cpp
 * @brief Runtime test executor - loads test cases from test_cases.ini
 * 
 * This executable reads test cases from an INI configuration file and executes them.
 * Allows adding new test cases without recompiling the application.
 * Uses Catch2 testing framework for assertions and reporting.
 */

#include "message.h"
#include "test_loader.h"
#define CATCH_CONFIG_MAIN
#include "../thirdPartyIncludes/catchorg-Catch2-31b1109/extras/catch_amalgamated.hpp"

/**
 * @brief Execute creation type tests
 * Creates a message and validates it was created correctly
 */
static void executeCreationTest(const TestCase& test) {
    std::string sender = test.getParam("sender");
    std::string content = test.getParam("content");
    std::string expected = test.getParam("expected_result", "valid");
    
    Message msg(sender, content);
    
    if (expected == "valid") {
        if (msg.getSender().empty()) {
            std::cout << "\n[CREATION FAILED - EMPTY SENDER]\n"
                      << "  Input sender: '" << sender << "'\n"
                      << "  Got empty sender after creation!" << std::endl;
        }
        REQUIRE(!msg.getSender().empty());
        
        if (msg.getContent().empty()) {
            std::cout << "\n[CREATION FAILED - EMPTY CONTENT]\n"
                      << "  Input content: '" << content << "'\n"
                      << "  Got empty content after creation!" << std::endl;
        }
        REQUIRE(!msg.getContent().empty());
        
        if (msg.getTimestamp() <= 0) {
            std::cout << "\n[CREATION FAILED - INVALID TIMESTAMP]\n"
                      << "  Expected timestamp > 0\n"
                      << "  Got: " << msg.getTimestamp() << std::endl;
        }
        REQUIRE(msg.getTimestamp() > 0);
    } else if (expected == "valid_trim") {
        // Trim whitespace from inputs
        if (msg.getSender().empty()) {
            std::cout << "\n[CREATION FAILED WITH TRIM - EMPTY SENDER]\n"
                      << "  Input sender: '" << sender << "'\n"
                      << "  Expected trimmed sender, got empty!" << std::endl;
        }
        REQUIRE(!msg.getSender().empty());
        
        if (msg.getContent().empty()) {
            std::cout << "\n[CREATION FAILED WITH TRIM - EMPTY CONTENT]\n"
                      << "  Input content: '" << content << "'\n"
                      << "  Expected trimmed content, got empty!" << std::endl;
        }
        REQUIRE(!msg.getContent().empty());
    }
}

/**
 * @brief Execute validation type tests
 * Tests the validate() method with various inputs
 */
static void executeValidationTest(const TestCase& test) {
    std::string sender = test.getParam("sender");
    std::string content = test.getParam("content");
    std::string expected = test.getParam("expected_result", "valid");
    std::string error_contains = test.getParam("error_contains", "");
    
    Message msg(sender, content);
    bool is_valid = msg.validate();
    
    if (expected == "valid") {
        if (!is_valid) {
            std::cout << "\n[VALIDATION FAILED]\n"
                      << "  Sender: " << sender << " (len=" << sender.length() << ")\n"
                      << "  Content: " << content.substr(0, 50) << (content.length() > 50 ? "..." : "") 
                      << " (len=" << content.length() << ")\n"
                      << "  Expected: valid\n"
                      << "  Got: invalid\n"
                      << "  Error: " << msg.getValidationError() << std::endl;
        }
        REQUIRE(is_valid);
    } else if (expected == "invalid") {
        if (is_valid) {
            std::cout << "\n[VALIDATION FAILED]\n"
                      << "  Sender: " << sender << " (len=" << sender.length() << ")\n"
                      << "  Content: " << content.substr(0, 50) << (content.length() > 50 ? "..." : "") 
                      << " (len=" << content.length() << ")\n"
                      << "  Expected: invalid\n"
                      << "  Got: valid\n"
                      << "  Should have failed validation!" << std::endl;
        }
        REQUIRE(!is_valid);
        if (!error_contains.empty()) {
            std::string error_msg = msg.getValidationError();
            // Check if error message contains expected substring (pipe-separated alternatives)
            bool found = false;
            std::istringstream iss(error_contains);
            std::string expected_part;
            while (std::getline(iss, expected_part, '|')) {
                expected_part = std::string(expected_part.begin(), 
                                          std::find_if(expected_part.rbegin(), expected_part.rend(), 
                                                       [](unsigned char ch) { return !std::isspace(ch); }).base());
                if (error_msg.find(expected_part) != std::string::npos) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::cout << "\n[ERROR MESSAGE MISMATCH]\n"
                          << "  Expected to contain one of: " << error_contains << "\n"
                          << "  Got error: " << error_msg << std::endl;
            }
            REQUIRE(found);
        }
    }
}

/**
 * @brief Execute serialization type tests
 * Tests message serialization and deserialization
 */
static void executeSerializationTest(const TestCase& test) {
    std::string sender = test.getParam("sender");
    std::string content = test.getParam("content");
    std::string expected = test.getParam("expected_result", "roundtrip_match");
    
    Message msg1(sender, content);
    auto serialized = msg1.serialize();
    
    Message msg2 = Message::deserialize(serialized.data(), serialized.size());
    
    if (expected == "roundtrip_match") {
        if (msg2.getSender() != sender) {
            std::cout << "\n[SERIALIZATION FAILED - SENDER MISMATCH]\n"
                      << "  Original sender: " << sender << "\n"
                      << "  Received sender: " << msg2.getSender() << std::endl;
        }
        REQUIRE(msg2.getSender() == sender);
        
        if (msg2.getContent() != content) {
            std::cout << "\n[SERIALIZATION FAILED - CONTENT MISMATCH]\n"
                      << "  Original content length: " << content.length() << "\n"
                      << "  Received content length: " << msg2.getContent().length() << std::endl;
        }
        REQUIRE(msg2.getContent() == content);
        
        if (msg2.getTimestamp() != msg1.getTimestamp()) {
            std::cout << "\n[SERIALIZATION FAILED - TIMESTAMP MISMATCH]\n"
                      << "  Original timestamp: " << msg1.getTimestamp() << "\n"
                      << "  Received timestamp: " << msg2.getTimestamp() << std::endl;
        }
        REQUIRE(msg2.getTimestamp() == msg1.getTimestamp());
        
        // Verify received message passes validation on receiver side
        if (!msg2.validate()) {
            std::cout << "\n[RECEIVED MESSAGE VALIDATION FAILED]\n"
                      << "  Sender: " << msg2.getSender() << "\n"
                      << "  Content length: " << msg2.getContent().length() << "\n"
                      << "  Validation error: " << msg2.getValidationError() << std::endl;
        }
        REQUIRE(msg2.validate());
    }
}

/**
 * @brief Execute deserialization type tests
 * Tests deserialization with invalid or empty data
 */
static void executeDeserializationTest(const TestCase& test) {
    std::string input_data = test.getParam("input_data");
    std::string input_size_str = test.getParam("input_size", "0");
    std::string expected = test.getParam("expected_result", "empty");
    
    size_t input_size = std::stoul(input_size_str);
    Message msg = Message::deserialize(input_data.c_str(), input_size);
    
    if (expected == "empty") {
        if (!msg.isEmpty()) {
            std::cout << "\n[DESERIALIZATION FAILED - NOT EMPTY]\n"
                      << "  Input size: " << input_size << "\n"
                      << "  Expected empty message\n"
                      << "  Got sender: '" << msg.getSender() << "'\n"
                      << "  Got content: '" << msg.getContent() << "'" << std::endl;
        }
        REQUIRE(msg.isEmpty());
    }
}

/**
 * @brief Execute getter type tests
 * Tests getter methods with various conditions
 */
static void executeGetterTest(const TestCase& test) {
    std::string sender = test.getParam("sender");
    std::string content = test.getParam("content");
    std::string getter_type = test.getParam("getter_type", "sender");
    std::string expected = test.getParam("expected_result", "valid");
    std::string exception_type = test.getParam("exception_type", "runtime_error");
    
    Message msg(sender, content);
    
    if (expected == "throws_exception") {
        if (getter_type == "sender") {
            try {
                msg.getSender();
                std::cout << "\n[GETTER FAILED - NO EXCEPTION]\n"
                          << "  Expected getSender() to throw runtime_error\n"
                          << "  Got no exception" << std::endl;
            } catch (...) {}
            REQUIRE_THROWS_AS(msg.getSender(), std::runtime_error);
        } else if (getter_type == "content") {
            try {
                msg.getContent();
                std::cout << "\n[GETTER FAILED - NO EXCEPTION]\n"
                          << "  Expected getContent() to throw runtime_error\n"
                          << "  Got no exception" << std::endl;
            } catch (...) {}
            REQUIRE_THROWS_AS(msg.getContent(), std::runtime_error);
        }
    }
}

/**
 * @brief Main test case that executes all loaded tests
 */
TEST_CASE("Dynamic Test Execution from test_cases.ini", "[message]") {
    TestLoader loader("tests/test_cases.ini");
    REQUIRE(loader.loadTests());
    REQUIRE(loader.getTestCount() > 0);

    const auto& tests = loader.getTests();

    for (const auto& test : tests) {
        SECTION(test.name) {
            if (test.test_type == "creation") {
                executeCreationTest(test);
            } else if (test.test_type == "validation") {
                executeValidationTest(test);
            } else if (test.test_type == "serialization") {
                executeSerializationTest(test);
            } else if (test.test_type == "deserialization") {
                executeDeserializationTest(test);
            } else if (test.test_type == "getter") {
                executeGetterTest(test);
            }
        }
    }
}