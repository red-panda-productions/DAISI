#include "BlackBoxData.h"

#include "robot.h"

/**
 * Constructs a data type for holding data provided to the black box
 * Makes deep copies of the provided data
 * @param p_car       The car data in the simulation
 * @param p_situation The environment data in the simulation
 * @param p_tickCount The tick that this driving simulation is from
 */
BlackBoxData::BlackBoxData(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount, tTrackSeg* p_nextSegments, int p_nextSegmentsCount)
    : Car(*p_car),
      Situation(*p_situation),
      TickCount(p_tickCount)
{
    Car.index = p_car->index;

    // Copy p_car.info
#define p_info p_car->info
    for (int i = 0; i < MAX_NAME_LEN; i++)
    {
        Car.info.name[i] = p_info.name[i];
    }

    for (int i = 0; i < MAX_NAME_LEN; i++)
    {
        Car.info.sname[i] = p_info.sname[i];
    }

    for (int i = 0; i < 4; i++)
    {
        Car.info.codename[i] = p_info.codename[i];
    }

    for (int i = 0; i < MAX_NAME_LEN; i++)
    {
        Car.info.teamname[i] = p_info.teamname[i];
    }

    for (int i = 0; i < MAX_NAME_LEN; i++)
    {
        Car.info.carName[i] = p_info.carName[i];
    }

    for (int i = 0; i < MAX_NAME_LEN; i++)
    {
        Car.info.category[i] = p_info.category[i];
    }
    Car.info.raceNumber = p_info.raceNumber;
    Car.info.startRank = p_info.startRank;
    Car.info.driverType = p_info.driverType;
    Car.info.networkplayer = p_info.networkplayer;
    Car.info.skillLevel = p_info.skillLevel;

    for (int i = 0; i < 3; i++)
    {
        Car.info.iconColor[i] = p_info.iconColor[i];
    }
    Car.info.dimension.x = p_info.dimension.x;
    Car.info.dimension.y = p_info.dimension.y;
    Car.info.dimension.z = p_info.dimension.z;
    Car.info.drvPos.x = p_info.drvPos.x;
    Car.info.drvPos.y = p_info.drvPos.y;
    Car.info.drvPos.z = p_info.drvPos.z;
    Car.info.bonnetPos.x = p_info.bonnetPos.x;
    Car.info.bonnetPos.y = p_info.bonnetPos.y;
    Car.info.bonnetPos.z = p_info.bonnetPos.z;
    Car.info.tank = p_info.tank;
    Car.info.steerLock = p_info.steerLock;
    Car.info.statGC.x = p_info.statGC.x;
    Car.info.statGC.y = p_info.statGC.y;
    Car.info.statGC.z = p_info.statGC.z;

    for (int i = 0; i < 4; i++)
    {
        Car.info.wheel[i].rimRadius = p_info.wheel[i].rimRadius;
        Car.info.wheel[i].tireHeight = p_info.wheel[i].tireHeight;
        Car.info.wheel[i].tireWidth = p_info.wheel[i].tireWidth;
        Car.info.wheel[i].brakeDiskRadius = p_info.wheel[i].brakeDiskRadius;
        Car.info.wheel[i].wheelRadius = p_info.wheel[i].wheelRadius;
    }

    Car.info.visualAttr.exhaustNb = p_info.visualAttr.exhaustNb;

    for (int i = 0; i < 2; i++)
    {
        Car.info.visualAttr.exhaustPos[i].x = p_info.visualAttr.exhaustPos[i].x;
        Car.info.visualAttr.exhaustPos[i].y = p_info.visualAttr.exhaustPos[i].y;
        Car.info.visualAttr.exhaustPos[i].z = p_info.visualAttr.exhaustPos[i].z;
    }

    Car.info.visualAttr.exhaustPower = p_info.visualAttr.exhaustPower;

    for (int i = 0; i < MAX_NAME_LEN; i++)
    {
        Car.info.masterModel[i] = p_info.masterModel[i];
    }

    for (int i = 0; i < MAX_NAME_LEN; i++)
    {
        Car.info.skinName[i] = p_info.skinName[i];
    }
    Car.info.skinTargets = p_info.skinTargets;

    // Copy p_car.pub
#define p_pub p_car->pub
    Car.pub.DynGC.pos.x = p_pub.DynGC.pos.x;
    Car.pub.DynGC.pos.y = p_pub.DynGC.pos.y;
    Car.pub.DynGC.pos.z = p_pub.DynGC.pos.z;
    Car.pub.DynGC.pos.xy = p_pub.DynGC.pos.xy;
    Car.pub.DynGC.pos.ax = p_pub.DynGC.pos.ax;
    Car.pub.DynGC.pos.ay = p_pub.DynGC.pos.ay;
    Car.pub.DynGC.pos.az = p_pub.DynGC.pos.az;
    Car.pub.DynGC.vel.x = p_pub.DynGC.vel.x;
    Car.pub.DynGC.vel.y = p_pub.DynGC.vel.y;
    Car.pub.DynGC.vel.z = p_pub.DynGC.vel.z;
    Car.pub.DynGC.vel.xy = p_pub.DynGC.vel.xy;
    Car.pub.DynGC.vel.ax = p_pub.DynGC.vel.ax;
    Car.pub.DynGC.vel.ay = p_pub.DynGC.vel.ay;
    Car.pub.DynGC.vel.az = p_pub.DynGC.vel.az;
    Car.pub.DynGC.acc.x = p_pub.DynGC.acc.x;
    Car.pub.DynGC.acc.y = p_pub.DynGC.acc.y;
    Car.pub.DynGC.acc.z = p_pub.DynGC.acc.z;
    Car.pub.DynGC.acc.xy = p_pub.DynGC.acc.xy;
    Car.pub.DynGC.acc.ax = p_pub.DynGC.acc.ax;
    Car.pub.DynGC.acc.ay = p_pub.DynGC.acc.ay;
    Car.pub.DynGC.acc.az = p_pub.DynGC.acc.az;

    Car.pub.DynGCg.pos.x = p_pub.DynGCg.pos.x;
    Car.pub.DynGCg.pos.y = p_pub.DynGCg.pos.y;
    Car.pub.DynGCg.pos.z = p_pub.DynGCg.pos.z;
    Car.pub.DynGCg.pos.xy = p_pub.DynGCg.pos.xy;
    Car.pub.DynGCg.pos.ax = p_pub.DynGCg.pos.ax;
    Car.pub.DynGCg.pos.ay = p_pub.DynGCg.pos.ay;
    Car.pub.DynGCg.pos.az = p_pub.DynGCg.pos.az;
    Car.pub.DynGCg.vel.x = p_pub.DynGCg.vel.x;
    Car.pub.DynGCg.vel.y = p_pub.DynGCg.vel.y;
    Car.pub.DynGCg.vel.z = p_pub.DynGCg.vel.z;
    Car.pub.DynGCg.vel.xy = p_pub.DynGCg.vel.xy;
    Car.pub.DynGCg.vel.ax = p_pub.DynGCg.vel.ax;
    Car.pub.DynGCg.vel.ay = p_pub.DynGCg.vel.ay;
    Car.pub.DynGCg.vel.az = p_pub.DynGCg.vel.az;
    Car.pub.DynGCg.acc.x = p_pub.DynGCg.acc.x;
    Car.pub.DynGCg.acc.y = p_pub.DynGCg.acc.y;
    Car.pub.DynGCg.acc.z = p_pub.DynGCg.acc.z;
    Car.pub.DynGCg.acc.xy = p_pub.DynGCg.acc.xy;
    Car.pub.DynGCg.acc.ax = p_pub.DynGCg.acc.ax;
    Car.pub.DynGCg.acc.ay = p_pub.DynGCg.acc.ay;
    Car.pub.DynGCg.acc.az = p_pub.DynGCg.acc.az;

    Car.pub.speed = p_pub.speed;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            Car.pub.posMat[i][j] = p_pub.posMat[i][j];
        }
    }

    bool skip = false;
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
    }

    Car.pub.trkPos.type = p_pub.trkPos.type;
    Car.pub.trkPos.toStart = p_pub.trkPos.toStart;
    Car.pub.trkPos.toRight = p_pub.trkPos.toRight;
    Car.pub.trkPos.toMiddle = p_pub.trkPos.toMiddle;
    Car.pub.trkPos.toLeft = p_pub.trkPos.toLeft;
    Car.pub.state = p_pub.state;

    for (int i = 0; i < 4; i++)
    {
        Car.pub.corner[i].x = p_pub.corner[i].x;
        Car.pub.corner[i].y = p_pub.corner[i].y;
        Car.pub.corner[i].z = p_pub.corner[i].z;
        Car.pub.corner[i].xy = p_pub.corner[i].xy;
        Car.pub.corner[i].ax = p_pub.corner[i].ax;
        Car.pub.corner[i].ay = p_pub.corner[i].ay;
        Car.pub.corner[i].az = p_pub.corner[i].az;
    }
    Car.pub.glance = p_pub.glance;
    Car.pub.oldglance = p_pub.oldglance;

    // Copy p_car.race
