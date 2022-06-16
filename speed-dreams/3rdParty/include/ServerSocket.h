#pragma once
#include "ipclib_export.h"
#include "ipclib_portability.h"
#include "Socket.h"

/// @brief A class that represents a server that can communicate with a client
///		   It can receive messages and connect asynchronously and send messages synchronously
class IPCLIB_EXPORT ServerSocket : public Socket
{
public:
    explicit ServerSocket(IPC_IP_TYPE p_ip = LOCAL_HOST, int p_port = 8888, int p_connections = 1);

    int Initialize();

    void ConnectAsync();

    int AwaitClientConnection();

    int SendData(const char* p_data, int p_size) const;

    int Disconnect();

    int CloseServer();

    bool Connected();

    ~ServerSocket();

private:
    void ConnectThreadFunction();

    int Connect();

    IPC_IP_TYPE m_ip;
    int m_port;
    int m_connections;

    SOCKET m_serverSocket = 0;
    struct sockaddr_in m_client;
    struct sockaddr_in m_server;

    bool m_connecting;

    bool m_open = false;

    int m_connectErrorCode = IPCLIB_SUCCEED;
};