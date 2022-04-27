#include <gtest/gtest.h>
#include "BlackBoxData.h"
#include "robot.h"
#include "../rppUtils/Random.hpp"

/// @brief   Generates a random name; guarantees the last character is '\0', although one could pop up earlier
/// @param a The array to generate the name in
/// @param b The length of the array
#define RAND_NAME(a, b)                   \
    for (int i = 0; i < (b)-1; i++)       \
    {                                     \
        (a)[i] = (char)random.NextByte(); \
    }                                     \
    (a)[(b)-1] = '\0';

/// @brief   Generates a random t3D
/// @param a The identifier of a t3D to randomize
#define RAND_T3D(a)             \
    (a).x = random.NextFloat(); \
    (a).y = random.NextFloat(); \
    (a).z = random.NextFloat();

/// @brief   Generates a random tPosd
/// @param a The identifier of a tPosd to randomize
#define RAND_TPOSD(a)            \
    (a).x = random.NextFloat();  \
    (a).y = random.NextFloat();  \
    (a).z = random.NextFloat();  \
    (a).xy = random.NextFloat(); \
    (a).ax = random.NextFloat(); \
    (a).ay = random.NextFloat(); \
    (a).az = random.NextFloat();

/// @brief   Generates a random tDynPt
/// @param a The identifier of a tDynPt to randomize
#define RAND_TDYNPT(a)  \
    RAND_TPOSD((a).pos) \
    RAND_TPOSD((a).vel) \
    RAND_TPOSD((a).acc)

/// @brief   Generates a random tCarSetupItem
/// @param a The identifier of a tCarSetupItem to randomize
#define RAND_TCARSETUPITEM(a)               \
    (a).value = random.NextFloat();         \
    (a).min = random.NextFloat();           \
    (a).max = random.NextFloat();           \
    (a).desired_value = random.NextFloat(); \
    (a).stepsize = random.NextFloat();      \
    (a).changed = random.NextBool();

/// @brief   Generates a random tDashboardItem
/// @param a The identifier of a tDashboardItem to randomize
#define RAND_TDASHBOARDITEM(a)       \
    (a).type = random.NextInt();     \
    (a).setup = new tCarSetupItem(); \
    RAND_TCARSETUPITEM(*(a).setup)

/// @brief   Generates a random tTrkLocPos
/// @param a The identifier of a tTrkLocPos to randomize
/// @note    Doesn't actually do a.seg, that is left to whatever desires a tTrkLocPos to determine how they want that
/// as exclusively the copy for p_car.pub.seg is implemented
#define RAND_TRKPOS(a)                 \
    (a).type = random.NextInt(3);      \
    (a).toStart = random.NextFloat();  \
    (a).toRight = random.NextFloat();  \
    (a).toMiddle = random.NextFloat(); \
    (a).toLeft = random.NextFloat();

// Sadly, structs don't have an (==) operator by default. We're either typing those somewhere else and using them here,
// or just comparing them here element-wise. Either way we're going to create a long comparison

/// @brief          Performs a test on two elements that are elementary
/// @param a        Identifier of the first element
/// @param b        Identifier of the second element
/// @param p_eqOrNe A bool defined in the environment this define in is called that determines
/// whether to compare the values of the identifiers or the pointers of the identifiers
/// @note           These are EXPECTS instead of ASSERTS as there are pointers created during a test and these need to be deleted
#define COMP_ELEM(a, b)        \
    if ((p_eqOrNe))            \
    {                          \
        EXPECT_EQ((a), (b));   \
    }                          \
    else                       \
    {                          \
        EXPECT_NE(&(a), &(b)); \
    }

/// @brief          Performs a test on two char*
/// @param a        Identifier of the first char*
/// @param b        Identifier of the second char*
/// @param p_eqOrNe A bool defined in the environment this define in is called that determines
/// whether to string compare the values of the identifiers or to compare pointers of the identifiers
/// @note These are EXPECTS instead of ASSERTS as there are pointers created during a test and these need to be deleted
#define COMP_NAME(a, b)                 \
    if ((p_eqOrNe))                     \
    {                                   \
        EXPECT_EQ(strcmp((a), (b)), 0); \
    }                                   \
    else                                \
    {                                   \
        EXPECT_NE(&(a), &(b));          \
    }

/// @brief   Performs a test on two t3D elements
/// @param a Identifier of the first element
/// @param b Identifier of the second element
#define COMP_T3D(a, b)      \
    COMP_ELEM((a).x, (b).x) \
    COMP_ELEM((a).y, (b).y) \
    COMP_ELEM((a).z, (b).z)

/// @brief   Performs a test on two tPosd elements
/// @param a Identifier of the first element
/// @param b Identifier of the second element
#define COMP_TPOSD(a, b)      \
    COMP_ELEM((a).x, (b).x)   \
    COMP_ELEM((a).y, (b).y)   \
    COMP_ELEM((a).z, (b).z)   \
    COMP_ELEM((a).xy, (b).xy) \
    COMP_ELEM((a).ax, (b).ax) \
    COMP_ELEM((a).ay, (b).ay) \
    COMP_ELEM((a).az, (b).az)

/// @brief   Performs a test on two tDynPt elements
/// @param a Identifier of the first element
/// @param b Identifier of the second element
#define COMP_TDYNPT(a, b)        \
    COMP_TPOSD((a).pos, (b).pos) \
    COMP_TPOSD((a).vel, (b).vel) \
    COMP_TPOSD((a).acc, (b).acc)

/// @brief   Performs a test on two tCarSetupItem element
/// @param a Identifier of the first element
/// @param b Identifier of the second element
#define COMP_TCARSETUPITEM(a, b)                    \
    COMP_ELEM((a).value, (b).value)                 \
    COMP_ELEM((a).min, (b).min)                     \
    COMP_ELEM((a).max, (b).max)                     \
    COMP_ELEM((a).desired_value, (b).desired_value) \
    COMP_ELEM((a).stepsize, (b).stepsize)           \
    COMP_ELEM((a).changed, (b).changed)

/// @brief   Performs a test on two tTrkLocPos element
/// @param a Identifier of the first element
/// @param b Identifier of the second element
/// @note    Much like RAND_TRKPOS, this ignores a.seg and b.seg
#define COMP_TRKPOS(a, b)                 \
    COMP_ELEM((a).type, (b).type)         \
    COMP_ELEM((a).toStart, (b).toStart)   \
    COMP_ELEM((a).toRight, (b).toRight)   \
    COMP_ELEM((a).toMiddle, (b).toMiddle) \
    COMP_ELEM((a).toLeft, (b).toLeft)