#define p_race p_car->race
    Car.race.bestLapTime = p_race.bestLapTime;
    Car.race.commitBestLapTime = p_race.commitBestLapTime;

    // TODO: best split time

    Car.race.deltaBestLapTime = p_race.deltaBestLapTime;
    Car.race.curLapTime = p_race.curLapTime;

    // TODO: current split time

    Car.race.lastLapTime = p_race.lastLapTime;
    Car.race.curTime = p_race.curTime;
    Car.race.topSpeed = p_race.topSpeed;
    Car.race.currentMinSpeedForLap = p_race.currentMinSpeedForLap;
    Car.race.laps = p_race.laps;
    Car.race.bestLap = p_race.bestLap;
    Car.race.nbPitStops = p_race.nbPitStops;
    Car.race.remainingLaps = p_race.remainingLaps;
    Car.race.pos = p_race.pos;
    Car.race.timeBehindLeader = p_race.timeBehindLeader;
    Car.race.lapsBehindLeader = p_race.lapsBehindLeader;
    Car.race.timeBehindPrev = p_race.timeBehindPrev;
    Car.race.timeBeforeNext = p_race.timeBeforeNext;
    Car.race.distRaced = p_race.distRaced;
    Car.race.distFromStartLine = p_race.distFromStartLine;
    Car.race.currentSector = p_race.currentSector;
    Car.race.nbSectors = p_race.nbSectors;
    Car.race.trackLength = p_race.trackLength;
    Car.race.scheduledEventTime = p_race.scheduledEventTime;
    Car.race.pit = nullptr;  // TODO *pit
    Car.race.event = p_race.event;
    // TODO penaltyList
    Car.race.penaltyTime = p_race.penaltyTime;
    Car.race.prevFromStartLine = p_race.prevFromStartLine;
    Car.race.wrongWayTime = p_race.wrongWayTime;

    // Copy p_car.priv
