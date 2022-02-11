/***************************************************************************

    file                 : driver.cpp
    created              : 2006-08-31 01:21:49 UTC
    copyright            : (C) Daniel Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <teammanager.h>

#include "driver.h"

#include "MyParam.h"
#include "Utils.h"

#include <sstream>
#include <iostream>
#include <string>
#include <stdio.h>

#include <portability.h>


//#define TIME_ANALYSIS
#ifdef TIME_ANALYSIS
#include "Timer.h"
#endif

#define RANDOM_SEED 0xfded
#define RANDOM_A    1664525
#define RANDOM_C    1013904223

#define SECT_SKILL                  "skill"
#define PRV_SKILL_LEVEL             "level"
#define PRV_SKILL_AGGRO             "aggression"

Driver::Driver(int index) :
    INDEX(index),
    driver_aggression(0.0),
    mGarage(false),
    mFrontCollMargin(6.0),
    mOvtMargin(2.0)
{
    // Names assigned in constructor for VS 2013 compatibility
    mFlagNames.push_back("STATE_CHANGE");
    mFlagNames.push_back("DRIVING_FAST");
    mFlagNames.push_back("FRICT_LR");
    mFlagNames.push_back("COLL");
    mFlagNames.push_back("WAIT");
    mFlagNames.push_back("LET_PASS");
    mFlagNames.push_back("CATCH");
    mFlagNames.push_back("OVERTAKE");
    mFlagNames.push_back("FAST_BEHIND");
}

//==========================================================================*
// Get Team info
//--------------------------------------------------------------------------*
void Driver::TeamInfo(tCarElt* car, tSituation* situation)
{
    //RtTeamManagerShowInfo();
    mTeamIndex = RtTeamManagerIndex(car, track, situation);
    LogUSR.info("#Team index = %i\n", mTeamIndex);
    RtTeamManagerDump();
}

void Driver::InitTrack(tTrack* Track, void* carHandle, void** carParmHandle, tSituation* situation)
{
    LogUSR.debug(".......... %s Driver initrack .........\n", mDriverName);
    track = Track;

    const int BUFSIZE = 1024;
    char buffer[BUFSIZE];
    char carName[BUFSIZE];
    /*------------------------------------------------------*/
    /*     Load a custom setup if one is available.    */
    /*------------------------------------------------------*/
    // Get a pointer to the first char of the track filename.
    char trackname[100];
    std::string tName;
    std::string cName;
    std::string rName;
    strncpy( trackname, strrchr(track->filename, '/') + 1, sizeof(trackname) - 1);
    *strrchr(trackname, '.') = '\0';
    LogUSR.info( " # USR trackName: '%s'\n", trackname );
    tName = trackname;

    if (strcmp(trackname, "garage") == 0)
        mGarage = true;

    // Setup for this robot
    //void *newParmHandle;
    *carParmHandle = NULL;
    //newParmHandle = NULL;

    const char *car_sect = SECT_GROBJECTS "/" LST_RANGES "/" "1";
    strncpy(carName, GfParmGetStr(carHandle, car_sect, PRM_CAR, ""), sizeof(carName) - 1);
    char *p = strrchr(carName, '.');

    if (p)
        *p = '\0';

    cName = carName;
    rName = mDriverName;

    LogUSR.info( " # USR carName: '%s'\n", carName );

    Meteorology(track);

    std::string mData(std::string(GetDataDir()) + "drivers/" + rName + "/" + cName + "/");
    mDataDir = mData;

    // Assign to carParmHandle my parameters file handle, it will later be merged with carHandle by TORCS
    MyParam param(carParmHandle, mDataDir, Track->internalname, weathercode);
    LogUSR.info(" # Driver %s call param \n", mDriverName);

    // Read the parameters
    mTestPath = (PathType)((int)param.getNum("private", "test line")); // (int) for VS 2013 compatibility
    //mMsgOn = (int)param.getNum("private", "message on"); // (int) for VS 2013 compatibility
    mDataLogOn = (int)param.getNum("private", "data log on");
    mPitDamage = (int)param.getNum("private", "pitdamage");
    mPitGripFactor = param.getNum("private", "pitgripfactor");
    mPitEntryMargin = param.getNum("private", "pitentrymargin");
    mPitEntrySpeed = param.getNum("private", "pitentryspeed");
    mPitExitSpeed = param.getNum("private", "pitexitspeed");
    mPitTest = (int)param.getNum("private", "pittest");
    mSegLen = param.getNum("private", "seglen");
    mClothFactor = param.getNum("private", "clothoidfactor");
    mVMaxK = param.getNum("private", "vmaxk");
    mVMaxKFactor = param.getNum("private", "vmaxkfactor");

    if (mVMaxK == 0.0)
        mVMaxK = 0.0018;

    if (mVMaxKFactor == 0.0)
        mVMaxKFactor = 0.9;

    mCar.readPrivateSection(param);
    mCar.readVarSpecs(param);
    mCar.readConstSpecs(carHandle);

    // Init my track
    mTrack.init(track, mSegLen);

    // Set initial fuel
    double distance = 0.0;


    if (mPitTest > 0)
        distance = mTrack.length() * 2 + 0.3;
    else
        distance = 1.05 * situation->_totLaps * mTrack.length();

    LogUSR.info("distance fuel = %.3f\n", distance);

    double startfuel = mCar.calcFuel(distance);
    LogUSR.info("Start fuel : %.3f\n", startfuel);
    double initFuel = param.getNum("private", "max fuel");

    if (initFuel > 1.0)
        startfuel = MIN(startfuel, initFuel);

    param.setNum(SECT_CAR, PRM_FUEL, startfuel);

    // load the global skill level, range 0 - 10
    snprintf(buffer, sizeof(buffer), "%sconfig/raceman/extra/skill.xml", GetLocalDir());
    void *skillHandle = GfParmReadFile(buffer, GFPARM_RMODE_REREAD);

    if(!skillHandle)
    {
        snprintf(buffer, sizeof(buffer), "%sconfig/raceman/extra/skill.xml", GetDataDir());
        skillHandle = GfParmReadFile(buffer, GFPARM_RMODE_REREAD);
    }//if !skillHandle

    if (skillHandle)
    {
        SkillGlobal = GfParmGetNum(skillHandle, SECT_SKILL, PRV_SKILL_LEVEL, (char *) NULL, 30.0f);
        GfParmReleaseHandle(skillHandle);
    }

    SkillGlobal = MAX(0.7, 1.0 - 0.5 * SkillGlobal / 10.0);

    LogUSR.info(" # Global Skill: %.3f\n", SkillGlobal);

    //load the driver skill level, range 0 - 1
    SkillDriver = 0.0f;
    snprintf(buffer, sizeof(buffer), "%sdrivers/%s/%d/skill.xml", GetDataDir(), mDriverName, INDEX);
    LogUSR.info("Path skill driver: %s\n", buffer);
    skillHandle = GfParmReadFile(buffer, GFPARM_RMODE_STD);

    if (skillHandle)
    {
        SkillDriver = GfParmGetNum(skillHandle, SECT_SKILL, PRV_SKILL_LEVEL, (char *) NULL, 0.0);
        driver_aggression = (double)GfParmGetNum(skillHandle, SECT_SKILL, PRV_SKILL_AGGRO, (char *)NULL, 0.0);
        GfParmReleaseHandle(skillHandle);
        LogUSR.info( "# driver skill: %.2f - driver agression: %.3f\n", SkillDriver, driver_aggression);
        SkillDriver = MAX(0.95, 1.0 - 0.05 * SkillDriver);
        //driver_aggression /= driver_aggression;
        driver_aggression = MIN(1.0, MAX(0.7, 0.99 + driver_aggression));

        LogUSR.info(" # Global skill = %.2f - driver skill: %.2f - driver agression: %.3f\n", SkillGlobal, SkillDriver, driver_aggression);
    }
    else
        LogUSR.info("Couldn't load : %s\n", buffer);
}

