#include "../../include/ClientAuthInc/Client.hpp"

Client::Client(SocketType fd) : socket_fd(fd) {}

// Destructor for Client class
Client::~Client() {
    if (socket_fd != INVALID_SOCKET) {
        CLOSESOCKET(socket_fd);
    }
}

void Client::enqueueMessage(const std::string& msg) {
    std::lock_guard<std::mutex> lock(queue_mutex);
    message_queue.push(msg);
    queue_cv.notify_one();
}

bool Client::dequeueMessage(std::string& msg_out) {
    std::unique_lock<std::mutex> lock(queue_mutex);
    if (message_queue.empty()) return false;
    msg_out = message_queue.front();
    message_queue.pop();
    return true;
}