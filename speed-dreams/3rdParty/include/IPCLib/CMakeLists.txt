# CMakeList.txt : CMake project for IPCLib, include source and define
# project specific logic here.
#
#cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_library (IPCLib SHARED "ClientSocketAsync.cpp" "ServerSocketAsync.cpp" "ClientSocketAsync.h" "ServerSocketAsync.h" "SocketAsync.h" "SocketAsync.cpp")

include(GenerateExportHeader)
GENERATE_EXPORT_HEADER(IPCLib)

message( ${CMAKE_BINARY_DIR}/exports)
target_include_directories(IPCLib PUBLIC ${CMAKE_BINARY_DIR}/IPCLib)

INSTALL(TARGETS IPCLib)