void Driver::NewRace(tCarElt* car, tSituation* situation)
{
    LogUSR.info("********** %s : NewRace() **********\n", mDriverName);

    mSituation = situation;
    mSimTime = -1.0;
    initVars();

    TeamInfo(car, mSituation);
    mCar.init(car, &mTrack);
    mPit.init(mTrack.torcsTrack(), situation, &mCar, mPitDamage, mPitGripFactor, mPitEntryMargin);

    // Create paths
    mPath.clear();

    for (unsigned i = 0; i < sizeof(PathNames)/sizeof(PathNames[0]); i++)
    {
        mPath.push_back(Path(&mTrack, mDataDir, mClothFactor, mVMaxK, mVMaxKFactor, (PathType)i));
    }

    mOpps.init(mTrack.torcsTrack(), situation, &mCar, &mPath[PATH_O]);

    // Get mu factors
    mMuFactors.read(mDataDir, mTrack.name());

    // Create path states
    mPathState.clear();

    for (unsigned i = 0; i < sizeof(PathNames)/sizeof(PathNames[0]); i++)
    {
        mPathState.push_back(PathState(&mPath[i], &mCar, &mMuFactors));
    }

    // Messages
    //mMsg.init(mLocalDir, &mCar);

    // Data log
    mDataLog.init(mLocalDir, mCar.car()->_name);
    mDataLog.add("time", &mSimTime, 1.0);
    mDataLog.add("fs", &mFromStart, 1.0);
    mDataLog.add("10*LRtarget", &mLRTargetPortion, 10.0);
    mDataLog.add("10*pathoffs", &mPathOffs, 10.0);
    mDataLog.add("vmax", &mMaxspeed, 1.0);
    mDataLog.add("v", &mCar.mSpeed, 1.0);
    mDataLog.add("acc", &mAccelPedal, 100.0);
    mDataLog.add("brake", &mBrakePedal, -100.0);
    mDataLog.add("steer", &mSteer, 100.0);
    mDataLog.add("steeryaw", &mYawSteer, 270.0);
    mDataLog.add("steeryawrate", &mYawRateSteer, 270.0);
    mDataLog.add("steercurv", &mCurvSteer, 270.0);
    mDataLog.add("steeroffs", &mOffsSteer, 270.0);
    mDataLog.add("steeroffsderiv", &mOffsDerivSteer, 270.0);
    mDataLog.add("10*slip", &mCar.mSlip, 10.0);
    mDataLog.add("sideslip", &mCar.mSideSlip, 1.0);
    mDataLog.add("10*toMid", &mCar.mToMiddle, 10.0);

    LogUSR.info("__________  %s initialized __________\n", mDriverName);
}

void Driver::Drive()
{
#ifdef TIME_ANALYSIS
    Timer tmr;
#endif
    if (mGarage)
        return;

    //mMsg.clearPanel();
    updateTime();
    updateBasics();
    updateOpponents();
    updatePathState(); // avg. 200usec on e-track-1 (debug build)
    updateOvertakePath();
    updateDrivingFast();
    updateLetPass();
    updateOnCollision();
    calcStateAndPath();
    calcOffsetAndYaw();
    calcMaxspeed();
    //  std::cout << mFromStart << " vmax: " << 3.6*mMaxspeed << " offs: " << mPathOffs << std::endl;
    setControls();
    printInfos();
    setPrevVars();
    //std::cout << mCar.car()->_name << " dist=" << mCar.car()->_distFromStartLine << std::endl;
#ifdef TIME_ANALYSIS
    double usec = 1000 * 1000 * tmr.elapsed();
    std::cout << "usec " << usec << std::endl;
#endif
}

int Driver::PitCmd() // handle pitstop
{
    mPit.pitCommand();

    return ROB_PIT_IM;  // ready to be serviced
}

void Driver::EndRace()
{
    // This is never called by TORCS! Don't use it!
}

void Driver::Shutdown()
{
    /*if (mMsgOn)
    {
        mMsg.write();
    }*/

    if (mDataLogOn)
    {
        mDataLog.write();
    }
}

void Driver::updateTime()
{
    mDeltaTime = mSituation->currentTime - mSimTime;
    mSimTime   = mSituation->currentTime;
}

