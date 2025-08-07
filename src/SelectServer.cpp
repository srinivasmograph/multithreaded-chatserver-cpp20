/****************************************************
 * Author      : Phyu H. Lwin
 * Date        : 2025 August 5
 * Filename    : select_server.cpp
 * Description : Accept multiple client connection. Use select() to monitor sockets.
                 Handle non-blocking I/O (no threads). Echo messages back to clients.
 ****************************************************/

#include "..\include\SelectServer.hpp"

 // Constructor/Destructor

SelectServer::SelectServer() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        exit(1);
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);
    setNonBlocking(serverSocket);

    std::cout << "Server listening on port " << PORT << "...\n";
}

SelectServer::~SelectServer() {
    cleanup();
    WSACleanup();
}

void SelectServer::cleanup() {
    for (const auto& client : clients) {
        closesocket(client.first);
    }
    closesocket(serverSocket);
}

// Utility Functions

void SelectServer::setNonBlocking(SOCKET socket) {
    u_long mode = 1;
    ioctlsocket(socket, FIONBIO, &mode);
}

std::string SelectServer::sanitize(const std::string& input) {
    std::string result = input;
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
    return result;
}

void SelectServer::sendToClient(SOCKET client, const std::string& msg) {
    send(client, msg.c_str(), static_cast<int>(msg.size()), 0);
}

void SelectServer::broadcastMessage(const std::string& msg, SOCKET sender) {
    for (const auto& clientPair : clients) {
        SOCKET client = clientPair.first;
        const std::string& name = clientPair.second;
        if (client != sender && !name.empty()) {
            sendToClient(client, msg);
        }
    }
}

// Connection Handling

void SelectServer::handleNewConnection() {
    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket != INVALID_SOCKET) {
        setNonBlocking(clientSocket);
        clients[clientSocket] = "";  // Will be set later
        std::cout << "New client connected: " << clientSocket << "\n";
        sendToClient(clientSocket, "Enter your username:\n");
    }
}

// Client Message Handling
void SelectServer::handleClientMessage(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);

    if (bytesReceived <= 0) {
        std::cout << "Client disconnected: " << clientSocket << "\n";
        closesocket(clientSocket);
        clients.erase(clientSocket);
        messageBuffers.erase(clientSocket);
        return;
    }

    // Append received data to client's buffer
    std::string& bufferRef = messageBuffers[clientSocket];
    bufferRef.append(buffer, bytesReceived);

    std::string& username = clients[clientSocket];

    size_t pos;
    while ((pos = bufferRef.find('\n')) != std::string::npos) {
        std::string line = bufferRef.substr(0, pos);
        bufferRef.erase(0, pos + 1);
        line = sanitize(line);

        if (username.empty()) {
            username = line;
            std::string welcome = "Welcome, " + username + "!\n";
            sendToClient(clientSocket, welcome);
            std::cout << "Client " << clientSocket << " set username to '" << username << "'\n";
        }
        else {
            std::string fullMessage = username + ": " + line + "\n";
            std::cout << fullMessage;
            broadcastMessage(fullMessage, clientSocket);
        }
    }
}

// Main Loop

void SelectServer::run() {
    while (true) {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(serverSocket, &readSet);
        SOCKET maxSocket = serverSocket;

        for (const auto& clientPair : clients) {
            SOCKET clientSocket = clientPair.first;
            FD_SET(clientSocket, &readSet);
            if (clientSocket > maxSocket) maxSocket = clientSocket;
        }

        int activity = select(0, &readSet, nullptr, nullptr, nullptr);
        if (activity < 0) {
            std::cerr << "select() failed\n";
            break;
        }

        if (FD_ISSET(serverSocket, &readSet)) {
            handleNewConnection();
        }

        std::vector<SOCKET> toProcess;
        for (auto it = clients.begin(); it != clients.end(); ++it) {
            SOCKET clientSocket = it->first;
            if (FD_ISSET(clientSocket, &readSet)) {
                toProcess.push_back(clientSocket);
            }
        }

        for (SOCKET clientSocket : toProcess) {
            handleClientMessage(clientSocket);
        }
    }
}


//cl /EHsc select_server.cpp /link Ws2_32.lib
// Current issue: backspacing in the console may not work as expected due to the way input is handled.