// For structure elements where the copy is not implemented, no implementation is given for the generation either,
// nor are there any checks related to that element

struct TestSegments
{
    tTrackSeg* NextSegments;
    int NextSegmentsCount;
};

/// @brief Generates a random segment
/// @returns A tTrackSeg
tTrackSeg GenerateSegment()
{
    Random random;
    tTrackSeg segment = {};
    segment.name = new char[MAX_NAME_LEN];
    RAND_NAME(segment.name, MAX_NAME_LEN)
    segment.id = random.NextInt();
    segment.type = random.NextInt(1, 4);
    segment.type2 = random.NextInt(1, 6);
    segment.style = random.NextInt(5);
    segment.length = random.NextFloat();
    segment.width = random.NextFloat();
    segment.startWidth = random.NextFloat();
    segment.endWidth = random.NextFloat();
    segment.lgfromstart = random.NextFloat();
    segment.radius = random.NextFloat();
    segment.radiusr = random.NextFloat();
    segment.radiusl = random.NextFloat();
    segment.arc = random.NextFloat();
    RAND_T3D(segment.center)
    for (int i = 0; i < 4; i++)
    {
        RAND_T3D(segment.vertex[i])
    }
    for (int i = 0; i < 7; i++)
    {
        segment.angle[i] = random.NextFloat();
    }
    segment.sin = random.NextFloat(-1, 1);
    segment.cos = random.NextFloat(-1, 1);
    segment.Kzl = random.NextFloat();
    segment.Kzw = random.NextFloat();
    segment.Kyl = random.NextFloat();
    RAND_T3D(segment.rgtSideNormal)
    segment.envIndex = random.NextInt();
    segment.height = random.NextFloat();
    segment.raceInfo = random.NextInt(1024);
    segment.DoVfactor = random.NextFloat();
    segment.ext = nullptr;      // COPY NOT IMPLEMENTED
    segment.surface = nullptr;  // COPY NOT IMPLEMENTED
    for (int i = 0; i < 2; i++)
    {
        segment.barrier[i] = nullptr;  // COPY NOT IMPLEMENTED
    }
    segment.cam = nullptr;  // COPY NOT IMPLEMENTED
    segment.next = nullptr;
    segment.prev = nullptr;

    return segment;
}

/// @brief Generates a series of random segments
/// @returns A TestSegments containing an pointer of segments, as well as the length of the array following the pointer
TestSegments GenerateSegments()
{
    Random random;
    TestSegments testSegments = {};
#define MAX_TESTSEGMENT_COUNT 16
    testSegments.NextSegmentsCount = random.NextInt(1, MAX_TESTSEGMENT_COUNT);
    testSegments.NextSegments = new tTrackSeg[testSegments.NextSegmentsCount];
    testSegments.NextSegments[0] = GenerateSegment();
    for (int i = 1; i < testSegments.NextSegmentsCount; i++)
    {
        testSegments.NextSegments[i] = GenerateSegment();
        testSegments.NextSegments[i - 1].next = &testSegments.NextSegments[i];
        testSegments.NextSegments[i].prev = &testSegments.NextSegments[0];
    }
    return testSegments;
}

/// @brief Deletes the generated pointers in the given set of segments
/// @param p_testSegments The TestSegments to destroy
void DestroySegments(TestSegments& p_testSegments)
{
    for (int i = 0; i < p_testSegments.NextSegmentsCount; i++)
    {
        // With the current copy implementation, the copies of the segments are written to the same location as they are copied from
        delete[] p_testSegments.NextSegments[i].name;  // DEEP COPY NOT IMPLEMENTED; IF IT WERE, DUE TO THE ABOVE, IT WOULD LEAK MEMORY
        delete p_testSegments.NextSegments[i].ext;     // COPY NOT IMPLEMENTED
        for (int j = 0; j < 2; j++)
        {
            delete p_testSegments.NextSegments[i].barrier[j];  // COPY NOT IMPLEMENTED
        }
        delete p_testSegments.NextSegments[i].cam;  // COPY NOT IMPLEMENTED
    }
}

