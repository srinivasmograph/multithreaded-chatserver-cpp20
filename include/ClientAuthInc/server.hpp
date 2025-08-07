/****************************************************
 * Author      : Phyu H. Lwin
 * Date        : 2025 August 6
 * Filename    : server.hpp
 * Description : Defines the Server class for a multithreaded chat server
 ****************************************************/

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#include "client_handler.hpp"

class Server {
public:
    Server(int port);
    void start();
private:
    int port;
    int server_fd;

    // Function to set up the server socket
    void setupServerSocket();
	// Function to accept incoming connections and handle them in separate threads
    void acceptConnections();
};