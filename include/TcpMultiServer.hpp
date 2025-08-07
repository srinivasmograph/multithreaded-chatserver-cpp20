#pragma once

#ifdef _WIN32
/****************************************************
 * Author      : Phyu H. Lwin
 * Date        : 2025 August 5
 * Filename    : TcpMultiServer.hpp
 * Description : Multithreaded TCP Server Implementation
 ****************************************************/

#include <winsock2.h>
#else
#error "This TcpMultiServer implementation only supports Windows (Winsock)"
#endif

#include <thread>
#include <vector>
#include <ws2tcpip.h>
#include <iostream>
#include <mutex>

class TcpMultiServer {
public:
    TcpMultiServer(int port);
    ~TcpMultiServer();

    void start();

private:
    int port;
    SOCKET server_fd;

    void setupSocket();
    void acceptClients();
    static void handleClient(SOCKET client_socket);
    static void handleClientWithID(SOCKET client_socket);

    std::vector<std::thread> client_threads;

    static std::string getClientName(SOCKET client_socket);
    static void processClientMessages(SOCKET client_socket, const std::string& clientName);
    static void sendMessage(SOCKET socket, const std::string& message);

};
