#include "ChatApp.h"

#include <iostream>
#include <string>
#include <thread>

namespace chatmessenger {

ChatApp::ChatApp(IUdpTransport& transport, std::istream& input, std::ostream& output, std::ostream& errorOutput)
    : transport_(transport), input_(input), output_(output), errorOutput_(errorOutput), running_(false) {}

int ChatApp::run(const ChatConfig& config) {
    std::string error;
    if (!transport_.open(config.source, error)) {
        errorOutput_ << error << '\n';
        return 1;
    }

    running_ = true;
    std::thread receiver(&ChatApp::receiveLoop, this);

    output_ << "Chat ready. Type a message and press Enter. Type /quit to exit.\n";
    printPrompt();

    std::string line;
    while (running_ && std::getline(input_, line)) {
        if (line == "/quit") {
            running_ = false;
            break;
        }
        if (!line.empty() && !transport_.sendTo(config.destination, line, error)) {
            errorOutput_ << error << '\n';
        }
        printPrompt();
    }

    running_ = false;
    transport_.close();
    if (receiver.joinable()) {
        receiver.join();
    }
    return 0;
}

void ChatApp::receiveLoop() {
    while (running_) {
        std::string message;
        Endpoint sender{};
        std::string error;
        if (!transport_.receive(message, sender, error)) {
            if (running_) {
                errorOutput_ << error << '\n';
            }
            return;
        }

        output_ << "\r[" << sender.ip << ':' << sender.port << "] " << message << "\n> ";
        output_.flush();
    }
}

void ChatApp::printPrompt() {
    output_ << "> ";
    output_.flush();
}

} // namespace chatmessenger
