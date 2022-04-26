#include <gtest/gtest.h>
#include "BlackBoxData.h"
#include "../rppUtils/Random.hpp"

#define RAND_NAME(a, b)                   \
    for (int i = 0; i < (b)-1; i++)       \
    {                                     \
        (a)[i] = (char)random.NextByte(); \
    }                                     \
    (a)[(b)-1] = '\0';

#define RAND_T3D(a)             \
    (a).x = random.NextFloat(); \
    (a).y = random.NextFloat(); \
    (a).z = random.NextFloat();

#define RAND_TPOSD(a)            \
    (a).x = random.NextFloat();  \
    (a).y = random.NextFloat();  \
    (a).z = random.NextFloat();  \
    (a).xy = random.NextFloat(); \
    (a).ax = random.NextFloat(); \
    (a).ay = random.NextFloat(); \
    (a).az = random.NextFloat();

#define RAND_TDYNPT(a)  \
    RAND_TPOSD((a).pos) \
    RAND_TPOSD((a).vel) \
    RAND_TPOSD((a).acc)

#define RAND_TCARSETUPITEM(a)               \
    (a).value = random.NextFloat();         \
    (a).min = random.NextFloat();           \
    (a).max = random.NextFloat();           \
    (a).desired_value = random.NextFloat(); \
    (a).stepsize = random.NextFloat();      \
    (a).changed = random.NextBool();

#define RAND_TDASHBOARDITEM(a)       \
    (a).type = random.NextInt();     \
    (a).setup = new tCarSetupItem(); \
    RAND_TCARSETUPITEM(*(a).setup)

#define COMP_ELEM(a, b, c)     \
    if ((c))                   \
    {                          \
        EXPECT_EQ((a), (b));   \
    }                          \
    else                       \
    {                          \
        EXPECT_NE(&(a), &(b)); \
    }

// Doesn't actually do a.trkPos.seg, that is left to whatever desires a trkPos to determine how they want that
#define RAND_TRKPOS(a)                 \
    (a).type = random.NextInt(3);      \
    (a).toStart = random.NextFloat();  \
    (a).toRight = random.NextFloat();  \
    (a).toMiddle = random.NextFloat(); \
    (a).toLeft = random.NextFloat();

// For structure elements where the copy is not implemented, no implementation is given for the generation either,
// nor are there any checks related to that element

struct TestSegments
{
    tTrackSeg* nextSegments;
    int nextSegmentsCount;
};

TestSegments GenerateSegments()
{
    Random random;
    TestSegments testSegments;
    testSegments.nextSegmentsCount = random.NextInt(4);
    testSegments.nextSegments = nullptr;  //new tTrackSeg[testSegments.nextSegmentsCount];
    return testSegments;
}

void DestroySegments(TestSegments& p_testSegments)
{
}