#define p_priv p_car->priv
    Car.priv.paramsHandle = nullptr;  // TODO paramsHandle
    Car.priv.carHandle = nullptr;     // TODO carHandle
    Car.priv.driverIndex = p_priv.driverIndex;
    Car.priv.moduleIndex = p_priv.moduleIndex;

    for (int i = 0; i < 4; i++)
    {
        Car.priv.modName[i] = p_priv.modName[i];
    }
    // TODO wheel

    for (int i = 0; i < 4; i++)
    {
        Car.priv.corner[i].x = p_priv.corner[i].x;
        Car.priv.corner[i].y = p_priv.corner[i].y;
        Car.priv.corner[i].z = p_priv.corner[i].z;
        Car.priv.corner[i].xy = p_priv.corner[i].xy;
        Car.priv.corner[i].ax = p_priv.corner[i].ax;
        Car.priv.corner[i].ay = p_priv.corner[i].ay;
        Car.priv.corner[i].az = p_priv.corner[i].az;
    }
    Car.priv.gear = p_priv.gear;
    Car.priv.gearNext = p_priv.gearNext;
    Car.priv.fuel = p_priv.fuel;
    Car.priv.fuel_consumption_total = p_priv.fuel_consumption_total;
    Car.priv.fuel_consumption_instant = p_priv.fuel_consumption_instant;
    Car.priv.enginerpm = p_priv.enginerpm;
    Car.priv.enginerpmRedLine = p_priv.enginerpmRedLine;
    Car.priv.enginerpmMax = p_priv.enginerpmMax;
    Car.priv.enginerpmMaxTq = p_priv.enginerpmMaxTq;
    Car.priv.enginerpmMaxPw = p_priv.enginerpmMaxPw;
    Car.priv.engineMaxTq = p_priv.engineMaxTq;
    Car.priv.engineMaxPw = p_priv.engineMaxPw;

    for (int i = 0; i < MAX_GEARS; i++)
    {
        Car.priv.gearRatio[i] = p_priv.gearRatio[i];
    }
    Car.priv.gearNb = p_priv.gearNb;
    Car.priv.gearOffset = p_priv.gearOffset;

    for (int i = 0; i < 4; i++)
    {
        Car.priv.skid[i] = p_priv.skid[i];
    }

    for (int i = 0; i < 4; i++)
    {
        Car.priv.reaction[i] = p_priv.reaction[i];
    }
    Car.priv.collision = p_priv.collision;
    Car.priv.simcollision = p_priv.simcollision;
    Car.priv.smoke = p_priv.smoke;
    Car.priv.normal.x = p_priv.normal.x;
    Car.priv.normal.y = p_priv.normal.y;
    Car.priv.normal.z = p_priv.normal.z;
    Car.priv.collpos.x = p_priv.collpos.x;
    Car.priv.collpos.y = p_priv.collpos.y;
    Car.priv.collpos.z = p_priv.collpos.z;
    Car.priv.dammage = p_priv.dammage;
    Car.priv.debug = p_priv.debug;
    Car.priv.collision_state.collision_count = p_priv.collision_state.collision_count;

    for (int i = 0; i < 3; i++)
    {
        Car.priv.collision_state.pos[i] = p_priv.collision_state.pos[i];
    }

    for (int i = 0; i < 3; i++)
    {
        Car.priv.collision_state.force[i] = p_priv.collision_state.force[i];
    }
    Car.priv.localPressure = p_priv.localPressure;

    // TODO memoryPool

    Car.priv.driveSkill = p_priv.driveSkill;
    Car.priv.steerTqCenter = p_priv.steerTqCenter;
    Car.priv.steerTqAlign = p_priv.steerTqAlign;

    for (int i = 0; i < NR_DI_INSTANT; i++)
    {
        Car.priv.dashboardInstant[i] = p_priv.dashboardInstant[i];  // TODO: check if this copy works
    }
    Car.priv.dashboardInstantNb = p_priv.dashboardInstantNb;

    for (int i = 0; i < NR_DI_REQUEST; i++)
    {
        Car.priv.dashboardRequest[i] = p_priv.dashboardRequest[i];  // TODO: check if this copy works
    }
    Car.priv.dashboardRequestNb = p_priv.dashboardRequestNb;
    Car.priv.dashboardActiveItem = p_priv.dashboardActiveItem;

    // Copy p_car.ctrl
#define p_ctrl p_car->ctrl
    Car.ctrl.steer = p_ctrl.steer;
    Car.ctrl.accelCmd = p_ctrl.accelCmd;
    Car.ctrl.brakeCmd = p_ctrl.brakeCmd;
    Car.ctrl.clutchCmd = p_ctrl.clutchCmd;
    Car.ctrl.brakeFrontLeftCmd = p_ctrl.brakeFrontLeftCmd;
    Car.ctrl.brakeFrontRightCmd = p_ctrl.brakeFrontRightCmd;
    Car.ctrl.brakeRearLeftCmd = p_ctrl.brakeRearLeftCmd;
    Car.ctrl.brakeRearRightCmd = p_ctrl.brakeRearRightCmd;
    Car.ctrl.wingFrontCmd = p_ctrl.wingFrontCmd;
    Car.ctrl.wingRearCmd = p_ctrl.wingRearCmd;
    Car.ctrl.reserved1 = p_ctrl.reserved1;
    Car.ctrl.reserved2 = p_ctrl.reserved2;
    Car.ctrl.gear = p_ctrl.gear;
    Car.ctrl.raceCmd = p_ctrl.raceCmd;
    Car.ctrl.lightCmd = p_ctrl.lightCmd;
    Car.ctrl.ebrakeCmd = p_ctrl.ebrakeCmd;
    Car.ctrl.wingControlMode = p_ctrl.wingControlMode;
    Car.ctrl.singleWheelBrakeMode = p_ctrl.singleWheelBrakeMode;
    Car.ctrl.switch3 = p_ctrl.switch3;
    Car.ctrl.telemetryMode = p_ctrl.telemetryMode;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < RM_CMD_MAX_MSG_SIZE; j++)
        {
            Car.ctrl.msg[i][j] = p_ctrl.msg[i][j];
        }
    }

    for (int i = 0; i < 4; i++)
    {
        Car.ctrl.msgColor[i] = p_ctrl.msgColor[i];
    }

    // TODO: this might be an array which will need to be checked
    // tDashboardItem* ctrlDashboardItem = new tDashboardItem();
    // ctrlDashboardItem->type = p_ctrl.setupChangeCmd->type;
    // tCarSetupItem* ctrlCarSetupItem = new tCarSetupItem();
    // ctrlCarSetupItem->value = p_ctrl.setupChangeCmd->setup->value;
    // ctrlCarSetupItem->min = p_ctrl.setupChangeCmd->setup->min;
    // ctrlCarSetupItem->max = p_ctrl.setupChangeCmd->setup->max;
    // ctrlCarSetupItem->desired_value = p_ctrl.setupChangeCmd->setup->desired_value;
    // ctrlCarSetupItem->stepsize = p_ctrl.setupChangeCmd->setup->stepsize;
    // ctrlCarSetupItem->changed = p_ctrl.setupChangeCmd->setup->changed;
    // ctrlDashboardItem->setup = ctrlCarSetupItem;
    // Car.ctrl.setupChangeCmd = ctrlDashboardItem;

    // Copy p_car.setup
