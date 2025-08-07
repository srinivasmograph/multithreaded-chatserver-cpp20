/****************************************************
 * Author      : Phyu H. Lwin
 * Date        : 2025 August 5
 * Filename    : TcpMultiServer.cpp
 * Description : Multithreaded TCP Server Implementation
 ****************************************************/

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstring>
#include <thread>

#include "..\include\TcpMultiServer.hpp"
#pragma comment(lib, "Ws2_32.lib")

#include <mutex> 

static std::mutex console_mutex;

// Helper Function: sanitize the client username properly
std::string cleanInput(const std::string& input) {
    std::string result = input;
    while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
        result.pop_back();
    }
    return result;
}

TcpMultiServer::TcpMultiServer(int port) : port(port), server_fd(INVALID_SOCKET) {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        exit(EXIT_FAILURE);
    }

    setupSocket();
}

TcpMultiServer::~TcpMultiServer() {
    for (auto& t : client_threads) {
        if (t.joinable()) t.join();
    }

    if (server_fd != INVALID_SOCKET) closesocket(server_fd);
    WSACleanup();
}

void TcpMultiServer::setupSocket() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        exit(EXIT_FAILURE);
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << port << "...\n";
}

void TcpMultiServer::acceptClients() {
    while (true) {
        sockaddr_in client_addr{};
        int client_size = sizeof(client_addr);

		// Accept a new client connection
        SOCKET client_socket = accept(server_fd, (sockaddr*)&client_addr, &client_size);

        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            continue;
        }

        try {
            client_threads.emplace_back([this, client_socket]() {
                this->handleClientWithID(client_socket);
                });
        }
        catch (const std::system_error& e) {
            std::cerr << "Thread creation failed: " << e.what() << std::endl;
            closesocket(client_socket);  // clean up
        }
    }
}

void TcpMultiServer::handleClient(SOCKET client_socket) {
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    std::string messageBuffer;

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            std::cout << "Client disconnected.\n";
            break;
        }

        // Append received data to the buffer
        messageBuffer.append(buffer, bytes_received);

        // Check for complete lines
        size_t pos;
        while ((pos = messageBuffer.find('\n')) != std::string::npos) {
            std::string fullMessage = messageBuffer.substr(0, pos);
            messageBuffer.erase(0, pos + 1);  // remove processed message

            // Optional: trim \r if telnet sends \r\n
            if (!fullMessage.empty() && fullMessage.back() == '\r') {
                fullMessage.pop_back();
            }

            std::cout << "Client: " << fullMessage << std::endl;

            // Echo back to client
            std::string response = "You said: " + fullMessage + "\n";
            send(client_socket, response.c_str(), response.size(), 0);
        }
    }

    closesocket(client_socket);
}

// function to prompt the client for their name
std::string TcpMultiServer::getClientName(SOCKET client_socket) {
    const std::string prompt = "Enter your username:\n";
    TcpMultiServer::sendMessage(client_socket, prompt);

    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    std::string nameBuffer;

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            return "";
        }

        nameBuffer.append(buffer, bytes_received);

        // Check if full line (user hit Enter)
        size_t newlinePos = nameBuffer.find('\n');
        if (newlinePos != std::string::npos) {
            std::string name = nameBuffer.substr(0, newlinePos);
            return cleanInput(name);
        }
    }
}

// Function to send a message to the client
void TcpMultiServer::sendMessage(SOCKET socket, const std::string& message) {
    send(socket, message.c_str(), message.size(), 0);
}

// Handles Message in Loop
void TcpMultiServer::processClientMessages(SOCKET client_socket, const std::string& clientName) {
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    std::string messageBuffer;

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) break;

        messageBuffer.append(buffer, bytes_received);

        size_t pos;
        while ((pos = messageBuffer.find('\n')) != std::string::npos) {
            std::string msg = cleanInput(messageBuffer.substr(0, pos));
            messageBuffer.erase(0, pos + 1);

            {
                std::lock_guard<std::mutex> lock(console_mutex);
                std::cout << clientName << ": " << msg << std::endl;
            }

            std::string response = "You said: " + msg + "\n";
            TcpMultiServer::sendMessage(client_socket, response);
        }
    }
}

// Handles Client with ID
void TcpMultiServer::handleClientWithID(SOCKET client_socket) {
    std::string clientName = TcpMultiServer::getClientName(client_socket);

    if (clientName.empty()) {
        closesocket(client_socket);
        return;
    }

    {
        std::lock_guard<std::mutex> lock(console_mutex);
        std::cout << "Client \"" << clientName << "\" connected.\n";
    }

    TcpMultiServer::processClientMessages(client_socket, clientName);

    {
        std::lock_guard<std::mutex> lock(console_mutex);
        std::cout << "Client \"" << clientName << "\" disconnected.\n";
    }

    closesocket(client_socket);
}


void TcpMultiServer::start() {
    acceptClients();
}
