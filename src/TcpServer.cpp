/****************************************************
 * Author      : Phyu H. Lwin
 * Date        : 2025 August 5
 * Filename    : TcpServer.cpp
 * Description : Single Client TCP Server Implementation
 ****************************************************/

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstring>

#include "..\include\TcpServer.hpp"
#pragma comment(lib, "Ws2_32.lib")

// Constructor: Initializes Winsock and sets up the server socket
TcpServer::TcpServer(int port)
    : port(port), server_fd(INVALID_SOCKET), client_socket(INVALID_SOCKET) {
    WSADATA wsaData;
    // Start Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set up the server socket
    setupSocket();
}

// Destructor: Cleans up sockets and Winsock
TcpServer::~TcpServer() {
    // Close client socket if open
    if (client_socket != INVALID_SOCKET) closesocket(client_socket);
    // Close server socket if open
    if (server_fd != INVALID_SOCKET) closesocket(server_fd);
    // Cleanup Winsock
    WSACleanup();
}

void TcpServer::setupSocket() {
    // Create a TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure for the server address
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;           // Use IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;   // Listen on any network interface
    server_addr.sin_port = htons(port);         // Set port (network byte order)

    // Bind the socket to the specified IP and port
    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Output that the server is ready and listening
    std::cout << "Server listening on port " << port << "...\n";
}

void TcpServer::acceptClient() {
    // Prepare structure to hold client address
    sockaddr_in client_addr{};
    int client_size = sizeof(client_addr);
    // Accept an incoming client connection
    client_socket = accept(server_fd, (sockaddr*)&client_addr, &client_size);
    if (client_socket == INVALID_SOCKET) {
        // Print error if accept fails
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Notify that a client has connected
    std::cout << "Client connected.\n";
}

void TcpServer::handleClient() {
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    std::string input;

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            std::cout << "Client disconnected.\n";
            break;
        }

        input.append(buffer, bytes_received);

        // Look for newline (end of message)
        size_t pos;
        while ((pos = input.find('\n')) != std::string::npos) {
            std::string line = input.substr(0, pos);
            std::cout << "Client: " << line << std::endl;

            // Echo the message back
            send(client_socket, line.c_str(), line.size(), 0);
            send(client_socket, "\n", 1, 0); // maintain newline in response

            input.erase(0, pos + 1);  // Remove processed line
        }
    }
}

void TcpServer::start() {
    // Accept a client connection
    acceptClient();
    // Handle communication with the client
    handleClient();
}
