#pragma once
#pragma comment(lib,"ws2_32.lib")
#include "ipclib_export.h"
#include <WinSock2.h>
#include "SocketAsync.h"

#ifndef SERVER_BUFFER_BYTE_SIZE
#define SERVER_BUFFER_BYTE_SIZE 512
#endif

/// <summary>
/// A class that represents a server that can communicate with a client
/// It can receive messages and connect asynchronously and send messages synchronously
/// </summary>
class IPCLIB_EXPORT ServerSocketAsync : public SocketAsync
{
public:
	ServerSocketAsync(PCWSTR p_ip = L"127.0.0.1", int p_port = 8888, int p_connections = 1);

	void ConnectAsync();

	void AwaitClientConnection() const;

	void SendData(const char* p_data, const int p_size) const;

	void Disconnect();

	void CloseServer();

	~ServerSocketAsync();

private:

	void Connect();

	SOCKET m_serverSocket = 0;
	struct sockaddr_in m_client;
	struct sockaddr_in m_server;

	bool m_open;
};