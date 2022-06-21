/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once

///@brief Windows specific defines
#ifdef WIN32
#pragma comment(lib, "ws2_32.lib")
#include <WS2tcpip.h>
#include <WinSock2.h>
#define IPC_IP_TYPE                     PCWSTR
#define IPC_DATA_TYPE                   WSADATA
#define CLOSE_SOCKET(p_socket)          closesocket(p_socket)
#define CLEANUP_SOCKET()                WSACleanup()
#define GET_LAST_ERROR()                WSAGetLastError()
#define INET_PTON(p_inet, p_ip, p_addr) InetPtonW(p_inet, p_ip, p_addr)
#define SOCKET_LENGTH                   int
#define LOCAL_HOST                      L"127.0.0.1"
#define SOCKET_LIBRARY_NAME             "[WSA] "
#endif  // WIN32

///@brief Linux specific defines
#ifdef __linux__
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cwchar>
#include <string>
#include <error.h>
#define IPC_IP_TYPE            char*
#define IPC_DATA_TYPE          std::string
#define SOCKET                 int
#define CLOSE_SOCKET(p_socket) shutdown(p_socket, SHUT_RDWR)
#define CLEANUP_SOCKET()
#define INVALID_SOCKET                  -1
#define SOCKET_ERROR                    -1
#define GET_LAST_ERROR()                errno
#define SOCKET_LENGTH                   socklen_t
#define INET_PTON(p_inet, p_ip, p_addr) inet_pton(p_inet, p_ip, p_addr)
#define LOCAL_HOST                      "127.0.0.1"
#define SOCKET_LIBRARY_NAME             "[Linux] "
#endif  // __linux__

// ------------------------------- Common defines --------------------------------------------
#include <iostream>

#define IPCLIB_ERROR(p_message, p_errorCode) \
    std::cerr << p_message << std::endl;     \
    return p_errorCode;

#define IPCLIB_WARNING(p_message) \
    std::cerr << p_message << std::endl;

#define SOCKET_LIBRARY_ERROR           -1
#define IPCLIB_SUCCEED                 0
#define IPCLIB_SERVER_ERROR            1
#define IPCLIB_CLOSED_CONNECTION_ERROR 2

// ----------------------------- platform specific functions ----------------------------------------

int ConnectToServer(IPC_IP_TYPE p_ip, int p_port, IPC_DATA_TYPE& p_data, SOCKET& p_socket, sockaddr_in& p_server, bool& p_disconnected);

int StartServer(IPC_IP_TYPE p_ip, int p_port, int p_connections, IPC_DATA_TYPE& p_wsa, SOCKET& p_socket, sockaddr_in& p_server, sockaddr_in& p_client, bool& p_open);
