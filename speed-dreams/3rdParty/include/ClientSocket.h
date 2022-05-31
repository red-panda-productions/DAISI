#pragma once
#include "ipclib_portability.h"
#include "ipclib_export.h"
#include "Socket.h"

/// @brief A class that represents a client that can communicate with a server
///	       It can receive messages and connect asynchronously and send messages synchronously
class IPCLIB_EXPORT ClientSocket : public Socket
{
public:
    explicit ClientSocket(IPC_IP_TYPE p_ip = LOCAL_HOST, int p_port = 8888);

    int Initialize();

    int SendData(const char* p_data, int p_size) const;

    int Disconnect();

    ~ClientSocket();

private:
    struct sockaddr_in m_server;
    IPC_IP_TYPE m_ip;
    int m_port;
};