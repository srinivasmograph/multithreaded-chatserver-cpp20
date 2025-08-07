/****************************************************
 * Author      : Phyu H. Lwin
 * Date        : 2025 August 6
 * Filename    : server.cpp
 * Description : Defines the Server class for a multithreaded chat server
 ****************************************************/

#include "../../include/ClientAuthInc/server.hpp"

Server::Server(int port) : port(port), server_fd(INVALID_SOCKET) {}

// Function to set up the server socket
void Server::setupServerSocket() {
    WSADATA wsaData;
	// Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
	// Check if socket creation was successful
    if (server_fd == INVALID_SOCKET) {
        throw std::runtime_error("Socket creation failed");
    }

    sockaddr_in addr{};
	// Set up the address structure
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
	// Set socket options to allow address reuse
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

	// Bind the socket to the address and port
    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        throw std::runtime_error("Bind failed");
    }
	// Set the socket to listen for incoming connections
    if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR) {
        throw std::runtime_error("Listen failed");
    }
	// Print the port number the server is listening on
    std::cout << "Server listening on port " << port << std::endl;
}

// Function to accept incoming connections and handle them in separate threads
void Server::acceptConnections() {
    while (true) {
        sockaddr_in client_addr{};
        int len = sizeof(client_addr);
        SOCKET client_fd = accept(server_fd, (sockaddr*)&client_addr, &len);
        if (client_fd == INVALID_SOCKET) continue;

        // Use a lambda to wrap the function call for std::thread
        std::thread([client_fd]() {
            ClientHandler::handleClient(client_fd);
        }).detach();
    }
}

// Function to start the server
void Server::start() {
    setupServerSocket();
    acceptConnections();
}
