/****************************************************
 * Author      : Phyu H. Lwin
 * Date        : 2025 August 6
 * Filename    : client_handler.cpp
 * Description : Handles client connections and communication
 ****************************************************/

#include "../../include/ClientAuthInc/client_handler.hpp"

namespace {
	// Constants
    constexpr int BUFFER_SIZE = 4096;
    std::unordered_map<int, std::shared_ptr<Client>> clients;
    std::mutex clients_mutex;

	// Function to send a message to a socket
    void sendToSocket(int fd, const std::string& msg) {
        send(fd, msg.c_str(), static_cast<int>(msg.size()), 0);
    }

    void clientWriter(std::shared_ptr<Client> client) {
        std::string msg;
        while (true) {
            // Wait for a message to be available in the queue
            std::unique_lock<std::mutex> lock(client->queue_mutex);
            client->queue_cv.wait(lock, [&] { return !client->message_queue.empty(); });

            if (client->socket_fd == INVALID_SOCKET) break;

			// Process all messages in the queue
            while (!client->message_queue.empty()) {
                msg = client->message_queue.front();
                client->message_queue.pop();
				// Unlock the mutex while sending to avoid deadlock
                lock.unlock();
                sendToSocket(client->socket_fd, msg);
                lock.lock();
            }
        }
    }
}

std::string ClientHandler::authenticateClient(int client_fd) {
    char buffer[BUFFER_SIZE];
    sendToSocket(client_fd, "Enter your username: ");
    int len = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (len <= 0) return "";

    buffer[len] = '\0';
    std::string username(buffer);
    username.erase(username.find_last_not_of(" \r\n") + 1);
    return username;
}

std::shared_ptr<Client> ClientHandler::registerClient(int client_fd, const std::string& username) {
    auto client = std::make_shared<Client>(client_fd);
    client->username = username;

    std::lock_guard<std::mutex> lock(clients_mutex);
    clients[client_fd] = client;
    return client;
}

void ClientHandler::handleClientCommands(std::shared_ptr<Client> client) {
    char buffer[BUFFER_SIZE];
    int client_fd = client->socket_fd;

    while (true) {
        int len = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (len <= 0) break;

        buffer[len] = '\0';
        std::string input(buffer);
        input.erase(input.find_last_not_of(" \r\n") + 1);

        if (input == "/quit") break;
        else if (input.compare(0, 5, "/join") == 0)
            RoomManager::joinRoom(input, client_fd, client, clients_mutex, clients);
        else if (input == "/leave")
            RoomManager::leaveRoom(client_fd, client, clients_mutex);
        else if (input == "/rooms")
            RoomManager::listRooms(client);
        else
            RoomManager::broadcastMessage(input, client_fd, client, clients_mutex, clients);
    }
}

void ClientHandler::cleanupClient(std::shared_ptr<Client> client) {
    int fd = client->socket_fd;
    RoomManager::leaveRoom(fd, client, clients_mutex);
    client->socket_fd = INVALID_SOCKET;

    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(fd);
}

void ClientHandler::handleClient(int client_fd) {
    std::string username = ClientHandler::authenticateClient(client_fd);
    if (username.empty()) return;

    auto client = ClientHandler::registerClient(client_fd, username);
	std::thread writer(clientWriter, client); // defined in anonymous namespace
    writer.detach();

    sendToSocket(client_fd, "Welcome, " + username + "!\n");
    ClientHandler::handleClientCommands(client);
    ClientHandler::cleanupClient(client);
}
