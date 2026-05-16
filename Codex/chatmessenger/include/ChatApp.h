#ifndef CHATMESSENGER_CHATAPP_H
#define CHATMESSENGER_CHATAPP_H

#include "ChatConfig.h"
#include "UdpSocket.h"

#include <atomic>
#include <iosfwd>
#include <string>

namespace chatmessenger {

/**
 * @brief Coordinates console input, received datagrams, and transport I/O.
 *
 * ChatApp uses dependency injection for the UDP transport and the streams so
 * parsing, networking, and presentation remain independently testable.
 */
class ChatApp {
public:
    ChatApp(IUdpTransport& transport, std::istream& input, std::ostream& output, std::ostream& errorOutput);

    /**
     * @brief Runs the chat loop until EOF, /quit, or a transport error occurs.
     * @return 0 on clean exit, non-zero when startup fails.
     */
    int run(const ChatConfig& config);

private:
    void receiveLoop();
    void printPrompt();

    IUdpTransport& transport_;
    std::istream& input_;
    std::ostream& output_;
    std::ostream& errorOutput_;
    std::atomic<bool> running_;
};

} // namespace chatmessenger

#endif // CHATMESSENGER_CHATAPP_H
