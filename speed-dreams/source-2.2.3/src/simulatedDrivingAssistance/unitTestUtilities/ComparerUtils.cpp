#include "ComparerUtils.h"
#include <gtest/gtest.h>
#include <stdexcept>

// Lifted wholesale from BlackBoxDataTests.cpp, provides comparer functions for the following structs: tCarElt, tSituation, and for tTrackSeg*
// Does not compare values of pointers, as none of these values are being copied in BlackBoxData and are thus not useable anyhow within a black box

/// @brief                  Performs a comparison on two elements that are elementary
/// @param a                Identifier of the first element
/// @param b                Identifier of the second element
/// @param p_comparisonType A bool defined in the environment this define is called in that determines
/// whether to compare the values of the identifiers or the pointers of the identifiers
#define COMP_ELEM(a, b)        \
    if (p_comparisonType)      \
    {                          \
        ASSERT_EQ((a), (b));   \
    }                          \
    else                       \
    {                          \
        ASSERT_NE(&(a), &(b)); \
    }

/// @brief                  Performs a comparison on two char*
/// @param a                Identifier of the first char*
/// @param b                Identifier of the second char*
/// @param p_comparisonType A bool defined in the environment this define is called in that determines
/// whether to string compare the values of the identifiers (in this case: the contents of the char*) or to compare pointers of the identifiers
#define COMP_NAME(a, b)                 \
    if (p_comparisonType)               \
    {                                   \
        ASSERT_EQ(strcmp((a), (b)), 0); \
    }                                   \
    else                                \
    {                                   \
        ASSERT_NE(&(a), &(b));          \
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

/// @brief   Performs a test on two tCarSetupItem elements
/// @param a Identifier of the first element
/// @param b Identifier of the second element
#define COMP_TCARSETUPITEM(a, b)                    \
    COMP_ELEM((a).value, (b).value)                 \
    COMP_ELEM((a).min, (b).min)                     \
    COMP_ELEM((a).max, (b).max)                     \
    COMP_ELEM((a).desired_value, (b).desired_value) \
    COMP_ELEM((a).stepsize, (b).stepsize)           \
    COMP_ELEM((a).changed, (b).changed)

/// @brief   Performs a test on two tTrkLocPos elements
/// @param a Identifier of the first element
/// @param b Identifier of the second element
/// @note    Much like RAND_TRKPOS, this ignores a.seg and b.seg
#define COMP_TRKPOS(a, b)                 \
    COMP_ELEM((a).type, (b).type)         \
    COMP_ELEM((a).toStart, (b).toStart)   \
    COMP_ELEM((a).toRight, (b).toRight)   \
    COMP_ELEM((a).toMiddle, (b).toMiddle) \
    COMP_ELEM((a).toLeft, (b).toLeft)

/// @brief                  Compares the values of two cars, not any of the values inside elements pointed to by a pointer
/// @param p_car1           Car 1 to compare
/// @param p_car2           Car 2 to compare
/// @param p_comparisonType Whether to compare cars for equality by values (COMP_UTIL_VALUE_EQUALITY) or inequality of pointers (COMP_UTIL_POINTER_INEQUALITY)
/// (the latter is only really relevant for testing BlackBoxData() works)
void CompareCars(tCarElt& p_car1, tCarElt& p_car2, bool p_comparisonType)
{
    bool res = true;
    // Compare car.index
    COMP_ELEM(p_car1.index, p_car2.index)

    // Compare car.info
    COMP_NAME(p_car1.info.name, p_car2.info.name)
    COMP_NAME(p_car1.info.sname, p_car2.info.sname)
    COMP_NAME(p_car1.info.codename, p_car2.info.codename)
    COMP_NAME(p_car1.info.teamname, p_car2.info.teamname)
    COMP_NAME(p_car1.info.carName, p_car2.info.carName)
    COMP_NAME(p_car1.info.category, p_car2.info.category)
    COMP_ELEM(p_car1.info.raceNumber, p_car2.info.raceNumber)
    COMP_ELEM(p_car1.info.startRank, p_car2.info.startRank)
    COMP_ELEM(p_car1.info.driverType, p_car2.info.driverType)
    COMP_ELEM(p_car1.info.networkplayer, p_car2.info.networkplayer)
    COMP_ELEM(p_car1.info.skillLevel, p_car2.info.skillLevel)
    for (int i = 0; i < 3; i++)
    {
        COMP_ELEM(p_car1.info.iconColor[i], p_car2.info.iconColor[i])
    }
    COMP_T3D(p_car1.info.dimension, p_car2.info.dimension)
    COMP_T3D(p_car1.info.drvPos, p_car2.info.drvPos)
    COMP_T3D(p_car1.info.bonnetPos, p_car2.info.bonnetPos)
    COMP_ELEM(p_car1.info.tank, p_car2.info.tank)
    COMP_ELEM(p_car1.info.steerLock, p_car2.info.steerLock)
    COMP_T3D(p_car1.info.statGC, p_car2.info.statGC)
    for (int i = 0; i < 4; i++)
    {
        COMP_ELEM(p_car1.info.wheel[i].rimRadius, p_car2.info.wheel[i].rimRadius)
        COMP_ELEM(p_car1.info.wheel[i].tireHeight, p_car2.info.wheel[i].tireHeight)
        COMP_ELEM(p_car1.info.wheel[i].tireWidth, p_car2.info.wheel[i].tireWidth)
        COMP_ELEM(p_car1.info.wheel[i].brakeDiskRadius, p_car2.info.wheel[i].brakeDiskRadius)
        COMP_ELEM(p_car1.info.wheel[i].wheelRadius, p_car2.info.wheel[i].wheelRadius)
    }
    COMP_ELEM(p_car1.info.visualAttr.exhaustNb, p_car2.info.visualAttr.exhaustNb)
    for (int i = 0; i < 2; i++)
    {
        COMP_T3D(p_car1.info.visualAttr.exhaustPos[i], p_car2.info.visualAttr.exhaustPos[i])
    }
    COMP_ELEM(p_car1.info.visualAttr.exhaustPower, p_car2.info.visualAttr.exhaustPower)
    COMP_NAME(p_car1.info.masterModel, p_car2.info.masterModel)
    COMP_NAME(p_car1.info.skinName, p_car2.info.skinName)
    COMP_ELEM(p_car1.info.skinTargets, p_car2.info.skinTargets)

    // Compare car.pub (no deep compare of trkPos.seg)
    COMP_TDYNPT(p_car1.pub.DynGC, p_car2.pub.DynGC)
    COMP_TDYNPT(p_car1.pub.DynGCg, p_car2.pub.DynGCg)
    COMP_ELEM(p_car1.pub.speed, p_car2.pub.speed)
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            COMP_ELEM(p_car1.pub.posMat[i][j], p_car2.pub.posMat[i][j])
        }
    }
    COMP_TRKPOS(p_car1.pub.trkPos, p_car2.pub.trkPos)
    for (int i = 0; i < 4; i++)
    {
        COMP_TPOSD(p_car1.pub.corner[i], p_car2.pub.corner[i])
    }
    COMP_ELEM(p_car1.pub.glance, p_car2.pub.glance)
    COMP_ELEM(p_car1.pub.oldglance, p_car2.pub.oldglance)

    // Compare car.race
    COMP_ELEM(p_car1.race.bestLapTime, p_car2.race.bestLapTime)
    COMP_ELEM(p_car1.race.commitBestLapTime, p_car2.race.commitBestLapTime)
    // COPY NOT IMPLEMENTED FOR car.race.bestSplitTime
    COMP_ELEM(p_car1.race.deltaBestLapTime, p_car2.race.deltaBestLapTime)
    COMP_ELEM(p_car1.race.curLapTime, p_car2.race.curLapTime)
    // COPY NOT IMPLEMENTED FOR car.race.curSplitTime
    COMP_ELEM(p_car1.race.lastLapTime, p_car2.race.lastLapTime)
    COMP_ELEM(p_car1.race.curTime, p_car2.race.curTime)
    COMP_ELEM(p_car1.race.topSpeed, p_car2.race.topSpeed)
    COMP_ELEM(p_car1.race.currentMinSpeedForLap, p_car2.race.currentMinSpeedForLap)
    COMP_ELEM(p_car1.race.laps, p_car2.race.laps)
    COMP_ELEM(p_car1.race.bestLap, p_car2.race.bestLap)
    COMP_ELEM(p_car1.race.nbPitStops, p_car2.race.nbPitStops)
    COMP_ELEM(p_car1.race.remainingLaps, p_car2.race.remainingLaps)
    COMP_ELEM(p_car1.race.pos, p_car2.race.pos)
    COMP_ELEM(p_car1.race.timeBehindLeader, p_car2.race.timeBehindLeader)
    COMP_ELEM(p_car1.race.lapsBehindLeader, p_car2.race.lapsBehindLeader)
    COMP_ELEM(p_car1.race.timeBehindPrev, p_car2.race.timeBehindPrev)
    COMP_ELEM(p_car1.race.timeBeforeNext, p_car2.race.timeBeforeNext)
    COMP_ELEM(p_car1.race.distRaced, p_car2.race.distRaced)
    COMP_ELEM(p_car1.race.distFromStartLine, p_car2.race.distFromStartLine)
    COMP_ELEM(p_car1.race.currentSector, p_car2.race.currentSector)
    COMP_ELEM(p_car1.race.nbSectors, p_car2.race.nbSectors)
    COMP_ELEM(p_car1.race.trackLength, p_car2.race.trackLength)
    COMP_ELEM(p_car1.race.scheduledEventTime, p_car2.race.scheduledEventTime)
    // COPY NOT IMPLEMENTED FOR car.race.pit
    COMP_ELEM(p_car1.race.event, p_car2.race.event)
    // COPY NOT IMPLEMENTED for car.race.penaltyList
    COMP_ELEM(p_car1.race.penaltyTime, p_car2.race.penaltyTime)
    COMP_ELEM(p_car1.race.prevFromStartLine, p_car2.race.prevFromStartLine)
    COMP_ELEM(p_car1.race.wrongWayTime, p_car2.race.wrongWayTime)

    // Compare car.priv
    COMP_ELEM(p_car1.priv.driverIndex, p_car2.priv.driverIndex)
    COMP_ELEM(p_car1.priv.moduleIndex, p_car2.priv.moduleIndex)
    COMP_NAME(p_car1.priv.modName, p_car2.priv.modName)
    for (int i = 0; i < 4; i++)
    {
        COMP_TPOSD(p_car1.priv.wheel[i].relPos, p_car2.priv.wheel[i].relPos)
        COMP_ELEM(p_car1.priv.wheel[i].spinVel, p_car2.priv.wheel[i].spinVel)
        COMP_ELEM(p_car1.priv.wheel[i].brakeTemp, p_car2.priv.wheel[i].brakeTemp)
        COMP_ELEM(p_car1.priv.wheel[i].state, p_car2.priv.wheel[i].state)
        // COPY NOT IMPLEMENTED for car.priv.wheel[i].seg
        COMP_ELEM(p_car1.priv.wheel[i].rollRes, p_car2.priv.wheel[i].rollRes)
        COMP_ELEM(p_car1.priv.wheel[i].temp_in, p_car2.priv.wheel[i].temp_in)
        COMP_ELEM(p_car1.priv.wheel[i].temp_mid, p_car2.priv.wheel[i].temp_mid)
        COMP_ELEM(p_car1.priv.wheel[i].temp_out, p_car2.priv.wheel[i].temp_out)
        COMP_ELEM(p_car1.priv.wheel[i].temp_opt, p_car2.priv.wheel[i].temp_opt)
        COMP_ELEM(p_car1.priv.wheel[i].condition, p_car2.priv.wheel[i].condition)
        COMP_ELEM(p_car1.priv.wheel[i].treadDepth, p_car2.priv.wheel[i].treadDepth)
        COMP_ELEM(p_car1.priv.wheel[i].critTreadDepth, p_car2.priv.wheel[i].critTreadDepth)
        COMP_ELEM(p_car1.priv.wheel[i].slipNorm, p_car2.priv.wheel[i].slipNorm)
        COMP_ELEM(p_car1.priv.wheel[i].slipOpt, p_car2.priv.wheel[i].slipOpt)
        COMP_ELEM(p_car1.priv.wheel[i].slipSide, p_car2.priv.wheel[i].slipSide)
        COMP_ELEM(p_car1.priv.wheel[i].slipAccel, p_car2.priv.wheel[i].slipAccel)
        COMP_ELEM(p_car1.priv.wheel[i].Fx, p_car2.priv.wheel[i].Fx)
        COMP_ELEM(p_car1.priv.wheel[i].Fy, p_car2.priv.wheel[i].Fy)
        COMP_ELEM(p_car1.priv.wheel[i].Fz, p_car2.priv.wheel[i].Fz)
        COMP_ELEM(p_car1.priv.wheel[i].effectiveMu, p_car2.priv.wheel[i].effectiveMu)
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TPOSD(p_car1.priv.corner[i], p_car2.priv.corner[i])
    }
    COMP_ELEM(p_car1.priv.gear, p_car2.priv.gear)
    COMP_ELEM(p_car1.priv.gearNext, p_car2.priv.gearNext)
    COMP_ELEM(p_car1.priv.fuel, p_car2.priv.fuel)
    COMP_ELEM(p_car1.priv.fuel_consumption_total, p_car2.priv.fuel_consumption_total)
    COMP_ELEM(p_car1.priv.fuel_consumption_instant, p_car2.priv.fuel_consumption_instant)
    COMP_ELEM(p_car1.priv.enginerpm, p_car2.priv.enginerpm)
    COMP_ELEM(p_car1.priv.enginerpmRedLine, p_car2.priv.enginerpmRedLine)
    COMP_ELEM(p_car1.priv.enginerpmMax, p_car2.priv.enginerpmMax)
    COMP_ELEM(p_car1.priv.enginerpmMaxTq, p_car2.priv.enginerpmMaxTq)
    COMP_ELEM(p_car1.priv.enginerpmMaxPw, p_car2.priv.enginerpmMaxPw)
    COMP_ELEM(p_car1.priv.engineMaxTq, p_car2.priv.engineMaxTq)
    COMP_ELEM(p_car1.priv.engineMaxPw, p_car2.priv.engineMaxPw)
    for (int i = 0; i < MAX_GEARS; i++)
    {
        COMP_ELEM(p_car1.priv.gearRatio[i], p_car2.priv.gearRatio[i])
    }
    COMP_ELEM(p_car1.priv.gearNb, p_car2.priv.gearNb)
    COMP_ELEM(p_car1.priv.gearOffset, p_car2.priv.gearOffset)
    for (int i = 0; i < 4; i++)
    {
        COMP_ELEM(p_car1.priv.skid[i], p_car2.priv.skid[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_ELEM(p_car1.priv.reaction[i], p_car2.priv.reaction[i])
    }
    COMP_ELEM(p_car1.priv.collision, p_car2.priv.collision)
    COMP_ELEM(p_car1.priv.simcollision, p_car2.priv.simcollision)
    COMP_ELEM(p_car1.priv.smoke, p_car2.priv.smoke)
    COMP_T3D(p_car1.priv.normal, p_car2.priv.normal)
    COMP_T3D(p_car1.priv.collpos, p_car2.priv.collpos)
    COMP_ELEM(p_car1.priv.dammage, p_car2.priv.dammage)
    COMP_ELEM(p_car1.priv.debug, p_car2.priv.debug)
    COMP_ELEM(p_car1.priv.collision_state.collision_count, p_car2.priv.collision_state.collision_count)
    for (int i = 0; i < 3; i++)
    {
        COMP_ELEM(p_car1.priv.collision_state.pos[i], p_car2.priv.collision_state.pos[i])
        COMP_ELEM(p_car1.priv.collision_state.force[i], p_car2.priv.collision_state.force[i])
    }
    COMP_ELEM(p_car1.priv.localPressure, p_car2.priv.localPressure)
    // COPY NOT IMPLEMENTED FOR car.priv.memoryPool
    COMP_ELEM(p_car1.priv.driveSkill, p_car2.priv.driveSkill)
    COMP_ELEM(p_car1.priv.steerTqCenter, p_car2.priv.steerTqCenter)
    COMP_ELEM(p_car1.priv.steerTqAlign, p_car2.priv.steerTqAlign)
    for (int i = 0; i < NR_DI_INSTANT; i++)
    {
        COMP_ELEM(p_car1.priv.dashboardInstant[i].type, p_car2.priv.dashboardInstant[i].type)
        // COPY NOT IMPLEMENTED FOR dashboardItem.setup
    }
    COMP_ELEM(p_car1.priv.dashboardInstantNb, p_car2.priv.dashboardInstantNb)
    for (int i = 0; i < NR_DI_REQUEST; i++)
    {
        COMP_ELEM(p_car1.priv.dashboardRequest[i].type, p_car2.priv.dashboardRequest[i].type)
        // COPY NOT IMPLEMENTED FOR dashboardItem.setup
    }
    COMP_ELEM(p_car1.priv.dashboardRequestNb, p_car2.priv.dashboardRequestNb)
    COMP_ELEM(p_car1.priv.dashboardActiveItem, p_car2.priv.dashboardActiveItem)

    // Compare car.ctrl
    COMP_ELEM(p_car1.ctrl.steer, p_car2.ctrl.steer)
    COMP_ELEM(p_car1.ctrl.accelCmd, p_car2.ctrl.accelCmd)
    COMP_ELEM(p_car1.ctrl.brakeCmd, p_car2.ctrl.brakeCmd)
    COMP_ELEM(p_car1.ctrl.clutchCmd, p_car2.ctrl.clutchCmd)
    COMP_ELEM(p_car1.ctrl.brakeFrontRightCmd, p_car2.ctrl.brakeFrontRightCmd)
    COMP_ELEM(p_car1.ctrl.brakeFrontLeftCmd, p_car2.ctrl.brakeFrontLeftCmd)
    COMP_ELEM(p_car1.ctrl.brakeRearRightCmd, p_car2.ctrl.brakeRearRightCmd)
    COMP_ELEM(p_car1.ctrl.brakeRearLeftCmd, p_car2.ctrl.brakeRearLeftCmd)
    COMP_ELEM(p_car1.ctrl.wingFrontCmd, p_car2.ctrl.wingFrontCmd)
    COMP_ELEM(p_car1.ctrl.wingRearCmd, p_car2.ctrl.wingRearCmd)
    COMP_ELEM(p_car1.ctrl.reserved1, p_car2.ctrl.reserved1)
    COMP_ELEM(p_car1.ctrl.reserved2, p_car2.ctrl.reserved2)
    COMP_ELEM(p_car1.ctrl.gear, p_car2.ctrl.gear)
    COMP_ELEM(p_car1.ctrl.raceCmd, p_car2.ctrl.raceCmd)
    COMP_ELEM(p_car1.ctrl.lightCmd, p_car2.ctrl.lightCmd)
    COMP_ELEM(p_car1.ctrl.ebrakeCmd, p_car2.ctrl.ebrakeCmd)
    COMP_ELEM(p_car1.ctrl.wingControlMode, p_car2.ctrl.wingControlMode)
    COMP_ELEM(p_car1.ctrl.singleWheelBrakeMode, p_car2.ctrl.singleWheelBrakeMode)
    COMP_ELEM(p_car1.ctrl.switch3, p_car2.ctrl.switch3)
    COMP_ELEM(p_car1.ctrl.telemetryMode, p_car2.ctrl.telemetryMode)
    for (int i = 0; i < 4; i++)
    {
        COMP_NAME(p_car1.ctrl.msg[i], p_car2.ctrl.msg[i])
        COMP_ELEM(p_car1.ctrl.msgColor[i], p_car2.ctrl.msgColor[i])
    }
    // COPY NOT IMPLEMENTED FOR car.ctrl.setupChangeCmd

    // Compare car.setup
    COMP_TCARSETUPITEM(p_car1.setup.FRWeightRep, p_car2.setup.FRWeightRep)
    COMP_TCARSETUPITEM(p_car1.setup.FRLWeightRep, p_car2.setup.FRLWeightRep)
    COMP_TCARSETUPITEM(p_car1.setup.RRLWeightRep, p_car2.setup.RRLWeightRep)
    COMP_TCARSETUPITEM(p_car1.setup.fuel, p_car2.setup.fuel)
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.wingAngle[i], p_car2.setup.wingAngle[i])
    }
    COMP_TCARSETUPITEM(p_car1.setup.revsLimiter, p_car2.setup.revsLimiter)
    for (int i = 0; i < MAX_GEARS; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.gearRatio[i], p_car2.setup.gearRatio[i])
    }
    for (int i = 0; i < 3; i++)
    {
        COMP_ELEM(p_car1.setup.differentialType[i], p_car2.setup.differentialType[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.differentialRatio[i], p_car2.setup.differentialRatio[i])
    }
    for (int i = 0; i < 3; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.differentialMinTqBias[i], p_car2.setup.differentialMinTqBias[i])
    }
    for (int i = 0; i < 3; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.differentialMaxTqBias[i], p_car2.setup.differentialMaxTqBias[i])
    }
    for (int i = 0; i < 3; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.differentialViscosity[i], p_car2.setup.differentialViscosity[i])
    }
    for (int i = 0; i < 3; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.differentialLockingTq[i], p_car2.setup.differentialLockingTq[i])
    }
    for (int i = 0; i < 3; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.differentialMaxSlipBias[i], p_car2.setup.differentialMaxSlipBias[i])
    }
    for (int i = 0; i < 3; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.differentialCoastMaxSlipBias[i], p_car2.setup.differentialCoastMaxSlipBias[i])
    }
    COMP_TCARSETUPITEM(p_car1.setup.steerLock, p_car2.setup.steerLock)
    COMP_TCARSETUPITEM(p_car1.setup.brakeRepartition, p_car2.setup.brakeRepartition)
    COMP_TCARSETUPITEM(p_car1.setup.brakePressure, p_car2.setup.brakePressure)
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.rideHeight[i], p_car2.setup.rideHeight[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.toe[i], p_car2.setup.toe[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.camber[i], p_car2.setup.camber[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.tirePressure[i], p_car2.setup.tirePressure[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.tireOpLoad[i], p_car2.setup.tireOpLoad[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.arbSpring[i], p_car2.setup.arbSpring[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.arbBellcrank[i], p_car2.setup.arbBellcrank[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.heaveSpring[i], p_car2.setup.heaveSpring[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.heaveBellcrank[i], p_car2.setup.heaveBellcrank[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.heaveInertance[i], p_car2.setup.heaveInertance[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.heaveFastBump[i], p_car2.setup.heaveFastBump[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.heaveSlowBump[i], p_car2.setup.heaveSlowBump[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.heaveBumpLvel[i], p_car2.setup.heaveBumpLvel[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.heaveFastRebound[i], p_car2.setup.heaveFastRebound[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.heaveSlowRebound[i], p_car2.setup.heaveSlowRebound[i])
    }
    for (int i = 0; i < 2; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.heaveReboundLvel[i], p_car2.setup.heaveReboundLvel[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.suspSpring[i], p_car2.setup.suspSpring[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.suspBellcrank[i], p_car2.setup.suspBellcrank[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.suspInertance[i], p_car2.setup.suspInertance[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.suspCourse[i], p_car2.setup.suspCourse[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.suspPacker[i], p_car2.setup.suspPacker[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.suspFastBump[i], p_car2.setup.suspFastBump[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.suspSlowBump[i], p_car2.setup.suspSlowBump[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.suspBumpLvel[i], p_car2.setup.suspBumpLvel[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.suspFastRebound[i], p_car2.setup.suspFastRebound[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.suspSlowRebound[i], p_car2.setup.suspSlowRebound[i])
    }
    for (int i = 0; i < 4; i++)
    {
        COMP_TCARSETUPITEM(p_car1.setup.suspReboundLvel[i], p_car2.setup.suspReboundLvel[i])
    }
    COMP_TCARSETUPITEM(p_car1.setup.reqRepair, p_car2.setup.reqRepair)
    COMP_TCARSETUPITEM(p_car1.setup.reqTireset, p_car2.setup.reqTireset)
    COMP_TCARSETUPITEM(p_car1.setup.reqPenalty, p_car2.setup.reqPenalty)

    // Compare car.pitcmd
    COMP_ELEM(p_car1.pitcmd.fuel, p_car2.pitcmd.fuel)
    COMP_ELEM(p_car1.pitcmd.repair, p_car2.pitcmd.repair)
    COMP_ELEM(p_car1.pitcmd.stopType, p_car2.pitcmd.stopType)
    COMP_ELEM(p_car1.pitcmd.setupChanged, p_car2.pitcmd.setupChanged)
    COMP_ELEM(p_car1.pitcmd.tireChange, p_car2.pitcmd.tireChange)

    // COPY NOT IMPLEMENTED FOR car.robot

    // COPY NOT IMPLEMENTED FOR car.nex
}

/// @brief                  Compares the values of two situation, not any of the values inside elements pointed to by a pointer
/// @param p_situation1     Situation 1 to compare
/// @param p_situation2     Situation 2 to compare
/// @param p_comparisonType Whether to compare situations for equality by values (COMP_UTIL_VALUE_EQUALITY) or inequality of pointers (COMP_UTIL_POINTER_INEQUALITY)
/// (the latter is only really relevant for testing BlackBoxData() works)
void CompareSituations(tSituation& p_situation1, tSituation& p_situation2, bool p_comparisonType)
{
    COMP_ELEM(p_situation1.raceInfo.ncars, p_situation2.raceInfo.ncars)
    COMP_ELEM(p_situation1.raceInfo.totLaps, p_situation2.raceInfo.totLaps)
    COMP_ELEM(p_situation1.raceInfo.extraLaps, p_situation2.raceInfo.extraLaps)
    COMP_ELEM(p_situation1.raceInfo.totTime, p_situation2.raceInfo.totTime)
    COMP_ELEM(p_situation1.raceInfo.state, p_situation2.raceInfo.state)
    COMP_ELEM(p_situation1.raceInfo.type, p_situation2.raceInfo.type)
    COMP_ELEM(p_situation1.raceInfo.maxDammage, p_situation2.raceInfo.maxDammage)
    COMP_ELEM(p_situation1.raceInfo.fps, p_situation2.raceInfo.fps)
    COMP_ELEM(p_situation1.raceInfo.features, p_situation2.raceInfo.features)
    COMP_ELEM(p_situation1.deltaTime, p_situation2.deltaTime)
    COMP_ELEM(p_situation1.currentTime, p_situation2.currentTime)
    COMP_ELEM(p_situation1.accelTime, p_situation2.accelTime)
    COMP_ELEM(p_situation1.nbPlayers, p_situation2.nbPlayers)
    // COPY NOT IMPLEMENTED FOR situation.cars
}

/// @brief                  Compares the values of two segment pointers, continuing through seg.next
/// @param p_segments1      Segment pointer 1 to compare
/// @param p_segments2      Segment pointer 2 to compare
/// @param p_segmentsCount  The amount of segments to check
/// @param p_comparisonType Whether to compare segments for equality by values (COMP_UTIL_VALUE_EQUALITY) or inequality of pointers (COMP_UTIL_POINTER_INEQUALITY)
/// (the latter is only really relevant for testing BlackBoxData() works)
void CompareSegments(tTrackSeg* p_segments1, tTrackSeg* p_segments2, int p_segmentsCount, bool p_comparisonType)
{
    if (p_segments1 && p_segments2)
    {
        tTrackSeg* segOrig = &p_segments1[0];
        tTrackSeg* segCopy = &p_segments2[0];
        for (int i = 0; i < p_segmentsCount; i++)
        {
            // Input validation was added after being extracted to a utility function from BlackBoxDataTests
            // Previously unnecessary as the use case there guaranteed segments of the same length
            if (!segOrig || !segCopy) throw std::invalid_argument("one or both of the segments is shorter than p_segmentCount");
            // DEEP COPY NOT IMPLEMENTED FOR seg.name
            COMP_ELEM((*segOrig).id, (*segCopy).id)
            COMP_ELEM((*segOrig).type, (*segCopy).type)
            COMP_ELEM((*segOrig).type2, (*segCopy).type2)
            COMP_ELEM((*segOrig).style, (*segCopy).style)
            COMP_ELEM((*segOrig).length, (*segCopy).length)
            COMP_ELEM((*segOrig).SpeedLimit, (*segCopy).SpeedLimit)
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