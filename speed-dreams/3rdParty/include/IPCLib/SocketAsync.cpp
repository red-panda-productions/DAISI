#include "SocketAsync.h"
#include <thread>
#include <iostream>
#include <WS2tcpip.h>
#include <iostream>
#include <cstdio>
#include <thread>

/// <summary>
/// Receive data asynchronously by spawning a thread
/// </summary>
void SocketAsync::ReceiveDataAsync()
{
	std::thread t(&SocketAsync::ReceiveData, this);
	t.detach();
	m_receiving = true;
}

/// <summary>
/// Receive data by waiting until data has been written on the socket
/// </summary>
void SocketAsync::ReceiveData()
{
	m_size = recv(m_socket, m_dataBuffer, BUFFER_BYTE_SIZE, 0);
	if (m_size == SOCKET_ERROR)
	{
		printf("Failed to receive message");
		std::cin.get();
		// environment exit? exit(-1);
	}
	m_received = true;
}

/// <summary>
/// Awaits until data has been written to the socket
/// </summary>
/// <param name="p_dataBuffer"> The data buffer for storing the data </param>
/// <param name="p_size"> The size of the buffer </param>
void SocketAsync::AwaitData(char* p_dataBuffer, int& p_size)
{
	if(!m_receiving)
	{
		printf("ReceiveData was not called");
		std::cin.get();
		// environment exit? exit(-1);
	}
	while (!m_received) {}
	GetData(p_dataBuffer, p_size);
}

/// <summary>
 /// Gets the data from the socket, but can receive no data if no data has been written
 /// </summary>
 /// <param name="p_dataBuffer"> The data buffer for storing the data </param>
 /// <param name="p_size"> The size of the buffer </param>
 /// <returns> If it received data or not </return>
bool SocketAsync::GetData(char* p_dataBuffer, int& p_size)
{
	if (!m_received) return false;
	strcpy_s(p_dataBuffer, p_size, m_dataBuffer);
	if (m_size >= 0 && m_size < p_size) p_dataBuffer[m_size] = '\0';
	m_received = false;
	m_receiving = false;
	return true;
}