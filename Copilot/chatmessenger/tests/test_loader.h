/**
 * @file test_loader.h
 * @brief Test case loader from INI configuration file
 * 
 * Reads test cases from test_cases.ini and executes them dynamically.
 * Allows adding new test cases without recompiling.
 */

#ifndef TEST_LOADER_H
#define TEST_LOADER_H

#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <functional>

/**
 * @class TestCase
 * @brief Represents a single test case from configuration file
 */
struct TestCase {
    std::string name;                           ///< Test case name
    std::string test_type;                      ///< Type of test (creation, validation, etc.)
    std::map<std::string, std::string> params;  ///< Test parameters
    
    /**
     * @brief Get parameter value
     * @param key Parameter key
     * @param default_value Default if not found
     * @return Parameter value or default
     */
    std::string getParam(const std::string& key, const std::string& default_value = "") const {
        auto it = params.find(key);
        return (it != params.end()) ? it->second : default_value;
    }
};

/**
 * @class TestLoader
 * @brief Loads and manages test cases from INI file
 */
class TestLoader {
private:
    std::vector<TestCase> test_cases;
    std::string config_file;
    
    /**
     * @brief Trim whitespace from string
     */
    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }
    
public:
    /**
     * @brief Constructor
     * @param config_path Path to test_cases.ini file
     */
    explicit TestLoader(const std::string& config_path) : config_file(config_path) {}
    
    /**
     * @brief Load test cases from INI file
     * @return true if successful, false if file not found
     */
    bool loadTests() {
        std::ifstream file(config_file);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open test configuration file: " << config_file << "\n";
            return false;
        }
        
        std::string line;
        std::string current_section;
        TestCase current_test;
        
        while (std::getline(file, line)) {
            line = trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') continue;
            
            // Parse section headers [test_name]
            if (line[0] == '[' && line[line.length() - 1] == ']') {
                // Save previous test case
                if (!current_section.empty()) {
                    current_test.name = current_section;
                    test_cases.push_back(current_test);
                }
                
                // Start new test case
                current_section = line.substr(1, line.length() - 2);
                current_test.params.clear();
                current_test.test_type = "";
                continue;
            }
            
            // Parse key=value pairs
            size_t delimiter = line.find('=');
            if (delimiter != std::string::npos && !current_section.empty()) {
                std::string key = trim(line.substr(0, delimiter));
                std::string value = trim(line.substr(delimiter + 1));
                
                if (key == "type") {
                    current_test.test_type = value;
                } else {
                    current_test.params[key] = value;
                }
            }
        }
        
        // Save last test case
        if (!current_section.empty()) {
            current_test.name = current_section;
            test_cases.push_back(current_test);
        }
        
        file.close();
        return true;
    }
    
    /**
     * @brief Get number of loaded test cases
     */
    size_t getTestCount() const {
        return test_cases.size();
    }
    
    /**
     * @brief Get test case by index
     */
    const TestCase* getTest(size_t index) const {
        return (index < test_cases.size()) ? &test_cases[index] : nullptr;
    }
    
    /**
     * @brief Get all test cases
     */
    const std::vector<TestCase>& getTests() const {
        return test_cases;
    }
    
    /**
     * @brief Get test cases filtered by type
     */
    std::vector<const TestCase*> getTestsByType(const std::string& type) const {
        std::vector<const TestCase*> results;
        for (const auto& test : test_cases) {
            if (test.test_type == type) {
                results.push_back(&test);
            }
        }
        return results;
    }
    
    /**
     * @brief Print all loaded tests (for debugging)
     */
    void printTests() const {
        std::cout << "Loaded " << test_cases.size() << " test cases:\n";
        for (const auto& test : test_cases) {
            std::cout << "  [" << test.name << "] type=" << test.test_type << "\n";
            for (const auto& param : test.params) {
                std::cout << "    " << param.first << "=" << param.second << "\n";
            }
        }
    }
};

#endif // TEST_LOADER_H