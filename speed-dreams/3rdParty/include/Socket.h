/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once
#include "ipclib_portability.h"
#include "ipclib_export.h"
#include <thread>
#include <functional>

#define IPC_BUFFER_BYTE_SIZE 512
#define EMPTY_STATE          0b10000000

/// @brief A worker thread that can be commanded to receive data
class IPCLIB_EXPORT ReceivingThread
{
public:
    explicit ReceivingThread(const std::function<int()>& p_receiveDataFunc);

    bool HasReceivedMessage() const;

    bool HasError() const;

    bool StartedReceiving() const;

    int GetErrorCode();

    void StartReceive();

    void Stop();

    void Reset();

    ~ReceivingThread();

private:
    void ReceivingLoop();

    uint8_t m_state = EMPTY_STATE;

    int m_error = IPCLIB_SUCCEED;

    std::function<int()>* m_receiveDataFunc = nullptr;

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
    int ReceiveData();

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
    IPC_DATA_TYPE SocketData = {};
};
