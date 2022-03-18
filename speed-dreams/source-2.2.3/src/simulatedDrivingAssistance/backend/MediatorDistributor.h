#pragma once
#include "Mediator.h"
#include "ServerSocket.h"

template<typename Mediator>
class MediatorDistributor
{
public:
	void Run();

private:
	void Loop();

	Mediator m_mediator;
	ServerSocket m_server = ServerSocket(L"127.0.0.1",8889,2);
};

#define SMediatorDistributor MediatorDistributor<SMediator>