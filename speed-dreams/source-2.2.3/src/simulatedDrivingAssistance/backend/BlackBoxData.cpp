#include "BlackBoxData.h"

#include "robot.h"

/**
 * Constructs a data type for holding data provided to the black box
 * Makes deep copies of the provided data
 * @param p_car       The car data in the simulation
 * @param p_situation The environment data in the simulation
 * @param p_tickCount The tick that this driving simulation is from
 */
BlackBoxData::BlackBoxData(tCarElt* p_car, tSituation* p_situation, int p_tickCount) : TickCount(p_tickCount)
{
    Car.index = p_car->index;

    // Copy p_car.info
#define p_info p_car->info
    Car.info = p_info;

    // Copy p_car.pub
#define p_pub p_car->pub
    Car.pub = p_pub;
    Car.pub.trkPos.seg = nullptr; // TODO

    // Copy p_car.race
#define p_race p_car->race
    Car.race = p_race;
    Car.race.bestSplitTime = nullptr; // TODO
    Car.race.curSplitTime = nullptr; // TODO
    Car.race.pit = nullptr; // TODO
    // TODO penaltyList

    // Copy p_car.priv
#define p_priv p_car->priv
    Car.priv = p_priv;
    Car.priv.paramsHandle = nullptr; // TODO
    Car.priv.carHandle = nullptr; // TODO
    for (int i = 0; i < 4; i++)
    {
        Car.priv.wheel[i].seg = nullptr; // TODO
    }
    // TODO memoryPool
    for (int i = 0; i < NR_DI_INSTANT; i++)
    {
        Car.priv.dashboardInstant[i].setup = nullptr; // TODO
    }
    for (int i = 0; i < NR_DI_REQUEST; i++)
    {
        Car.priv.dashboardRequest[i].setup = nullptr; // TODO
    }

    // Copy p_car.ctrl
#define p_ctrl p_car->ctrl
    Car.ctrl = p_ctrl;
    Car.ctrl.setupChangeCmd = nullptr; // TODO

    // Copy p_car.setup
#define p_setup p_car->setup
    Car.setup = p_setup;

    // Copy p_car.pitcmd
#define p_pitcmd p_car->pitcmd
    Car.pitcmd = p_pitcmd;
    
    // Copy p_car.robot
    Car.robot = nullptr; // TODO

    // Copy p_car.next
    Car.next = nullptr; // TODO
    
    // Copy p_situation
    Situation = *p_situation;
    Situation.cars = nullptr;
}

BlackBoxData::~BlackBoxData()
{
    // None of these are assigned a value other than nullptr so far, but deleting a nullptr is harmless

    delete Car.pub.trkPos.seg;
    delete Car.race.bestSplitTime;
    delete Car.race.curSplitTime;
    delete Car.race.pit;
    // TODO penaltyList
    //delete Car.priv.paramsHandle; These are void pointers
    //delete Car.priv.carHandle;
    for (int i = 0; i < 4; i++)
    {
        delete Car.priv.wheel[i].seg;
    }
    // TODO memoryPool
    for (int i = 0; i < NR_DI_INSTANT; i++)
    {
        delete Car.priv.dashboardInstant[i].setup;
    }
    for (int i = 0; i < NR_DI_REQUEST; i++)
    {
        delete Car.priv.dashboardRequest[i].setup;
    }
    if (Car.ctrl.setupChangeCmd != nullptr)
    {
        delete Car.ctrl.setupChangeCmd->setup;
    }
    delete Car.ctrl.setupChangeCmd;
    delete Car.robot;
    delete Car.next;
    delete Situation.cars;
}
