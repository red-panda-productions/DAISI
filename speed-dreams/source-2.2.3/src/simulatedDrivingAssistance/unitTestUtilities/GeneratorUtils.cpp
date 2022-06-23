/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#include "GeneratorUtils.h"
#include "Random.hpp"
#include "robot.h"

// Lifted wholesale from BlackBoxDataTests.cpp, provides generator methods for the following structs: tCarElt, tSituation, tTrackSeg and for aTrackSeg*
// Does not assign values to pointers, with the exception of tCarElt.pub.trkPos.seg, which assigns the provided value of a tTrackSeg*,
// as none of these values are being copied in BlackBoxData and are thus not useable anyhow within a black box

/// @brief   Generates a random name; guarantees the last character is '\0', although one could pop up earlier
/// @param a The array to generate the name in
/// @param b The length of the array
#define RAND_NAME(a, b)                          \
    for (int i = 0; i < (b)-1; i++)              \
    {                                            \
        (a)[i] = (char)random.NextByte(32, 126); \
    }                                            \
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
#define RAND_TDASHBOARDITEM(a)   \
    (a).type = random.NextInt(); \
    (a).setup = nullptr;

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

/// @brief Generates a random segment
/// @returns A tTrackSeg
tTrackSeg GenerateSegment()
{
    Random random;
    tTrackSeg segment = {};
    segment.name = nullptr;  // COPY NOT IMPLEMENTED
    segment.id = random.NextInt();
    segment.type = random.NextInt(1, 4);
    segment.type2 = random.NextInt(1, 6);
    segment.style = random.NextInt(5);
    segment.SpeedLimit = random.NextFloat();
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
    // Main application uses 10 as its p_nextSegmentsCount (SocketBlackBox::GetDecisions()), so test around that range, not excessively more
#define MAX_TESTSEGMENT_COUNT 16
    testSegments.NextSegmentsCount = random.NextInt(2, MAX_TESTSEGMENT_COUNT);
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
/// @param p_segments The TestSegments to destroy
void DestroySegments(TestSegments& p_segments)
{
    for (int i = 0; i < p_segments.NextSegmentsCount; i++)
    {
        // With the current copy implementation, the copies of the segments are written to the same location as they are copied from
        delete[] p_segments.NextSegments[i].name;  // DEEP COPY NOT IMPLEMENTED; IF IT WERE, DUE TO THE ABOVE, IT WOULD LEAK MEMORY
        delete p_segments.NextSegments[i].ext;     // COPY NOT IMPLEMENTED
        for (int j = 0; j < 2; j++)
        {
            delete p_segments.NextSegments[i].barrier[j];  // COPY NOT IMPLEMENTED
        }
        delete p_segments.NextSegments[i].cam;  // COPY NOT IMPLEMENTED
    }
    delete[] p_segments.NextSegments;
}

/// @brief Generates a random car
/// @param p_segments A set of earlier obtained segments to put in retval.car.pub.trkPos.seg
/// @returns A tCarElt
tCarElt GenerateCar(TestSegments& p_segments)
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
    car.pub.trkPos.seg = p_segments.NextSegments;
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
    car.race.bestSplitTime = nullptr;  // COPY NOT IMPLEMENTED
    car.race.deltaBestLapTime = random.NextFloat();
    car.race.curLapTime = random.NextFloat();
    car.race.curSplitTime = nullptr;  // COPY NOT IMPLEMENTED
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
    car.race.pit = nullptr;  // COPY NOT IMPLEMENTED
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
    car.priv.gearOffset = random.NextInt(0, 2);  // client bb can't handle this
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
    car.ctrl.setupChangeCmd = nullptr;  // COPY NOT IMPLEMENTED

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
    car.robot = nullptr;  // COPY NOT IMPLEMENTED

    // Assign value to car.next ~not yet at least~
    car.next = nullptr;  // COPY NOT IMPLEMENTED

    return car;
}