tCarElt Generatecar(TestSegments& p_testSegments)
{
    Random random;
    tCarElt car{};

    // Assign to car.info
    RAND_NAME(car.info.name, MAX_NAME_LEN)
    RAND_NAME(car.info.sname, MAX_NAME_LEN)
    RAND_NAME(car.info.codename, 4)
    RAND_NAME(car.info.teamname, MAX_NAME_LEN)
    RAND_NAME(car.info.carName, MAX_NAME_LEN)
    RAND_NAME(car.info.category, MAX_NAME_LEN)
    car.info.raceNumber = random.NextInt();
    car.info.startRank = random.NextInt();
    car.info.driverType = random.NextInt();
    car.info.networkplayer = random.NextInt();
    car.info.skillLevel = random.NextInt(4);
    for (int i = 0; i < 3; i++)
    {
        car.info.iconColor[i] = random.NextFloat();
    }
    RAND_T3D(car.info.dimension)
    RAND_T3D(car.info.drvPos)
    RAND_T3D(car.info.bonnetPos)
    car.info.tank = random.NextFloat();
    car.info.steerLock = random.NextFloat();
    RAND_T3D(car.info.statGC)
    for (int i = 0; i < 4; i++)
    {
        car.info.wheel[i].rimRadius = random.NextFloat();
        car.info.wheel[i].tireHeight = random.NextFloat();
        car.info.wheel[i].tireWidth = random.NextFloat();
        car.info.wheel[i].brakeDiskRadius = random.NextFloat();
        car.info.wheel[i].wheelRadius = random.NextFloat();
    }
    car.info.visualAttr.exhaustNb = random.NextInt();
    for (int j = 0; j < 1; j++)
    {
        RAND_T3D(car.info.visualAttr.exhaustPos[j])
    }
    car.info.visualAttr.exhaustPower = random.NextFloat();
    RAND_NAME(car.info.masterModel, MAX_NAME_LEN)
    RAND_NAME(car.info.skinName, MAX_NAME_LEN)
    int possibleSkinTargets[] = {RM_CAR_SKIN_TARGET_WHOLE_LIVERY, RM_CAR_SKIN_TARGET_3D_WHEELS, RM_CAR_SKIN_TARGET_INTERIOR,
                                 RM_CAR_SKIN_TARGET_BOARD, RM_CAR_SKIN_TARGET_DRIVER, RM_CAR_SKIN_TARGET_PIT_DOOR};
    car.info.skinTargets = 0;
    for (int i = 0; i < 6; i++)
    {
        car.info.skinTargets |= possibleSkinTargets[i] * random.NextBool();
    }

    // Assign to car.pub
    RAND_TDYNPT(car.pub.DynGC)
    RAND_TDYNPT(car.pub.DynGCg)
    car.pub.speed = random.NextFloat();
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            car.pub.posMat[i][j] = random.NextFloat();
        }
    }
    // TODO car.pub.trkPos.seg
    RAND_TRKPOS(car.pub.trkPos)
    int possibleStates[] = {RM_CAR_STATE_FINISH, RM_CAR_STATE_FINISH, RM_CAR_STATE_DNF, RM_CAR_STATE_PULLUP, RM_CAR_STATE_PULLSIDE, RM_CAR_STATE_PULLDN,
                            RM_CAR_STATE_OUT, RM_CAR_STATE_NO_SIMU, RM_CAR_STATE_BROKEN, RM_CAR_STATE_OUTOFGAS, RM_CAR_STATE_ELIMINATED,
                            RM_CAR_STATE_ENDRACE_CALLED, RM_CAR_STATE_SIMU_NO_MOVE};
    car.pub.state = possibleStates[random.NextInt(13)];
    for (int i = 0; i < 4; i++)
    {
        RAND_TPOSD(car.pub.corner[i])
    }
    car.pub.glance = random.NextFloat();
    car.pub.oldglance = random.NextFloat();

    // Assign values to car.race
    car.race.bestLapTime = random.NextFloat();
    car.race.commitBestLapTime = random.NextBool();
    car.race.bestSplitTime = new double(random.NextFloat());
    car.race.deltaBestLapTime = random.NextFloat();
    car.race.curLapTime = random.NextFloat();
    car.race.curSplitTime = new double(random.NextFloat());
    car.race.lastLapTime = random.NextFloat();
    car.race.curTime = random.NextFloat();
    car.race.topSpeed = random.NextFloat();
    car.race.currentMinSpeedForLap = random.NextFloat();
    car.race.laps = random.NextInt();
    car.race.bestLap = random.NextInt();
    car.race.nbPitStops = random.NextInt();
    car.race.remainingLaps = random.NextInt();
    car.race.pos = random.NextInt();
    car.race.timeBehindLeader = random.NextFloat();
    car.race.lapsBehindLeader = random.NextInt();
    car.race.timeBehindPrev = random.NextFloat();
    car.race.timeBeforeNext = random.NextFloat();
    car.race.distRaced = random.NextFloat();
    car.race.distFromStartLine = random.NextFloat();
    car.race.currentSector = random.NextInt();
    car.race.nbSectors = random.NextInt(car.race.currentSector + 1);
    car.race.trackLength = random.NextFloat();
    car.race.scheduledEventTime = random.NextFloat();
    car.race.pit = new tTrackOwnPit();
    car.race.pit->pos.seg = nullptr;  // COPY NOT IMPLEMENTED
    RAND_TRKPOS(car.race.pit->pos)
    car.race.pit->pitCarIndex = random.NextInt(-1, 4);
    car.race.pit->lmin = random.NextFloat();
    car.race.pit->lmax = random.NextFloat();
    for (int i = 0; i < TR_PIT_MAXCARPERPIT; i++)
    {
        car.race.pit->car[i] = nullptr;  // COPY NOT IMPLEMENTED
    }
    car.race.pit->freeCarIndex = car.race.pit->pitCarIndex + 1;
    car.race.event = random.NextInt();
    car.race.penaltyList.tqh_first = nullptr;  // COPY NOT IMPLEMENTED
    car.race.penaltyList.tqh_last = nullptr;   // COPY NOT IMPLEMENTED
    car.race.penaltyTime = random.NextFloat();
    car.race.prevFromStartLine = random.NextFloat();
    car.race.wrongWayTime = random.NextFloat();

    // Assign values to car.priv
    car.priv.paramsHandle = nullptr;  // COPY IMPLEMENTED, NO IDEA ON WHAT CONTENTS WOULD BE FOR GENERATION
    car.priv.carHandle = nullptr;     // AS THESE ARE VOID POINTERS
    car.priv.driverIndex = random.NextInt();
    car.priv.moduleIndex = random.NextInt();
    RAND_NAME(car.priv.modName, MAX_NAME_LEN)
    for (int i = 0; i < 4; i++)
    {
        RAND_TPOSD(car.priv.wheel[i].relPos)
        car.priv.wheel[i].spinVel = random.NextFloat();
        car.priv.wheel[i].brakeTemp = random.NextFloat();
        car.priv.wheel[i].state = random.NextInt();
        car.priv.wheel[i].seg = nullptr;  // COPY NOT IMPLEMENTED
        car.priv.wheel[i].rollRes = random.NextFloat();
        car.priv.wheel[i].temp_in = random.NextFloat();
        car.priv.wheel[i].temp_mid = random.NextFloat();
        car.priv.wheel[i].temp_out = random.NextFloat();
        car.priv.wheel[i].temp_opt = random.NextFloat();
        car.priv.wheel[i].condition = random.NextFloat();
        car.priv.wheel[i].treadDepth = random.NextFloat();
        car.priv.wheel[i].critTreadDepth = random.NextFloat();
        car.priv.wheel[i].slipNorm = random.NextFloat();
        car.priv.wheel[i].slipOpt = random.NextFloat();
        car.priv.wheel[i].slipSide = random.NextFloat();
        car.priv.wheel[i].slipAccel = random.NextFloat();
        car.priv.wheel[i].Fx = random.NextFloat();
        car.priv.wheel[i].Fy = random.NextFloat();
        car.priv.wheel[i].Fz = random.NextFloat();
        car.priv.wheel[i].effectiveMu = random.NextFloat();
    }
    for (int i = 0; i < 4; i++)
    {
        RAND_TPOSD(car.priv.corner[i])
    }
    car.priv.gear = random.NextInt(MAX_GEARS);
    car.priv.gearNext = car.priv.gear + 1;
    car.priv.fuel = random.NextFloat();
    car.priv.fuel_consumption_total = random.NextFloat();
    car.priv.fuel_consumption_instant = random.NextFloat();
    car.priv.enginerpm = random.NextFloat();
    car.priv.enginerpmRedLine = random.NextFloat();
    car.priv.enginerpmMax = random.NextFloat();
    car.priv.enginerpmMaxTq = random.NextFloat();
    car.priv.enginerpmMaxPw = random.NextFloat();
    car.priv.engineMaxTq = random.NextFloat();
    car.priv.engineMaxPw = random.NextFloat();
    for (int i = 0; i < MAX_GEARS; i++)
    {
        car.priv.gearRatio[i] = random.NextFloat();
    }
    car.priv.gearNb = random.NextInt();
    car.priv.gearOffset = random.NextInt();
    for (int i = 0; i < 4; i++)
    {
        car.priv.skid[i] = random.NextFloat();
    }
    for (int i = 0; i < 4; i++)
    {
        car.priv.reaction[i] = random.NextFloat();
    }
    car.priv.collision = random.NextInt();
    car.priv.simcollision = random.NextInt();
    car.priv.smoke = random.NextFloat();
    RAND_T3D(car.priv.normal)
    RAND_T3D(car.priv.collpos)
    car.priv.dammage = random.NextInt();
    car.priv.debug = random.NextInt();
    car.priv.collision_state.collision_count = random.NextInt();
    for (int i = 0; i < 3; i++)
    {
        car.priv.collision_state.pos[i] = random.NextFloat();
        car.priv.collision_state.force[i] = random.NextFloat();
    }
    car.priv.localPressure = random.NextFloat();
    car.priv.memoryPool.newTrack = nullptr;  // COPY NOT IMPLEMENTED
    car.priv.memoryPool.newRace = nullptr;   // COPY NOT IMPLEMENTED
    car.priv.memoryPool.endRace = nullptr;   // COPY NOT IMPLEMENTED
    car.priv.memoryPool.shutdown = nullptr;  // COPY NOT IMPLEMENTED
    car.priv.driveSkill = random.NextFloat(10);
    car.priv.steerTqCenter = random.NextFloat();
    car.priv.steerTqAlign = random.NextFloat();
    for (int i = 0; i < NR_DI_INSTANT; i++)
    {
        RAND_TDASHBOARDITEM(car.priv.dashboardInstant[i])
    }
    car.priv.dashboardInstantNb = random.NextInt();
    for (int i = 0; i < NR_DI_REQUEST; i++)
    {
        RAND_TDASHBOARDITEM(car.priv.dashboardRequest[i])
    }
    car.priv.dashboardRequestNb = random.NextInt();
    car.priv.dashboardActiveItem = random.NextInt();

    // Assign values to car.ctrl

    return car;
}

