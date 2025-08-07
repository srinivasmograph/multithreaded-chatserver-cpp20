#pragma once

#ifdef _WIN32
#include <winsock2.h>
#define CLOSESOCKET closesocket
using SocketType = SOCKET;
#else
#include <unistd.h>
#define INVALID_SOCKET -1
#define CLOSESOCKET close
using SocketType = int;
#endif

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>

class Client {
public:
    SocketType socket_fd;
    std::string username;
    std::string current_room;

    std::queue<std::string> message_queue;
    std::mutex queue_mutex;
    std::condition_variable queue_cv;

    Client(SocketType fd);
    ~Client();
    void enqueueMessage(const std::string& msg);
    bool dequeueMessage(std::string& msg_out);
};
