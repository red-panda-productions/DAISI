#pragma once
#include "Mediator.h"

template<typename Object>
class PointerDistributor
{
public:
	void Distribute(std::string p_pointerName);
	Object Obj;
};



#define SMediatorDistributor PointerDistributor<SMediator>
