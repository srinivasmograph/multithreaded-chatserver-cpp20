/****************************************************
 * Author      : Phyu H. Lwin
 * Date        : 2025 August 5
 * Filename    : select_server.hpp
 * Description : Accept multiple client connection. Use select() to monitor sockets.
                 Handle non-blocking I/O (no threads). Echo messages back to clients.
 ****************************************************/

#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cstring>
#include <unordered_map>

#pragma comment(lib, "Ws2_32.lib")

class SelectServer {
private:
    static const int PORT = 54000;
    static const int BUFFER_SIZE = 1024;

    SOCKET serverSocket;
    std::map<SOCKET, std::string> clients;
    std::unordered_map<SOCKET, std::string> messageBuffers;

    void setNonBlocking(SOCKET socket);
    std::string sanitize(const std::string& input);
    void sendToClient(SOCKET client, const std::string& msg);
    void broadcastMessage(const std::string& msg, SOCKET sender);
    void handleNewConnection();
    void handleClientMessage(SOCKET clientSocket);
    void cleanup();

public:
    SelectServer();
    ~SelectServer();
    void run(); // Main server loop
};