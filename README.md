# Multithreaded Chat Server
#### Systems-Level + Modern C++ (Core Competency Project)

A **multithreaded chat server** that can:

* Handle multiple clients concurrently
* Support chat rooms
* Broadcast messages using a message queue

### Tools & Technologies:

* **C++20** – Modern language features.
* **Compiler** – GCC or Clang (with C++20 support).
* **Build System** – CMake.
* **Socket API** – Winsock (Windows).
* **Threading** – `std::thread`, `std::mutex`, `std::condition_variable`.
* **I/O Multiplexing** – `select()` or `epoll` (Linux).
* **Development Environment** – VS Code.
* **Version Control** – Git.
* **Message Queue** – std::queue + mutex/condition\_variable or use a concurrent queue.

---

### **Screenshots**

![App Screenshot 1](screenshots/app-ss%20(1).png)
![App Screenshot 2](screenshots/app-ss%20(2).png)
![App Screenshot 3](screenshots/app-ss%20(3).png)

---

### **Project Key Concepts:**

* Networking with raw sockets
* Parallel processing using threads
* Resource management using RAII and smart pointers
* Efficient I/O handling at scale
* Clean, modular modern C++ code
