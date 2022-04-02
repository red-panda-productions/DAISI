#include "BlackBoxData.h"

#include "robot.h"

/**
 * Constructs a data type for holding data provided to the black box
 * Makes deep copies of the provided data
 * @param p_car       The car data in the simulation
 * @param p_situation The environment data in the simulation
 * @param p_tickCount The tick that this driving simulation is from
 */
BlackBoxData::BlackBoxData(tCarElt* p_car, tSituation* p_situation, int p_tickCount)
{
    TickCount = *new int(p_tickCount);

    Car.index = *new int(p_car->index);

    // Copy p_car.info
#define p_info p_car->info
    int infoNameSize = sizeof p_info.name / sizeof(char);
    for (int i = 0; i < infoNameSize; i++)
    {
        Car.info.name[i] = *new char(p_info.name[i]);
    }
	int infoSnameSize = sizeof p_info.sname / sizeof(char);
    for (int i = 0; i < infoSnameSize; i++)
    {
        Car.info.sname[i] = *new char(p_info.sname[i]);
    }
    int infoCodenameSize = sizeof p_info.codename / sizeof(char);
    for (int i = 0; i < infoCodenameSize; i++)
    {
        Car.info.codename[i] = *new char(p_info.codename[i]);
    }
    int infoTeamnameSize = sizeof p_info.teamname / sizeof(char);
    for (int i = 0; i < infoTeamnameSize; i++)
    {
        Car.info.teamname[i] = *new char(p_info.teamname[i]);
    }
    int infoCarnameSize = sizeof p_info.carName / sizeof(char);
    for (int i = 0; i < infoCarnameSize; i++)
    {
        Car.info.carName[i] = *new char(p_info.carName[i]);
    }
    int infoCategorySize = sizeof p_info.category / sizeof(char);
    for (int i = 0; i < infoCategorySize; i++)
    {
        Car.info.category[i] = *new char(p_info.category[i]);
    }
    Car.info.raceNumber = *new int(p_info.raceNumber);
    Car.info.startRank = *new int(p_info.startRank);
    Car.info.driverType = *new int(p_info.driverType);
    Car.info.networkplayer = *new int(p_info.networkplayer);
    Car.info.skillLevel = *new int(p_info.skillLevel);
    int infoIconColorCount = sizeof p_info.iconColor / sizeof(tdble);
    for (int i = 0; i < infoIconColorCount; i++)
    {
        Car.info.iconColor[i] = *new tdble(p_info.iconColor[i]);
    }
    Car.info.dimension.x = *new tdble(p_info.dimension.x);
    Car.info.dimension.y = *new tdble(p_info.dimension.y);
    Car.info.dimension.z = *new tdble(p_info.dimension.z);
    Car.info.drvPos.x = *new tdble(p_info.drvPos.x);
    Car.info.drvPos.y = *new tdble(p_info.drvPos.y);
    Car.info.drvPos.z = *new tdble(p_info.drvPos.z);
    Car.info.bonnetPos.x = *new tdble(p_info.bonnetPos.x);
    Car.info.bonnetPos.y = *new tdble(p_info.bonnetPos.y);
    Car.info.bonnetPos.z = *new tdble(p_info.bonnetPos.z);
    Car.info.tank = *new tdble(p_info.tank);
    Car.info.steerLock = *new tdble(p_info.steerLock);
    Car.info.statGC.x = *new tdble(p_info.statGC.x);
    Car.info.statGC.y = *new tdble(p_info.statGC.y);
    Car.info.statGC.z = *new tdble(p_info.statGC.z);
    int infoWheelCount = sizeof p_info.wheel / sizeof(tWheelSpec);
    for (int i = 0; i < infoWheelCount; i++)
    {
        Car.info.wheel[i].rimRadius = *new tdble(p_info.wheel[i].rimRadius);
        Car.info.wheel[i].tireHeight = *new tdble(p_info.wheel[i].tireHeight);
        Car.info.wheel[i].tireWidth = *new tdble(p_info.wheel[i].tireWidth);
        Car.info.wheel[i].brakeDiskRadius = *new tdble(p_info.wheel[i].brakeDiskRadius);
        Car.info.wheel[i].wheelRadius = *new tdble(p_info.wheel[i].wheelRadius);
    }
    Car.info.visualAttr.exhaustNb = *new int(p_info.visualAttr.exhaustNb);
    int infoExhaustCount = sizeof p_info.visualAttr.exhaustPos / sizeof(t3Dd);
    for (int i = 0; i < infoExhaustCount; i++)
    {
        Car.info.visualAttr.exhaustPos[i].x = *new tdble(p_info.visualAttr.exhaustPos[i].x);
        Car.info.visualAttr.exhaustPos[i].y = *new tdble(p_info.visualAttr.exhaustPos[i].y);
        Car.info.visualAttr.exhaustPos[i].z = *new tdble(p_info.visualAttr.exhaustPos[i].z);
    }
    Car.info.visualAttr.exhaustPower = *new tdble(p_info.visualAttr.exhaustPower);
    int infoMasterModelSize = sizeof p_info.masterModel / sizeof(char);
    for (int i = 0; i < infoMasterModelSize; i++)
    {
        Car.info.masterModel[i] = *new char(p_info.masterModel[i]);
    }
    int infoSkinNameSize = sizeof p_info.skinName / sizeof(char);
    for (int i = 0; i < infoSkinNameSize; i++)
    {
        Car.info.skinName[i] = *new char(p_info.skinName[i]);
    }
    Car.info.skinTargets = *new int(p_info.skinTargets);

    // Copy p_car.pub
#define p_pub p_car->pub
    Car.pub.DynGC.pos.x = *new tdble(p_pub.DynGC.pos.x);
    Car.pub.DynGC.pos.y = *new tdble(p_pub.DynGC.pos.y);
    Car.pub.DynGC.pos.z = *new tdble(p_pub.DynGC.pos.z);
    Car.pub.DynGC.pos.xy = *new tdble(p_pub.DynGC.pos.xy);
    Car.pub.DynGC.pos.ax = *new tdble(p_pub.DynGC.pos.ax);
    Car.pub.DynGC.pos.ay = *new tdble(p_pub.DynGC.pos.ay);
    Car.pub.DynGC.pos.az = *new tdble(p_pub.DynGC.pos.az);
    Car.pub.DynGC.vel.x = *new tdble(p_pub.DynGC.vel.x);
    Car.pub.DynGC.vel.y = *new tdble(p_pub.DynGC.vel.y);
    Car.pub.DynGC.vel.z = *new tdble(p_pub.DynGC.vel.z);
    Car.pub.DynGC.vel.xy = *new tdble(p_pub.DynGC.vel.xy);
    Car.pub.DynGC.vel.ax = *new tdble(p_pub.DynGC.vel.ax);
    Car.pub.DynGC.vel.ay = *new tdble(p_pub.DynGC.vel.ay);
    Car.pub.DynGC.vel.az = *new tdble(p_pub.DynGC.vel.az);
    Car.pub.DynGC.acc.x = *new tdble(p_pub.DynGC.acc.x);
    Car.pub.DynGC.acc.y = *new tdble(p_pub.DynGC.acc.y);
    Car.pub.DynGC.acc.z = *new tdble(p_pub.DynGC.acc.z);
    Car.pub.DynGC.acc.xy = *new tdble(p_pub.DynGC.acc.xy);
    Car.pub.DynGC.acc.ax = *new tdble(p_pub.DynGC.acc.ax);
    Car.pub.DynGC.acc.ay = *new tdble(p_pub.DynGC.acc.ay);
    Car.pub.DynGC.acc.az = *new tdble(p_pub.DynGC.acc.az);

    Car.pub.DynGCg.pos.x = *new tdble(p_pub.DynGCg.pos.x);
    Car.pub.DynGCg.pos.y = *new tdble(p_pub.DynGCg.pos.y);
    Car.pub.DynGCg.pos.z = *new tdble(p_pub.DynGCg.pos.z);
    Car.pub.DynGCg.pos.xy = *new tdble(p_pub.DynGCg.pos.xy);
    Car.pub.DynGCg.pos.ax = *new tdble(p_pub.DynGCg.pos.ax);
    Car.pub.DynGCg.pos.ay = *new tdble(p_pub.DynGCg.pos.ay);
    Car.pub.DynGCg.pos.az = *new tdble(p_pub.DynGCg.pos.az);
    Car.pub.DynGCg.vel.x = *new tdble(p_pub.DynGCg.vel.x);
    Car.pub.DynGCg.vel.y = *new tdble(p_pub.DynGCg.vel.y);
    Car.pub.DynGCg.vel.z = *new tdble(p_pub.DynGCg.vel.z);
    Car.pub.DynGCg.vel.xy = *new tdble(p_pub.DynGCg.vel.xy);
    Car.pub.DynGCg.vel.ax = *new tdble(p_pub.DynGCg.vel.ax);
    Car.pub.DynGCg.vel.ay = *new tdble(p_pub.DynGCg.vel.ay);
    Car.pub.DynGCg.vel.az = *new tdble(p_pub.DynGCg.vel.az);
    Car.pub.DynGCg.acc.x = *new tdble(p_pub.DynGCg.acc.x);
    Car.pub.DynGCg.acc.y = *new tdble(p_pub.DynGCg.acc.y);
    Car.pub.DynGCg.acc.z = *new tdble(p_pub.DynGCg.acc.z);
    Car.pub.DynGCg.acc.xy = *new tdble(p_pub.DynGCg.acc.xy);
    Car.pub.DynGCg.acc.ax = *new tdble(p_pub.DynGCg.acc.ax);
    Car.pub.DynGCg.acc.ay = *new tdble(p_pub.DynGCg.acc.ay);
    Car.pub.DynGCg.acc.az = *new tdble(p_pub.DynGCg.acc.az);

    Car.pub.speed = *new tdble(p_pub.speed);
    int pubPosMatRowCount = sizeof p_pub.posMat / sizeof p_pub.posMat[0];
    int pubPosMatColumnCount = sizeof p_pub.posMat[0] / sizeof(float);
    for (int i = 0; i < pubPosMatRowCount; i++)
    {
	    for (int j = 0; j < pubPosMatColumnCount; j++)
	    {
            Car.pub.posMat[i][j] = *new float(p_pub.posMat[i][j]);
	    }
    }
    // TODO trkPos.seg
    Car.pub.trkPos.type = *new int(p_pub.trkPos.type);
    Car.pub.trkPos.toStart = *new tdble(p_pub.trkPos.toStart);
    Car.pub.trkPos.toRight = *new tdble(p_pub.trkPos.toRight);
    Car.pub.trkPos.toMiddle = *new tdble(p_pub.trkPos.toMiddle);
    Car.pub.trkPos.toLeft = *new tdble(p_pub.trkPos.toLeft);
    Car.pub.state = *new int(p_pub.state);
    int pubCornerCount = sizeof p_pub.corner / sizeof(tPosd);
    for (int i = 0; i < pubCornerCount; i++)
    {
        Car.pub.corner[i].x = *new tdble(p_pub.corner[i].x);
        Car.pub.corner[i].y = *new tdble(p_pub.corner[i].y);
        Car.pub.corner[i].z = *new tdble(p_pub.corner[i].z);
        Car.pub.corner[i].xy = *new tdble(p_pub.corner[i].xy);
        Car.pub.corner[i].ax = *new tdble(p_pub.corner[i].ax);
        Car.pub.corner[i].ay = *new tdble(p_pub.corner[i].ay);
        Car.pub.corner[i].az = *new tdble(p_pub.corner[i].az);
    }
    Car.pub.glance = *new tdble(p_pub.glance);
    Car.pub.oldglance = *new tdble(p_pub.oldglance);

    // Copy p_car.race
#define p_race p_car->race
    Car.race.bestLapTime = *new double(p_race.bestLapTime);
    Car.race.commitBestLapTime = *new bool(p_race.commitBestLapTime);
    Car.race.bestSplitTime = new double(*p_race.bestSplitTime);
    Car.race.deltaBestLapTime = *new double(p_race.deltaBestLapTime);
    Car.race.curLapTime = *new double(p_race.curLapTime);
    Car.race.curSplitTime = new double(*p_race.curSplitTime);
    Car.race.lastLapTime = *new double(p_race.lastLapTime);
    Car.race.curTime = *new double(p_race.curTime);
    Car.race.topSpeed = *new tdble(p_race.topSpeed);
    Car.race.currentMinSpeedForLap = *new tdble(p_race.currentMinSpeedForLap);
    Car.race.laps = *new int(p_race.laps);
    Car.race.bestLap = *new int(p_race.bestLap);
    Car.race.nbPitStops = *new int(p_race.nbPitStops);
    Car.race.remainingLaps = *new int(p_race.remainingLaps);
    Car.race.pos = *new int(p_race.pos);
    Car.race.timeBehindLeader = *new double(p_race.timeBehindLeader);
    Car.race.lapsBehindLeader = *new int(p_race.lapsBehindLeader);
    Car.race.timeBehindPrev = *new double(p_race.timeBehindPrev);
    Car.race.timeBeforeNext = *new double(p_race.timeBeforeNext);
    Car.race.distRaced = *new tdble(p_race.distRaced);
    Car.race.distFromStartLine = *new tdble(p_race.distFromStartLine);
    Car.race.currentSector = *new int(p_race.currentSector);
    Car.race.nbSectors = *new int(p_race.nbSectors);
    Car.race.trackLength = *new double(p_race.trackLength);
    Car.race.scheduledEventTime = *new double(p_race.scheduledEventTime);
    Car.race.pit = nullptr; // TODO *pit
    Car.race.event = *new int(p_race.event);
    // TODO penaltyList
    Car.race.penaltyTime = *new tdble(p_race.penaltyTime);
    Car.race.prevFromStartLine = *new tdble(p_race.prevFromStartLine);
    Car.race.wrongWayTime = *new double(p_race.wrongWayTime);

    // Copy p_car.priv
#define p_priv p_car->priv
    Car.priv.paramsHandle = nullptr; // TODO paramsHandle
    Car.priv.carHandle = nullptr; // TODO carHandle
    Car.priv.driverIndex = *new int(p_priv.driverIndex);
    Car.priv.moduleIndex = *new int(p_priv.moduleIndex);
    int privModNameSize = sizeof p_priv.modName / sizeof(char);
    for (int i = 0; i < privModNameSize; i++)
    {
        Car.priv.modName[i] = *new char(p_priv.modName[i]);
    }
    // TODO wheel
    int privCornerCount = sizeof p_priv.corner / sizeof(tPosd);
    for (int i = 0; i < privCornerCount; i++)
    {
        Car.priv.corner[i].x = *new tdble(p_priv.corner[i].x);
        Car.priv.corner[i].y = *new tdble(p_priv.corner[i].y);
        Car.priv.corner[i].z = *new tdble(p_priv.corner[i].z);
        Car.priv.corner[i].xy = *new tdble(p_priv.corner[i].xy);
        Car.priv.corner[i].ax = *new tdble(p_priv.corner[i].ax);
        Car.priv.corner[i].ay = *new tdble(p_priv.corner[i].ay);
        Car.priv.corner[i].az = *new tdble(p_priv.corner[i].az);
    }
    Car.priv.gear = *new int(p_priv.gear);
    Car.priv.gearNext = *new int(p_priv.gearNext);
    Car.priv.fuel = *new tdble(p_priv.fuel);
    Car.priv.fuel_consumption_total = *new tdble(p_priv.fuel_consumption_total);
    Car.priv.fuel_consumption_instant = *new tdble(p_priv.fuel_consumption_instant);
    Car.priv.enginerpm = *new tdble(p_priv.enginerpm);
    Car.priv.enginerpmRedLine = *new tdble(p_priv.enginerpmRedLine);
    Car.priv.enginerpmMax = *new tdble(p_priv.enginerpmMax);
    Car.priv.enginerpmMaxTq = *new tdble(p_priv.enginerpmMaxTq);
    Car.priv.enginerpmMaxPw = *new tdble(p_priv.enginerpmMaxPw);
    Car.priv.engineMaxTq = *new tdble(p_priv.engineMaxTq);
    Car.priv.engineMaxPw = *new tdble(p_priv.engineMaxPw);
    int privGearRatioCount = sizeof p_priv.gearRatio / sizeof(tdble);
    for (int i = 0; i < privGearRatioCount; i++)
    {
        Car.priv.gearRatio[i] = *new tdble(p_priv.gearRatio[i]);
    }
    Car.priv.gearNb = *new int(p_priv.gearNb);
    Car.priv.gearOffset = *new int(p_priv.gearOffset);
    int privSkidCount = sizeof p_priv.skid / sizeof(tdble);
    for (int i = 0; i < privSkidCount; i++)
    {
        Car.priv.skid[i] = *new tdble(p_priv.skid[i]);
    }
    int privReactionCount = sizeof p_priv.reaction / sizeof(tdble);
    for (int i = 0; i < privReactionCount; i++)
    {
        Car.priv.reaction[i] = *new tdble(p_priv.reaction[i]);
    }
    Car.priv.collision = *new int(p_priv.collision);
    Car.priv.simcollision = *new int(p_priv.simcollision);
    Car.priv.smoke = *new float(p_priv.smoke);
    Car.priv.normal.x = *new tdble(p_priv.normal.x);
    Car.priv.normal.y = *new tdble(p_priv.normal.y);
    Car.priv.normal.z = *new tdble(p_priv.normal.z);
    Car.priv.collpos.x = *new tdble(p_priv.collpos.x);
    Car.priv.collpos.y = *new tdble(p_priv.collpos.y);
    Car.priv.collpos.z = *new tdble(p_priv.collpos.z);
    Car.priv.dammage = *new int(p_priv.dammage);
    Car.priv.debug = *new int(p_priv.debug);
    Car.priv.collision_state.collision_count = *new int(p_priv.collision_state.collision_count);
    int privPosCount = sizeof p_priv.collision_state.pos / sizeof(float);
    for (int i = 0; i < privPosCount; i++)
    {
        Car.priv.collision_state.pos[i] = *new float(p_priv.collision_state.pos[i]);
    }
    int privForceCount = sizeof p_priv.collision_state.force / sizeof(float);
    for (int i = 0; i < privForceCount; i++)
    {
        Car.priv.collision_state.force[i] = *new float(p_priv.collision_state.force[i]);
    }
    Car.priv.localPressure = *new tdble(p_priv.localPressure);
    // TODO memoryPool
    Car.priv.driveSkill = *new tdble(p_priv.driveSkill);
    Car.priv.steerTqCenter = *new tdble(p_priv.steerTqCenter);
    Car.priv.steerTqAlign = *new tdble(p_priv.steerTqAlign);
    int raceDashboardInstantCount = sizeof p_priv.dashboardInstant / sizeof(tDashboardItem);
    for (int i = 0; i < raceDashboardInstantCount; i++)
    {
        Car.priv.dashboardInstant[i] = *new tDashboardItem();
        Car.priv.dashboardInstant[i].type = *new int(p_priv.dashboardInstant[i].type);
        tCarSetupItem* setupItem = new tCarSetupItem();
        setupItem->value = *new tdble(p_priv.dashboardInstant[i].setup->value);
        setupItem->min = *new tdble(p_priv.dashboardInstant[i].setup->min);
        setupItem->max = *new tdble(p_priv.dashboardInstant[i].setup->max);
        setupItem->desired_value = *new tdble(p_priv.dashboardInstant[i].setup->desired_value);
        setupItem->stepsize = *new tdble(p_priv.dashboardInstant[i].setup->stepsize);
        setupItem->changed = *new bool(p_priv.dashboardInstant[i].setup->changed);
        Car.priv.dashboardInstant[i].setup = setupItem;
    }
    Car.priv.dashboardInstantNb = *new int(p_priv.dashboardInstantNb);
    int raceDashboardRequestCount = sizeof p_priv.dashboardRequest / sizeof(tDashboardItem);
    for (int i = 0; i < raceDashboardRequestCount; i++)
    {
        Car.priv.dashboardRequest[i] = *new tDashboardItem();
        Car.priv.dashboardRequest[i].type = *new int(p_priv.dashboardRequest[i].type);
        tCarSetupItem* setupItem = new tCarSetupItem();
        setupItem->value = *new tdble(p_priv.dashboardRequest[i].setup->value);
        setupItem->min = *new tdble(p_priv.dashboardRequest[i].setup->min);
        setupItem->max = *new tdble(p_priv.dashboardRequest[i].setup->max);
        setupItem->desired_value = *new tdble(p_priv.dashboardRequest[i].setup->desired_value);
        setupItem->stepsize = *new tdble(p_priv.dashboardRequest[i].setup->stepsize);
        setupItem->changed = *new bool(p_priv.dashboardRequest[i].setup->changed);
        Car.priv.dashboardRequest[i].setup = setupItem;
    }
    Car.priv.dashboardRequestNb = *new int(p_priv.dashboardRequestNb);
    Car.priv.dashboardActiveItem = *new int(p_priv.dashboardActiveItem);

    // Copy p_car.ctrl
#define p_ctrl p_car->ctrl
    Car.ctrl.steer = *new tdble(p_ctrl.steer);
    Car.ctrl.accelCmd = *new tdble(p_ctrl.accelCmd);
    Car.ctrl.brakeCmd = *new tdble(p_ctrl.brakeCmd);
    Car.ctrl.clutchCmd = *new tdble(p_ctrl.clutchCmd);
    Car.ctrl.brakeFrontLeftCmd = *new tdble(p_ctrl.brakeFrontLeftCmd);
    Car.ctrl.brakeFrontRightCmd = *new tdble(p_ctrl.brakeFrontRightCmd);
    Car.ctrl.brakeRearLeftCmd = *new tdble(p_ctrl.brakeRearLeftCmd);
    Car.ctrl.brakeRearRightCmd = *new tdble(p_ctrl.brakeRearRightCmd);
    Car.ctrl.wingFrontCmd = *new tdble(p_ctrl.wingFrontCmd);
    Car.ctrl.wingRearCmd = *new tdble(p_ctrl.wingRearCmd);
    Car.ctrl.reserved1 = *new tdble(p_ctrl.reserved1);
    Car.ctrl.reserved2 = *new tdble(p_ctrl.reserved2);
    Car.ctrl.gear = *new int(p_ctrl.gear);
    Car.ctrl.raceCmd = *new int(p_ctrl.raceCmd);
    Car.ctrl.lightCmd = *new int(p_ctrl.lightCmd);
    Car.ctrl.ebrakeCmd = *new int(p_ctrl.ebrakeCmd);
    Car.ctrl.wingControlMode = *new int(p_ctrl.wingControlMode);
    Car.ctrl.singleWheelBrakeMode = *new int(p_ctrl.singleWheelBrakeMode);
    Car.ctrl.switch3 = *new int(p_ctrl.switch3);
    Car.ctrl.telemetryMode = *new int(p_ctrl.telemetryMode);
    int ctrlMsgRowCount = sizeof p_ctrl.msg / sizeof p_ctrl.msg[0];
    int ctrlMsgColumnCount = sizeof p_ctrl.msg[0] / sizeof(char);
    for (int i = 0; i < ctrlMsgRowCount; i++)
    {
	    for (int j = 0; j < ctrlMsgColumnCount; j++)
	    {
            Car.ctrl.msg[i][j] = *new char(p_ctrl.msg[i][j]);
	    }
    }
    int ctrlMsgColorCount = sizeof p_ctrl.msgColor / sizeof(float);
    for (int i = 0; i < ctrlMsgColorCount; i++)
    {
        Car.ctrl.msgColor[i] = *new float(p_ctrl.msgColor[i]);
    }
    tDashboardItem* ctrlDashboardItem = new tDashboardItem();
    ctrlDashboardItem->type = *new int(p_ctrl.setupChangeCmd->type);
    tCarSetupItem* ctrlCarSetupItem = new tCarSetupItem();
    ctrlCarSetupItem->value = *new tdble(p_ctrl.setupChangeCmd->setup->value);
    ctrlCarSetupItem->min = *new tdble(p_ctrl.setupChangeCmd->setup->min);
    ctrlCarSetupItem->max = *new tdble(p_ctrl.setupChangeCmd->setup->max);
    ctrlCarSetupItem->desired_value = *new tdble(p_ctrl.setupChangeCmd->setup->desired_value);
    ctrlCarSetupItem->stepsize = *new tdble(p_ctrl.setupChangeCmd->setup->stepsize);
    ctrlCarSetupItem->changed = *new bool(p_ctrl.setupChangeCmd->setup->changed);
    ctrlDashboardItem->setup = ctrlCarSetupItem;
    Car.ctrl.setupChangeCmd = ctrlDashboardItem;

    // Copy p_car.setup
#define p_setup p_car->setup

    Car.setup.FRWeightRep.value = *new tdble(p_setup.FRWeightRep.value);
    Car.setup.FRWeightRep.min = *new tdble(p_setup.FRWeightRep.min);
    Car.setup.FRWeightRep.max = *new tdble(p_setup.FRWeightRep.max);
    Car.setup.FRWeightRep.desired_value = *new tdble(p_setup.FRWeightRep.desired_value);
    Car.setup.FRWeightRep.stepsize = *new tdble(p_setup.FRWeightRep.stepsize);
    Car.setup.FRWeightRep.changed = *new bool(p_setup.FRWeightRep.changed);

    Car.setup.FRLWeightRep.value = *new tdble(p_setup.FRLWeightRep.value);
    Car.setup.FRLWeightRep.min = *new tdble(p_setup.FRLWeightRep.min);
    Car.setup.FRLWeightRep.max = *new tdble(p_setup.FRLWeightRep.max);
    Car.setup.FRLWeightRep.desired_value = *new tdble(p_setup.FRLWeightRep.desired_value);
    Car.setup.FRLWeightRep.stepsize = *new tdble(p_setup.FRLWeightRep.stepsize);
    Car.setup.FRLWeightRep.changed = *new bool(p_setup.FRLWeightRep.changed);

    Car.setup.RRLWeightRep.value = *new tdble(p_setup.RRLWeightRep.value);
    Car.setup.RRLWeightRep.min = *new tdble(p_setup.RRLWeightRep.min);
    Car.setup.RRLWeightRep.max = *new tdble(p_setup.RRLWeightRep.max);
    Car.setup.RRLWeightRep.desired_value = *new tdble(p_setup.RRLWeightRep.desired_value);
    Car.setup.RRLWeightRep.stepsize = *new tdble(p_setup.RRLWeightRep.stepsize);
    Car.setup.RRLWeightRep.changed = *new bool(p_setup.RRLWeightRep.changed);

    Car.setup.fuel.value = *new tdble(p_setup.fuel.value);
    Car.setup.fuel.min = *new tdble(p_setup.fuel.min);
    Car.setup.fuel.max = *new tdble(p_setup.fuel.max);
    Car.setup.fuel.desired_value = *new tdble(p_setup.fuel.desired_value);
    Car.setup.fuel.stepsize = *new tdble(p_setup.fuel.stepsize);
    Car.setup.fuel.changed = *new bool(p_setup.fuel.changed);

    int setupWingAngleCount = sizeof p_setup.wingAngle / sizeof(tCarSetupItem);
    for (int i = 0; i < setupWingAngleCount; i++)
    {
        Car.setup.wingAngle[i].value = *new tdble(p_setup.wingAngle[i].value);
        Car.setup.wingAngle[i].min = *new tdble(p_setup.wingAngle[i].min);
        Car.setup.wingAngle[i].max = *new tdble(p_setup.wingAngle[i].max);
        Car.setup.wingAngle[i].desired_value = *new tdble(p_setup.wingAngle[i].desired_value);
        Car.setup.wingAngle[i].stepsize = *new tdble(p_setup.wingAngle[i].stepsize);
        Car.setup.wingAngle[i].changed = *new bool(p_setup.wingAngle[i].changed);
    }

    Car.setup.revsLimiter.value = *new tdble(p_setup.revsLimiter.value);
    Car.setup.revsLimiter.min = *new tdble(p_setup.revsLimiter.min);
    Car.setup.revsLimiter.max = *new tdble(p_setup.revsLimiter.max);
    Car.setup.revsLimiter.desired_value = *new tdble(p_setup.revsLimiter.desired_value);
    Car.setup.revsLimiter.stepsize = *new tdble(p_setup.revsLimiter.stepsize);
    Car.setup.revsLimiter.changed = *new bool(p_setup.revsLimiter.changed);

    int setupGearRatioCount = sizeof p_setup.gearRatio / sizeof(tCarSetupItem);
    for (int i = 0; i < setupGearRatioCount; i++)
    {
        Car.setup.gearRatio[i].value = *new tdble(p_setup.gearRatio[i].value);
        Car.setup.gearRatio[i].min = *new tdble(p_setup.gearRatio[i].min);
        Car.setup.gearRatio[i].max = *new tdble(p_setup.gearRatio[i].max);
        Car.setup.gearRatio[i].desired_value = *new tdble(p_setup.gearRatio[i].desired_value);
        Car.setup.gearRatio[i].stepsize = *new tdble(p_setup.gearRatio[i].stepsize);
        Car.setup.gearRatio[i].changed = *new bool(p_setup.gearRatio[i].changed);
    }

    int setupDifferentialTypeCount = sizeof p_setup.differentialType / sizeof(int);
    for (int i = 0; i < setupDifferentialTypeCount; i++)
    {
        Car.setup.differentialType[i] = *new int(p_setup.differentialType[i]);
    }

    int setupDifferentialRatioCount = sizeof p_setup.differentialRatio / sizeof(tCarSetupItem);
    for (int i = 0; i < setupDifferentialRatioCount; i++)
    {
        Car.setup.differentialRatio[i].value = *new tdble(p_setup.differentialRatio[i].value);
        Car.setup.differentialRatio[i].min = *new tdble(p_setup.differentialRatio[i].min);
        Car.setup.differentialRatio[i].max = *new tdble(p_setup.differentialRatio[i].max);
        Car.setup.differentialRatio[i].desired_value = *new tdble(p_setup.differentialRatio[i].desired_value);
        Car.setup.differentialRatio[i].stepsize = *new tdble(p_setup.differentialRatio[i].stepsize);
        Car.setup.differentialRatio[i].changed = *new bool(p_setup.differentialRatio[i].changed);
    }

    int setupDifferentialMinTqCount = sizeof p_setup.differentialMinTqBias / sizeof(tCarSetupItem);
    for (int i = 0; i < setupDifferentialMinTqCount; i++)
    {
        Car.setup.differentialMinTqBias[i].value = *new tdble(p_setup.differentialMinTqBias[i].value);
        Car.setup.differentialMinTqBias[i].min = *new tdble(p_setup.differentialMinTqBias[i].min);
        Car.setup.differentialMinTqBias[i].max = *new tdble(p_setup.differentialMinTqBias[i].max);
        Car.setup.differentialMinTqBias[i].desired_value = *new tdble(p_setup.differentialMinTqBias[i].desired_value);
        Car.setup.differentialMinTqBias[i].stepsize = *new tdble(p_setup.differentialMinTqBias[i].stepsize);
        Car.setup.differentialMinTqBias[i].changed = *new bool(p_setup.differentialMinTqBias[i].changed);
    }

    int setupDifferentialMaxTqCount = sizeof p_setup.differentialMaxTqBias / sizeof(tCarSetupItem);
    for (int i = 0; i < setupDifferentialMaxTqCount; i++)
    {
        Car.setup.differentialMaxTqBias[i].value = *new tdble(p_setup.differentialMaxTqBias[i].value);
        Car.setup.differentialMaxTqBias[i].min = *new tdble(p_setup.differentialMaxTqBias[i].min);
        Car.setup.differentialMaxTqBias[i].max = *new tdble(p_setup.differentialMaxTqBias[i].max);
        Car.setup.differentialMaxTqBias[i].desired_value = *new tdble(p_setup.differentialMaxTqBias[i].desired_value);
        Car.setup.differentialMaxTqBias[i].stepsize = *new tdble(p_setup.differentialMaxTqBias[i].stepsize);
        Car.setup.differentialMaxTqBias[i].changed = *new bool(p_setup.differentialMaxTqBias[i].changed);
    }

    int setupDifferentialViscosityCount = sizeof p_setup.differentialViscosity / sizeof(tCarSetupItem);
    for (int i = 0; i < setupDifferentialViscosityCount; i++) {
        Car.setup.differentialViscosity[i].value = *new tdble(p_setup.differentialViscosity[i].value);
        Car.setup.differentialViscosity[i].min = *new tdble(p_setup.differentialViscosity[i].min);
        Car.setup.differentialViscosity[i].max = *new tdble(p_setup.differentialViscosity[i].max);
        Car.setup.differentialViscosity[i].desired_value = *new tdble(p_setup.differentialViscosity[i].desired_value);
        Car.setup.differentialViscosity[i].stepsize = *new tdble(p_setup.differentialViscosity[i].stepsize);
        Car.setup.differentialViscosity[i].changed = *new bool(p_setup.differentialViscosity[i].changed);
    }

    int setupDifferentialLockingTqCount = sizeof p_setup.differentialLockingTq / sizeof(tCarSetupItem);
    for (int i = 0; i < setupDifferentialLockingTqCount; i++)
    {
        Car.setup.differentialLockingTq[i].value = *new tdble(p_setup.differentialLockingTq[i].value);
        Car.setup.differentialLockingTq[i].min = *new tdble(p_setup.differentialLockingTq[i].min);
        Car.setup.differentialLockingTq[i].max = *new tdble(p_setup.differentialLockingTq[i].max);
        Car.setup.differentialLockingTq[i].desired_value = *new tdble(p_setup.differentialLockingTq[i].desired_value);
        Car.setup.differentialLockingTq[i].stepsize = *new tdble(p_setup.differentialLockingTq[i].stepsize);
        Car.setup.differentialLockingTq[i].changed = *new bool(p_setup.differentialLockingTq[i].changed);
    }

    int setupDifferentialMaxSlipCount = sizeof p_setup.differentialMaxSlipBias / sizeof(tCarSetupItem);
    for (int i = 0; i < setupDifferentialMaxSlipCount; i++)
    {
        Car.setup.differentialMaxSlipBias[i].value = *new tdble(p_setup.differentialMaxSlipBias[i].value);
        Car.setup.differentialMaxSlipBias[i].min = *new tdble(p_setup.differentialMaxSlipBias[i].min);
        Car.setup.differentialMaxSlipBias[i].max = *new tdble(p_setup.differentialMaxSlipBias[i].max);
        Car.setup.differentialMaxSlipBias[i].desired_value = *new tdble(p_setup.differentialMaxSlipBias[i].desired_value);
        Car.setup.differentialMaxSlipBias[i].stepsize = *new tdble(p_setup.differentialMaxSlipBias[i].stepsize);
        Car.setup.differentialMaxSlipBias[i].changed = *new bool(p_setup.differentialMaxSlipBias[i].changed);
    }

    int setupDifferentialCoastMaxSlipCount = sizeof p_setup.differentialCoastMaxSlipBias / sizeof(tCarSetupItem);
    for (int i = 0; i < setupDifferentialCoastMaxSlipCount; i++)
    {
        Car.setup.differentialCoastMaxSlipBias[i].value = *new tdble(p_setup.differentialCoastMaxSlipBias[i].value);
        Car.setup.differentialCoastMaxSlipBias[i].min = *new tdble(p_setup.differentialCoastMaxSlipBias[i].min);
        Car.setup.differentialCoastMaxSlipBias[i].max = *new tdble(p_setup.differentialCoastMaxSlipBias[i].max);
        Car.setup.differentialCoastMaxSlipBias[i].desired_value = *new tdble(p_setup.differentialCoastMaxSlipBias[i].desired_value);
        Car.setup.differentialCoastMaxSlipBias[i].stepsize = *new tdble(p_setup.differentialCoastMaxSlipBias[i].stepsize);
        Car.setup.differentialCoastMaxSlipBias[i].changed = *new bool(p_setup.differentialCoastMaxSlipBias[i].changed);
    }

    Car.setup.steerLock.value = *new tdble(p_setup.steerLock.value);
    Car.setup.steerLock.min = *new tdble(p_setup.steerLock.min);
    Car.setup.steerLock.max = *new tdble(p_setup.steerLock.max);
    Car.setup.steerLock.desired_value = *new tdble(p_setup.steerLock.desired_value);
    Car.setup.steerLock.stepsize = *new tdble(p_setup.steerLock.stepsize);
    Car.setup.steerLock.changed = *new bool(p_setup.steerLock.changed);

    Car.setup.brakeRepartition.value = *new tdble(p_setup.brakeRepartition.value);
    Car.setup.brakeRepartition.min = *new tdble(p_setup.brakeRepartition.min);
    Car.setup.brakeRepartition.max = *new tdble(p_setup.brakeRepartition.max);
    Car.setup.brakeRepartition.desired_value = *new tdble(p_setup.brakeRepartition.desired_value);
    Car.setup.brakeRepartition.stepsize = *new tdble(p_setup.brakeRepartition.stepsize);
    Car.setup.brakeRepartition.changed = *new bool(p_setup.brakeRepartition.changed);

    Car.setup.brakePressure.value = *new tdble(p_setup.brakePressure.value);
    Car.setup.brakePressure.min = *new tdble(p_setup.brakePressure.min);
    Car.setup.brakePressure.max = *new tdble(p_setup.brakePressure.max);
    Car.setup.brakePressure.desired_value = *new tdble(p_setup.brakePressure.desired_value);
    Car.setup.brakePressure.stepsize = *new tdble(p_setup.brakePressure.stepsize);
    Car.setup.brakePressure.changed = *new bool(p_setup.brakePressure.changed);

    int setupRideHeightCount = sizeof p_setup.rideHeight / sizeof(tCarSetupItem);
    for (int i = 0; i < setupRideHeightCount; i++)
    {
        Car.setup.rideHeight[i].value = *new tdble(p_setup.rideHeight[i].value);
        Car.setup.rideHeight[i].min = *new tdble(p_setup.rideHeight[i].min);
        Car.setup.rideHeight[i].max = *new tdble(p_setup.rideHeight[i].max);
        Car.setup.rideHeight[i].desired_value = *new tdble(p_setup.rideHeight[i].desired_value);
        Car.setup.rideHeight[i].stepsize = *new tdble(p_setup.rideHeight[i].stepsize);
        Car.setup.rideHeight[i].changed = *new bool(p_setup.rideHeight[i].changed);
    }

    int setupToeCount = sizeof p_setup.toe / sizeof(tCarSetupItem);
    for (int i = 0; i < setupToeCount; i++)
    {
        Car.setup.toe[i].value = *new tdble(p_setup.toe[i].value);
        Car.setup.toe[i].min = *new tdble(p_setup.toe[i].min);
        Car.setup.toe[i].max = *new tdble(p_setup.toe[i].max);
        Car.setup.toe[i].desired_value = *new tdble(p_setup.toe[i].desired_value);
        Car.setup.toe[i].stepsize = *new tdble(p_setup.toe[i].stepsize);
        Car.setup.toe[i].changed = *new bool(p_setup.toe[i].changed);
    }

    int setupCamberCount = sizeof p_setup.camber / sizeof(tCarSetupItem);
    for (int i = 0; i < setupCamberCount; i++) 
    {
        Car.setup.camber[i].value = *new tdble(p_setup.camber[i].value);
        Car.setup.camber[i].min = *new tdble(p_setup.camber[i].min);
        Car.setup.camber[i].max = *new tdble(p_setup.camber[i].max);
        Car.setup.camber[i].desired_value = *new tdble(p_setup.camber[i].desired_value);
        Car.setup.camber[i].stepsize = *new tdble(p_setup.camber[i].stepsize);
        Car.setup.camber[i].changed = *new bool(p_setup.camber[i].changed);
    }

    int setupTirePressureCount = sizeof p_setup.tirePressure / sizeof(tCarSetupItem);
    for (int i = 0; i < setupTirePressureCount; i++)
    {
        Car.setup.tirePressure[i].value = *new tdble(p_setup.tirePressure[i].value);
        Car.setup.tirePressure[i].min = *new tdble(p_setup.tirePressure[i].min);
        Car.setup.tirePressure[i].max = *new tdble(p_setup.tirePressure[i].max);
        Car.setup.tirePressure[i].desired_value = *new tdble(p_setup.tirePressure[i].desired_value);
        Car.setup.tirePressure[i].stepsize = *new tdble(p_setup.tirePressure[i].stepsize);
        Car.setup.tirePressure[i].changed = *new bool(p_setup.tirePressure[i].changed);
    }

    int setupTireOpLoadCount = sizeof p_setup.tireOpLoad / sizeof(tCarSetupItem);
    for (int i = 0; i < setupTireOpLoadCount; i++)
    {
        Car.setup.tireOpLoad[i].value = *new tdble(p_setup.tireOpLoad[i].value);
        Car.setup.tireOpLoad[i].min = *new tdble(p_setup.tireOpLoad[i].min);
        Car.setup.tireOpLoad[i].max = *new tdble(p_setup.tireOpLoad[i].max);
        Car.setup.tireOpLoad[i].desired_value = *new tdble(p_setup.tireOpLoad[i].desired_value);
        Car.setup.tireOpLoad[i].stepsize = *new tdble(p_setup.tireOpLoad[i].stepsize);
        Car.setup.tireOpLoad[i].changed = *new bool(p_setup.tireOpLoad[i].changed);
    }

    int setupArbSpringCount = sizeof p_setup.arbSpring / sizeof(tCarSetupItem);
    for (int i = 0; i < setupArbSpringCount; i++)
    {
        Car.setup.arbSpring[i].value = *new tdble(p_setup.arbSpring[i].value);
        Car.setup.arbSpring[i].min = *new tdble(p_setup.arbSpring[i].min);
        Car.setup.arbSpring[i].max = *new tdble(p_setup.arbSpring[i].max);
        Car.setup.arbSpring[i].desired_value = *new tdble(p_setup.arbSpring[i].desired_value);
        Car.setup.arbSpring[i].stepsize = *new tdble(p_setup.arbSpring[i].stepsize);
        Car.setup.arbSpring[i].changed = *new bool(p_setup.arbSpring[i].changed);
    }

    int setupArbBellcrankCount = sizeof p_setup.arbBellcrank / sizeof(tCarSetupItem);
    for (int i = 0; i < setupArbBellcrankCount; i++)
    {
        Car.setup.arbBellcrank[i].value = *new tdble(p_setup.arbBellcrank[i].value);
        Car.setup.arbBellcrank[i].min = *new tdble(p_setup.arbBellcrank[i].min);
        Car.setup.arbBellcrank[i].max = *new tdble(p_setup.arbBellcrank[i].max);
        Car.setup.arbBellcrank[i].desired_value = *new tdble(p_setup.arbBellcrank[i].desired_value);
        Car.setup.arbBellcrank[i].stepsize = *new tdble(p_setup.arbBellcrank[i].stepsize);
        Car.setup.arbBellcrank[i].changed = *new bool(p_setup.arbBellcrank[i].changed);
    }

    int setupHeaveSpringCount = sizeof p_setup.heaveSpring / sizeof(tCarSetupItem);
    for (int i = 0; i < setupHeaveSpringCount; i++)
    {
        Car.setup.heaveSpring[i].value = *new tdble(p_setup.heaveSpring[i].value);
        Car.setup.heaveSpring[i].min = *new tdble(p_setup.heaveSpring[i].min);
        Car.setup.heaveSpring[i].max = *new tdble(p_setup.heaveSpring[i].max);
        Car.setup.heaveSpring[i].desired_value = *new tdble(p_setup.heaveSpring[i].desired_value);
        Car.setup.heaveSpring[i].stepsize = *new tdble(p_setup.heaveSpring[i].stepsize);
        Car.setup.heaveSpring[i].changed = *new bool(p_setup.heaveSpring[i].changed);
    }

    int setupHeaveBellcrankCount = sizeof p_setup.heaveBellcrank / sizeof(tCarSetupItem);
    for (int i = 0; i < setupHeaveBellcrankCount; i++)
    {
        Car.setup.heaveBellcrank[i].value = *new tdble(p_setup.heaveBellcrank[i].value);
        Car.setup.heaveBellcrank[i].min = *new tdble(p_setup.heaveBellcrank[i].min);
        Car.setup.heaveBellcrank[i].max = *new tdble(p_setup.heaveBellcrank[i].max);
        Car.setup.heaveBellcrank[i].desired_value = *new tdble(p_setup.heaveBellcrank[i].desired_value);
        Car.setup.heaveBellcrank[i].stepsize = *new tdble(p_setup.heaveBellcrank[i].stepsize);
        Car.setup.heaveBellcrank[i].changed = *new bool(p_setup.heaveBellcrank[i].changed);
    }

    int setupHeaveInertanceCount = sizeof p_setup.heaveInertance / sizeof(tCarSetupItem);
    for (int i = 0; i < setupHeaveInertanceCount; i++)
    {
        Car.setup.heaveInertance[i].value = *new tdble(p_setup.heaveInertance[i].value);
        Car.setup.heaveInertance[i].min = *new tdble(p_setup.heaveInertance[i].min);
        Car.setup.heaveInertance[i].max = *new tdble(p_setup.heaveInertance[i].max);
        Car.setup.heaveInertance[i].desired_value = *new tdble(p_setup.heaveInertance[i].desired_value);
        Car.setup.heaveInertance[i].stepsize = *new tdble(p_setup.heaveInertance[i].stepsize);
        Car.setup.heaveInertance[i].changed = *new bool(p_setup.heaveInertance[i].changed);
    }

    int setupHeaveFastBumpCount = sizeof p_setup.heaveFastBump / sizeof(tCarSetupItem);
    for (int i = 0; i < setupHeaveFastBumpCount; i++)
    {
        Car.setup.heaveFastBump[i].value = *new tdble(p_setup.heaveFastBump[i].value);
        Car.setup.heaveFastBump[i].min = *new tdble(p_setup.heaveFastBump[i].min);
        Car.setup.heaveFastBump[i].max = *new tdble(p_setup.heaveFastBump[i].max);
        Car.setup.heaveFastBump[i].desired_value = *new tdble(p_setup.heaveFastBump[i].desired_value);
        Car.setup.heaveFastBump[i].stepsize = *new tdble(p_setup.heaveFastBump[i].stepsize);
        Car.setup.heaveFastBump[i].changed = *new bool(p_setup.heaveFastBump[i].changed);
    }

    int setupHeaveSlowBumpCount = sizeof p_setup.heaveSlowBump / sizeof(tCarSetupItem);
    for (int i = 0; i < setupHeaveSlowBumpCount; i++)
    {
        Car.setup.heaveSlowBump[i].value = *new tdble(p_setup.heaveSlowBump[i].value);
        Car.setup.heaveSlowBump[i].min = *new tdble(p_setup.heaveSlowBump[i].min);
        Car.setup.heaveSlowBump[i].max = *new tdble(p_setup.heaveSlowBump[i].max);
        Car.setup.heaveSlowBump[i].desired_value = *new tdble(p_setup.heaveSlowBump[i].desired_value);
        Car.setup.heaveSlowBump[i].stepsize = *new tdble(p_setup.heaveSlowBump[i].stepsize);
        Car.setup.heaveSlowBump[i].changed = *new bool(p_setup.heaveSlowBump[i].changed);
    }

    int setupHeaveBumpLvlCount = sizeof p_setup.heaveBumpLvel / sizeof(tCarSetupItem);
    for (int i = 0; i < setupHeaveBumpLvlCount; i++)
    {
        Car.setup.heaveBumpLvel[i].value = *new tdble(p_setup.heaveBumpLvel[i].value);
        Car.setup.heaveBumpLvel[i].min = *new tdble(p_setup.heaveBumpLvel[i].min);
        Car.setup.heaveBumpLvel[i].max = *new tdble(p_setup.heaveBumpLvel[i].max);
        Car.setup.heaveBumpLvel[i].desired_value = *new tdble(p_setup.heaveBumpLvel[i].desired_value);
        Car.setup.heaveBumpLvel[i].stepsize = *new tdble(p_setup.heaveBumpLvel[i].stepsize);
        Car.setup.heaveBumpLvel[i].changed = *new bool(p_setup.heaveBumpLvel[i].changed);
    }

    int setupFastReboundCount = sizeof p_setup.heaveFastRebound / sizeof(tCarSetupItem);
    for (int i = 0; i < setupFastReboundCount; i++)
    {
        Car.setup.heaveFastRebound[i].value = *new tdble(p_setup.heaveFastRebound[i].value);
        Car.setup.heaveFastRebound[i].min = *new tdble(p_setup.heaveFastRebound[i].min);
        Car.setup.heaveFastRebound[i].max = *new tdble(p_setup.heaveFastRebound[i].max);
        Car.setup.heaveFastRebound[i].desired_value = *new tdble(p_setup.heaveFastRebound[i].desired_value);
        Car.setup.heaveFastRebound[i].stepsize = *new tdble(p_setup.heaveFastRebound[i].stepsize);
        Car.setup.heaveFastRebound[i].changed = *new bool(p_setup.heaveFastRebound[i].changed);
    }

    int setupSlowReboundCount = sizeof p_setup.heaveSlowRebound / sizeof(tCarSetupItem);
    for (int i = 0; i < setupSlowReboundCount; i++)
    {
        Car.setup.heaveSlowRebound[i].value = *new tdble(p_setup.heaveSlowRebound[i].value);
        Car.setup.heaveSlowRebound[i].min = *new tdble(p_setup.heaveSlowRebound[i].min);
        Car.setup.heaveSlowRebound[i].max = *new tdble(p_setup.heaveSlowRebound[i].max);
        Car.setup.heaveSlowRebound[i].desired_value = *new tdble(p_setup.heaveSlowRebound[i].desired_value);
        Car.setup.heaveSlowRebound[i].stepsize = *new tdble(p_setup.heaveSlowRebound[i].stepsize);
        Car.setup.heaveSlowRebound[i].changed = *new bool(p_setup.heaveSlowRebound[i].changed);
    }

    int setupReboundLvlCount = sizeof p_setup.heaveReboundLvel / sizeof(tCarSetupItem);
    for (int i = 0; i < setupReboundLvlCount; i++)
    {
        Car.setup.heaveReboundLvel[i].value = *new tdble(p_setup.heaveReboundLvel[i].value);
        Car.setup.heaveReboundLvel[i].min = *new tdble(p_setup.heaveReboundLvel[i].min);
        Car.setup.heaveReboundLvel[i].max = *new tdble(p_setup.heaveReboundLvel[i].max);
        Car.setup.heaveReboundLvel[i].desired_value = *new tdble(p_setup.heaveReboundLvel[i].desired_value);
        Car.setup.heaveReboundLvel[i].stepsize = *new tdble(p_setup.heaveReboundLvel[i].stepsize);
        Car.setup.heaveReboundLvel[i].changed = *new bool(p_setup.heaveReboundLvel[i].changed);
    }

    int setupSuspSpringCount = sizeof p_setup.suspSpring / sizeof(tCarSetupItem);
    for (int i = 0; i < setupSuspSpringCount; i++)
    {
        Car.setup.suspSpring[i].value = *new tdble(p_setup.suspSpring[i].value);
        Car.setup.suspSpring[i].min = *new tdble(p_setup.suspSpring[i].min);
        Car.setup.suspSpring[i].max = *new tdble(p_setup.suspSpring[i].max);
        Car.setup.suspSpring[i].desired_value = *new tdble(p_setup.suspSpring[i].desired_value);
        Car.setup.suspSpring[i].stepsize = *new tdble(p_setup.suspSpring[i].stepsize);
        Car.setup.suspSpring[i].changed = *new bool(p_setup.suspSpring[i].changed);
    }

    int setupSuspBellcrankCount = sizeof p_setup.suspBellcrank / sizeof(tCarSetupItem);
    for (int i = 0; i < setupSuspBellcrankCount; i++)
    {
        Car.setup.suspBellcrank[i].value = *new tdble(p_setup.suspBellcrank[i].value);
        Car.setup.suspBellcrank[i].min = *new tdble(p_setup.suspBellcrank[i].min);
        Car.setup.suspBellcrank[i].max = *new tdble(p_setup.suspBellcrank[i].max);
        Car.setup.suspBellcrank[i].desired_value = *new tdble(p_setup.suspBellcrank[i].desired_value);
        Car.setup.suspBellcrank[i].stepsize = *new tdble(p_setup.suspBellcrank[i].stepsize);
        Car.setup.suspBellcrank[i].changed = *new bool(p_setup.suspBellcrank[i].changed);
    }

    int setupSuspInertanceCount = sizeof p_setup.suspInertance / sizeof(tCarSetupItem);
    for (int i = 0; i < setupSuspInertanceCount; i++)
    {
        Car.setup.suspInertance[i].value = *new tdble(p_setup.suspInertance[i].value);
        Car.setup.suspInertance[i].min = *new tdble(p_setup.suspInertance[i].min);
        Car.setup.suspInertance[i].max = *new tdble(p_setup.suspInertance[i].max);
        Car.setup.suspInertance[i].desired_value = *new tdble(p_setup.suspInertance[i].desired_value);
        Car.setup.suspInertance[i].stepsize = *new tdble(p_setup.suspInertance[i].stepsize);
        Car.setup.suspInertance[i].changed = *new bool(p_setup.suspInertance[i].changed);
    }

    int setupSuspCourseCount = sizeof p_setup.suspCourse / sizeof(tCarSetupItem);
    for (int i = 0; i < setupSuspCourseCount; i++)
    {
        Car.setup.suspCourse[i].value = *new tdble(p_setup.suspCourse[i].value);
        Car.setup.suspCourse[i].min = *new tdble(p_setup.suspCourse[i].min);
        Car.setup.suspCourse[i].max = *new tdble(p_setup.suspCourse[i].max);
        Car.setup.suspCourse[i].desired_value = *new tdble(p_setup.suspCourse[i].desired_value);
        Car.setup.suspCourse[i].stepsize = *new tdble(p_setup.suspCourse[i].stepsize);
        Car.setup.suspCourse[i].changed = *new bool(p_setup.suspCourse[i].changed);
    }

    int setupSuspPackerCount = sizeof p_setup.suspPacker / sizeof(tCarSetupItem);
    for (int i = 0; i < setupSuspPackerCount; i++)
    {
        Car.setup.suspPacker[i].value = *new tdble(p_setup.suspPacker[i].value);
        Car.setup.suspPacker[i].min = *new tdble(p_setup.suspPacker[i].min);
        Car.setup.suspPacker[i].max = *new tdble(p_setup.suspPacker[i].max);
        Car.setup.suspPacker[i].desired_value = *new tdble(p_setup.suspPacker[i].desired_value);
        Car.setup.suspPacker[i].stepsize = *new tdble(p_setup.suspPacker[i].stepsize);
        Car.setup.suspPacker[i].changed = *new bool(p_setup.suspPacker[i].changed);
    }

    int setupSuspFastBumpCount = sizeof p_setup.suspFastBump / sizeof(tCarSetupItem);
    for (int i = 0; i < setupSuspFastBumpCount; i++)
    {
        Car.setup.suspFastBump[i].value = *new tdble(p_setup.suspFastBump[i].value);
        Car.setup.suspFastBump[i].min = *new tdble(p_setup.suspFastBump[i].min);
        Car.setup.suspFastBump[i].max = *new tdble(p_setup.suspFastBump[i].max);
        Car.setup.suspFastBump[i].desired_value = *new tdble(p_setup.suspFastBump[i].desired_value);
        Car.setup.suspFastBump[i].stepsize = *new tdble(p_setup.suspFastBump[i].stepsize);
        Car.setup.suspFastBump[i].changed = *new bool(p_setup.suspFastBump[i].changed);
    }

    int setupSuspSlowBumpCount = sizeof p_setup.suspSlowBump / sizeof(tCarSetupItem);
    for (int i = 0; i < setupSuspSlowBumpCount; i++)
    {
        Car.setup.suspSlowBump[i].value = *new tdble(p_setup.suspSlowBump[i].value);
        Car.setup.suspSlowBump[i].min = *new tdble(p_setup.suspSlowBump[i].min);
        Car.setup.suspSlowBump[i].max = *new tdble(p_setup.suspSlowBump[i].max);
        Car.setup.suspSlowBump[i].desired_value = *new tdble(p_setup.suspSlowBump[i].desired_value);
        Car.setup.suspSlowBump[i].stepsize = *new tdble(p_setup.suspSlowBump[i].stepsize);
        Car.setup.suspSlowBump[i].changed = *new bool(p_setup.suspSlowBump[i].changed);
    }

    int setupSuspBumpLvlCount = sizeof p_setup.suspBumpLvel / sizeof(tCarSetupItem);
    for (int i = 0; i < setupSuspBumpLvlCount; i++)
    {
        Car.setup.suspBumpLvel[i].value = *new tdble(p_setup.suspBumpLvel[i].value);
        Car.setup.suspBumpLvel[i].min = *new tdble(p_setup.suspBumpLvel[i].min);
        Car.setup.suspBumpLvel[i].max = *new tdble(p_setup.suspBumpLvel[i].max);
        Car.setup.suspBumpLvel[i].desired_value = *new tdble(p_setup.suspBumpLvel[i].desired_value);
        Car.setup.suspBumpLvel[i].stepsize = *new tdble(p_setup.suspBumpLvel[i].stepsize);
        Car.setup.suspBumpLvel[i].changed = *new bool(p_setup.suspBumpLvel[i].changed);
    }

    int setupSuspFastReboundCount = sizeof p_setup.suspFastRebound / sizeof(tCarSetupItem);
    for (int i = 0; i < setupSuspFastReboundCount; i++)
    {
        Car.setup.suspFastRebound[i].value = *new tdble(p_setup.suspFastRebound[i].value);
        Car.setup.suspFastRebound[i].min = *new tdble(p_setup.suspFastRebound[i].min);
        Car.setup.suspFastRebound[i].max = *new tdble(p_setup.suspFastRebound[i].max);
        Car.setup.suspFastRebound[i].desired_value = *new tdble(p_setup.suspFastRebound[i].desired_value);
        Car.setup.suspFastRebound[i].stepsize = *new tdble(p_setup.suspFastRebound[i].stepsize);
        Car.setup.suspFastRebound[i].changed = *new bool(p_setup.suspFastRebound[i].changed);
    }

    int setupSuspSlowReboundCount = sizeof p_setup.suspSlowRebound / sizeof(tCarSetupItem);
    for (int i = 0; i < setupSlowReboundCount; i++)
    {
        Car.setup.suspSlowRebound[i].value = *new tdble(p_setup.suspSlowRebound[i].value);
        Car.setup.suspSlowRebound[i].min = *new tdble(p_setup.suspSlowRebound[i].min);
        Car.setup.suspSlowRebound[i].max = *new tdble(p_setup.suspSlowRebound[i].max);
        Car.setup.suspSlowRebound[i].desired_value = *new tdble(p_setup.suspSlowRebound[i].desired_value);
        Car.setup.suspSlowRebound[i].stepsize = *new tdble(p_setup.suspSlowRebound[i].stepsize);
        Car.setup.suspSlowRebound[i].changed = *new bool(p_setup.suspSlowRebound[i].changed);
    }

    int setupSuspReboundLvlCount = sizeof p_setup.suspReboundLvel / sizeof(tCarSetupItem);
    for (int i = 0; i < setupSuspReboundLvlCount; i++)
    {
        Car.setup.suspReboundLvel[i].value = *new tdble(p_setup.suspReboundLvel[i].value);
        Car.setup.suspReboundLvel[i].min = *new tdble(p_setup.suspReboundLvel[i].min);
        Car.setup.suspReboundLvel[i].max = *new tdble(p_setup.suspReboundLvel[i].max);
        Car.setup.suspReboundLvel[i].desired_value = *new tdble(p_setup.suspReboundLvel[i].desired_value);
        Car.setup.suspReboundLvel[i].stepsize = *new tdble(p_setup.suspReboundLvel[i].stepsize);
        Car.setup.suspReboundLvel[i].changed = *new bool(p_setup.suspReboundLvel[i].changed);
    }

    Car.setup.reqRepair.value = *new tdble(p_setup.reqRepair.value);
    Car.setup.reqRepair.min = *new tdble(p_setup.reqRepair.min);
    Car.setup.reqRepair.max = *new tdble(p_setup.reqRepair.max);
    Car.setup.reqRepair.desired_value = *new tdble(p_setup.reqRepair.desired_value);
    Car.setup.reqRepair.stepsize = *new tdble(p_setup.reqRepair.stepsize);
    Car.setup.reqRepair.changed = *new bool(p_setup.reqRepair.changed);

    Car.setup.reqTireset.value = *new tdble(p_setup.reqTireset.value);
    Car.setup.reqTireset.min = *new tdble(p_setup.reqTireset.min);
    Car.setup.reqTireset.max = *new tdble(p_setup.reqTireset.max);
    Car.setup.reqTireset.desired_value = *new tdble(p_setup.reqTireset.desired_value);
    Car.setup.reqTireset.stepsize = *new tdble(p_setup.reqTireset.stepsize);
    Car.setup.reqTireset.changed = *new bool(p_setup.reqTireset.changed);

    Car.setup.reqPenalty.value = *new tdble(p_setup.reqPenalty.value);
    Car.setup.reqPenalty.min = *new tdble(p_setup.reqPenalty.min);
    Car.setup.reqPenalty.max = *new tdble(p_setup.reqPenalty.max);
    Car.setup.reqPenalty.desired_value = *new tdble(p_setup.reqPenalty.desired_value);
    Car.setup.reqPenalty.stepsize = *new tdble(p_setup.reqPenalty.stepsize);
    Car.setup.reqPenalty.changed = *new bool(p_setup.reqPenalty.changed);

    // Copy p_car.pitcmd
#define p_pitcmd p_car->pitcmd
    Car.pitcmd.fuel = *new tdble(p_pitcmd.fuel);
    Car.pitcmd.repair = *new int(p_pitcmd.repair);
    Car.pitcmd.stopType = *new int(p_pitcmd.stopType);
    Car.pitcmd.setupChanged = *new bool(p_pitcmd.setupChanged);
    Car.pitcmd.tireChange = *new CarPitCmd::TireChange(p_pitcmd.tireChange);

    // Copy p_car.robot
    RobotItf* robot = new RobotItf();
    Car.robot = robot;
    // TODO Void pointers

    // Copy p_car.next
    // TODO Another car..

    // Copy p_situation
    Situation.raceInfo.ncars = *new int(p_situation->raceInfo.ncars);
    Situation.raceInfo.totLaps = *new int(p_situation->raceInfo.totLaps);
    Situation.raceInfo.extraLaps = *new int(p_situation->raceInfo.extraLaps);
    Situation.raceInfo.totTime = *new double(p_situation->raceInfo.totTime);
    Situation.raceInfo.state = *new int(p_situation->raceInfo.state);
    Situation.raceInfo.type = *new int(p_situation->raceInfo.type);
    Situation.raceInfo.maxDammage = *new int(p_situation->raceInfo.maxDammage);
    Situation.raceInfo.fps = *new unsigned long(p_situation->raceInfo.fps);
    Situation.raceInfo.features = *new int(p_situation->raceInfo.features);
    Situation.deltaTime = *new double(p_situation->deltaTime);
    Situation.currentTime = *new double(p_situation->currentTime);
    Situation.accelTime = *new double(p_situation->accelTime);
    Situation.nbPlayers = *new int(p_situation->nbPlayers);
    // TODO **cars
}