/// @brief Deletes the generated pointers in a given car
/// @param p_car The car to destory
void DestroyCar(tCarElt& p_car)
{
    delete p_car.race.bestSplitTime;          // COPY NOT IMPLEMENTED
    delete p_car.race.curSplitTime;           // COPY NOT IMPLEMENTED
    delete p_car.race.pit;                    // COPY NOT IMPLEMENTED
    delete p_car.race.penaltyList.tqh_first;  // COPY NOT IMPLEMENTED
    delete p_car.race.penaltyList.tqh_last;   // COPY NOT IMPLEMENTED
    for (int i = 0; i < NR_DI_INSTANT; i++)
    {
        delete p_car.priv.dashboardInstant[i].setup;  // COPY NOT IMPLEMENTED
    }
    for (int i = 0; i < NR_DI_REQUEST; i++)
    {
        delete p_car.priv.dashboardRequest[i].setup;  // COPY NOT IMPLEMENTED
    }
    delete p_car.ctrl.setupChangeCmd;  // COPY NOT IMPLEMENTED
    delete p_car.robot;                // COPY NOT IMPLEMENTED
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

/// @brief Generate a tCar with random values for all variables except ctrl, params or carElt;
/// @return the tCar
tCar GenerateSimCar()
{
    Random random;
    tCar car{};
    car.ctrl = nullptr;
    car.params = nullptr;
    car.carElt = nullptr;

    // preCtrl
    car.preCtrl.accelCmd = random.NextFloat();
    car.preCtrl.brakeCmd = random.NextFloat();
    car.preCtrl.brakeFrontLeftCmd = random.NextFloat();
    car.preCtrl.brakeFrontRightCmd = random.NextFloat();
    car.preCtrl.brakeRearLeftCmd = random.NextFloat();
    car.preCtrl.brakeRearRightCmd = random.NextFloat();
    car.preCtrl.clutchCmd = random.NextFloat();
    car.preCtrl.ebrakeCmd = random.NextInt(0, 2);
    car.preCtrl.gear = random.NextInt(-1, 2);
    car.preCtrl.lightCmd = random.NextInt(0, 2);

    // axle
    for (int i = 0; i < 2; i++)
    {
        car.axle[i].I = random.NextFloat();
        car.axle[i].force[0] = random.NextFloat();
        car.axle[i].force[1] = random.NextFloat();
        car.axle[i].wheight0 = random.NextFloat();
        car.axle[i].xpos = random.NextFloat();

        // suspension
        car.axle[i].arbSusp.a = random.NextFloat();
        car.axle[i].arbSusp.force = random.NextFloat();
        car.axle[i].arbSusp.inertance = random.NextFloat();
        car.axle[i].arbSusp.v = random.NextFloat();
        car.axle[i].arbSusp.x = random.NextFloat();
        car.axle[i].arbSusp.state = random.NextInt(0, 1);

        // damper
        car.axle[i].arbSusp.damper.bump.C1 = random.NextFloat();
        car.axle[i].arbSusp.damper.bump.C2 = random.NextFloat();
        car.axle[i].arbSusp.damper.bump.b1 = random.NextFloat();
        car.axle[i].arbSusp.damper.bump.b2 = random.NextFloat();
        car.axle[i].arbSusp.damper.bump.v1 = random.NextFloat();

        car.axle[i].arbSusp.damper.rebound.C1 = random.NextFloat();
        car.axle[i].arbSusp.damper.rebound.C2 = random.NextFloat();
        car.axle[i].arbSusp.damper.rebound.b1 = random.NextFloat();
        car.axle[i].arbSusp.damper.rebound.b2 = random.NextFloat();
        car.axle[i].arbSusp.damper.rebound.v1 = random.NextFloat();

        // spring
        car.axle[i].arbSusp.spring.F0 = random.NextFloat();
        car.axle[i].arbSusp.spring.K = random.NextFloat();
        car.axle[i].arbSusp.spring.bellcrank = random.NextFloat();
        car.axle[i].arbSusp.spring.packers = random.NextFloat();
        car.axle[i].arbSusp.spring.x0 = random.NextFloat();
        car.axle[i].arbSusp.spring.xMax = random.NextFloat();
    }

    // wheels
    for (int i = 0; i < 4; i++)
    {
        car.wheel[i].AlignTqFactor = random.NextFloat();
        car.wheel[i].I = random.NextFloat();
        car.wheel[i].Tinit = random.NextFloat();
        car.wheel[i].Topt = random.NextFloat();
        car.wheel[i].Ttire = random.NextFloat();
        car.wheel[i].aircoolm = random.NextFloat();
        car.wheel[i].axleFz = random.NextFloat();
        car.wheel[i].axleFz3rd = random.NextFloat();
        car.wheel[i].camber = random.NextFloat();
        car.wheel[i].rollRes = random.NextFloat();

        // susp
        car.wheel[i].susp.force = random.NextFloat();
        car.wheel[i].susp.a = random.NextFloat();
        car.wheel[i].susp.inertance = random.NextFloat();
        car.wheel[i].susp.v = random.NextFloat();
        car.wheel[i].susp.x = random.NextFloat();
        car.wheel[i].susp.state = random.NextInt();

        // brake
        car.wheel[i].brake.I = random.NextFloat();
        car.wheel[i].brake.ABS = random.NextFloat();
        car.wheel[i].brake.TCL = random.NextFloat();
        car.wheel[i].brake.Tq = random.NextFloat();
        car.wheel[i].brake.pressure = random.NextFloat();
    }

    // steer
    car.steer.steer = random.NextFloat();
    car.steer.maxSpeed = random.NextFloat();
    car.steer.steerLock = random.NextFloat();

    // brakeSyst
    car.brkSyst.coeff = random.NextFloat();
    car.brkSyst.ebrake_pressure = random.NextFloat();
    car.brkSyst.rep = random.NextFloat();

    // aero
    car.aero.Cd = random.NextFloat();
    car.aero.CdBody = random.NextFloat();
    car.aero.Clift[0] = random.NextFloat();
    car.aero.Clift[1] = random.NextFloat();
    car.aero.drag = random.NextFloat();
    car.aero.lift[0] = random.NextFloat();
    car.aero.lift[1] = random.NextFloat();

    // transmission
    car.transmission.clutch.mode = random.NextInt(0, 2);
    car.transmission.clutch.releaseTime = random.NextFloat();
    car.transmission.clutch.state = random.NextInt(0, 2);
    car.transmission.clutch.timeToRelease = random.NextFloat();
    car.transmission.clutch.transferValue = random.NextFloat();
    car.transmission.curI = random.NextFloat();

    for (int i = 0; i < 10; i++)
    {
        car.transmission.driveI[i] = random.NextFloat();
        car.transmission.freeI[i] = random.NextFloat();
        car.transmission.gearEff[i] = random.NextFloat();
        car.transmission.gearI[i] = random.NextFloat();
        car.transmission.overallRatio[i] = random.NextFloat();
    }
    return car;
}

/// @brief Generate a random tCar element with ctrl carElt and trkPos set from p_car
/// @param p_car The tCarElt to set the variables from
/// @return the generated tCar element
tCar GenerateSimCar(tCarElt& p_car)
{
    tCar car = GenerateSimCar();
    car.ctrl = &p_car.ctrl;
    car.carElt = &p_car;
    car.trkPos = p_car.pub.trkPos;

    for (int i = 0; i < 4; i++)
    {
        car.wheel[i].trkPos = p_car.pub.trkPos;
    }
    return car;
}