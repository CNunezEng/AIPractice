#include "ChatConfig.h"

#include "catch_amalgamated.hpp"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace {

struct TestCase {
    std::string name;
    std::map<std::string, std::string> values;
};

std::string trim(const std::string& value) {
    const std::size_t first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    const std::size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::vector<std::string> split(const std::string& text, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream stream(text);
    std::string part;
    while (std::getline(stream, part, delimiter)) {
        parts.push_back(part);
    }
    return parts;
}

std::string join(const std::vector<std::string>& values, const std::string& delimiter) {
    std::ostringstream output;
    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index > 0) {
            output << delimiter;
        }
        output << values[index];
    }
    return output.str();
}

std::vector<TestCase> loadTests(const std::string& path) {
    std::ifstream file(path);
    std::vector<TestCase> tests;
    TestCase current{};
    std::string line;

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }
        if (line.front() == '[' && line.back() == ']') {
            if (!current.name.empty()) {
                tests.push_back(current);
            }
            current = TestCase{line.substr(1, line.size() - 2), {}};
            continue;
        }

        const std::size_t separator = line.find('=');
        if (separator != std::string::npos) {
            current.values[trim(line.substr(0, separator))] = trim(line.substr(separator + 1));
        }
    }

    if (!current.name.empty()) {
        tests.push_back(current);
    }
    return tests;
}

bool runCase(const TestCase& test, std::string& detail) {
    if (test.values.count("function") == 0 || test.values.count("input") == 0 || test.values.count("expected") == 0) {
        detail = "Test case must define function, input, and expected.";
        return false;
    }

    const std::string function = test.values.find("function")->second;
    const std::string input = test.values.find("input")->second;
    const bool shouldPass = test.values.find("expected")->second == "ok";
    bool passed = false;

    if (function == "parsePort") {
        uint16_t port = 0;
        std::string error;
        passed = chatmessenger::parsePort(input, port, error);
        detail = passed ? std::to_string(port) : error;
    } else if (function == "parseEndpoint") {
        chatmessenger::Endpoint endpoint{};
        std::string error;
        passed = chatmessenger::parseEndpoint(input, endpoint, error);
        detail = passed ? endpoint.ip + ":" + std::to_string(endpoint.port) : error;
    } else if (function == "parseChatConfig") {
        const std::vector<std::string> args = split(input, '|');
        std::vector<char*> argv;
        argv.reserve(args.size());
        for (const std::string& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        chatmessenger::ChatConfig config{};
        std::string error;
        passed = chatmessenger::parseChatConfig(static_cast<int>(argv.size()), argv.data(), config, error);
        detail = passed ? config.source.ip + " -> " + config.destination.ip : error;
    } else {
        detail = "Unknown function in test case.";
        return false;
    }

    return passed == shouldPass;
}

} // namespace

TEST_CASE("INI-driven parser validation", "[ini][parser]") {
    const std::string path = "tests/test_cases.ini";
    const std::vector<TestCase> tests = loadTests(path);
    REQUIRE_FALSE(tests.empty());

    std::cout << "\nINI test case summary\n";
    std::vector<std::string> failedNames;
    for (const TestCase& test : tests) {
        std::string detail;
        const bool ok = runCase(test, detail);
        std::cout << (ok ? "[PASS] " : "[FAIL] ") << test.name << " - " << detail << '\n';
        if (!ok) {
            failedNames.push_back(test.name);
        }
    }

    CAPTURE(failedNames);
    CAPTURE(join(failedNames, ", "));
    CHECK(failedNames.empty());
}
