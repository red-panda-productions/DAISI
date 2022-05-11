#pragma once
#pragma comment(lib, "ws2_32.lib")
#include "ipclib_export.h"
#include <WinSock2.h>
#include "Socket.h"

/// @brief A class that represents a client that can communicate with a server
///	       It can receive messages and connect asynchronously and send messages synchronously
class IPCLIB_EXPORT ClientSocket : public Socket
{
public:
    explicit ClientSocket(PCWSTR p_ip = L"127.0.0.1", int p_port = 8888);

    int Initialize();

    int SendData(const char* p_data, int p_size) const;

    int Disconnect();

    ~ClientSocket();

private:
    struct sockaddr_in m_server;
    PCWSTR m_ip;
    int m_port;
};