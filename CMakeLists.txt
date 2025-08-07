# CMakeList.txt : CMake project for multithreaded-chatserver, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("multithreaded-chatserver")

# Add source to this project's executable.
add_executable (multithreaded-chatserver 
    "src/main.cpp" 
    "src/helper.cpp" 
    "include/TcpServer.hpp" 
    "src/TcpServer.cpp"  
    "src/TcpMultiServer.cpp" 
    "include/TcpMultiServer.hpp" 
    "src/TcpMultiServer.cpp" 
    "include/SelectServer.hpp" 
    "src/SelectServer.cpp" 
    "include/ClientAuthInc/Client.hpp"
    "include/ClientAuthInc/client_handler.hpp"
    "include/ClientAuthInc/room_manager.hpp"
    "include/ClientAuthInc/server.hpp"
    "src/ClientAuthSrc/Client.cpp"
    "src/ClientAuthSrc/client_handler.cpp"
    "src/ClientAuthSrc/room_manager.cpp"
    "src/ClientAuthSrc/server.cpp"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET multithreaded-chatserver PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