void DestroyCar(tCarElt& p_car)
{
    delete p_car.race.bestSplitTime;
    delete p_car.race.curSplitTime;
    for (int i = 0; i < TR_PIT_MAXCARPERPIT; i++)
    {
        delete p_car.race.pit->car[i];  // COPY NOT IMPLEMENTED
    }
    delete p_car.race.pit->pos.seg;  // COPY NOT IMPLEMENTED
    delete p_car.race.pit;
    delete p_car.race.penaltyList.tqh_first;  // COPY NOT IMPLEMENTED
    delete p_car.race.penaltyList.tqh_last;   // COPY NOT IMPLEMENTED
}

tSituation GenerateSituation()
{
    Random random;
    tSituation situation{};
    situation.raceInfo.ncars = random.NextInt();
    situation.raceInfo.totLaps = random.NextInt();
    situation.raceInfo.extraLaps = random.NextInt();
    situation.raceInfo.totTime = random.NextFloat();
    int possibleRaceInfoStates[] = {RM_RACE_RUNNING, RM_RACE_FINISHING, RM_RACE_ENDED, RM_RACE_STARTING, RM_RACE_PRESTART, RM_RACE_PAUSED};
    situation.raceInfo.state = possibleRaceInfoStates[random.NextInt(6)];
    situation.raceInfo.type = random.NextInt(3);
    situation.raceInfo.maxDammage = random.NextInt();
    situation.raceInfo.fps = random.NextUInt();
    situation.raceInfo.features = random.NextInt(16);
    situation.deltaTime = random.NextFloat();
    situation.currentTime = random.NextFloat();
    situation.accelTime = random.NextFloat();
    situation.nbPlayers = random.NextInt();
    situation.cars = nullptr;  // COPY NOT IMPLEMENTED
    return situation;
}

void DestroySituation(tSituation& p_situation)
{
    // Empty body as the only ptr is not implemented
}

class BlackBoxDataTestFixture : public ::testing::Test, public testing::WithParamInterface<bool>
{
protected:
    void SetUp() override
    {
        Random random;
        testSegments = GenerateSegments();
        car = Generatecar(testSegments);
        situation = GenerateSituation();
        tickCount = random.NextInt();
    }

    TestSegments testSegments;
    tCarElt car;
    tSituation situation;
    unsigned long tickCount;

    void TearDown() override
    {
        DestroySituation(situation);
        DestroyCar(car);
        DestroySegments(testSegments);
    }
};

/// @brief          Tests whether elements are the correct value or pointer
/// @param p_eqOrNe true: test for equality of values, false: test for inequality of pointers to the values
TEST_P(BlackBoxDataTestFixture, ElementCompareTests)
{
    bool p_eqOrNe = GetParam();
    BlackBoxData data(&car, &situation, tickCount, testSegments.nextSegments, testSegments.nextSegmentsCount);
    COMP_ELEM(car.index, data.Car.index, p_eqOrNe)
}

INSTANTIATE_TEST_SUITE_P(BlackBoxDataTests, BlackBoxDataTestFixture, ::testing::Values(true, false));