#define p_setup p_car->setup

    Car.setup.FRWeightRep.value = p_setup.FRWeightRep.value;
    Car.setup.FRWeightRep.min = p_setup.FRWeightRep.min;
    Car.setup.FRWeightRep.max = p_setup.FRWeightRep.max;
    Car.setup.FRWeightRep.desired_value = p_setup.FRWeightRep.desired_value;
    Car.setup.FRWeightRep.stepsize = p_setup.FRWeightRep.stepsize;
    Car.setup.FRWeightRep.changed = p_setup.FRWeightRep.changed;

    Car.setup.FRLWeightRep.value = p_setup.FRLWeightRep.value;
    Car.setup.FRLWeightRep.min = p_setup.FRLWeightRep.min;
    Car.setup.FRLWeightRep.max = p_setup.FRLWeightRep.max;
    Car.setup.FRLWeightRep.desired_value = p_setup.FRLWeightRep.desired_value;
    Car.setup.FRLWeightRep.stepsize = p_setup.FRLWeightRep.stepsize;
    Car.setup.FRLWeightRep.changed = p_setup.FRLWeightRep.changed;

    Car.setup.RRLWeightRep.value = p_setup.RRLWeightRep.value;
    Car.setup.RRLWeightRep.min = p_setup.RRLWeightRep.min;
    Car.setup.RRLWeightRep.max = p_setup.RRLWeightRep.max;
    Car.setup.RRLWeightRep.desired_value = p_setup.RRLWeightRep.desired_value;
    Car.setup.RRLWeightRep.stepsize = p_setup.RRLWeightRep.stepsize;
    Car.setup.RRLWeightRep.changed = p_setup.RRLWeightRep.changed;

    Car.setup.fuel.value = p_setup.fuel.value;
    Car.setup.fuel.min = p_setup.fuel.min;
    Car.setup.fuel.max = p_setup.fuel.max;
    Car.setup.fuel.desired_value = p_setup.fuel.desired_value;
    Car.setup.fuel.stepsize = p_setup.fuel.stepsize;
    Car.setup.fuel.changed = p_setup.fuel.changed;

    for (int i = 0; i < 2; i++)
    {
        Car.setup.wingAngle[i].value = p_setup.wingAngle[i].value;
        Car.setup.wingAngle[i].min = p_setup.wingAngle[i].min;
        Car.setup.wingAngle[i].max = p_setup.wingAngle[i].max;
        Car.setup.wingAngle[i].desired_value = p_setup.wingAngle[i].desired_value;
        Car.setup.wingAngle[i].stepsize = p_setup.wingAngle[i].stepsize;
        Car.setup.wingAngle[i].changed = p_setup.wingAngle[i].changed;
    }

    Car.setup.revsLimiter.value = p_setup.revsLimiter.value;
    Car.setup.revsLimiter.min = p_setup.revsLimiter.min;
    Car.setup.revsLimiter.max = p_setup.revsLimiter.max;
    Car.setup.revsLimiter.desired_value = p_setup.revsLimiter.desired_value;
    Car.setup.revsLimiter.stepsize = p_setup.revsLimiter.stepsize;
    Car.setup.revsLimiter.changed = p_setup.revsLimiter.changed;

    for (int i = 0; i < MAX_GEARS; i++)
    {
        Car.setup.gearRatio[i].value = p_setup.gearRatio[i].value;
        Car.setup.gearRatio[i].min = p_setup.gearRatio[i].min;
        Car.setup.gearRatio[i].max = p_setup.gearRatio[i].max;
        Car.setup.gearRatio[i].desired_value = p_setup.gearRatio[i].desired_value;
        Car.setup.gearRatio[i].stepsize = p_setup.gearRatio[i].stepsize;
        Car.setup.gearRatio[i].changed = p_setup.gearRatio[i].changed;
    }

    for (int i = 0; i < 3; i++)
    {
        Car.setup.differentialType[i] = p_setup.differentialType[i];
    }

    for (int i = 0; i < 3; i++)
    {
        Car.setup.differentialRatio[i].value = p_setup.differentialRatio[i].value;
        Car.setup.differentialRatio[i].min = p_setup.differentialRatio[i].min;
        Car.setup.differentialRatio[i].max = p_setup.differentialRatio[i].max;
        Car.setup.differentialRatio[i].desired_value = p_setup.differentialRatio[i].desired_value;
        Car.setup.differentialRatio[i].stepsize = p_setup.differentialRatio[i].stepsize;
        Car.setup.differentialRatio[i].changed = p_setup.differentialRatio[i].changed;
    }

    for (int i = 0; i < 3; i++)
    {
        Car.setup.differentialMinTqBias[i].value = p_setup.differentialMinTqBias[i].value;
        Car.setup.differentialMinTqBias[i].min = p_setup.differentialMinTqBias[i].min;
        Car.setup.differentialMinTqBias[i].max = p_setup.differentialMinTqBias[i].max;
        Car.setup.differentialMinTqBias[i].desired_value = p_setup.differentialMinTqBias[i].desired_value;
        Car.setup.differentialMinTqBias[i].stepsize = p_setup.differentialMinTqBias[i].stepsize;
        Car.setup.differentialMinTqBias[i].changed = p_setup.differentialMinTqBias[i].changed;
    }

    for (int i = 0; i < 3; i++)
    {
        Car.setup.differentialMaxTqBias[i].value = p_setup.differentialMaxTqBias[i].value;
        Car.setup.differentialMaxTqBias[i].min = p_setup.differentialMaxTqBias[i].min;
        Car.setup.differentialMaxTqBias[i].max = p_setup.differentialMaxTqBias[i].max;
        Car.setup.differentialMaxTqBias[i].desired_value = p_setup.differentialMaxTqBias[i].desired_value;
        Car.setup.differentialMaxTqBias[i].stepsize = p_setup.differentialMaxTqBias[i].stepsize;
        Car.setup.differentialMaxTqBias[i].changed = p_setup.differentialMaxTqBias[i].changed;
    }

    for (int i = 0; i < 3; i++)
    {
        Car.setup.differentialViscosity[i].value = p_setup.differentialViscosity[i].value;
        Car.setup.differentialViscosity[i].min = p_setup.differentialViscosity[i].min;
        Car.setup.differentialViscosity[i].max = p_setup.differentialViscosity[i].max;
        Car.setup.differentialViscosity[i].desired_value = p_setup.differentialViscosity[i].desired_value;
        Car.setup.differentialViscosity[i].stepsize = p_setup.differentialViscosity[i].stepsize;
        Car.setup.differentialViscosity[i].changed = p_setup.differentialViscosity[i].changed;
    }

    for (int i = 0; i < 3; i++)
    {
        Car.setup.differentialLockingTq[i].value = p_setup.differentialLockingTq[i].value;
        Car.setup.differentialLockingTq[i].min = p_setup.differentialLockingTq[i].min;
        Car.setup.differentialLockingTq[i].max = p_setup.differentialLockingTq[i].max;
        Car.setup.differentialLockingTq[i].desired_value = p_setup.differentialLockingTq[i].desired_value;
        Car.setup.differentialLockingTq[i].stepsize = p_setup.differentialLockingTq[i].stepsize;
        Car.setup.differentialLockingTq[i].changed = p_setup.differentialLockingTq[i].changed;
    }

    for (int i = 0; i < 3; i++)
    {
        Car.setup.differentialMaxSlipBias[i].value = p_setup.differentialMaxSlipBias[i].value;
        Car.setup.differentialMaxSlipBias[i].min = p_setup.differentialMaxSlipBias[i].min;
        Car.setup.differentialMaxSlipBias[i].max = p_setup.differentialMaxSlipBias[i].max;
        Car.setup.differentialMaxSlipBias[i].desired_value = p_setup.differentialMaxSlipBias[i].desired_value;
        Car.setup.differentialMaxSlipBias[i].stepsize = p_setup.differentialMaxSlipBias[i].stepsize;
        Car.setup.differentialMaxSlipBias[i].changed = p_setup.differentialMaxSlipBias[i].changed;
    }

    for (int i = 0; i < 3; i++)
    {
        Car.setup.differentialCoastMaxSlipBias[i].value = p_setup.differentialCoastMaxSlipBias[i].value;
        Car.setup.differentialCoastMaxSlipBias[i].min = p_setup.differentialCoastMaxSlipBias[i].min;
        Car.setup.differentialCoastMaxSlipBias[i].max = p_setup.differentialCoastMaxSlipBias[i].max;
        Car.setup.differentialCoastMaxSlipBias[i].desired_value = p_setup.differentialCoastMaxSlipBias[i].desired_value;
        Car.setup.differentialCoastMaxSlipBias[i].stepsize = p_setup.differentialCoastMaxSlipBias[i].stepsize;
        Car.setup.differentialCoastMaxSlipBias[i].changed = p_setup.differentialCoastMaxSlipBias[i].changed;
    }

    Car.setup.steerLock.value = p_setup.steerLock.value;
    Car.setup.steerLock.min = p_setup.steerLock.min;
    Car.setup.steerLock.max = p_setup.steerLock.max;
    Car.setup.steerLock.desired_value = p_setup.steerLock.desired_value;
    Car.setup.steerLock.stepsize = p_setup.steerLock.stepsize;
    Car.setup.steerLock.changed = p_setup.steerLock.changed;

    Car.setup.brakeRepartition.value = p_setup.brakeRepartition.value;
    Car.setup.brakeRepartition.min = p_setup.brakeRepartition.min;
    Car.setup.brakeRepartition.max = p_setup.brakeRepartition.max;
    Car.setup.brakeRepartition.desired_value = p_setup.brakeRepartition.desired_value;
    Car.setup.brakeRepartition.stepsize = p_setup.brakeRepartition.stepsize;
    Car.setup.brakeRepartition.changed = p_setup.brakeRepartition.changed;

    Car.setup.brakePressure.value = p_setup.brakePressure.value;
    Car.setup.brakePressure.min = p_setup.brakePressure.min;
    Car.setup.brakePressure.max = p_setup.brakePressure.max;
    Car.setup.brakePressure.desired_value = p_setup.brakePressure.desired_value;
    Car.setup.brakePressure.stepsize = p_setup.brakePressure.stepsize;
    Car.setup.brakePressure.changed = p_setup.brakePressure.changed;

    for (int i = 0; i < 4; i++)
    {
        Car.setup.rideHeight[i].value = p_setup.rideHeight[i].value;
        Car.setup.rideHeight[i].min = p_setup.rideHeight[i].min;
        Car.setup.rideHeight[i].max = p_setup.rideHeight[i].max;
        Car.setup.rideHeight[i].desired_value = p_setup.rideHeight[i].desired_value;
        Car.setup.rideHeight[i].stepsize = p_setup.rideHeight[i].stepsize;
        Car.setup.rideHeight[i].changed = p_setup.rideHeight[i].changed;
    }

    for (int i = 0; i < 4; i++)
    {
        Car.setup.toe[i].value = p_setup.toe[i].value;
        Car.setup.toe[i].min = p_setup.toe[i].min;
        Car.setup.toe[i].max = p_setup.toe[i].max;
        Car.setup.toe[i].desired_value = p_setup.toe[i].desired_value;
        Car.setup.toe[i].stepsize = p_setup.toe[i].stepsize;
        Car.setup.toe[i].changed = p_setup.toe[i].changed;
    }

    for (int i = 0; i < 4; i++)
    {
        Car.setup.camber[i].value = p_setup.camber[i].value;
        Car.setup.camber[i].min = p_setup.camber[i].min;
        Car.setup.camber[i].max = p_setup.camber[i].max;
        Car.setup.camber[i].desired_value = p_setup.camber[i].desired_value;
        Car.setup.camber[i].stepsize = p_setup.camber[i].stepsize;
        Car.setup.camber[i].changed = p_setup.camber[i].changed;
    }

    for (int i = 0; i < 4; i++)
    {
        Car.setup.tirePressure[i].value = p_setup.tirePressure[i].value;
        Car.setup.tirePressure[i].min = p_setup.tirePressure[i].min;
        Car.setup.tirePressure[i].max = p_setup.tirePressure[i].max;
        Car.setup.tirePressure[i].desired_value = p_setup.tirePressure[i].desired_value;
        Car.setup.tirePressure[i].stepsize = p_setup.tirePressure[i].stepsize;
        Car.setup.tirePressure[i].changed = p_setup.tirePressure[i].changed;
    }

    for (int i = 0; i < 4; i++)
    {
        Car.setup.tireOpLoad[i].value = p_setup.tireOpLoad[i].value;
        Car.setup.tireOpLoad[i].min = p_setup.tireOpLoad[i].min;
        Car.setup.tireOpLoad[i].max = p_setup.tireOpLoad[i].max;
        Car.setup.tireOpLoad[i].desired_value = p_setup.tireOpLoad[i].desired_value;
        Car.setup.tireOpLoad[i].stepsize = p_setup.tireOpLoad[i].stepsize;
        Car.setup.tireOpLoad[i].changed = p_setup.tireOpLoad[i].changed;
    }

    for (int i = 0; i < 2; i++)
    {
        Car.setup.arbSpring[i].value = p_setup.arbSpring[i].value;
        Car.setup.arbSpring[i].min = p_setup.arbSpring[i].min;
        Car.setup.arbSpring[i].max = p_setup.arbSpring[i].max;
        Car.setup.arbSpring[i].desired_value = p_setup.arbSpring[i].desired_value;
        Car.setup.arbSpring[i].stepsize = p_setup.arbSpring[i].stepsize;
        Car.setup.arbSpring[i].changed = p_setup.arbSpring[i].changed;
    }

    for (int i = 0; i < 2; i++)
    {
        Car.setup.arbBellcrank[i].value = p_setup.arbBellcrank[i].value;
        Car.setup.arbBellcrank[i].min = p_setup.arbBellcrank[i].min;
        Car.setup.arbBellcrank[i].max = p_setup.arbBellcrank[i].max;
        Car.setup.arbBellcrank[i].desired_value = p_setup.arbBellcrank[i].desired_value;
        Car.setup.arbBellcrank[i].stepsize = p_setup.arbBellcrank[i].stepsize;
        Car.setup.arbBellcrank[i].changed = p_setup.arbBellcrank[i].changed;
    }

    for (int i = 0; i < 2; i++)
    {
        Car.setup.heaveSpring[i].value = p_setup.heaveSpring[i].value;
        Car.setup.heaveSpring[i].min = p_setup.heaveSpring[i].min;
        Car.setup.heaveSpring[i].max = p_setup.heaveSpring[i].max;
        Car.setup.heaveSpring[i].desired_value = p_setup.heaveSpring[i].desired_value;
        Car.setup.heaveSpring[i].stepsize = p_setup.heaveSpring[i].stepsize;
        Car.setup.heaveSpring[i].changed = p_setup.heaveSpring[i].changed;
    }

    for (int i = 0; i < 2; i++)
    {
        Car.setup.heaveBellcrank[i].value = p_setup.heaveBellcrank[i].value;
        Car.setup.heaveBellcrank[i].min = p_setup.heaveBellcrank[i].min;
        Car.setup.heaveBellcrank[i].max = p_setup.heaveBellcrank[i].max;
        Car.setup.heaveBellcrank[i].desired_value = p_setup.heaveBellcrank[i].desired_value;
        Car.setup.heaveBellcrank[i].stepsize = p_setup.heaveBellcrank[i].stepsize;
        Car.setup.heaveBellcrank[i].changed = p_setup.heaveBellcrank[i].changed;
    }

    for (int i = 0; i < 2; i++)
    {
        Car.setup.heaveInertance[i].value = p_setup.heaveInertance[i].value;
        Car.setup.heaveInertance[i].min = p_setup.heaveInertance[i].min;
        Car.setup.heaveInertance[i].max = p_setup.heaveInertance[i].max;
        Car.setup.heaveInertance[i].desired_value = p_setup.heaveInertance[i].desired_value;
        Car.setup.heaveInertance[i].stepsize = p_setup.heaveInertance[i].stepsize;
        Car.setup.heaveInertance[i].changed = p_setup.heaveInertance[i].changed;
    }

    for (int i = 0; i < 2; i++)
    {
        Car.setup.heaveFastBump[i].value = p_setup.heaveFastBump[i].value;
        Car.setup.heaveFastBump[i].min = p_setup.heaveFastBump[i].min;
        Car.setup.heaveFastBump[i].max = p_setup.heaveFastBump[i].max;
        Car.setup.heaveFastBump[i].desired_value = p_setup.heaveFastBump[i].desired_value;
        Car.setup.heaveFastBump[i].stepsize = p_setup.heaveFastBump[i].stepsize;
        Car.setup.heaveFastBump[i].changed = p_setup.heaveFastBump[i].changed;
    }

    for (int i = 0; i < 2; i++)
    {
        Car.setup.heaveSlowBump[i].value = p_setup.heaveSlowBump[i].value;
        Car.setup.heaveSlowBump[i].min = p_setup.heaveSlowBump[i].min;
        Car.setup.heaveSlowBump[i].max = p_setup.heaveSlowBump[i].max;
        Car.setup.heaveSlowBump[i].desired_value = p_setup.heaveSlowBump[i].desired_value;
        Car.setup.heaveSlowBump[i].stepsize = p_setup.heaveSlowBump[i].stepsize;
        Car.setup.heaveSlowBump[i].changed = p_setup.heaveSlowBump[i].changed;
    }

    for (int i = 0; i < 2; i++)
    {
        Car.setup.heaveBumpLvel[i].value = p_setup.heaveBumpLvel[i].value;
        Car.setup.heaveBumpLvel[i].min = p_setup.heaveBumpLvel[i].min;
        Car.setup.heaveBumpLvel[i].max = p_setup.heaveBumpLvel[i].max;
        Car.setup.heaveBumpLvel[i].desired_value = p_setup.heaveBumpLvel[i].desired_value;
        Car.setup.heaveBumpLvel[i].stepsize = p_setup.heaveBumpLvel[i].stepsize;
        Car.setup.heaveBumpLvel[i].changed = p_setup.heaveBumpLvel[i].changed;
    }

    for (int i = 0; i < 2; i++)
    {
        Car.setup.heaveFastRebound[i].value = p_setup.heaveFastRebound[i].value;
        Car.setup.heaveFastRebound[i].min = p_setup.heaveFastRebound[i].min;
        Car.setup.heaveFastRebound[i].max = p_setup.heaveFastRebound[i].max;
        Car.setup.heaveFastRebound[i].desired_value = p_setup.heaveFastRebound[i].desired_value;
        Car.setup.heaveFastRebound[i].stepsize = p_setup.heaveFastRebound[i].stepsize;
        Car.setup.heaveFastRebound[i].changed = p_setup.heaveFastRebound[i].changed;
    }

    for (int i = 0; i < 2; i++)
    {
        Car.setup.heaveSlowRebound[i].value = p_setup.heaveSlowRebound[i].value;
        Car.setup.heaveSlowRebound[i].min = p_setup.heaveSlowRebound[i].min;
        Car.setup.heaveSlowRebound[i].max = p_setup.heaveSlowRebound[i].max;
        Car.setup.heaveSlowRebound[i].desired_value = p_setup.heaveSlowRebound[i].desired_value;
        Car.setup.heaveSlowRebound[i].stepsize = p_setup.heaveSlowRebound[i].stepsize;
        Car.setup.heaveSlowRebound[i].changed = p_setup.heaveSlowRebound[i].changed;
    }

    for (int i = 0; i < 2; i++)
    {
        Car.setup.heaveReboundLvel[i].value = p_setup.heaveReboundLvel[i].value;
        Car.setup.heaveReboundLvel[i].min = p_setup.heaveReboundLvel[i].min;
        Car.setup.heaveReboundLvel[i].max = p_setup.heaveReboundLvel[i].max;
        Car.setup.heaveReboundLvel[i].desired_value = p_setup.heaveReboundLvel[i].desired_value;
        Car.setup.heaveReboundLvel[i].stepsize = p_setup.heaveReboundLvel[i].stepsize;
        Car.setup.heaveReboundLvel[i].changed = p_setup.heaveReboundLvel[i].changed;
    }

    for (int i = 0; i < 4; i++)
    {
        Car.setup.suspSpring[i].value = p_setup.suspSpring[i].value;
        Car.setup.suspSpring[i].min = p_setup.suspSpring[i].min;
        Car.setup.suspSpring[i].max = p_setup.suspSpring[i].max;
        Car.setup.suspSpring[i].desired_value = p_setup.suspSpring[i].desired_value;
        Car.setup.suspSpring[i].stepsize = p_setup.suspSpring[i].stepsize;
        Car.setup.suspSpring[i].changed = p_setup.suspSpring[i].changed;
    }

    for (int i = 0; i < 4; i++)
    {
        Car.setup.suspBellcrank[i].value = p_setup.suspBellcrank[i].value;
        Car.setup.suspBellcrank[i].min = p_setup.suspBellcrank[i].min;
        Car.setup.suspBellcrank[i].max = p_setup.suspBellcrank[i].max;
        Car.setup.suspBellcrank[i].desired_value = p_setup.suspBellcrank[i].desired_value;
        Car.setup.suspBellcrank[i].stepsize = p_setup.suspBellcrank[i].stepsize;
        Car.setup.suspBellcrank[i].changed = p_setup.suspBellcrank[i].changed;
    }

    for (int i = 0; i < 4; i++)
    {
        Car.setup.suspInertance[i].value = p_setup.suspInertance[i].value;
        Car.setup.suspInertance[i].min = p_setup.suspInertance[i].min;
        Car.setup.suspInertance[i].max = p_setup.suspInertance[i].max;
        Car.setup.suspInertance[i].desired_value = p_setup.suspInertance[i].desired_value;
        Car.setup.suspInertance[i].stepsize = p_setup.suspInertance[i].stepsize;
        Car.setup.suspInertance[i].changed = p_setup.suspInertance[i].changed;
    }

    for (int i = 0; i < 4; i++)
    {
        Car.setup.suspCourse[i].value = p_setup.suspCourse[i].value;
        Car.setup.suspCourse[i].min = p_setup.suspCourse[i].min;
        Car.setup.suspCourse[i].max = p_setup.suspCourse[i].max;
        Car.setup.suspCourse[i].desired_value = p_setup.suspCourse[i].desired_value;
        Car.setup.suspCourse[i].stepsize = p_setup.suspCourse[i].stepsize;
        Car.setup.suspCourse[i].changed = p_setup.suspCourse[i].changed;
    }

    for (int i = 0; i < 4; i++)
    {
        Car.setup.suspPacker[i].value = p_setup.suspPacker[i].value;
        Car.setup.suspPacker[i].min = p_setup.suspPacker[i].min;
        Car.setup.suspPacker[i].max = p_setup.suspPacker[i].max;
        Car.setup.suspPacker[i].desired_value = p_setup.suspPacker[i].desired_value;
        Car.setup.suspPacker[i].stepsize = p_setup.suspPacker[i].stepsize;
        Car.setup.suspPacker[i].changed = p_setup.suspPacker[i].changed;
    }

    for (int i = 0; i < 4; i++)
    {
        Car.setup.suspFastBump[i].value = p_setup.suspFastBump[i].value;
        Car.setup.suspFastBump[i].min = p_setup.suspFastBump[i].min;
        Car.setup.suspFastBump[i].max = p_setup.suspFastBump[i].max;
        Car.setup.suspFastBump[i].desired_value = p_setup.suspFastBump[i].desired_value;
        Car.setup.suspFastBump[i].stepsize = p_setup.suspFastBump[i].stepsize;
        Car.setup.suspFastBump[i].changed = p_setup.suspFastBump[i].changed;
    }

    for (int i = 0; i < 4; i++)
    {
        Car.setup.suspSlowBump[i].value = p_setup.suspSlowBump[i].value;
        Car.setup.suspSlowBump[i].min = p_setup.suspSlowBump[i].min;
        Car.setup.suspSlowBump[i].max = p_setup.suspSlowBump[i].max;
        Car.setup.suspSlowBump[i].desired_value = p_setup.suspSlowBump[i].desired_value;
        Car.setup.suspSlowBump[i].stepsize = p_setup.suspSlowBump[i].stepsize;
        Car.setup.suspSlowBump[i].changed = p_setup.suspSlowBump[i].changed;
    }

    for (int i = 0; i < 4; i++)
    {
        Car.setup.suspBumpLvel[i].value = p_setup.suspBumpLvel[i].value;
        Car.setup.suspBumpLvel[i].min = p_setup.suspBumpLvel[i].min;
        Car.setup.suspBumpLvel[i].max = p_setup.suspBumpLvel[i].max;
        Car.setup.suspBumpLvel[i].desired_value = p_setup.suspBumpLvel[i].desired_value;
        Car.setup.suspBumpLvel[i].stepsize = p_setup.suspBumpLvel[i].stepsize;
        Car.setup.suspBumpLvel[i].changed = p_setup.suspBumpLvel[i].changed;
    }

    for (int i = 0; i < 4; i++)
    {
        Car.setup.suspFastRebound[i].value = p_setup.suspFastRebound[i].value;
        Car.setup.suspFastRebound[i].min = p_setup.suspFastRebound[i].min;
        Car.setup.suspFastRebound[i].max = p_setup.suspFastRebound[i].max;
        Car.setup.suspFastRebound[i].desired_value = p_setup.suspFastRebound[i].desired_value;
        Car.setup.suspFastRebound[i].stepsize = p_setup.suspFastRebound[i].stepsize;
        Car.setup.suspFastRebound[i].changed = p_setup.suspFastRebound[i].changed;
    }

    for (int i = 0; i < 4; i++)
    {
        Car.setup.suspSlowRebound[i].value = p_setup.suspSlowRebound[i].value;
        Car.setup.suspSlowRebound[i].min = p_setup.suspSlowRebound[i].min;
        Car.setup.suspSlowRebound[i].max = p_setup.suspSlowRebound[i].max;
        Car.setup.suspSlowRebound[i].desired_value = p_setup.suspSlowRebound[i].desired_value;
        Car.setup.suspSlowRebound[i].stepsize = p_setup.suspSlowRebound[i].stepsize;
        Car.setup.suspSlowRebound[i].changed = p_setup.suspSlowRebound[i].changed;
    }

    for (int i = 0; i < 4; i++)
    {
        Car.setup.suspReboundLvel[i].value = p_setup.suspReboundLvel[i].value;
        Car.setup.suspReboundLvel[i].min = p_setup.suspReboundLvel[i].min;
        Car.setup.suspReboundLvel[i].max = p_setup.suspReboundLvel[i].max;
        Car.setup.suspReboundLvel[i].desired_value = p_setup.suspReboundLvel[i].desired_value;
        Car.setup.suspReboundLvel[i].stepsize = p_setup.suspReboundLvel[i].stepsize;
        Car.setup.suspReboundLvel[i].changed = p_setup.suspReboundLvel[i].changed;
    }

    Car.setup.reqRepair.value = p_setup.reqRepair.value;
    Car.setup.reqRepair.min = p_setup.reqRepair.min;
    Car.setup.reqRepair.max = p_setup.reqRepair.max;
    Car.setup.reqRepair.desired_value = p_setup.reqRepair.desired_value;
    Car.setup.reqRepair.stepsize = p_setup.reqRepair.stepsize;
    Car.setup.reqRepair.changed = p_setup.reqRepair.changed;

    Car.setup.reqTireset.value = p_setup.reqTireset.value;
    Car.setup.reqTireset.min = p_setup.reqTireset.min;
    Car.setup.reqTireset.max = p_setup.reqTireset.max;
    Car.setup.reqTireset.desired_value = p_setup.reqTireset.desired_value;
    Car.setup.reqTireset.stepsize = p_setup.reqTireset.stepsize;
    Car.setup.reqTireset.changed = p_setup.reqTireset.changed;

    Car.setup.reqPenalty.value = p_setup.reqPenalty.value;
    Car.setup.reqPenalty.min = p_setup.reqPenalty.min;
    Car.setup.reqPenalty.max = p_setup.reqPenalty.max;
    Car.setup.reqPenalty.desired_value = p_setup.reqPenalty.desired_value;
    Car.setup.reqPenalty.stepsize = p_setup.reqPenalty.stepsize;
    Car.setup.reqPenalty.changed = p_setup.reqPenalty.changed;

    // Copy p_car.pitcmd