void Driver::updateBasics()
{
    //  std::cout << mFromStart << " k " <<  mPath[mDrvPath].curvature(mFromStart) << " kz " <<  mPath[mDrvPath].curvZ(mFromStart) << std::endl;
    mCar.update(mDeltaTime);
    mFromStart = fromStart(mCar.car()->_distFromStartLine); // TORCS problem with negative _distFromStartLine
    mPit.update();
    mCurrMu = mMuFactors.muFactor(mFromStart) * mCar.segMu();
    // Brake pedal
    double brakeforce = mCar.brakeForce(mCar.v(), mPath[mDrvPath].curvature(mFromStart), mPath[mDrvPath].curvZ(mFromStart), mCurrMu * mCar.brakeMuFactor(), 0.0, 0.0, PATH_O);
    brakeforce = std::max(0.15 * mCar.maxBrakeForce(), brakeforce);
    mBrakePedalRace = brakeforce / mCar.maxBrakeForce() + 0.1;
    mBrakePedalRace = Utils::clip(mBrakePedalRace, 0.0, 1.0);
    // Accel pedal for 2 wheel drive
    double accelpedal = 0.5 * brakeforce / mCar.maxAccelForce();
    accelpedal = Utils::clip(accelpedal, 0.0, 1.0);
    mAccelPedalRace = 0.8 * accelpedal;
    // LR tire friction balance
    m[FRICT_LR] = false;

    if (fabs(mCar.tires()->frictionBalanceLR()) > 0.2)
    {
        m[FRICT_LR] = true;
    }
}

void Driver::updateOpponents()
{
    mOpps.update();
    m[FAST_BEHIND] = mOpps.oppComingFastBehind();
    // Distances
    if (mOpps.oppNear() != NULL)
    {
        m[CATCH] = Utils::hysteresis(m[CATCH], 6.0 - mOpps.oppNear()->catchTime(), 3.0);
    }
}

void Driver::updatePathState()
{
    for (unsigned i = 0; i < sizeof(PathNames)/sizeof(PathNames[0]); i++)
    {
        mPathState[i].update(mDeltaTime);
    }
}

void Driver::calcOffsetAndYaw()
{
    // Set LR target portion
    double maxStepLR = 0.01;
    double stepdiff = 0.002;

    if (mDrvPath == PATH_L && (m[OVERTAKE] || m[LET_PASS] || mDrvState == STATE_PITLANE || mPit.pitstop() || mTestPath != PATH_O))
    {
        if (mLRTargetPortion > 0.98 && mLRTargetStep >= stepdiff)
        {
            mLRTargetStep -=  stepdiff;
        }
        else  if (mLRTargetPortion <= 0.98)
        {
            mLRTargetStep +=  stepdiff;
        }
    }
    else if (mDrvPath == PATH_R && (m[OVERTAKE] || m[LET_PASS] || mDrvState == STATE_PITLANE || mPit.pitstop() || mTestPath != PATH_O))
    {
        if (mLRTargetPortion < -0.98 && mLRTargetStep <= -stepdiff)
        {
            mLRTargetStep +=  stepdiff;
        }
        else  if (mLRTargetPortion >= -0.98)
        {
            mLRTargetStep -=  stepdiff;
        }
    }
    else if (mDrvPath != PATH_O && fabs(mLRTargetPortion) > maxStepLR)
    {
        double sign = copysign(1.0, pathOffs(PATH_O));
        mLRTargetStep += stepdiff * sign;
    }
    else
    {
        mLRTargetStep = 0.0;
        mLRTargetPortion = 0.0;
    }

    mLRTargetStep = Utils::clip(mLRTargetStep, -maxStepLR, maxStepLR);
    mLRTargetPortion += mLRTargetStep;
    mLRTargetPortion = Utils::clip(mLRTargetPortion, -1.0, 1.0);

    if (mLRTargetPortion > 0.0)
    {
        mPathOffs = pathOffs(PATH_O) + mLRTargetPortion * (pathOffs(PATH_L) - pathOffs(PATH_O));
        mPathYaw = Utils::normPiPi(mPathState[PATH_O].yaw() + mLRTargetPortion * Utils::normPiPi(mPathState[PATH_L].yaw() - mPathState[PATH_O].yaw()));
        mPathCurvature = mPathState[PATH_O].curvature() + mLRTargetPortion * (mPathState[PATH_L].curvature() - mPathState[PATH_O].curvature());
    }
    else
    {
        mPathOffs = pathOffs(PATH_O) - mLRTargetPortion * (pathOffs(PATH_R) - pathOffs(PATH_O));
        mPathYaw = Utils::normPiPi(mPathState[PATH_O].yaw() - mLRTargetPortion * Utils::normPiPi(mPathState[PATH_R].yaw() - mPathState[PATH_O].yaw()));
        mPathCurvature = mPathState[PATH_O].curvature() - mLRTargetPortion * (mPathState[PATH_R].curvature() - mPathState[PATH_O].curvature());
    }

    switch (mDrvState)
    {
    case STATE_RACE:
    {
        break;
    }

    case STATE_STUCK:
    {
        break;
    }

    case STATE_OFFTRACK:
    {
        if (mCar.wallDist() < 0.0)
        {
            // We are on the wrong side of the pit wall
            double toMiddle = copysign(1.0, mCar.toMid()) * (mCar.wallToMiddleAbs() + 2.0);
            mPathOffs = toMiddle - mCar.toMid();
        }
        break;
    }
    case STATE_PITLANE:
    {
        double pitToMiddle = mPit.pathToMiddle(mFromStart);
        double pitToMiddleForward05 = mPit.pathToMiddle(fromStart(mFromStart + 0.5));
        double pitToMiddleForward1 = mPit.pathToMiddle(fromStart(mFromStart + 1.0));
        mPathOffs = pitToMiddle - mCar.toMid();
        mPathYaw = mTrack.yaw(mFromStart) + 2.0 * (pitToMiddleForward05 - pitToMiddle);
        double pathYawForward05 = mTrack.yaw(mFromStart) + 2.0 * (pitToMiddleForward1 - pitToMiddleForward05);
        mPathCurvature = mTrack.curvature(mFromStart) + 2.0 * (pathYawForward05 - mPathYaw);
        break;
    }
    case STATE_PITSTOP:
    {
        break;
    }
    }

    mPathYaw_carSpeedYaw_diff = Utils::normPiPi(mPathYaw - mCar.speedYaw());
    mPathOffsDeriv = (mPathOffsDeriv + (mPathOffs - mPrevPathOffs) / mDeltaTime) / 2.0;

    if (m[STATE_CHANGE])
    {
        mPrevPathOffs = 0.0;
        mPathOffsDeriv = 0.0;
    }
}

