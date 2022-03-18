#pragma once
#include "MediatorDistributor.h"
#include <thread>
#include <sstream>


#define CREATE_MEDIATOR_DISTRIBUTOR_INSTANCE(type) \
	template void MediatorDistributor<type>::Run();\
	template void MediatorDistributor<type>::Loop();

template<typename Mediator>
void MediatorDistributor<Mediator>::Run()
{
	std::thread t(&MediatorDistributor::Loop, this);
	t.detach();
}

template<typename Mediator>
void MediatorDistributor<Mediator>::Loop()
{
	Mediator* pointer = &m_mediator;

	std::stringstream ss;

	ss << pointer;

	const std::string name = ss.str();

	while (true)
	{
		m_server.ConnectAsync();
		m_server.AwaitClientConnection();
		m_server.SendData(name.c_str(), name.size());
		m_server.Disconnect();
	}

}