#define p_pitcmd p_car->pitcmd
    Car.pitcmd.fuel = p_pitcmd.fuel;
    Car.pitcmd.repair = p_pitcmd.repair;
    Car.pitcmd.stopType = p_pitcmd.stopType;
    Car.pitcmd.setupChanged = p_pitcmd.setupChanged;
    Car.pitcmd.tireChange = p_pitcmd.tireChange;

    // TODO: Maybe set this to nullptrs for the time being
    // Copy p_car.robot
    // RobotItf* robot = new RobotItf();
    // Car.robot = robot;
    // TODO Void pointers

    // Copy p_car.next
    // TODO Another car..

    // Copy p_situation
    Situation.raceInfo.ncars = p_situation->raceInfo.ncars;
    Situation.raceInfo.totLaps = p_situation->raceInfo.totLaps;
    Situation.raceInfo.extraLaps = p_situation->raceInfo.extraLaps;
    Situation.raceInfo.totTime = p_situation->raceInfo.totTime;
    Situation.raceInfo.state = p_situation->raceInfo.state;
    Situation.raceInfo.type = p_situation->raceInfo.type;
    Situation.raceInfo.maxDammage = p_situation->raceInfo.maxDammage;
    Situation.raceInfo.fps = p_situation->raceInfo.fps;
    Situation.raceInfo.features = p_situation->raceInfo.features;
    Situation.deltaTime = p_situation->deltaTime;
    Situation.currentTime = p_situation->currentTime;
    Situation.accelTime = p_situation->accelTime;
    Situation.nbPlayers = p_situation->nbPlayers;
    // TODO **cars
}
