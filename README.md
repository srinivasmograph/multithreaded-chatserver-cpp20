https://github.com/srinivasmograph/multithreaded-chatserver-cpp20/releases

# Multithreaded C++20 Chat Server — Secure Socket Server 🚀

[![Releases](https://img.shields.io/badge/Releases-download-blue?logo=github)](https://github.com/srinivasmograph/multithreaded-chatserver-cpp20/releases)

![Network diagram](https://upload.wikimedia.org/wikipedia/commons/4/4e/Network_diagram.svg)

A systems-level chat server written in modern C++ (C++20). It focuses on memory-safe patterns, OS-level sockets, and multithreading. Use it to learn safe concurrency, socket programming, and simple protocols. The repo includes a CMake build, tests, and sample client usage via telnet.

Table of contents
- About
- Features
- Quick download
- Build from source
- Run the server
- Connect with telnet
- Protocol and messages
- Design and internals
- Memory safety and C++ patterns
- Threading, concurrency, and networking
- Logging and diagnostics
- Tests and benchmarks
- Examples
- Contributing
- License

About
This project implements a chat server that handles multiple clients over TCP. It targets Linux and macOS. The code uses C++20, the standard library, and minimal external dependencies. The design favors clear ownership, RAII, and explicit synchronization.

Features
- Native C++20 code
- Multithreaded server using a thread pool
- OS sockets (BSD sockets API)
- Simple thread-safe message routing
- Client support using telnet or a simple TCP client
- CMake-based build and targets
- Unit tests and integration tests
- Small, focused codebase for learning systems programming

Quick download
The latest prebuilt releases live on GitHub Releases. Download the release binary and execute it from that page:
https://github.com/srinivasmograph/multithreaded-chatserver-cpp20/releases

Click the Releases badge above to open the list of assets. Download the binary that matches your platform. After download, make the binary executable and run it:

```bash
# example for a downloaded Linux binary
chmod +x multithreaded-chatserver-cpp20-linux
./multithreaded-chatserver-cpp20-linux --port 9000
```

Build from source
Prerequisites
- CMake 3.16 or newer
- A C++20-capable compiler (g++ 10+, clang 11+, MSVC with C++20)
- make or ninja
- Unix-like shell for run commands (Windows support via MSVC/CMake)

Clone and build

```bash
git clone https://github.com/srinivasmograph/multithreaded-chatserver-cpp20.git
cd multithreaded-chatserver-cpp20
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --parallel
```

Build options
- BUILD_TESTS=ON — enable unit and integration tests
- ENABLE_SANITIZERS=ON — build with ASAN/UBSAN for debugging

Run the server
The server uses a simple CLI. Default port is 9000.

```bash
# run with defaults
./bin/chatserver

# specify a port and thread pool size
./bin/chatserver --port 9000 --threads 8 --max-clients 128
```

The server prints startup info and listens on the chosen port. It manages clients with a thread pool and a connection queue.

Connect with telnet
Use telnet as a quick client. Telnet is widely available and shows the raw text protocol.

```bash
# connect from the same machine
telnet localhost 9000
```

When a client connects the server assigns a short id and broadcasts join/leave events. To send messages, type a line and press Enter. The server broadcasts simple JSON-like lines:

{"from":"client-3","msg":"Hello"}

Protocol and messages
Keep the protocol minimal and human readable.

- Join
  - Server assigns id: client-#
  - Server sends a welcome line with id
- Message
  - Client sends a UTF-8 text line
  - Server prepends metadata and broadcasts to other clients
- Leave
  - Server broadcasts a leave notification on disconnect

Example raw exchange (lines):

Server -> Client:
WELCOME client-3
Server -> All:
JOIN client-3
Client -> Server:
Hello everyone
Server -> All:
MSG client-3 Hello everyone
Client -> Server:
/who
Server -> Client:
CLIENTS client-1,client-2,client-3

Supported commands
- /who — list connected clients
- /name <alias> — set a display name
- /quit — close the connection from client side

Design and internals
The server uses a small set of components.

- Listener
  - Binds to a port and accepts connections
  - Pushes accepted sockets into a connection queue
- Connection queue
  - A thread-safe queue with a condition variable
  - Balances new clients across worker threads
- Worker threads (thread pool)
  - Each thread picks connections from the queue
  - Each worker handles I/O on assigned sockets
  - Uses non-blocking reads with short timeouts for fairness
- Router
  - Receives parsed messages
  - Routes messages to client message queues
- Client state
  - Socket, id, display name, outgoing queue
  - Protected by mutex per-client for I/O safety

The code favors small, single-purpose classes and clear ownership. Each socket wraps into a Connection object that closes the socket on destructor. The thread pool owns workers, and the main server owns the listener and router.

Memory safety and C++ patterns
The code uses modern C++ idioms to reduce errors.

- RAII
  - Sockets and file descriptors wrap in types that close on destruction
- Smart pointers
  - std::unique_ptr for exclusive ownership
  - std::shared_ptr only for shared runtime state where necessary
- Optional and expected
  - std::optional for nullable values
  - small error wrappers for parse and I/O results
- No manual new/delete in logic code
- Clear lifetimes for threads and resources

Threading, concurrency, and networking
The server uses OS-level threads and the native sockets API.

- Threading model
  - Fixed thread pool with configurable size
  - Workers handle both accept and client tasks via queue
  - Per-client mutex and an atomic flag for send state
- Synchronization
  - std::mutex, std::unique_lock, std::condition_variable
  - std::atomic for counters and running flags
- I/O strategy
  - Blocking accept on listener thread
  - Short blocking or non-blocking reads on workers
  - Use of select/poll is optional; implementation keeps code portable
- Backpressure
  - Per-client output queues prevent slow clients from blocking the main flow
  - If a queue exceeds a limit, server drops oldest messages for that client

Logging and diagnostics
The project uses a small logger with levels.

- Levels: ERROR, INFO, DEBUG
- Logs print to stdout by default
- Build with DEBUG to enable verbose trace
- Use sanitizers to detect memory issues during development

Tests and benchmarks
The repo contains unit tests for core components and an integration test that spins up the server and several clients.

Run tests:

```bash
cd build
ctest --output-on-failure
```

Benchmark
A simple load test script lives in tools/bench. It spawns N telnet-like clients, connects, sends messages at a controlled rate, and reports messages per second and latency percentiles.

Examples
Example: start server, then open two telnet clients:

Terminal 1:
./bin/chatserver --port 9000

Terminal 2:
telnet localhost 9000
WELCOME client-1
JOIN client-1

Terminal 3:
telnet localhost 9000
WELCOME client-2
JOIN client-2

In Terminal 2:
Hello from client-1
Server broadcasts:
MSG client-1 Hello from client-1

Integration tips
- Increase thread count for many CPU cores
- Tune max clients and output queue sizes for high load
- Use ulimit -n on Linux to raise open file descriptors if you hit limits

Security and hardening
- The project uses plain TCP and plain text. For production, place the server behind TLS or a proxy that handles TLS.
- Validate and sanitize input for any real deployment.
- Limit rate per client to avoid floods.

Assets and images
Use the diagram above to understand basic flow. For visual demos, record a short terminal session using asciinema and link the cast in docs.

Contributing
- Open an issue for bugs or feature requests
- Create a branch per feature or fix
- Keep commits small and focused
- Add tests for new behavior

Releases and downloads
Get release binaries on GitHub Releases and execute the downloaded binary. The Releases page lists platform-specific assets. Visit the Releases page and download the appropriate file:
https://github.com/srinivasmograph/multithreaded-chatserver-cpp20/releases

License
This repo uses the MIT license. Review the LICENSE file for full terms.