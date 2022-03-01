#include "ClientSocketAsync.h"
#include <WS2tcpip.h>
#include <iostream>
#include <cstdio>
#include <thread>

 /// <summary>
 /// Connects a client to a server
 /// </summary>
 /// <param name="p_ip"> The IP adress of the server </param>
 /// <param name="p_port"> The port of the server </param>
 /// <param name="p_wsa"> WSAData from the client </param>
 /// <param name="p_socket"> The socket of the client </param>
 /// <param name="p_server"> The server information </param>
 /// <param name="p_disconnected"> Disconnected bool of the client </param>
void ConnectToServer(const PCWSTR& p_ip, int p_port, WSADATA& p_wsa, SOCKET& p_socket, sockaddr_in& p_server, bool& p_disconnected)
{
	if (WSAStartup(MAKEWORD(2, 2), &p_wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		std::cin.get();
		// environment exit? exit(-1);
	}

	if ((p_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
		std::cin.get();
		// environment exit? exit(-1);
	}

	InetPtonW(AF_INET, p_ip, &p_server.sin_addr.s_addr);
	p_server.sin_family = AF_INET;
	p_server.sin_port = htons(p_port);

	auto c = connect(p_socket, (struct sockaddr*)&p_server, sizeof(p_server));
	if (c < 0)
	{
		puts("connect error");
		printf("%d", c);
		std::cin.get();
		// environment exit? exit(-1);
	}

	p_disconnected = false;
}

//--------------------------------------------------------------------- ClientSocketAsync.h -------------------------------------------------------

/// <summary>
/// Constructor of ClientSocketAsync
/// </summary>
/// <param name="p_ip"> IP address of the server </param>
/// <param name="p_port"> The port of the server </param>
ClientSocketAsync::ClientSocketAsync(PCWSTR p_ip, int p_port)
{
	ConnectToServer(p_ip, p_port, m_wsa, m_socket, m_server, m_disconnected);
}

/// <summary>
/// Sends data over a socket to the server
/// </summary>
/// <param name="p_data"> Data that needs to be send </param>
/// <param name="p_size"> The size of the data </param>
void ClientSocketAsync::SendData(const char* p_data, const int p_size) const
{
	send(m_socket, p_data, p_size, 0);
}

/// <summary>
/// Disconnects the client from the server
/// </summary>
void ClientSocketAsync::Disconnect()
{
	closesocket(m_socket);
	WSACleanup();
	m_disconnected = true;
}

/// <summary>
/// Deconstructs the ClientSocketAsync class
/// </summary>
ClientSocketAsync::~ClientSocketAsync()
{
	if (m_disconnected) return;
	Disconnect();
}