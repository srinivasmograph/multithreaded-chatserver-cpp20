/****************************************************
 * Author      : Phyu H. Lwin
 * Date        : 2025 August 5
 * Filename    : TcpServer.hpp
 * Description : Single Client TCP Server Implementation
 ****************************************************/

#pragma once

#ifdef _WIN32
#include <winsock2.h>
#else
#error This TcpServer implementation only supports Windows (Winsock)    
#endif

class TcpServer {
public:
    TcpServer(int port);
    ~TcpServer();

    void start();  // Starts the server loop

private:
    int port;
    SOCKET server_fd;
    SOCKET client_socket;

    void setupSocket();
    void acceptClient();
    void handleClient();
};