/// @brief Generates a random car
/// @param p_testSegments A set of earlier obtained segments to put in retval.car.pub.trkPos.seg
/// @returns A tCarElt
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
    car.pub.trkPos.seg = p_testSegments.NextSegments;
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
    car.ctrl.steer = random.NextFloat();
    car.ctrl.accelCmd = random.NextFloat();
    car.ctrl.brakeCmd = random.NextFloat();
    car.ctrl.clutchCmd = random.NextFloat();
    car.ctrl.brakeFrontLeftCmd = random.NextFloat();
    car.ctrl.brakeFrontRightCmd = random.NextFloat();
    car.ctrl.brakeRearLeftCmd = random.NextFloat();
    car.ctrl.brakeRearRightCmd = random.NextFloat();
    car.ctrl.wingFrontCmd = random.NextFloat();
    car.ctrl.wingRearCmd = random.NextFloat();
    car.ctrl.reserved1 = random.NextFloat();
    car.ctrl.reserved2 = random.NextFloat();
    car.ctrl.gear = random.NextInt(-1, MAX_GEARS - 1);
    car.ctrl.raceCmd = random.NextInt(2);
    car.ctrl.lightCmd = random.NextInt();
    car.ctrl.ebrakeCmd = random.NextInt();
    car.ctrl.wingControlMode = random.NextInt(3);
    car.ctrl.singleWheelBrakeMode = random.NextInt(2);
    car.ctrl.switch3 = random.NextInt();
    car.ctrl.telemetryMode = random.NextInt(2);
    for (int j = 0; j < 4; j++)
    {
        RAND_NAME(car.ctrl.msg[j], RM_CMD_MAX_MSG_SIZE)
        car.ctrl.msgColor[j] = random.NextFloat();
    }
    car.ctrl.setupChangeCmd = new tDashboardItem();
    RAND_TDASHBOARDITEM(*car.ctrl.setupChangeCmd)

    // Assign values to car.setup
    RAND_TCARSETUPITEM(car.setup.FRWeightRep)
    RAND_TCARSETUPITEM(car.setup.FRLWeightRep)
    RAND_TCARSETUPITEM(car.setup.RRLWeightRep)
    RAND_TCARSETUPITEM(car.setup.fuel)
    for (int i = 0; i < 2; i++)
    {
        RAND_TCARSETUPITEM(car.setup.wingAngle[i])
    }
    RAND_TCARSETUPITEM(car.setup.revsLimiter)
    for (int i = 0; i < MAX_GEARS; i++)
    {
        RAND_TCARSETUPITEM(car.setup.gearRatio[i])
    }
    for (int i = 0; i < 3; i++)
    {
        car.setup.differentialType[i] = random.NextInt();
    }
    for (int i = 0; i < 3; i++)
    {
        RAND_TCARSETUPITEM(car.setup.differentialRatio[i])
    }
    for (int i = 0; i < 3; i++)
    {
        RAND_TCARSETUPITEM(car.setup.differentialMinTqBias[i])
    }
    for (int i = 0; i < 3; i++)
    {
        RAND_TCARSETUPITEM(car.setup.differentialMaxTqBias[i])
    }
    for (int i = 0; i < 3; i++)
    {
        RAND_TCARSETUPITEM(car.setup.differentialViscosity[i])
    }
    for (int i = 0; i < 3; i++)
    {
        RAND_TCARSETUPITEM(car.setup.differentialLockingTq[i])
    }
    for (int i = 0; i < 3; i++)
    {
        RAND_TCARSETUPITEM(car.setup.differentialMaxSlipBias[i])
    }
    for (int i = 0; i < 3; i++)
    {
        RAND_TCARSETUPITEM(car.setup.differentialCoastMaxSlipBias[i])
    }
    RAND_TCARSETUPITEM(car.setup.steerLock)
    RAND_TCARSETUPITEM(car.setup.brakeRepartition)
    RAND_TCARSETUPITEM(car.setup.brakePressure)
    for (int i = 0; i < 4; i++)
    {
        RAND_TCARSETUPITEM(car.setup.rideHeight[i])
    }
    for (int i = 0; i < 4; i++)
    {
        RAND_TCARSETUPITEM(car.setup.toe[i])
    }
    for (int i = 0; i < 4; i++)
    {
        RAND_TCARSETUPITEM(car.setup.camber[i])
    }
    for (int i = 0; i < 4; i++)
    {
        RAND_TCARSETUPITEM(car.setup.tirePressure[i])
    }
    for (int i = 0; i < 4; i++)
    {
        RAND_TCARSETUPITEM(car.setup.tireOpLoad[i])
    }
    for (int i = 0; i < 2; i++)
    {
        RAND_TCARSETUPITEM(car.setup.arbSpring[i])
    }
    for (int i = 0; i < 2; i++)
    {
        RAND_TCARSETUPITEM(car.setup.arbBellcrank[i])
    }
    for (int i = 0; i < 3; i++)
    {
        RAND_TCARSETUPITEM(car.setup.heaveSpring[i])
    }
    for (int i = 0; i < 2; i++)
    {
        RAND_TCARSETUPITEM(car.setup.heaveBellcrank[i])
    }
    for (int i = 0; i < 2; i++)
    {
        RAND_TCARSETUPITEM(car.setup.heaveInertance[i])
    }
    for (int i = 0; i < 2; i++)
    {
        RAND_TCARSETUPITEM(car.setup.heaveFastBump[i])
    }
    for (int i = 0; i < 2; i++)
    {
        RAND_TCARSETUPITEM(car.setup.heaveSlowBump[i])
    }
    for (int i = 0; i < 2; i++)
    {
        RAND_TCARSETUPITEM(car.setup.heaveBumpLvel[i])
    }
    for (int i = 0; i < 2; i++)
    {
        RAND_TCARSETUPITEM(car.setup.heaveFastRebound[i])
    }
    for (int i = 0; i < 2; i++)
    {
        RAND_TCARSETUPITEM(car.setup.heaveSlowRebound[i])
    }
    for (int i = 0; i < 2; i++)
    {
        RAND_TCARSETUPITEM(car.setup.heaveReboundLvel[i])
    }
    for (int i = 0; i < 4; i++)
    {
        RAND_TCARSETUPITEM(car.setup.suspSpring[i])
    }
    for (int i = 0; i < 4; i++)
    {
        RAND_TCARSETUPITEM(car.setup.suspBellcrank[i])
    }
    for (int i = 0; i < 4; i++)
    {
        RAND_TCARSETUPITEM(car.setup.suspInertance[i])
    }
    for (int i = 0; i < 4; i++)
    {
        RAND_TCARSETUPITEM(car.setup.suspCourse[i])
    }
    for (int i = 0; i < 4; i++)
    {
        RAND_TCARSETUPITEM(car.setup.suspPacker[i])
    }
    for (int i = 0; i < 4; i++)
    {
        RAND_TCARSETUPITEM(car.setup.suspFastBump[i])
    }
    for (int i = 0; i < 4; i++)
    {
        RAND_TCARSETUPITEM(car.setup.suspSlowBump[i])
    }
    for (int i = 0; i < 4; i++)
    {
        RAND_TCARSETUPITEM(car.setup.suspBumpLvel[i])
    }
    for (int i = 0; i < 4; i++)
    {
        RAND_TCARSETUPITEM(car.setup.suspFastRebound[i])
    }
    for (int i = 0; i < 4; i++)
    {
        RAND_TCARSETUPITEM(car.setup.suspSlowRebound[i])
    }
    for (int i = 0; i < 4; i++)
    {
        RAND_TCARSETUPITEM(car.setup.suspReboundLvel[i])
    }
    RAND_TCARSETUPITEM(car.setup.reqRepair)
    RAND_TCARSETUPITEM(car.setup.reqTireset)
    RAND_TCARSETUPITEM(car.setup.reqPenalty)

    // Assign values to car.pitdmd
    car.pitcmd.fuel = random.NextFloat();
    car.pitcmd.repair = random.NextInt();
    car.pitcmd.stopType = random.NextInt(2);
    car.pitcmd.setupChanged = random.NextBool();
    CarPitCmd::TireChange possibleTireChanges[] = {CarPitCmd::NONE, CarPitCmd::ALL};
    car.pitcmd.tireChange = possibleTireChanges[random.NextInt(2)];

    // Assign values to car.robot
    car.robot = new RobotItf();
    car.robot->rbNewTrack = nullptr;  // COPY IMPLEMENTED, NO IDEA ON WHAT CONTENTS WOULD BE FOR GENERATION
    car.robot->rbNewRace = nullptr;   // AS ALL OF THESE NULLPTR'S ARE VOID POINTERS
    car.robot->rbPauseRace = nullptr;
    car.robot->rbResumeRace = nullptr;
    car.robot->rbEndRace = nullptr;
    car.robot->rbDrive = nullptr;
    car.robot->rbPitCmd = nullptr;
    car.robot->rbShutdown = nullptr;
    car.robot->index = random.NextInt();

    // Assign value to car.next ~not yet at least~
    car.next = nullptr;  // COPY NOT IMPLEMENTED

    return car;
}