void Driver::setControls()
{
    mBrakePedal = mCar.filterABS(getBrake(mMaxspeed));
    mAccelPedal = mCar.filterTCLSideSlip(mCar.filterTCL(getAccel(mMaxspeed))); // must be after brake
    mSteer = getSteer(mCar.car()->_steerLock);
    mCar.setControls(mAccelPedal, mBrakePedal, mSteer);
}

void Driver::printInfos()
{
    if (mDataLogOn)
    {
        mDataLog.update();
    }

    /*if (!mMsgOn)
    {
        return;
    }*/

    // TORCS message panel lines 1 and 2 reserved for driver (max. 31 chars)
    std::string flagdesc = "S P  ff  cw lco  b FS";
    std::stringstream flagss;
    flagss << mDrvState << " " << mDrvPath << " "
           << m[1] << m[2] << " "
           << m[3] << m[4] << " "
           << m[5] << m[6] << m[7] << " "
           << m[8] << " "
           << (int)mFromStart;
    std::string flagstring = flagss.str();
    //mMsg.displayOnPanel(flagdesc, flagstring);

    // Lap info
    if (mFromStart < 3.0)
    {
        if (mSimTime - mLapSimTime > 1.0)
        {
            double lapsimtime = mSimTime;

            if (mCar.v() > 0.001)
            {
                lapsimtime = mSimTime - mFromStart / mCar.v();
            }

            double laptime = lapsimtime - mLapSimTime;
            LogUSR.debug("%.3f %s laptime %.3f\n", mSimTime, flagstring.c_str(), laptime);
            mLapSimTime = lapsimtime;
            LogUSR.debug("%.3f %s avgfuelperlap %.3f\n", mSimTime, flagstring.c_str(), mPit.avgFuelPerLap());
        }
    }

    // Changed flags
    if (m != mPrev)
    {
        for (unsigned i = 0; i < mFlagNames.size(); ++i)
        {
            if (m[i] != mPrev[i])
            {
                LogUSR.debug("%.3f %s %s \n", mSimTime, flagstring.c_str(), mFlagNames[i].c_str());
            }
        }
    }
}

void Driver::initVars()
{
    mDrvState = STATE_RACE;
    mDrvPath = PATH_O;
    mDrvPathOld = PATH_O;
    mOvertakePath = PATH_L;
    mAccelPedal = 0.0;
    mSpeedController.mP = 0.5;
    mSpeedController.mD = 0.09;
    mLapSimTime = 0.0;
    mStuckTime = 0.0;

    mYawSteer = 0.0;
    mOffsDerivSteer = 0.0;
    mYawRateSteer = 0.0;
    mPathOffs = 0.0;
    mPrevPathOffs = 0.0;
    mPathOffsDeriv = 0.0;
    mLRTargetPortion = 0.0;
    mLRTargetStep = 0.0;

    m.clear();

    for (unsigned i = 0; i < mFlagNames.size(); ++i)
    {
        m.push_back(false);
    }

    setPrevVars();
}

void Driver::setPrevVars()
{
    mPrev = m;
    mPrevPathOffs = mPathOffs;
}

double Driver::pitSpeed()
{
    double pitlimitdist = fromStart(mPit.limitEntry() - mFromStart);
    double pitdist = mPit.dist();

    if (mPit.penalty() == RM_PENALTY_DRIVETHROUGH)
    {
        pitdist = 1000.0;
    }

    double speedEntryExit = mPit.pitstop() ? mPitEntrySpeed : mPitExitSpeed;
    double pitspeed = speedEntryExit;
    double mintrackspeed = std::min(pathSpeed(PATH_L), pathSpeed(PATH_R));
    pitspeed = std::min(pitspeed, 0.8 * mintrackspeed);

    if (pitlimitdist < brakeDist(PATH_O, mCar.v(), mPit.speedLimit()) || mPit.isPitLimit(mFromStart))
    {
        pitspeed = mPit.speedLimit();
    }

    if (pitdist < 2.0 * brakeDist(PATH_O, mCar.v(), 0.0))
    {
        pitspeed = 0.0;
    }

    return pitspeed;
}

double Driver::brakeDist(PathType path, double speed, double allowedspeed)
{
    double speeddiff = speed - allowedspeed;

    if (speeddiff <= 0.0)
    {
        return -1000.0;
    }

    double v1 = speed;
    double v2;
    double totaldist = 0.0;
    int idx = mPath[path].segIdx(mFromStart);
    idx++;
    double seglen = fromStart(mPath[path].seg(idx).fromStart - mFromStart);
    int maxidx = (int)(300.0 / mPath[path].segLenMid()); // (int) for VS 2013 compatibility

    for (int i = 0; i < maxidx; i++)
    {
        double curv = mPath[path].seg(idx + i).k;
        double curvz = mPath[path].seg(idx + i).kz;
        double pitch = mPath[path].seg(idx + i).pitchAngle;
        double roll = mPath[path].seg(idx + i).rollAngle;
        double force = mCar.brakeForce(v1, curv, curvz, mCurrMu * mCar.brakeMuFactor(), pitch, roll, path) + mCar.cw() * v1 * v1;
        double acc = -force / mCar.mass();
        double vv1 = v1 * v1;

        if (vv1 > -(2 * acc * seglen))
        {
            v2 = sqrt(vv1 + 2 * acc * seglen);
        }
        else
        {
            v2 = v1 + acc * seglen / v1;
        }

        if (v2 <= allowedspeed)
        {
            totaldist += seglen * (v1 - allowedspeed) / (v1 - v2);

            return 1.1 * totaldist;
        }
        totaldist += seglen;
        seglen = mPath[path].seg(idx + i).segLen;
        v1 = v2;
    }

    return 300.0;
}

double Driver::getBrake(double maxspeed) const
{
    double brakepedal = 0.0;

    if (mCar.v() > maxspeed)
    {
        brakepedal = mBrakePedalRace;
    }

    switch (mDrvState)
    {
    case STATE_RACE:
    {
        break;
    }

    case STATE_STUCK:
    {
        break;
    }

    case STATE_OFFTRACK:
    {
        brakepedal *= 0.2;
        break;
    }

    case STATE_PITLANE:
    {
        // Pit speed limiter
        if (mCar.v() > maxspeed - 0.1 && mCar.v() <= maxspeed)
        {
            brakepedal = 0.05;
        }
        break;
    }

    case STATE_PITSTOP:
    {
        brakepedal = mBrakePedalRace;
        break;
    }
    }

    if (m[COLL] || (mCar.vX() < -1.0 && mDrvState != STATE_STUCK))
    {
        brakepedal = mBrakePedalRace;
    }

    // Side slip braking
    if (fabs(mCar.sideSlip()) > 40.0)
    {
        brakepedal += 0.05;
    }
    else if (fabs(mCar.sideSlip()) > 45.0)
    {
        brakepedal += 0.1;
    }
    // LR tire friction unbalanced (grass on border)
    if (m[FRICT_LR])
    {
        brakepedal = 0.0;
    }

    brakepedal *= driver_aggression;

    return brakepedal;
}

