#include "AccelDecision.h"
#include "Mediator.h"

void AccelDecision::RunIndicateCommands()
{
    
}

void AccelDecision::RunInterveneCommands()
{
    SMediator::GetInstance()->CarController.SetAccelCmd(AccelAmount);
}