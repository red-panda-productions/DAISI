#include "BlackBoxData.h"

#include <stdexcept>

#include "robot.h"

/// @brief Constructs a data type for holding data provided to the black box.
/// Makes deep copies of the provided data
/// @param p_car       The car data in the simulation
/// @param p_situation The environment data in the simulation
/// @param p_tickCount The tick that this driving simulation is from
/// @param p_nextSegments      Segment pointer to store the copied segments in
/// @param p_nextSegmentsCount Amount of segments to copy
BlackBoxData::BlackBoxData(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount, tTrackSeg* p_nextSegments, int p_nextSegmentsCount)
    : Car({}), Situation({}), TickCount(p_tickCount)
{
    if (!p_car) throw std::invalid_argument("p_car cannot be null");
    if (!p_situation) throw std::invalid_argument("p_situation cannot be null");
    if (p_nextSegmentsCount < 0) throw std::invalid_argument("amount of segments cannot be less than 0");

    // Any pointers are marked with 'Pointer' so we can check if we even want them

    Car.index = p_car->index;

    // Copy p_car.info
    Car.info = p_car->info;

    // Copy p_car.pub
    Car.pub = p_car->pub;

    tTrackSeg* seg = p_car->pub.trkPos.seg;
    bool skip = !p_nextSegments || !seg || p_nextSegmentsCount == 0;
    Car.pub.trkPos.seg = skip ? nullptr : p_nextSegments;
    if (!skip)
    {
        Car.pub.trkPos.seg[0] = *seg;
        seg = seg->next;
    }
    // To make sure our indexing (for assigning next and prev) into p_nextSegments is always with a valid index, start from i = 1 so the lowest index is 0.
    // Copying values for index 0 is resolved in the above if statement
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
    Car.race = p_car->race;
    Car.race.bestSplitTime = p_car->race.bestSplitTime ? new double(*p_car->race.bestSplitTime) : nullptr;  // Pointer
    Car.race.curSplitTime = p_car->race.curSplitTime ? new double(*p_car->race.curSplitTime) : nullptr;     // Pointer
    Car.race.pit = p_car->race.pit ? new tTrackOwnPit(*p_car->race.pit) : nullptr;                          // Pointer
    if (p_car->race.pit)
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
    Car.priv = p_car->priv;
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
        Car.priv.dashboardInstant[i].setup = p_car->priv.dashboardInstant[i].setup ? new tCarSetupItem(*p_car->priv.dashboardInstant[i].setup) : nullptr;  // Pointer
    }
    for (int i = 0; i < NR_DI_REQUEST; i++)
    {
        Car.priv.dashboardRequest[i].setup = p_car->priv.dashboardRequest[i].setup ? new tCarSetupItem(*p_car->priv.dashboardRequest[i].setup) : nullptr;  // Pointer
    }

    // Copy p_car.ctrl
    Car.ctrl = p_car->ctrl;
    Car.ctrl.setupChangeCmd = nullptr;
    if (p_car->ctrl.setupChangeCmd)  // Pointer
    {
        Car.ctrl.setupChangeCmd = new tDashboardItem(*p_car->ctrl.setupChangeCmd);
        Car.ctrl.setupChangeCmd->setup = p_car->ctrl.setupChangeCmd->setup ? new tCarSetupItem(*p_car->ctrl.setupChangeCmd->setup) : nullptr;  // Pointer
    }

    // Copy p_car.setup
    Car.setup = p_car->setup;

    // Copy p_car.pitcmd
    Car.pitcmd = p_car->pitcmd;

    // Copy p_car.robot
    Car.robot = p_car->robot ? new RobotItf(*p_car->robot) : nullptr;  // Pointer

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