double Driver::getAccel(double maxspeed)
{
    double accel = mAccelPedal;

    if (mBrakePedal == 0.0)
    {
        accel = controlSpeed(accel, 0.98 * maxspeed);
    }
    // Save fuel when near opponent
    if (mSimTime > 100.0 && mOpps.oppNear() != NULL)
    {
        if (mOpps.oppNear()->dist() > 5.0 && mOpps.oppNear()->dist() < 25.0)
        {
            if (!mOpps.oppNear()->backMarker() && !mOpps.oppNear()->damaged())
            {
                if (mCar.v() > 0.9 * maxspeed)
                {
                    accel *= 0.5;
                }
            }
        }
    }

    if (m[LET_PASS])
    {
        accel *= 0.5;
    }
    else if (mOpps.mateFrontAside())
    {
        accel *= 0.7;
    }

    // LR tire friction unbalanced (grass on border)
    if (m[FRICT_LR] && mCar.v() > 25.0)
    {
        accel = 0.0;
    }

    if (mBrakePedal > 0.0 || (fabs(mPathYaw_carSpeedYaw_diff) > 0.11 && mCar.v() > 15.0) || mOppAsideAtPitEntry)
    {
        accel = 0.0;
    }

    if (mSimTime < 0.0)
    {
        if (mCar.car()->_enginerpm / mCar.car()->_enginerpmRedLine > 0.7) {
            accel = 0.0;
        }
    }

    accel *=SkillDriver;

    return accel;
}

double Driver::getSteer(double steerlock)
{
    double steer = getSteerAngle(steerlock);

    // Reverse when stuck
    if (mCar.vX() < 0.0)
    {
        if (fabs(mCar.angToTrack()) < 1.0)
        {
            steer = -mCar.angToTrack() / 4.0;
        }
        else
        {
            steer = -0.5 * copysign(1.0, mCar.angToTrack());
        }
    }

    // Warm up tyres in qualifying
    if (mSituation->_raceType == RM_TYPE_QUALIF && !m[DRIVING_FAST] && mCar.tires()->gripFactor() < 0.8)
    {
        if ((mCar.car()->_laps == 1) || (mCar.car()->_laps == 2 && 0.7 * mCar.car()->_lastLapTime > mCar.car()->_curLapTime))
        {
            steer += 0.2 * sin(30.0 * mSimTime);
        }
    }

    steer = Utils::clip(steer, -steerlock, steerlock);
    steer = steer / steerlock;

    double rearSlip = (mCar.car()->_wheelSlipSide(REAR_LFT) + mCar.car()->_wheelSlipSide(REAR_RGT)) / 2;
    double frntSlip = (mCar.car()->_wheelSlipSide(FRNT_LFT) + mCar.car()->_wheelSlipSide(FRNT_RGT)) / 2;

    if (fabs(rearSlip) > fabs(frntSlip) + 2.0)
    {
        rearSlip -= frntSlip;
        int sgn = (rearSlip > 0 ? 1 : -1);
        rearSlip -= 2.0 * sgn;
        rearSlip = rearSlip * (rearSlip / 3) * sgn;
        rearSlip = MAX(-0.9, MIN(0.9, rearSlip / (900.0 + MIN(mCar.car()->_accel_x, mCar.car()->_accel_x * 1.1))));
        rearSlip /= mCar.car()->_steerLock;
        steer += rearSlip;
    }

    return steer;
}

bool Driver::stateStuck()
{
    if (stuck())
    {
        mCar.driveBackwards();

        return true;
    }

    mCar.driveForward();

    return false;
}

bool Driver::stateOfftrack() const
{
    if (mDrvState != STATE_PITLANE && mDrvState != STATE_PITSTOP)
    {
        double limit = 2.0;

        if (mDrvState == STATE_OFFTRACK)
        {
            limit = 0.5;
        }

        if (mCar.borderDist() < -limit && mCar.borderFriction() < 1.0)
        {
            return true;
        }
    }

    return false;
}

bool Driver::statePitstop() const
{
    // Traffic in the way when leaving?
    if (mDrvState == STATE_PITSTOP)
    {
        if (mOpps.oppNear() != NULL)
        {
            if (fabs(mOpps.oppNear()->dist()) < 10.0 && mOpps.oppNear()->v() > 3.0 )
            {
                return true;
            }
        }

        if (mOpps.oppBack() != NULL)
        {
            if (mOpps.oppBack()->dist() > -20.0 && mOpps.oppBack()->v() > 5.0 && mOpps.oppBack()->v() < 25.0)
            {
                return true;
            }
        }
    }

    // Conditions for stop
    if ((mDrvState == STATE_PITLANE || mDrvState == STATE_PITSTOP) && mPit.pitstop())
    {
        float dl, dw;
        RtDistToPit((CarElt*)mCar.car(), (tTrack*)mTrack.torcsTrack(), &dl, &dw);

        if (fabs(dw) < 1.6 && (dl < 0.5 || dl > mTrack.length() - 1.0))
        {
            return true;
        }
    }

    return false;
}

bool Driver::statePitlane()
{
    mOppAsideAtPitEntry = false;
    double pitlanetomiddle = mPit.pathToMiddle(mFromStart);

    if (pitlanetomiddle != 0.0)
    {
        if (mOpps.oppNear() != NULL && !(mDrvState == STATE_PITLANE || mDrvState == STATE_PITSTOP))
        {
            if (mOpps.oppNear()->aside())
            {
                mOppAsideAtPitEntry = true;

                return false;
            }
        }

        return true;
    }

    return false;
}

