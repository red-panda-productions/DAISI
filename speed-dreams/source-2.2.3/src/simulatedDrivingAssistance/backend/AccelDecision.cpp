#include "AccelDecision.h"
#include "Mediator.h"

void AccelDecision::RunIndicateCommands()
{
    // TODO: Implement acceleration indication
}

void AccelDecision::RunInterveneCommands()
{
    SMediator::GetInstance()->CarController.SetAccelCmd(AccelAmount);
}