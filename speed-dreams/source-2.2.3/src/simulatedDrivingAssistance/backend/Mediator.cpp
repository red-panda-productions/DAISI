/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#include "Mediator.h"
#include "Mediator.inl"

template <>
SMediator* SMediator::m_instance = nullptr;

CREATE_MEDIATOR_IMPLEMENTATION(SDecisionMaker)
