#include "BlackBoxData.h"
#include "robot.h"

/// @brief Constructs a data type for holding data provided to the black box.
/// Makes deep copies of the provided data
/// @param p_car       The car data in the simulation
/// @param p_situation The environment data in the simulation
/// @param p_tickCount The tick that this driving simulation is from
/// @param p_nextSegments      Segment pointer to store the copied segments in
/// @param p_nextSegmentsCount Amount of segments to copy
BlackBoxData::BlackBoxData(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount, tTrackSeg* p_nextSegments, int p_nextSegmentsCount)
    : TickCount(p_tickCount)
{
    // Any pointers are marked with 'Pointer' so we can check if we even want them

    Car.index = p_car->index;

    // Copy p_car.info
#define p_info p_car->info
    Car.info = p_info;

    // Copy p_car.pub
#define p_pub p_car->pub
    Car.pub = p_pub;

    tTrackSeg* seg = p_pub.trkPos.seg;
    bool skip = !p_nextSegments || !seg;
    Car.pub.trkPos.seg = skip ? nullptr : p_nextSegments;
    if (!skip)
    {
        Car.pub.trkPos.seg[0] = *seg;
        seg = seg->next;
    }
    for (int i = 1; i < p_nextSegmentsCount; i++)
    {
        if (skip) break;
        // Does not make a deep copy of seg.name, seg.ext, seg.surface, seg.barrier[], seg.cam;
        // Deep copies of seg.next AND seg.prev would lead to circles: give it value in the loop, and assign the relevant pointers
        p_nextSegments[i] = *seg;
        p_nextSegments[i - 1].next = &p_nextSegments[i];
        p_nextSegments[i].prev = &p_nextSegments[i - 1];
        seg = seg->next;
    }

    // Copy p_car.race
#define p_race p_car->race
    Car.race = p_race;
    Car.race.bestSplitTime = p_race.bestSplitTime ? new double(*p_race.bestSplitTime) : nullptr;  // Pointer
    Car.race.curSplitTime = p_race.curSplitTime ? new double(*p_race.curSplitTime) : nullptr;     // Pointer
    Car.race.pit = p_race.pit ? new tTrackOwnPit(*p_race.pit) : nullptr;                          // Pointer
    if (p_race.pit)
    {
        Car.race.pit->pos.seg = nullptr;  // TODO (maybe)  // Pointer
        for (int i = 0; i < TR_PIT_MAXCARPERPIT; i++)
        {
            Car.race.pit->car[i] = nullptr;  // TODO (maybe) // Pointer
        }
    }
    Car.race.penaltyList.tqh_first = nullptr;  // TODO (maybe) // Pointer
    Car.race.penaltyList.tqh_last = nullptr;   // TODO (maybe) // Pointer

    // Copy p_car.priv
#define p_priv p_car->priv
    Car.priv = p_priv;
    for (int i = 0; i < 4; i++)
    {
        Car.priv.wheel[i].seg = nullptr;  // TODO (maybe)  // Pointer
    }
    // TODO memoryPool (maybe)
    Car.priv.memoryPool.newTrack = nullptr;
    Car.priv.memoryPool.newRace = nullptr;
    Car.priv.memoryPool.endRace = nullptr;
    Car.priv.memoryPool.shutdown = nullptr;
    // Pointer
    for (int i = 0; i < NR_DI_INSTANT; i++)
    {
        Car.priv.dashboardInstant[i].setup = p_priv.dashboardInstant[i].setup ? new tCarSetupItem(*p_priv.dashboardInstant[i].setup) : nullptr;  // Pointer
    }
    for (int i = 0; i < NR_DI_REQUEST; i++)
    {
        Car.priv.dashboardRequest[i].setup = p_priv.dashboardRequest[i].setup ? new tCarSetupItem(*p_priv.dashboardRequest[i].setup) : nullptr;  // Pointer
    }

    // Copy p_car.ctrl
#define p_ctrl p_car->ctrl
    Car.ctrl = p_ctrl;
    Car.ctrl.setupChangeCmd = nullptr;
    if (p_ctrl.setupChangeCmd)  // Pointer
    {
        Car.ctrl.setupChangeCmd = new tDashboardItem(*p_ctrl.setupChangeCmd);
        Car.ctrl.setupChangeCmd->setup = p_ctrl.setupChangeCmd->setup ? new tCarSetupItem(*p_ctrl.setupChangeCmd->setup) : nullptr;  // Pointer
    }

    // Copy p_car.setup
#define p_setup p_car->setup
    Car.setup = p_setup;

    // Copy p_car.pitcmd
#define p_pitcmd p_car->pitcmd
    Car.pitcmd = p_pitcmd;

    // Copy p_car.robot
#define p_robot p_car->robot
    Car.robot = p_robot ? new RobotItf(*p_robot) : nullptr;  // Pointer

    // Copy p_car.next
    Car.next = nullptr;  // TODO (maybe) // Pointer

    // Copy p_situation
    Situation = *p_situation;
    Situation.cars = nullptr;  // TODO (maybe) // Pointer
}

BlackBoxData::~BlackBoxData()
{
    // Some of these are copied, other are just nullptr, but deleting a nullptr is harmless
    delete Car.race.bestSplitTime;
    delete Car.race.curSplitTime;
    if (Car.race.pit)
    {
        delete Car.race.pit->pos.seg;  // TODO (only if copying this to begin with)
        for (int i = 0; i < TR_PIT_MAXCARPERPIT; i++)
        {
            delete Car.race.pit->car[i];  // TODO (only if copying these to begin with)
        }
    }
    delete Car.race.pit;
    // TODO penaltyList
    for (int i = 0; i < 4; i++)
    {
        delete Car.priv.wheel[i].seg;  // TODO (only if copying this to begin with)
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
    if (Car.ctrl.setupChangeCmd)
    {
        delete Car.ctrl.setupChangeCmd->setup;
    }
    delete Car.ctrl.setupChangeCmd;
    delete Car.robot;
    delete Car.next;        // TODO (only if copying this to begin with)
    delete Situation.cars;  // TODO (only if copying these to begin with)
}
