#pragma once
#include "ipclib_portability.h"
#include "ipclib_export.h"
#include <thread>
#include <functional>

#define IPC_BUFFER_BYTE_SIZE 512

/// @brief A worker thread that can be commanded to receive data
class IPCLIB_EXPORT ReceivingThread
{
public:
    explicit ReceivingThread(const std::function<void(bool*)>& p_receiveDataFunc);

    bool HasReceivedMessage() const;

    bool StartedReceiving() const;

    int GetErrorCode() const;

    void StartReceive();

    void Stop();

    void Reset();

    ~ReceivingThread();

private:
    void ReceivingLoop();

    bool m_stop = false;
    bool m_receiving = false;
    bool m_startedReceiving = false;
    bool m_received = false;

    int m_error = 0;

    std::function<void(bool*)>* m_receiveDataFunc = nullptr;

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
    void ReceiveData(bool* p_started = nullptr);

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