/// @brief Deletes the generated pointers in a given car
/// @param p_car The car to destory
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
    for (int i = 0; i < NR_DI_INSTANT; i++)
    {
        delete p_car.priv.dashboardInstant[i].setup;
    }
    for (int i = 0; i < NR_DI_REQUEST; i++)
    {
        delete p_car.priv.dashboardRequest[i].setup;
    }
    delete p_car.ctrl.setupChangeCmd->setup;
    delete p_car.ctrl.setupChangeCmd;
    delete p_car.robot;
}

/// @brief Gives a random situation
/// @returns A tSituation
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

/// @brief Deletes the generated pointers for a given situation
/// @param p_situation The situation to destory
void DestroySituation(tSituation& p_situation)
{
    delete p_situation.cars;  // COPY NOT IMPLEMENTED
}

/// @brief A testing fixture so there is less code duplication in test cases
class BlackBoxDataTestFixture : public ::testing::Test, public testing::WithParamInterface<bool>
{
protected:
    void SetUp() override
    {
        Random random;
        TestSegments = GenerateSegments();
        Car = Generatecar(TestSegments);
        Situation = GenerateSituation();
        TickCount = random.NextInt();
        Segments = new tTrackSeg[TestSegments.NextSegmentsCount];
    }

    TestSegments TestSegments = {};
    tTrackSeg* Segments = nullptr;
    tCarElt Car = {};
    tSituation Situation = {};
    unsigned long TickCount = 0;

    void TearDown() override
    {
        DestroySituation(Situation);
        DestroyCar(Car);
        DestroySegments(TestSegments);
        delete[] Segments;
    }
};

