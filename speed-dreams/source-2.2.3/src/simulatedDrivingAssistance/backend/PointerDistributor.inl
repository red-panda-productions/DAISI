#pragma once
#include <fstream>

#include "PointerDistributor.h"
#include <ostream>


#define CREATE_POINTER_DISTRIBUTOR_INSTANCE(type) \
	template void PointerDistributor<type>::Distribute(std::string p_pointerName);

template <typename Object>
void PointerDistributor<Object>::Distribute(std::string p_pointerName)
{
	Object* pointer = &Obj;
	std::ofstream file(p_pointerName);
	file << pointer;
	file.close();
}