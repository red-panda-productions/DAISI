#pragma once
#include "PointerDistributor.h"
#include <thread>
#include <sstream>


#define CREATE_POINTER_DISTRIBUTOR_INSTANCE(type) \
	template void PointerDistributor<type>::Run();\
	template void PointerDistributor<type>::Loop();

template<typename Object>
void PointerDistributor<Object>::Run()
{
	std::thread t(&PointerDistributor::Loop, this);
	t.detach();
}

template<typename Object>
void PointerDistributor<Object>::Loop()
{
	Object* pointer = &m_object;

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
