/****************************************************
 * Author      : Phyu H. Lwin
 * Date        : 2025 August 6
 * Filename    : client_handler.cpp
 * Description : Handles client connections and communication
 ****************************************************/

#include "Client.hpp"
#include "room_manager.hpp"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <iostream>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

#ifndef CLIENT_HANDLER_HPP
#define CLIENT_HANDLER_HPP

class ClientHandler {
private:
	static std::string authenticateClient(int client_fd);
	static std::shared_ptr<Client> registerClient(int client_fd, const std::string& username);
	static void handleClientCommands(std::shared_ptr<Client> client);
	static void cleanupClient(std::shared_ptr<Client> client);

public: 
	static void handleClient(int client_fd);
};

#endif