void Driver::updateLetPass()
{
    if (mOpps.oppLetPass() == NULL)
    {
        m[LET_PASS] = false;
        return;
    }

    if (mDrvState != STATE_RACE)
    {
        m[LET_PASS] = false;
        return;
    }
    // Check range
    double range = 15.0;

    if (m[LET_PASS])
    {
        range = 25.0;
    }

    if (mOpps.oppLetPass()->dist() < -range || mOpps.oppLetPass()->dist() > 0.0)
    {
        m[LET_PASS] = false;
        return;
    }

    // Check for other opponent between behind
    if (mOpps.oppBack() != NULL)
    {
        if (mOpps.oppBack() != mOpps.oppLetPass() && mOpps.oppBack()->dist() > mOpps.oppLetPass()->dist())
        {
            m[LET_PASS] = false;
            return;
        }
    }

    // Check for other opponent aside
    if (mOpps.oppNear() != NULL)
    {
        if (mOpps.oppNear() != mOpps.oppLetPass())
        {
            if (fabs(mOpps.oppNear()->dist()) < 3.0)
            {
                m[LET_PASS] = false;

                return;
            }
        }
    }

    // Check for bad conditions
    if (!m[LET_PASS])
    {
        if (m[DRIVING_FAST] || mCar.v() > mOpps.oppLetPass()->v() + 5.0)
        {
            return;
        }
    }

    m[LET_PASS] = true;
}

void Driver::setDrvState(DriveState state)
{
    // Update state changes
    m[STATE_CHANGE] = false;
    if (state != mDrvState)
    {
        mDrvState = state;
        m[STATE_CHANGE] = true;
    }
}

void Driver::setDrvPath(PathType path)
{
    // Check the conditions
    if (path != mDrvPath)
    {
        if (!m[OVERTAKE])
        {
            if (mCar.v() > pathSpeed(path) || (fabs(pathOffs(path)) > 0.1))
            {
                return;
            }
        }
        else
        {
            if (mCar.v() > pathSpeed(path))
            {
                if (mOpps.oppNear() != NULL)
                {
                    double dist = mOpps.oppNear()->dist();

                    if ((dist < 100.0 && dist > 10)
                            || (dist <= 10 && dist >= 0.0 && fabs(mOpps.oppNear()->sideDist()) > 4.0)
                            || (dist < 0.0))
                    {
                        return;
                    }
                }
            }
        }

        mDrvPathOld = mDrvPath;
        mDrvPath = path;
    }
}

void Driver::calcStateAndPath()
{
    PathType path = PATH_O;

    if (stateStuck())
    {
        setDrvState(STATE_STUCK);
    }
    else if (statePitstop())
    {
        setDrvState(STATE_PITSTOP);
    }
    else if (statePitlane())
    {
        setDrvState(STATE_PITLANE);
    }
    else if (stateOfftrack())
    {
        setDrvState(STATE_OFFTRACK);
    }
    else
    {
        setDrvState(STATE_RACE);

        if (m[LET_PASS])
        {
            if (mCar.toMid() > 0.0)
            {
                path = PATH_L;
            }
            else
            {
                path = PATH_R;
            }
        }

        if (overtakeOpponent())
        {
            path = mOvertakePath;
        }
        // Pre pit entry
        if (mPit.pitstop())
        {
            if (mPit.pitOnLeftSide())
            {
                path = PATH_L;
            }
            else
            {
                path = PATH_R;
            }
        }
        // Test paths
        if (mTestPath != PATH_O)
        {
            path = mTestPath;
        }
    }
    // Returning to track from excursion or pits
    if (mDrvState == STATE_OFFTRACK)
    {
        if (fabs(pathOffs(PATH_L)) < fabs(pathOffs(PATH_R)))
        {
            path = PATH_L;
        }
        else
        {
            path = PATH_R;
        }
    }

    if (mDrvState == STATE_PITLANE)
    {
        if (mPit.pitOnLeftSide())
        {
            path = PATH_L;
        }
        else
        {
            path = PATH_R;
        }
    }
    // Set the path
    setDrvPath(path);
}

bool Driver::overtakeOpponent()
{
    if (mOpps.oppNear() == NULL)
    {
        m[OVERTAKE] = false;

        return m[OVERTAKE];
    }

    // Normal overtake
    double maxdist = std::min(50.0, mFrontCollMargin + 5.0 + mCar.v());
    double dist = mOpps.oppNear()->dist();
    if (dist < maxdist && dist > mOvtMargin / 2.0
            && (mOpps.oppNear()->borderDist() > -3.0 || (mOpps.oppNear()->borderDist() <= -3.0 && mOpps.oppNear()->v() > 25 && fabs(mOpps.oppNear()->sideDist()) < 5.0)))
    {
        if ((((m[CATCH] || (dist < mFrontCollMargin + 2.0 && mCar.accelFiltered() < 0.9 && mCar.v() > mOpps.oppNear()->v())) && (!mOpps.oppNear()->teamMate() || mOpps.oppNear()->backMarker()))
             || (m[OVERTAKE] && dist < mFrontCollMargin + 10.0 && mCar.v() > mOpps.oppNear()->v() - 2.0)
             || (mOpps.oppNear()->v() < 20.0 && dist < mFrontCollMargin + 20.0))
                && (!(!m[OVERTAKE] && m[DRIVING_FAST])))
        {
            m[OVERTAKE] = true;
        } else {
            m[OVERTAKE] = false;
        }
    }
    else
    {
        m[OVERTAKE] = false;
    }
    // If aside always overtake
    if (dist >= -mOvtMargin && dist <= mOvtMargin / 2.0 && mOpps.oppNear()->borderDist() > -3.0
            && (fabs(mOpps.oppNear()->sideDist()) < 4.0 || mDrvPath != PATH_O))
    {
        m[OVERTAKE] = true;
    }

    // If in front and on raceline stay there
    if (dist < 0.0 && mDrvPath == PATH_O)
    {
        m[OVERTAKE] = false;
    }

    return m[OVERTAKE];
}

