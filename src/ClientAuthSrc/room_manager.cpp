/****************************************************
 * Author      : Phyu H. Lwin
 * Date        : 2025 August 6
 * Filename    : room_manager.cpp
 * Description : defines functions for managing chat rooms
 ****************************************************/

#include "../../include/ClientAuthInc/room_manager.hpp"

void RoomManager::joinRoom(const std::string& input, int client_fd, std::shared_ptr<Client> client,
    std::mutex& mutex,
    std::unordered_map<int, std::shared_ptr<Client>>& clients) {
    std::istringstream iss(input);
    std::string cmd, room;
    iss >> cmd >> room;
    if (room.empty()) {
        client->enqueueMessage("Usage: /join <room>\n");
        return;
    }

    std::lock_guard<std::mutex> lock(mutex);
    if (!client->current_room.empty())
        RoomManager::chat_rooms[client->current_room].erase(client_fd);

    client->current_room = room;
    RoomManager::chat_rooms[room].insert(client_fd);
    client->enqueueMessage("Joined room: " + room + "\n");
}

// Function to handle a client leaving a room
void RoomManager::leaveRoom(int client_fd, std::shared_ptr<Client> client, std::mutex& mutex) {
    std::lock_guard<std::mutex> lock(mutex);

    if (!client->current_room.empty()) {
		// Remove the client from the current room
        std::string room = client->current_room;
        RoomManager::chat_rooms[room].erase(client_fd);
		// If the client was the last one in the room, notify them
        client->enqueueMessage("Left room: " + room + "\n");
        client->current_room = "";

        // Clean up the room if it's empty
        if (RoomManager::chat_rooms[room].empty()) {
            RoomManager::chat_rooms.erase(room);
        }
    }
}

void RoomManager::listRooms(std::shared_ptr<Client> client) {
    std::string msg = "Active rooms:\n";
    for (const auto& pair : chat_rooms) {
        msg += "- " + pair.first + " (" + std::to_string(pair.second.size()) + " users)\n";
    }
    client->enqueueMessage(msg);
}

void RoomManager::broadcastMessage(const std::string& input, int client_fd,
    std::shared_ptr<Client> client,
    std::mutex& mutex,
    std::unordered_map<int, std::shared_ptr<Client>>& clients) {
    std::lock_guard<std::mutex> lock(mutex);
    if (client->current_room.empty()) {
        client->enqueueMessage("Join a room with /join <room> first.\n");
        return;
    }

    std::string full_msg = client->username + ": " + input + "\n";
    for (int fd : RoomManager::chat_rooms[client->current_room]) {
        if (fd != client_fd && clients.count(fd)) {
            clients[fd]->enqueueMessage(full_msg);
        }
    }
}
