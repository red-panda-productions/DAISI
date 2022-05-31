#include "Mediator.h"
#include "Mediator.inl"

template <>
SMediator* SMediator::m_instance = nullptr;

CREATE_MEDIATOR_IMPLEMENTATION(SDecisionMaker)
