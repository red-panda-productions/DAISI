#pragma once
#pragma comment(lib,"ws2_32.lib")
#include "ipclib_export.h"
#include <WinSock2.h>
#define IPC_BUFFER_BYTE_SIZE 512

/// @brief A base class for ServerSocket and ClientSocketAsync
class IPCLIB_EXPORT Socket
{
public:
	void ReceiveDataAsync();

	void AwaitData(char* p_dataBuffer, int p_size);

	bool GetData(char* p_dataBuffer, int p_size);

private:
	void ReceiveData();

protected:
	int m_size = IPC_BUFFER_BYTE_SIZE;
	char m_dataBuffer[IPC_BUFFER_BYTE_SIZE] = {'\0'};

	bool m_receiving = false;
	bool m_received = false;
	bool m_disconnected = true;

	SOCKET m_socket = -1;
	WSAData m_wsa;
};