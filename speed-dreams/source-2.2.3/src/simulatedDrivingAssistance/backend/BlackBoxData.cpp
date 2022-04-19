#include "BlackBoxData.h"

#include "robot.h"

/// @brief   Copies a track segment
/// @param   The track segment to copy
/// @returns A pointer to the copied track segment
static tTrackSeg* CopySegment(tTrackSeg* p_seg)
{
    if (!p_seg) { return nullptr; }

    tTrackSeg* copy = new tTrackSeg(*p_seg);
    if (p_seg->name)
    {
        int trkPosSegNameLength = strlen(p_seg->name) + 1;
        copy->name = new char[trkPosSegNameLength]();
        strcpy_s(copy->name, trkPosSegNameLength, p_seg->name);
    }
    copy->ext = nullptr;
    if (p_seg->ext)
    {
        copy->ext = new tSegExt(*p_seg->ext);
        if ((*p_seg->ext).marks)
        {
            int trkPosSegExtNbMarks = (*p_seg->ext).nbMarks;
            copy->ext->marks = new int[trkPosSegExtNbMarks]();
            for (int i = 0; i < trkPosSegExtNbMarks; i++)
            {
                copy->ext->marks[i] = p_seg->ext->marks[i];
            }
        }
    }


    return copy;

    // Sietze's copy segments:
    /* bool skip = false;
    tTrackSeg* seg = p_pub.trkPos.seg;
    Car.pub.trkPos.seg = p_nextSegments;
    if (p_nextSegmentsCount == 0 || p_nextSegments == nullptr || seg == nullptr)
    {
        Car.pub.trkPos.seg = nullptr;
        skip = true;
    }
    for (int i = 0; i < p_nextSegmentsCount; i++)
    {
        if (skip) break;
        p_nextSegments[i] = *seg;
        p_nextSegments[i].next = &p_nextSegments[i + 1];

        for (int j = 0; j < 4; j++)
        {
            p_nextSegments[i].vertex[j] = seg->vertex[j];
        }

        for (int j = 0; j < 7; j++)
        {
            p_nextSegments[i].angle[j] = seg->angle[j];
        }
        seg = seg->next;
    }*/
}

/// @brief Deletes a track segment
/// @param The track segment to delete
static void DeleteSegment(tTrackSeg* p_seg)
{
    if (p_seg)
    {
        delete[] p_seg->name;
        if (p_seg->ext)
        {
            delete[] p_seg->ext->marks;
        }
        delete p_seg->ext;
        delete p_seg->surface;
        for (int i = 0; i < 2; i++)
        {
            delete p_seg->barrier[i];
        }
        delete p_seg->cam;
        delete p_seg->next;
        delete p_seg->prev;
    }
    delete p_seg;
}


/// @brief Constructs a data type for holding data provided to the black box.
/// Makes deep copies of the provided data
/// @param p_car       The car data in the simulation
/// @param p_situation The environment data in the simulation
/// @param p_tickCount The tick that this driving simulation is from
/// @param p_nextSegments
/// @param p_nextSegmentsCount
BlackBoxData::BlackBoxData(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount, tTrackSeg* p_nextSegments, int p_nextSegmentsCount) : TickCount(p_tickCount)
{
    // Any pointers are marked with 'Pointer' so we can check if we even want them

    Car.index = p_car->index;

    // Copy p_car.info
#define p_info p_car->info
    Car.info = p_info;

    // Copy p_car.pub
#define p_pub p_car->pub
    Car.pub = p_pub;
    Car.pub.trkPos.seg = CopySegment(p_pub.trkPos.seg); // Pointer

    // Copy p_car.race
#define p_race p_car->race
    Car.race = p_race;  
    Car.race.bestSplitTime = p_race.bestSplitTime ? new double(*p_race.bestSplitTime) : nullptr; // Pointer
    Car.race.curSplitTime = p_race.curSplitTime ? new double(*p_race.curSplitTime) : nullptr; // Pointer
    Car.race.pit = p_race.pit ? new tTrackOwnPit(*p_race.pit) : nullptr; // Pointer
    if (p_race.pit)
    {
        Car.race.pit->pos.seg = CopySegment(p_race.pit->pos.seg); // Pointer
        for (int i = 0; i < TR_PIT_MAXCARPERPIT; i++)
        {
            Car.race.pit->car[i] = nullptr; // TODO (maybe) // Pointer
        }
    }
    // TODO penaltyList

    // Copy p_car.priv
#define p_priv p_car->priv
    Car.priv = p_priv;
    for (int i = 0; i < 4; i++)
    {
        Car.priv.wheel[i].seg = CopySegment(p_priv.wheel[i].seg); // Pointer
    }
    // TODO memoryPool
    for (int i = 0; i < NR_DI_INSTANT; i++)
    {
        Car.priv.dashboardInstant[i].setup = p_priv.dashboardInstant[i].setup ? new tCarSetupItem(*p_priv.dashboardInstant[i].setup) : nullptr; // Pointer
    }
    for (int i = 0; i < NR_DI_REQUEST; i++)
    {
        Car.priv.dashboardRequest[i].setup = p_priv.dashboardRequest[i].setup ? new tCarSetupItem(*p_priv.dashboardRequest[i].setup) : nullptr; // Pointer
    }

    // Copy p_car.ctrl
#define p_ctrl p_car->ctrl
    Car.ctrl = p_ctrl;
    Car.ctrl.setupChangeCmd = nullptr;
    if (p_ctrl.setupChangeCmd) // Pointer
    {
        Car.ctrl.setupChangeCmd = new tDashboardItem(*p_ctrl.setupChangeCmd);
        Car.ctrl.setupChangeCmd->setup = p_ctrl.setupChangeCmd->setup ? new tCarSetupItem(*p_ctrl.setupChangeCmd->setup) : nullptr; // Pointer
    }

    // Copy p_car.setup
#define p_setup p_car->setup
    Car.setup = p_setup;

    // Copy p_car.pitcmd
#define p_pitcmd p_car->pitcmd
    Car.pitcmd = p_pitcmd;
    
    // Copy p_car.robot
#define p_robot p_car->robot
    Car.robot = p_robot ? new RobotItf(*p_robot) : nullptr; // Pointer

    // Copy p_car.next
    Car.next = nullptr; // TODO (maybe) // Pointer
    
    // Copy p_situation
    Situation = *p_situation;
    Situation.cars = nullptr; // TODO (maybe) // Pointer
}

BlackBoxData::~BlackBoxData()
{
    // None of these are assigned a value other than nullptr so far, but deleting a nullptr is harmless
    DeleteSegment(Car.pub.trkPos.seg);
    delete Car.race.bestSplitTime;
    delete Car.race.curSplitTime;
    if (Car.race.pit)
    {
        DeleteSegment(Car.race.pit->pos.seg);
        for (int i = 0; i < TR_PIT_MAXCARPERPIT; i++)
        {
            delete Car.race.pit->car[i]; // TODO (only if copying these to begin with)
        }
    }
    delete Car.race.pit;
    // TODO penaltyList
    for (int i = 0; i < 4; i++)
    {
        DeleteSegment(Car.priv.wheel[i].seg);
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
    delete Car.next; // TODO (only if copying this to begin with)
    delete Situation.cars; // TODO (only if copying these to begin with)
}
