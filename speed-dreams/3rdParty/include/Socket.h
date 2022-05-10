#pragma once
#pragma comment(lib, "ws2_32.lib")
#include "ipclib_export.h"
#include <WinSock2.h>
#include <iostream>
#include <thread>
#include <functional>

#define IPC_BUFFER_BYTE_SIZE 512

#define THROW_IPCLIB_ERROR(p_message) \
    std::stringstream oss;            \
    oss << p_message;                 \
    throw std::runtime_error(oss.str());

#define IPCLIB_ERROR(p_message, p_errorCode) \
    std::cerr << p_message << std::endl;     \
    return p_errorCode;

#define IPCLIB_WARNING(p_message) \
    std::cerr << p_message << std::endl;

#define WSA_ERROR                      -1
#define IPCLIB_SUCCEED                 0
#define IPCLIB_SERVER_ERROR            1
#define IPCLIB_RECEIVE_ERROR           2
#define IPCLIB_CLOSED_CONNECTION_ERROR 3

/// @brief A worker thread that can be commanded to receive data
class IPCLIB_EXPORT ReceivingThread
{
public:
    explicit ReceivingThread(const std::function<void()>& p_receiveDataFunc);

    bool HasReceivedMessage() const;

    int GetErrorCode() const;

    void StartReceive();

    void Stop();

    void Reset();

    ~ReceivingThread();

private:
    void ReceivingLoop();

    bool m_stop = false;
    bool m_receiving = false;
    bool m_received = false;

    int m_error = 0;

    std::function<void()>* m_receiveDataFunc = nullptr;

    std::thread* m_thread = nullptr;
};

/// @brief A base class for ServerSocket and ClientSocketAsync
class IPCLIB_EXPORT Socket
{
public:
    void ReceiveDataAsync();

    int AwaitData(char* p_dataBuffer, int p_size);

    bool GetData(char* p_dataBuffer, int p_size);

private:
    void ReceiveData();

    bool m_internalReceive = false;
    bool m_externalReceive = false;
    ReceivingThread* m_receivingThread = nullptr;

protected:
    void Initialize();

    void Stop();

    int Size = IPC_BUFFER_BYTE_SIZE;
    char DataBuffer[IPC_BUFFER_BYTE_SIZE] = {'\0'};
    bool Disconnected = true;

    SOCKET MSocket = -1;
    WSAData Wsa = {};
};
