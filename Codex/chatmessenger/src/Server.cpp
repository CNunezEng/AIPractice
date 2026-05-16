#include "ChatApp.h"
#include "ChatConfig.h"
#include "UdpSocket.h"

#include <iostream>

int main(int argc, char* argv[]) {
    chatmessenger::ChatConfig config{};
    std::string error;
    if (!chatmessenger::parseChatConfig(argc, argv, config, error)) {
        std::cerr << error << '\n' << chatmessenger::usageFor(argv[0]) << '\n';
        return 1;
    }

    chatmessenger::UdpSocket socket;
    chatmessenger::ChatApp app(socket, std::cin, std::cout, std::cerr);
    return app.run(config);
}