void Driver::updateOvertakePath()
{
    if (mOpps.oppNear() == NULL)
    {
        return;
    }

    // Normal overtaking
    if ((mOpps.oppNear()->dist() > mOvtMargin && mOpps.oppNear()->catchTime() > 0.5)
            || (mOpps.oppNear()->dist() > 1.0 && mCar.v() < 10.0))
    {
        // Stay on your side when there is enough space
        double righttomiddle = mPath[PATH_R].toMiddle(mOpps.oppNear()->fromStart());
        double lefttomiddle = mPath[PATH_L].toMiddle(mOpps.oppNear()->fromStart());
        bool rightfree = fabs(righttomiddle - mOpps.oppNear()->toMiddle()) > 3.0;
        bool leftfree = fabs(lefttomiddle - mOpps.oppNear()->toMiddle()) > 3.0;
        if (mOpps.oppNear()->leftOfMe())
        {
            if (!(rightfree || leftfree))
            {
                mOvertakePath = PATH_R;
            }
            else if (rightfree)
            {
                mOvertakePath = PATH_R;
            }
            else
            {
                mOvertakePath = PATH_L;
            }
        }
        else
        {
            if (!(rightfree || leftfree))
            {
                mOvertakePath = PATH_L;
            }
            else if (leftfree)
            {
                mOvertakePath = PATH_L;
            }
            else
            {
                mOvertakePath = PATH_R;
            }
        }
    }
    else
    {
        // Always stay on your side if opponent aside
        if (mOpps.oppNear()->leftOfMe())
        {
            mOvertakePath = PATH_R;
        }
        else
        {
            mOvertakePath = PATH_L;
        }
    }
}

bool Driver::stuck()
{
    if (mSimTime > 0.0)
    {
        mStuckTime += mDeltaTime;
    }

    if (m[WAIT] || mDrvState == STATE_PITSTOP)
    {
        mStuckTime = 0.0;
    }

    if (mDrvState == STATE_STUCK)
    {
        if (mCar.v() > 8.0 || mStuckTime > 4.0)
        {
            mStuckTime = 0.0;
            return false;
        }
    }
    else if (mCar.v() < 1.0)
    {
        if (mStuckTime > 3.0)
        {
            mStuckTime = 0.0;
            return true;
        }
    }
    else
    {
        mStuckTime = 0.0;
    }

    return (mDrvState == STATE_STUCK);
}

bool Driver::updateOnCollision()
{
    m[COLL] = false;
    // Check opponents
    for (int i = 0; i < mOpps.nrOpponents(); i++)
    {
        Opponent* opp = mOpps.opp(i);
        // Conditions to ignore opponents
        if (!opp->racing() || opp->dist() < -5.0 || opp->dist() > 200.0)
        {
            continue;
        }

        if (!oppInCollisionZone(opp))
        {
            continue;
        }
        // Collision is possible
        double brakedist = 1.3 * brakeDist(mDrvPath, mCar.v(), opp->v());

        if (mCar.vX() < 0.0)
        {
            brakedist = brakeDist(mDrvPath, -mCar.v(), 0.0);
        }

        double colldist = std::max(0.0, fabs(opp->dist()) - frontCollFactor(opp) * mFrontCollMargin);
        // Simple way to account for the moving target
        double opptraveldist = 0.0;
        double factor = 0.0;

        if (opp->v() > 15.0 && !m[DRIVING_FAST])
        {
            factor = 0.5;
        }
        else if (m[DRIVING_FAST])
        {
            factor = 0.2;
        }

        opptraveldist = factor * std::min(opp->catchTime() * opp->v(), 100.0);
        double estcolldist = colldist + opptraveldist;
        bool incollmargin = colldist == 0.0;
        bool fasterthanopp = opp->v() > 15.0 && mCar.v() > fabs(opp->v()) - 0.5;

        if (brakedist > estcolldist || (incollmargin && fasterthanopp) || ((fabs(opp->dist()) < 1.0 && opp->distToStraight() < diffSpeedMargin(opp))))
        {
            m[COLL] = true;
        }
    }
    // Is track free to enter
    m[WAIT] = false;

    if (m[FAST_BEHIND] && mCar.borderDist() < -2.0 && mCar.borderDist() > -5.0 && mCar.v() < 9.0)
    {
        m[WAIT] = true;
        m[COLL] = true;
    }

    // Check for wall
    if (mCar.pointingToWall() && fabs(mCar.angToTrack()) > 1.0)
    {
        if (mCar.wallDist() - 2.5 < brakeDist(PATH_O, mCar.v(), 0.0) && mDrvState != STATE_STUCK)
        {
            m[COLL] = true;
        }
    }

    return m[COLL];
}

bool Driver::oppInCollisionZone(Opponent* opp) const
{
    if (opp->inDrivingDirection())
    {
        if (((opp->distToStraight() < diffSpeedMargin(opp))
             || (mCar.v() - opp->v() > 10.0 && opp->dist() < 50.0 && opp->dist() > 0.0 && opp->borderDist() > -3.0))
                && !(oppFast(opp) && opp->dist() > 50.0))
        {
            return true;
        }
    }

    return false;
}

bool Driver::oppFast(Opponent* opp) const
{
    double oppfs = fromStart(opp->fromStart());
    if ((opp->v() > 0.8 * mPathState[PATH_O].maxSpeed(oppfs))
            || (opp->v() > 40.0))
    {
        return true;
    }

    return false;
}

double Driver::diffSpeedMargin(Opponent* opp) const
{
    double speeddiff = std::max(0.0, mCar.v() - opp->v());
    double oppangle = opp->angle();
    double angle = std::min(0.3, fabs(oppangle));
    double factor = std::max(0.15, 1.0 * angle + 0.1 * m[DRIVING_FAST]);
    double diffspeedmargin = 2.5 + sin(fabs(oppangle)) + factor * speeddiff;

    if (mCar.v() < 5.0 || oppNoDanger(opp))
    {
        diffspeedmargin = 2.2 + sin(fabs(oppangle));
    }

    double maxmargin = 15.0;

    if (mPit.isBetween(opp->fromStart()))
    {
        maxmargin = 7.0;
    }
    return std::min(diffspeedmargin, maxmargin);
}



bool Driver::oppNoDanger(Opponent* opp) const
{
    if ((opp->borderDist() < -3.0 && fabs(opp->v()) < 1.0 && mCar.borderDist() > 0.0 && fabs(opp->dist()) > 1.0))
    {
        return true;
    }

    return false;
}

double Driver::fromStart(double fromstart) const
{
    double fs;
    double length = mTrack.length();

    if (fromstart > -length && fromstart < 2.0 * length)
    {
        if (fromstart > length)
        {
            fs = fromstart - length;
        }
        else if (fromstart < 0.0)
        {
            fs = fromstart + length;
        }
        else
        {
            fs = fromstart;
        }
    }
    else
    {
        fs = 0.0;
    }

    return fs;
}

