#pragma once
#include "Mediator.h"
#include "ServerSocket.h"

template<typename Object>
class PointerDistributor
{
public:
	void Run();

private:
	void Loop();

	Object m_object;
	ServerSocket m_server = ServerSocket(L"127.0.0.1",8889,2);
};

#define SMediatorDistributor PointerDistributor<SMediator>