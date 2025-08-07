/****************************************************
 * Author      : Phyu H. Lwin
 * Date        : 2025 August 6
 * Filename    : room_manager.hpp
 * Description : defines functions for managing chat rooms
 ****************************************************/

#pragma once
#include "Client.hpp"

#include <unordered_map>
#include <memory>
#include <mutex>
#include <unordered_set>
#include <sstream>

class RoomManager {
private:
    static std::unordered_map<std::string, std::unordered_set<int>> chat_rooms;
public:
	// Function declarations for managing chat rooms
    static void joinRoom(const std::string& input, int client_fd, std::shared_ptr<Client> client,
        std::mutex& mutex,
        std::unordered_map<int, std::shared_ptr<Client>>& clients);

    static void leaveRoom(int client_fd, std::shared_ptr<Client> client,
        std::mutex& mutex);

    static void listRooms(std::shared_ptr<Client> client);

    static void broadcastMessage(const std::string& input, int client_fd,
        std::shared_ptr<Client> client,
        std::mutex& mutex,
        std::unordered_map<int, std::shared_ptr<Client>>& clients);
};