double Driver::pathOffs(PathType path) const
{
    return mPathState[path].offset();
}

double Driver::pathSpeed(PathType path) const
{
    return mPathState[path].maxSpeed();
}

double Driver::pathAcceleration(PathType path) const
{
    return mPathState[path].acceleration();
}

void Driver::updateDrivingFast()
{
    double factor;

    if (m[DRIVING_FAST])
    {
        factor = 0.75;
    }
    else
    {
        factor = 0.9;
    }

    if (mDrvPath == PATH_O)
    {
        m[DRIVING_FAST] = mCar.v() > factor * pathSpeed(mOvertakePath);
    }
    else
    {
        m[DRIVING_FAST] = mCar.v() > factor * pathSpeed(mDrvPath);
    }

    // Going fast in curve
    if (pathAcceleration(mDrvPath) < 0.0 && mCar.v() > 0.75 * mMaxspeed)
    {
        m[DRIVING_FAST] = true;
    }
}

double Driver::frontCollFactor(Opponent* opp)
{
    double factor = 1.0;

    if (opp != NULL)
    {
        if ((m[OVERTAKE] || mCar.v() - opp->v() < 10.0) && !m[DRIVING_FAST])
        {
            factor = 0.5;
        }

        if (mCar.v() - opp->v() > 10.0 && m[DRIVING_FAST])
        {
            factor = 1.5;
        }

        if (fabs(opp->angle()) > 1.5 || fabs(opp->v()) < 2.0)
        {
            factor = 2.0;
        }

        if (mCar.v() < 2.0)
        {
            factor = 0.2;
        }
    }

    return factor;
}

void Driver::calcMaxspeed()
{
    switch (mDrvState)
    {
    case STATE_RACE:
    {
        if (mDrvPath != PATH_O)
        {
            mMaxspeed = pathSpeed(PATH_R) + 0.5 * (mLRTargetPortion + 1.0) * (pathSpeed(PATH_L) - pathSpeed(PATH_R));
        }
        else
        {
            mMaxspeed = pathSpeed(mDrvPath);
        }

        // Special cases
        if (mPit.pitstop())
        {
            mMaxspeed = mPitEntrySpeed; // pre pit entry
        }

        if (fabs(mCar.angToTrack()) > 1.0)
        {
            mMaxspeed = 10.0;
        }
        break;
    }
    case STATE_STUCK:
    {
        mMaxspeed = 10.0;
        break;
    }

    case STATE_OFFTRACK:
    {
        mMaxspeed = 5.0;
        break;
    }
    case STATE_PITLANE:
    {
        mMaxspeed = pitSpeed();
        break;
    }

    default:
    {
        break;
    }
    }

    mMaxspeed *= SkillGlobal;
}

double Driver::getSteerAngle(double steerlock)
{
    double yaw = Utils::normPiPi(mCar.yaw() - mPathYaw);
    double yawrate = mCar.yawRate() - mPathCurvature;
    double curvature = mPathCurvature;

    // Offset
    double offs = Utils::clip(mPathOffs, -0.35, 0.35);

    if (mSimTime > 10.0 && mCar.v() < 10.0 && !(fabs(yaw) > 0.75 && mCar.borderDist() > 0.0))
    {
        offs = mPathOffs;
    }

    // Offset derivative
    double offsderiv = Utils::clip(mPathOffsDeriv, -5.0, 5.0);

    // Factors
    double yawfactor = 1.0 - Utils::clip(fabs(mPathOffs) - 2.0, 0.0, 4.0) / 16.0;

    if (fabs(yaw) > 0.2)
    {
        yawfactor *= 1.5;
    }

    double yawratefactor = -3.0;

    if (m[COLL])
    {
        yawratefactor = -20.0;
    }

    // Steer components
    mYawSteer = (-yawfactor * yaw + mYawSteer) / 2.0;
    mYawRateSteer = (yawratefactor * yawrate + mYawRateSteer) / 2.0;
    mCurvSteer = 4.0 * curvature;
    mOffsSteer = 0.35 * offs;
    mOffsDerivSteer = 0.03 * offsderiv;

    // Steer sum
    double steer = mYawSteer + mYawRateSteer + mCurvSteer + mOffsSteer + mOffsDerivSteer;

    // Driving in wrong direction
    if (fabs(yaw) > PI / 2.0)
    {
        steer = -yaw;
    }

    return Utils::clip(steer, -steerlock, steerlock);
}

double Driver::controlSpeed(double accelerator, double maxspeed)
{
    accelerator += mSpeedController.sample(maxspeed - mCar.v(), mDeltaTime);

    return Utils::clip(accelerator, 0.0, 1.0);
}

// Meteorology
//--------------------------------------------------------------------------*
void Driver::Meteorology(const tTrack *t)
{
    tTrackSeg *Seg;
    tTrackSurface *Surf;
    rainintensity = 0;
    weathercode = GetWeather(t);
    LogUSR.info("Meteoroly : %i\n", weathercode);
    Seg = t->seg;

    for ( int I = 0; I < t->nseg; I++)
    {
        Surf = Seg->surface;
        rainintensity = MAX(rainintensity, Surf->kFrictionDry / Surf->kFriction);
        LogUSR.debug("# %.4f, %.4f %s\n",Surf->kFriction, Surf->kRollRes, Surf->material);
        Seg = Seg->next;
    }

    rainintensity -= 1;

    if (rainintensity > 0)
    {
        rain = true;
    }
    else
        rain = false;
}

//==========================================================================*
// Estimate weather
//--------------------------------------------------------------------------*
unsigned int Driver::GetWeather(const tTrack *t)
{
    return (t->local.rain << 4) + t->local.water;
};

//==========================================================================*
// Check if pit sharing is activated
//--------------------------------------------------------------------------*
bool Driver::CheckPitSharing(tCarElt *car)
{
  const tTrackOwnPit* OwnPit = car->_pit;           // Get my pit

  if (OwnPit == NULL)                            // If pit is NULL
  {                                              // nothing to do
      LogUSR.info(" #Pit = NULL\n\n");                 // here
    return false;
  }

  if (OwnPit->freeCarIndex > 1)
  {
      LogUSR.info(" #PitSharing = true\n\n");
      return true;
  }
  else
  {
      LogUSR.info(" #PitSharing = false\n\n");
      return false;
  }
}
