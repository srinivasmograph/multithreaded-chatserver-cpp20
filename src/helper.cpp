/****************************************************
 * Author      : Phyu H. Lwin
 * Date        : 2025 August 5
 * Filename    : helper.cpp
 * Description : Helper Class for the Project
 ****************************************************/

#include <iostream>

#include "../include/TcpServer.hpp"
#include "../include/TcpMultiServer.hpp"
#include "../include/SelectServer.hpp"
#include "../include/ClientAuthInc/server.hpp"

class Helper {
public:
    Helper() = delete;
    ~Helper() = delete;

    // Initializes a TCP server on port 8080 and starts it.
    // Single-threaded (phase 2).
    static void serverSingleThread() {
        TcpServer server(8080);
        server.start();
    }


	// Initializes a multithreaded TCP server on port 8080 and starts it.
	// Multithreaded (phase 3).
    static void serverMultiThread() {
        TcpMultiServer server(8080);
        server.start();
	}

	// Initializes a select-based server on port 8080 and starts it.
	// (phase 4).
    static void selectServer() {
        SelectServer server;
        server.run();
	}

    // Initializes a client authentication server on port 12345 and starts it.
    // This server handles client connections and authentication.
	// (phase 5).
    static void clientAuthServer() {
        Server server(12345);
        server.start();
        WSACleanup(); // Properly shuts down Winsock
    }
};

/*
- Enable Telnet Client on your system to test the server.
- In Power Shell, use the command: telnet localhost 8080 to connect to the server.
- Add another client by using the command: telnet 127.0.0.1 8080.
*/