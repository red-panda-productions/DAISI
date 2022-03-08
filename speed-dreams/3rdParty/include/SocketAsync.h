#pragma once
#pragma comment(lib,"ws2_32.lib")
#include "ipclib_export.h"
#include <WinSock2.h>

#define BUFFER_BYTE_SIZE 512

/// <summary>
/// A base class for ServerSocketAsync and ClientSocketAsync
/// </summary>
class IPCLIB_EXPORT SocketAsync
{
public:
	void ReceiveDataAsync();

	void AwaitData(char* p_dataBuffer, int& p_size);

	bool GetData(char* p_dataBuffer, int& p_size);

private:
	void ReceiveData();

protected:
	int m_size = BUFFER_BYTE_SIZE;
	char m_dataBuffer[BUFFER_BYTE_SIZE] = {'\0'};

	bool m_receiving = false;
	bool m_received = false;
	bool m_disconnected = true;

	SOCKET m_socket = -1;
	WSAData m_wsa;
};