/// @brief          Tests whether elements are the correct value or pointer
/// @param p_eqOrNe true: test for equality of values,
///                 false: test for inequality of pointers within the structs (that we don't have two stucture elements that map to the same memory location)
TEST_P(BlackBoxDataTestFixture, ElementCompareTests)
{
    bool p_eqOrNe = GetParam();
    BlackBoxData data(&Car, &Situation, TickCount, Segments, TestSegments.NextSegmentsCount);

    // Compare tickCount
    COMP_ELEM(TickCount, data.TickCount)

    // Compare car.index
    COMP_ELEM(Car.index, data.Car.index)

    // Compare car.info
    COMP_NAME(Car.info.name, data.Car.info.name)
    COMP_NAME(Car.info.sname, data.Car.info.sname)
    COMP_NAME(Car.info.codename, data.Car.info.codename)
    COMP_NAME(Car.info.teamname, data.Car.info.teamname)
    COMP_NAME(Car.info.carName, data.Car.info.carName)
    COMP_NAME(Car.info.category, data.Car.info.category)
    COMP_ELEM(Car.info.raceNumber, data.Car.info.raceNumber)
    COMP_ELEM(Car.info.startRank, data.Car.info.startRank)
    COMP_ELEM(Car.info.driverType, data.Car.info.driverType)
    COMP_ELEM(Car.info.networkplayer, data.Car.info.networkplayer)
    COMP_ELEM(Car.info.skillLevel, data.Car.info.skillLevel)
    for (int i = 0; i < 3; i++)
    {
        COMP_ELEM(Car.info.iconColor[i], data.Car.info.iconColor[i])
    }
    COMP_T3D(Car.info.dimension, data.Car.info.dimension)
    COMP_T3D(Car.info.drvPos, data.Car.info.drvPos)
    COMP_T3D(Car.info.bonnetPos, data.Car.info.bonnetPos)
    COMP_ELEM(Car.info.tank, data.Car.info.tank)
    COMP_ELEM(Car.info.steerLock, data.Car.info.steerLock)
    COMP_T3D(Car.info.statGC, data.Car.info.statGC)
    for (int i = 0; i < 4; i++)
    {
        COMP_ELEM(Car.info.wheel[i].rimRadius, data.Car.info.wheel[i].rimRadius)
        COMP_ELEM(Car.info.wheel[i].tireHeight, data.Car.info.wheel[i].tireHeight)
        COMP_ELEM(Car.info.wheel[i].tireWidth, data.Car.info.wheel[i].tireWidth)
        COMP_ELEM(Car.info.wheel[i].brakeDiskRadius, data.Car.info.wheel[i].brakeDiskRadius)
        COMP_ELEM(Car.info.wheel[i].wheelRadius, data.Car.info.wheel[i].wheelRadius)
    }
    COMP_ELEM(Car.info.visualAttr.exhaustNb, data.Car.info.visualAttr.exhaustNb)
    for (int i = 0; i < 2; i++)
    {
        COMP_T3D(Car.info.visualAttr.exhaustPos[i], data.Car.info.visualAttr.exhaustPos[i])
    }
    COMP_ELEM(Car.info.visualAttr.exhaustPower, data.Car.info.visualAttr.exhaustPower)
    COMP_NAME(Car.info.masterModel, data.Car.info.masterModel)
    COMP_NAME(Car.info.skinName, data.Car.info.skinName)
    COMP_ELEM(Car.info.skinTargets, data.Car.info.skinTargets)

    // Compare car.pub (no deep compare of trkPos.seg)
    COMP_TDYNPT(Car.pub.DynGC, data.Car.pub.DynGC)
    COMP_TDYNPT(Car.pub.DynGCg, data.Car.pub.DynGCg)
    COMP_ELEM(Car.pub.speed, data.Car.pub.speed)
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            COMP_ELEM(Car.pub.posMat[i][j], data.Car.pub.posMat[i][j])
        }
    }
    COMP_TRKPOS(Car.pub.trkPos, data.Car.pub.trkPos)
    COMP_ELEM(Segments, data.Car.pub.trkPos.seg)
    for (int i = 0; i < 4; i++)
    {
        COMP_TPOSD(Car.pub.corner[i], data.Car.pub.corner[i])
    }
    COMP_ELEM(Car.pub.glance, data.Car.pub.glance)
    COMP_ELEM(Car.pub.oldglance, data.Car.pub.oldglance)

    // Compare car.race
    COMP_ELEM(Car.race.bestLapTime, data.Car.race.bestLapTime)
    COMP_ELEM(Car.race.commitBestLapTime, data.Car.race.commitBestLapTime)
    if (Car.race.bestSplitTime && data.Car.race.bestSplitTime)
    {
        COMP_ELEM(*Car.race.bestSplitTime, *data.Car.race.bestSplitTime)
    }
    COMP_ELEM(Car.race.deltaBestLapTime, data.Car.race.deltaBestLapTime)
    COMP_ELEM(Car.race.curLapTime, data.Car.race.curLapTime)
    if (Car.race.curSplitTime && data.Car.race.curSplitTime)
    {
        COMP_ELEM(*Car.race.curSplitTime, *data.Car.race.curSplitTime)
    }
    COMP_ELEM(Car.race.lastLapTime, data.Car.race.lastLapTime)
    COMP_ELEM(Car.race.curTime, data.Car.race.curTime)
    COMP_ELEM(Car.race.topSpeed, data.Car.race.topSpeed)
    COMP_ELEM(Car.race.currentMinSpeedForLap, data.Car.race.currentMinSpeedForLap)
    COMP_ELEM(Car.race.laps, data.Car.race.laps)
    COMP_ELEM(Car.race.bestLap, data.Car.race.bestLap)
    COMP_ELEM(Car.race.nbPitStops, data.Car.race.nbPitStops)
    COMP_ELEM(Car.race.remainingLaps, data.Car.race.remainingLaps)
    COMP_ELEM(Car.race.pos, data.Car.race.pos)
    COMP_ELEM(Car.race.timeBehindLeader, data.Car.race.timeBehindLeader)
    COMP_ELEM(Car.race.lapsBehindLeader, data.Car.race.lapsBehindLeader)
    COMP_ELEM(Car.race.timeBehindPrev, data.Car.race.timeBehindPrev)
    COMP_ELEM(Car.race.timeBeforeNext, data.Car.race.timeBeforeNext)
    COMP_ELEM(Car.race.distRaced, data.Car.race.distRaced)
    COMP_ELEM(Car.race.distFromStartLine, data.Car.race.distFromStartLine)
    COMP_ELEM(Car.race.currentSector, data.Car.race.currentSector)
    COMP_ELEM(Car.race.nbSectors, data.Car.race.nbSectors)
    COMP_ELEM(Car.race.trackLength, data.Car.race.trackLength)
    COMP_ELEM(Car.race.scheduledEventTime, data.Car.race.scheduledEventTime)
    if (Car.race.pit && data.Car.race.pit)
    {
        COMP_TRKPOS(Car.race.pit->pos, data.Car.race.pit->pos)
        COMP_ELEM(Car.race.pit->pitCarIndex, data.Car.race.pit->pitCarIndex)
        COMP_ELEM(Car.race.pit->lmin, data.Car.race.pit->lmin)
        COMP_ELEM(Car.race.pit->lmax, data.Car.race.pit->lmax)
        COMP_ELEM(Car.race.pit->freeCarIndex, data.Car.race.pit->freeCarIndex)
        for (int i = 0; i < TR_PIT_MAXCARPERPIT; i++)
        {
            // COPY NOT IMPLEMENTED for car.race.pit->car
        }
    }
    COMP_ELEM(Car.race.event, data.Car.race.event)
    // COPY NOT IMPLEMENTED for car.race.penaltyList
    COMP_ELEM(Car.race.penaltyTime, data.Car.race.penaltyTime)
    COMP_ELEM(Car.race.prevFromStartLine, data.Car.race.prevFromStartLine)
    COMP_ELEM(Car.race.wrongWayTime, data.Car.race.wrongWayTime)

    // Compare car.priv
    COMP_ELEM(Car.priv.driverIndex, data.Car.priv.driverIndex)
    COMP_ELEM(Car.priv.moduleIndex, data.Car.priv.moduleIndex)
    COMP_NAME(Car.priv.modName, data.Car.priv.modName)
    for (int i = 0; i < 4; i++)
    {
        COMP_TPOSD(Car.priv.wheel[i].relPos, data.Car.priv.wheel[i].relPos)
        COMP_ELEM(Car.priv.wheel[i].spinVel, data.Car.priv.wheel[i].spinVel)
        COMP_ELEM(Car.priv.wheel[i].brakeTemp, data.Car.priv.wheel[i].brakeTemp)
        COMP_ELEM(Car.priv.wheel[i].state, data.Car.priv.wheel[i].state)
        // COPY NOT IMPLEMENTED for car.priv.wheel[i].seg
        COMP_ELEM(Car.priv.wheel[i].rollRes, data.Car.priv.wheel[i].rollRes)
        COMP_ELEM(Car.priv.wheel[i].temp_in, data.Car.priv.wheel[i].temp_in)
        COMP_ELEM(Car.priv.wheel[i].temp_mid, data.Car.priv.wheel[i].temp_mid)
        COMP_ELEM(Car.priv.wheel[i].temp_out, data.Car.priv.wheel[i].temp_out)
        COMP_ELEM(Car.priv.wheel[i].temp_opt, data.Car.priv.wheel[i].temp_opt)
        COMP_ELEM(Car.priv.wheel[i].condition, data.Car.priv.wheel[i].condition)
        COMP_ELEM(Car.priv.wheel[i].treadDepth, data.Car.priv.wheel[i].treadDepth)
        COMP_ELEM(Car.priv.wheel[i].critTreadDepth, data.Car.priv.wheel[i].critTreadDepth)
        COMP_ELEM(Car.priv.wheel[i].slipNorm, data.Car.priv.wheel[i].slipNorm)
        COMP_ELEM(Car.priv.wheel[i].slipOpt, data.Car.priv.wheel[i].slipOpt)
        COMP_ELEM(Car.priv.wheel[i].slipSide, data.Car.priv.wheel[i].slipSide)
        COMP_ELEM(Car.priv.wheel[i].slipAccel, data.Car.priv.wheel[i].slipAccel)
        COMP_ELEM(Car.priv.wheel[i].Fx, data.Car.priv.wheel[i].Fx)
        COMP_ELEM(Car.priv.wheel[i].Fy, data.Car.priv.wheel[i].Fy)
        COMP_ELEM(Car.priv.wheel[i].Fz, data.Car.priv.wheel[i].Fz)
        COMP_ELEM(Car.priv.wheel[i].effectiveMu, data.Car.priv.wheel[i].effectiveMu)
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TPOSD(Car.priv.corner[i], data.Car.priv.corner[i])
    }
    COMP_ELEM(Car.priv.gear, data.Car.priv.gear)
    COMP_ELEM(Car.priv.gearNext, data.Car.priv.gearNext)
    COMP_ELEM(Car.priv.fuel, data.Car.priv.fuel)
    COMP_ELEM(Car.priv.fuel_consumption_total, data.Car.priv.fuel_consumption_total)
    COMP_ELEM(Car.priv.fuel_consumption_instant, data.Car.priv.fuel_consumption_instant)
    COMP_ELEM(Car.priv.enginerpm, data.Car.priv.enginerpm)
    COMP_ELEM(Car.priv.enginerpmRedLine, data.Car.priv.enginerpmRedLine)
    COMP_ELEM(Car.priv.enginerpmMax, data.Car.priv.enginerpmMax)
    COMP_ELEM(Car.priv.enginerpmMaxTq, data.Car.priv.enginerpmMaxTq)
    COMP_ELEM(Car.priv.enginerpmMaxPw, data.Car.priv.enginerpmMaxPw)
    COMP_ELEM(Car.priv.engineMaxTq, data.Car.priv.engineMaxTq)
    COMP_ELEM(Car.priv.engineMaxPw, data.Car.priv.engineMaxPw)
    for (int i = 0; i < MAX_GEARS; i++)
    {
        COMP_ELEM(Car.priv.gearRatio[i], data.Car.priv.gearRatio[i])
    }
    COMP_ELEM(Car.priv.gearNb, data.Car.priv.gearNb)
    COMP_ELEM(Car.priv.gearOffset, data.Car.priv.gearOffset)
    for (int i = 0; i < 4; i++)
    {
        COMP_ELEM(Car.priv.skid[i], data.Car.priv.skid[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_ELEM(Car.priv.reaction[i], data.Car.priv.reaction[i])
    }
    COMP_ELEM(Car.priv.collision, data.Car.priv.collision)
    COMP_ELEM(Car.priv.simcollision, data.Car.priv.simcollision)
    COMP_ELEM(Car.priv.smoke, data.Car.priv.smoke)
    COMP_T3D(Car.priv.normal, data.Car.priv.normal)
    COMP_T3D(Car.priv.collpos, data.Car.priv.collpos)
    COMP_ELEM(Car.priv.dammage, data.Car.priv.dammage)
    COMP_ELEM(Car.priv.debug, data.Car.priv.debug)
    COMP_ELEM(Car.priv.collision_state.collision_count, data.Car.priv.collision_state.collision_count)
    for (int i = 0; i < 3; i++)
    {
        COMP_ELEM(Car.priv.collision_state.pos[i], data.Car.priv.collision_state.pos[i])
        COMP_ELEM(Car.priv.collision_state.force[i], data.Car.priv.collision_state.force[i])
    }
    COMP_ELEM(Car.priv.localPressure, data.Car.priv.localPressure)
    // COPY NOT IMPLEMENTED FOR car.priv.memoryPool
    COMP_ELEM(Car.priv.driveSkill, data.Car.priv.driveSkill)
    COMP_ELEM(Car.priv.steerTqCenter, data.Car.priv.steerTqCenter)
    COMP_ELEM(Car.priv.steerTqAlign, data.Car.priv.steerTqAlign)
    for (int i = 0; i < NR_DI_INSTANT; i++)
    {
        COMP_ELEM(Car.priv.dashboardInstant[i].type, data.Car.priv.dashboardInstant[i].type)
        if (Car.priv.dashboardInstant[i].setup)
        {
            COMP_TCARSETUPITEM(*Car.priv.dashboardInstant[i].setup, *data.Car.priv.dashboardInstant[i].setup)
        }
    }
    COMP_ELEM(Car.priv.dashboardInstantNb, data.Car.priv.dashboardInstantNb)
    for (int i = 0; i < NR_DI_REQUEST; i++)
    {
        COMP_ELEM(Car.priv.dashboardRequest[i].type, data.Car.priv.dashboardRequest[i].type)
        if (Car.priv.dashboardRequest[i].setup)
        {
            COMP_TCARSETUPITEM(*Car.priv.dashboardRequest[i].setup, *data.Car.priv.dashboardRequest[i].setup)
        }
    }
    COMP_ELEM(Car.priv.dashboardRequestNb, data.Car.priv.dashboardRequestNb)
    COMP_ELEM(Car.priv.dashboardActiveItem, data.Car.priv.dashboardActiveItem)

    // Compare car.ctrl
    COMP_ELEM(Car.ctrl.steer, data.Car.ctrl.steer)
    COMP_ELEM(Car.ctrl.accelCmd, data.Car.ctrl.accelCmd)
    COMP_ELEM(Car.ctrl.brakeCmd, data.Car.ctrl.brakeCmd)
    COMP_ELEM(Car.ctrl.clutchCmd, data.Car.ctrl.clutchCmd)
    COMP_ELEM(Car.ctrl.brakeFrontRightCmd, data.Car.ctrl.brakeFrontRightCmd)
    COMP_ELEM(Car.ctrl.brakeFrontLeftCmd, data.Car.ctrl.brakeFrontLeftCmd)
    COMP_ELEM(Car.ctrl.brakeRearRightCmd, data.Car.ctrl.brakeRearRightCmd)
    COMP_ELEM(Car.ctrl.brakeRearLeftCmd, data.Car.ctrl.brakeRearLeftCmd)
    COMP_ELEM(Car.ctrl.wingFrontCmd, data.Car.ctrl.wingFrontCmd)
    COMP_ELEM(Car.ctrl.wingRearCmd, data.Car.ctrl.wingRearCmd)
    COMP_ELEM(Car.ctrl.reserved1, data.Car.ctrl.reserved1)
    COMP_ELEM(Car.ctrl.reserved2, data.Car.ctrl.reserved2)
    COMP_ELEM(Car.ctrl.gear, data.Car.ctrl.gear)
    COMP_ELEM(Car.ctrl.raceCmd, data.Car.ctrl.raceCmd)
    COMP_ELEM(Car.ctrl.lightCmd, data.Car.ctrl.lightCmd)
    COMP_ELEM(Car.ctrl.ebrakeCmd, data.Car.ctrl.ebrakeCmd)
    COMP_ELEM(Car.ctrl.wingControlMode, data.Car.ctrl.wingControlMode)
    COMP_ELEM(Car.ctrl.singleWheelBrakeMode, data.Car.ctrl.singleWheelBrakeMode)
    COMP_ELEM(Car.ctrl.switch3, data.Car.ctrl.switch3)
    COMP_ELEM(Car.ctrl.telemetryMode, data.Car.ctrl.telemetryMode)
    for (int i = 0; i < 4; i++)
    {
        COMP_NAME(Car.ctrl.msg[i], data.Car.ctrl.msg[i])
        COMP_ELEM(Car.ctrl.msgColor[i], data.Car.ctrl.msgColor[i])
    }
    if (Car.ctrl.setupChangeCmd && data.Car.ctrl.setupChangeCmd)
    {
        COMP_ELEM(Car.ctrl.setupChangeCmd->type, data.Car.ctrl.setupChangeCmd->type)
        if (Car.ctrl.setupChangeCmd->setup && data.Car.ctrl.setupChangeCmd->setup)
        {
            COMP_TCARSETUPITEM(*Car.ctrl.setupChangeCmd->setup, *data.Car.ctrl.setupChangeCmd->setup)
        }
    }

    // Compare car.setup
    COMP_TCARSETUPITEM(Car.setup.FRWeightRep, data.Car.setup.FRWeightRep)
    COMP_TCARSETUPITEM(Car.setup.FRLWeightRep, data.Car.setup.FRLWeightRep)
    COMP_TCARSETUPITEM(Car.setup.RRLWeightRep, data.Car.setup.RRLWeightRep)
    COMP_TCARSETUPITEM(Car.setup.fuel, data.Car.setup.fuel)
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.wingAngle[i], data.Car.setup.wingAngle[i])
    }
    COMP_TCARSETUPITEM(Car.setup.revsLimiter, data.Car.setup.revsLimiter)
    for (int i = 0; i < MAX_GEARS; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.gearRatio[i], data.Car.setup.gearRatio[i])
    }
    for (int i = 0; i < 3; i++)
    {
        COMP_ELEM(Car.setup.differentialType[i], data.Car.setup.differentialType[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.differentialRatio[i], data.Car.setup.differentialRatio[i])
    }
    for (int i = 0; i < 3; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.differentialMinTqBias[i], data.Car.setup.differentialMinTqBias[i])
    }
    for (int i = 0; i < 3; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.differentialMaxTqBias[i], data.Car.setup.differentialMaxTqBias[i])
    }
    for (int i = 0; i < 3; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.differentialViscosity[i], data.Car.setup.differentialViscosity[i])
    }
    for (int i = 0; i < 3; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.differentialLockingTq[i], data.Car.setup.differentialLockingTq[i])
    }
    for (int i = 0; i < 3; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.differentialMaxSlipBias[i], data.Car.setup.differentialMaxSlipBias[i])
    }
    for (int i = 0; i < 3; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.differentialCoastMaxSlipBias[i], data.Car.setup.differentialCoastMaxSlipBias[i])
    }
    COMP_TCARSETUPITEM(Car.setup.steerLock, data.Car.setup.steerLock)
    COMP_TCARSETUPITEM(Car.setup.brakeRepartition, data.Car.setup.brakeRepartition)
    COMP_TCARSETUPITEM(Car.setup.brakePressure, data.Car.setup.brakePressure)
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.rideHeight[i], data.Car.setup.rideHeight[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.toe[i], data.Car.setup.toe[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.camber[i], data.Car.setup.camber[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.tirePressure[i], data.Car.setup.tirePressure[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.tireOpLoad[i], data.Car.setup.tireOpLoad[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.arbSpring[i], data.Car.setup.arbSpring[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.arbBellcrank[i], data.Car.setup.arbBellcrank[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.heaveSpring[i], data.Car.setup.heaveSpring[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.heaveBellcrank[i], data.Car.setup.heaveBellcrank[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.heaveInertance[i], data.Car.setup.heaveInertance[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.heaveFastBump[i], data.Car.setup.heaveFastBump[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.heaveSlowBump[i], data.Car.setup.heaveSlowBump[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.heaveBumpLvel[i], data.Car.setup.heaveBumpLvel[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.heaveFastRebound[i], data.Car.setup.heaveFastRebound[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.heaveSlowRebound[i], data.Car.setup.heaveSlowRebound[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.heaveReboundLvel[i], data.Car.setup.heaveReboundLvel[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.suspSpring[i], data.Car.setup.suspSpring[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.suspBellcrank[i], data.Car.setup.suspBellcrank[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.suspInertance[i], data.Car.setup.suspInertance[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.suspCourse[i], data.Car.setup.suspCourse[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.suspPacker[i], data.Car.setup.suspPacker[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.suspFastBump[i], data.Car.setup.suspFastBump[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.suspSlowBump[i], data.Car.setup.suspSlowBump[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.suspBumpLvel[i], data.Car.setup.suspBumpLvel[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.suspFastRebound[i], data.Car.setup.suspFastRebound[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.suspSlowRebound[i], data.Car.setup.suspSlowRebound[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(Car.setup.suspReboundLvel[i], data.Car.setup.suspReboundLvel[i])
    }
    COMP_TCARSETUPITEM(Car.setup.reqRepair, data.Car.setup.reqRepair)
    COMP_TCARSETUPITEM(Car.setup.reqTireset, data.Car.setup.reqTireset)
    COMP_TCARSETUPITEM(Car.setup.reqPenalty, data.Car.setup.reqPenalty)

    // Compare car.pitcmd
    COMP_ELEM(Car.pitcmd.fuel, data.Car.pitcmd.fuel)
    COMP_ELEM(Car.pitcmd.repair, data.Car.pitcmd.repair)
    COMP_ELEM(Car.pitcmd.stopType, data.Car.pitcmd.stopType)
    COMP_ELEM(Car.pitcmd.setupChanged, data.Car.pitcmd.setupChanged)
    COMP_ELEM(Car.pitcmd.tireChange, data.Car.pitcmd.tireChange)

    // Compare car.robot
    if (Car.robot && data.Car.robot)
    {
        COMP_ELEM(Car.robot->index, data.Car.robot->index)
    }

    // COPY NOT IMPLEMENTED FOR car.next

    // Compare situation
    COMP_ELEM(Situation.raceInfo.ncars, data.Situation.raceInfo.ncars)
    COMP_ELEM(Situation.raceInfo.totLaps, data.Situation.raceInfo.totLaps)
    COMP_ELEM(Situation.raceInfo.extraLaps, data.Situation.raceInfo.extraLaps)
    COMP_ELEM(Situation.raceInfo.totTime, data.Situation.raceInfo.totTime)
    COMP_ELEM(Situation.raceInfo.state, data.Situation.raceInfo.state)
    COMP_ELEM(Situation.raceInfo.type, data.Situation.raceInfo.type)
    COMP_ELEM(Situation.raceInfo.maxDammage, data.Situation.raceInfo.maxDammage)
    COMP_ELEM(Situation.raceInfo.fps, data.Situation.raceInfo.fps)
    COMP_ELEM(Situation.raceInfo.features, data.Situation.raceInfo.features)
    COMP_ELEM(Situation.deltaTime, data.Situation.deltaTime)
    COMP_ELEM(Situation.currentTime, data.Situation.currentTime)
    COMP_ELEM(Situation.accelTime, data.Situation.accelTime)
    COMP_ELEM(Situation.nbPlayers, data.Situation.nbPlayers)
    // COPY NOT IMPLEMENTED FOR situation.cars

    // Compare car.pub.trkPos.seg vs segments
    if (Car.pub.trkPos.seg && Segments)
    {
        tTrackSeg* segOrig = &Car.pub.trkPos.seg[0];
        tTrackSeg* segCopy = &Segments[0];
        for (int i = 0; i < TestSegments.NextSegmentsCount; i++)
        {
            // DEEP COPY NOT IMPLEMENTED FOR seg.name
            COMP_ELEM((*segOrig).id, (*segCopy).id)
            COMP_ELEM((*segOrig).type, (*segCopy).type)
            COMP_ELEM((*segOrig).type2, (*segCopy).type2)
            COMP_ELEM((*segOrig).style, (*segCopy).style)
            COMP_ELEM((*segOrig).length, (*segCopy).length)
            COMP_ELEM((*segOrig).width, (*segCopy).width)
            COMP_ELEM((*segOrig).startWidth, (*segCopy).startWidth)
            COMP_ELEM((*segOrig).endWidth, (*segCopy).endWidth)
            COMP_ELEM((*segOrig).lgfromstart, (*segCopy).lgfromstart)
            COMP_ELEM((*segOrig).radius, (*segCopy).radius)
            COMP_ELEM((*segOrig).radiusr, (*segCopy).radiusr)
            COMP_ELEM((*segOrig).radiusl, (*segCopy).radiusl)
            COMP_ELEM((*segOrig).arc, (*segCopy).arc)
            COMP_T3D((*segOrig).center, (*segCopy).center)
            for (int i = 0; i < 4; i++)
            {
                COMP_T3D((*segOrig).vertex[i], (*segCopy).vertex[i])
            }
            for (int i = 0; i < 7; i++)
            {
                COMP_ELEM((*segOrig).angle[i], (*segCopy).angle[i])
            }
            COMP_ELEM((*segOrig).sin, (*segCopy).sin)
            COMP_ELEM((*segOrig).cos, (*segCopy).cos)
            COMP_ELEM((*segOrig).Kzl, (*segCopy).Kzl)
            COMP_ELEM((*segOrig).Kzw, (*segCopy).Kzw)
            COMP_ELEM((*segOrig).Kyl, (*segCopy).Kyl)
            COMP_T3D((*segOrig).rgtSideNormal, (*segCopy).rgtSideNormal)
            COMP_ELEM((*segOrig).envIndex, (*segCopy).envIndex)
            COMP_ELEM((*segOrig).height, (*segCopy).height)
            COMP_ELEM((*segOrig).raceInfo, (*segCopy).raceInfo)
            COMP_ELEM((*segOrig).DoVfactor, (*segCopy).DoVfactor)
            // COPY NOT IMPLEMENTED FOR seg.ext, seg.surface, seg.barrier, seg.cam
            segOrig = (*segOrig).next;
            segCopy = (*segCopy).next;
        }
    }
}

INSTANTIATE_TEST_SUITE_P(BlackBoxDataTests, BlackBoxDataTestFixture, ::testing::Values(true, false));

/// @brief Tests whether elements that are pointers have actually been copied into a new pointer
TEST_F(BlackBoxDataTestFixture, PointerInequalityTest)
{
    BlackBoxData data(&Car, &Situation, TickCount, Segments, TestSegments.NextSegmentsCount);

    EXPECT_NE(Car.pub.trkPos.seg, data.Car.pub.trkPos.seg);
    EXPECT_NE(Car.race.bestSplitTime, data.Car.race.bestSplitTime);
    EXPECT_NE(Car.race.curSplitTime, data.Car.race.curSplitTime);
    EXPECT_NE(Car.race.pit, data.Car.race.pit);
    for (int i = 0; i < NR_DI_INSTANT; i++)
    {
        EXPECT_NE(Car.priv.dashboardInstant[i].setup, data.Car.priv.dashboardInstant[i].setup);
    }
    for (int i = 0; i < NR_DI_REQUEST; i++)
    {
        EXPECT_NE(Car.priv.dashboardRequest[i].setup, data.Car.priv.dashboardRequest[i].setup);
    }
    EXPECT_NE(Car.ctrl.setupChangeCmd, data.Car.ctrl.setupChangeCmd);
    if (Car.ctrl.setupChangeCmd && data.Car.ctrl.setupChangeCmd)
    {
        EXPECT_NE(Car.ctrl.setupChangeCmd->setup, data.Car.ctrl.setupChangeCmd->setup);
    }
    EXPECT_NE(Car.robot, data.Car.robot);
    if (Car.pub.trkPos.seg && Segments)
    {
        tTrackSeg* segOrig = &Car.pub.trkPos.seg[0];
        tTrackSeg* segCopy = &Segments[0];
        for (int i = 0; i < TestSegments.NextSegmentsCount; i++)
        {
            EXPECT_NE(segOrig, segCopy);
            segOrig = (*segOrig).next;
            segCopy = (*segCopy).next;
        }
    }
}

/// @brief Tests whether BlackBoxData() indeed throws if it receives invalid input
TEST_F(BlackBoxDataTestFixture, ExceptionsTest)
{
    ASSERT_THROW(BlackBoxData(nullptr, &Situation, TickCount, Segments, TestSegments.NextSegmentsCount), std::invalid_argument);
    ASSERT_THROW(BlackBoxData(&Car, nullptr, TickCount, Segments, TestSegments.NextSegmentsCount), std::invalid_argument);
    ASSERT_THROW(BlackBoxData(&Car, &Situation, TickCount, Segments, -TestSegments.NextSegmentsCount), std::invalid_argument);
}