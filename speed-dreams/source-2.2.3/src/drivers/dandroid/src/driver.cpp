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

  //#define DRIVER_PRINT_RACE_EVENTS
  //#define TIME_ANALYSIS // For Linux only

#include "driver.h"
#include "Utils.h"

#ifdef DANDROID_TORCS
#include "tgfclient.h"
#endif

#ifdef TIME_ANALYSIS
#include <sys/time.h>
#endif

#define GRAVITY 9.81

TDriver::TDriver(int index)
{
    mCarIndex = index;

    oCar = NULL;

    mLOOKAHEAD_CONST = 4.0;         // [m]
    mOVT_FRONTSPACE = 20.0;         // [m]
    mOVT_FRONTMARGIN = 5.0;         // [m]

    // Variables
    mTrack = NULL;
    mPrevgear = 0;
    mAccel = 0.0;
    mAccelAvg = 0.0;
    mAccelAvgSum = 0.0;
    mAccelAvgCount = 0;
    mTenthTimer = false;
    mStuck = false;
    mStuckcount = 0;
    mDrivingFastCount = 0;
    mOldTimer = 0.0;
    mAbsFactor = 0.5;
    mTclFactor = 0.5;
    mClutchtime = 0.0;
    mNormalTargetToMiddle = 0.0;
    mPrevTargetdiff = 0.0;
    mOppInFrontspace = false;
    mPath[PATH_O].carpos.radius = 1000.0;
    mTargetToMiddle = 0.0;
    mCentrifugal = 0.0;
    mSectSpeedfactor = 1.0;
    mLastDamage = 0;
    mCatchingOpp = false;
    mStateChange = false;
    mPathChange = false;
    mOppLeftHyst = false;
    mOppLeftOfMeHyst = false;
    mOvertakeTimer = 0;
    mLeavePit = false;
    mLearnSectTime = true;
    mGetLapTime = true;
    mLastLapTime = 0.0;
    mBestLapTime = 0.0;
    mLearnLap = true;
    mAllSectorsFaster = false;
    mLearnSingleSector = false;
    mLearnSector = -1;
    mOfftrackInSector = false;
    mLearnedAll = false;
    mShiftTimer = 0;
    mGear = 0;
    mAccelX = 0.0;
    mAccelXSum = 0.0;
    mAccelXCount = 0;
    mSkillGlobal = 1.0;
    mSkillDriver = 1.0;
    mGarage = false;
    mWatchdogCount = 0;
    initVars();
    setPrevVars();
}

TDriver::~TDriver()
{
}

void TDriver::InitTrack(PTrack Track, PCarHandle CarHandle, PCarSettings* CarParmHandle, PSituation Situation)
{
    LogDANDROID.info("# Dandroid Driver initrack ...\n");
    mTrack = Track;

    // Get file handles
    char* trackname = strrchr(mTrack->filename, '/') + 1;
    char trackname2[100];
    strncpy( trackname2, strrchr(Track->filename, '/') + 1, sizeof(trackname2) - 1);
    *strrchr(trackname2, '.') = '\0';

    char buffer[256];

    if (strcmp(trackname2, "garage") == 0)
        mGarage = true;

    LogDANDROID.info(" # Track Name = %s - mGarage = %i\n", trackname, mGarage);

    // Discover the car type used
    void* handle = NULL;
    sprintf(buffer, "drivers/%s/%s.xml", MyBotName, MyBotName);
    handle = GfParmReadFile(buffer, GFPARM_RMODE_STD);
    sprintf(buffer, "%s/%s/%d", ROB_SECT_ROBOTS, ROB_LIST_INDEX, mCarIndex);
    mCarType = GfParmGetStr(handle, buffer, (char*)ROB_ATTR_CAR, "no good");

    // Parameters that are the same for all tracks
    handle = NULL;
    sprintf(buffer, "drivers/%s/%s/_all_tracks.xml", MyBotName, mCarType.c_str());
    handle = GfParmReadFile(buffer, GFPARM_RMODE_STD);

    if (handle == NULL)
    {
        mLearning = 0;
        mTestpitstop = 0;
        mTestLine = 0;
        mDriverMsgLevel = 0;
        mDriverMsgCarIndex = 0;
        mFRONTCOLL_MARGIN = 4.0;
        mSTARTCLUTCHRATE = 0.01;
        mHASABS = false;
        mHASESP = false;
        mHASTCL = false;
        mHASTYC = false;
    }
    else
    {
        mLearning = GfParmGetNum(handle, "private", "learning", (char*)NULL, 0.0) != 0;
        //mLearning = 1;
        mTestpitstop = GfParmGetNum(handle, "private", "test pitstop", (char*)NULL, 0.0) != 0;
        //mTestpitstop = 1;
        mTestLine = (int)GfParmGetNum(handle, "private", "test line", (char*)NULL, 0.0);
        mDriverMsgLevel = (int)GfParmGetNum(handle, "private", "driver message", (char*)NULL, 0.0);
        mDriverMsgCarIndex = (int)GfParmGetNum(handle, "private", "driver message car index", (char*)NULL, 0.0);
        mFRONTCOLL_MARGIN = GfParmGetNum(handle, "private", "frontcollmargin", (char*)NULL, 4.0);
        mSTARTCLUTCHRATE = GfParmGetNum(handle, "private", "startclutchrate", (char*)NULL, 0.01);
    }

    // Parameters that are track specific
    *CarParmHandle = NULL;
    switch (Situation->_raceType)
    {
    case RM_TYPE_QUALIF:
        sprintf(buffer, "drivers/%s/%s/qualifying/%s", MyBotName, mCarType.c_str(), trackname);
        *CarParmHandle = GfParmReadFile(buffer, GFPARM_RMODE_STD);
        break;
    default:
        break;
    }

    if (*CarParmHandle == NULL)
    {
        sprintf(buffer, "drivers/%s/%s/%s", MyBotName, mCarType.c_str(), trackname);
        *CarParmHandle = GfParmReadFile(buffer, GFPARM_RMODE_STD);
    }

    if (*CarParmHandle == NULL)
    {
        sprintf(buffer, "drivers/%s/%s/default.xml", MyBotName, mCarType.c_str());
        *CarParmHandle = GfParmReadFile(buffer, GFPARM_RMODE_STD);
    }

    readPrivateSection(CarParmHandle);
    readConstSpecs(CarHandle);

    // Set initial fuel
    double distance = Situation->_totLaps * mTrack->length;
    mFuelStart = getFuel(distance);

    if (mLearning)
    {
        mFuelStart = 5.0;
        GfParmSetNum(*CarParmHandle, SECT_ENGINE, PRM_FUELCONS, (char*)NULL, 0.0);
    }

    GfParmSetNum(*CarParmHandle, SECT_CAR, PRM_FUEL, (char*)NULL, (tdble)mFuelStart);

    // Get skill level
    handle = NULL;
    sprintf(buffer, "%sconfig/raceman/extra/skill.xml", GetLocalDir());
    handle = GfParmReadFile(buffer, GFPARM_RMODE_REREAD);
    double globalskill = 0.0;
    if (handle != NULL)
    {
        // Skill levels: 0 pro, 3 semi-pro, 7 amateur, 10 rookie
        globalskill = GfParmGetNum(handle, "skill", "level", (char*)NULL, 0.0);
    }

    mSkillGlobal = MAX(0.9, 1.0 - 0.1 * globalskill / 10.0);
    //load the driver skill level, range 0 - 1
    handle = NULL;
    sprintf(buffer, "drivers/%s/%d/skill.xml", MyBotName, mCarIndex);
    handle = GfParmReadFile(buffer, GFPARM_RMODE_STD);
    double driverskill = 0.0;

    if (handle != NULL)
    {
        driverskill = GfParmGetNum(handle, "skill", "level", (char*)NULL, 0.0);
    }

    mSkillDriver = MAX(0.95, 1.0 - 0.05 * driverskill);
}

void TDriver::NewRace(PtCarElt Car, PSituation Situation)
{
    oCar = Car;
    oSituation = Situation;
    readVarSpecs(oCar->_carHandle);
    initCa(oCar->_carHandle);
    initCw(oCar->_carHandle);
    initBrakes();
    printSetup();
    mDanPath.init(mTrack, mMAXLEFT, mMAXRIGHT, mMARGININSIDE, mMARGINOUTSIDE, mCLOTHFACTOR, mSEGLEN);
    mOpponents.init(mTrack, Situation, Car);
    mPit.init(mTrack, Situation, Car, mPITDAMAGE, mPITENTRYMARGIN);

    // File with speed factors
    mNewFile = false;

    if (!readSectorSpeeds())
    {
        mSect = mDanPath.mSector;
        if (!mLearning)
        {
            for (int i = 0; i < (int)mSect.size(); i++)
            {
                mSect[i].brakedistfactor = mBRAKESCALE;//1.9;
                mSect[i].speedfactor = mMUSCALE;//0.9;
                LogDANDROID.info("# mSect[%i]- brake Distance factor = %.3f - speed factor = %.3f\n", i, mBRAKESCALE, mMUSCALE);
            }
        }

        writeSectorSpeeds();

        if (mLearning)
        {
            mNewFile = true;
        }
    }

    mPrevRacePos = Car->_pos;
}

void TDriver::Drive()
{
#ifdef TIME_ANALYSIS
    struct timeval tv;
    gettimeofday(&tv, NULL);
    double usec1 = tv.tv_usec;
#endif
    if (mGarage)
        return;

    updateTime();
    updateTimer();
    updateBasics();
    updateOpponents();
    updatePath();
    updateUtils();
    calcDrvState();
    calcTarget();
    calcMaxspeed();
    setControls();
    printChangedVars();
    setPrevVars();
#ifdef TIME_ANALYSIS
    gettimeofday(&tv, NULL);
    double usec2 = tv.tv_usec;
    driverMsgValue(0, "useconds", usec2 - usec1);
#endif
}

int TDriver::PitCmd()                               // Handle pitstop
{
    mPit.pitCommand();
    return ROB_PIT_IM;  // Ready to be serviced
}

void TDriver::EndRace()                             // Stop race
{
    // This is never called by TORCS! Don't use it!
}

void TDriver::Shutdown()                            // Cleanup
{
}

void TDriver::updateTime()
{
    oCurrSimTime = oSituation->currentTime;
}

void TDriver::updateTimer()
{
    double diff = oCurrSimTime - mOldTimer;

    if (diff >= 0.1)
    {
        mOldTimer += 0.1;
        mTenthTimer = true;
    }
    else
    {
        mTenthTimer = false;
    }
}

void TDriver::updateBasics()
{
    mMass = mCARMASS + mFUELWEIGHTFACTOR * oCar->_fuel;
    mSpeed = oCar->_speed_x;

    mAccelAvgSum += mAccel;
    mAccelAvgCount++;

    if (mTenthTimer)
    {
        mAccelAvg = mAccelAvgSum / mAccelAvgCount;
        mAccelAvgSum = 0.0;
        mAccelAvgCount = 0;
        LogDANDROID.debug("mAccelAvg=%g\n", mAccelAvg);
    }

    mAccelXSum += oCar->_accel_x;
    mAccelXCount++;

    if (mTenthTimer)
    {
        mAccelX = mAccelXSum / mAccelXCount;
        mAccelXSum = 0.0;
        mAccelXCount = 0;
        LogDANDROID.debug("mAccelX=%g\n", mAccelX);
    }

    mFromStart = fromStart(oCar->_distFromStartLine);
    mToMiddle = oCar->_trkPos.toMiddle;
    mOnLeftSide = mToMiddle > 0.0 ? true : false;
    mTargetOnLeftSide = mTargetToMiddle > 0.0 ? true : false;
    mBorderdist = oCar->_trkPos.seg->width / 2.0 - fabs(mToMiddle) - oCar->_dimension_y / 2.0;
    mWallToMiddleAbs = oCar->_trkPos.seg->width / 2.0;

    if (oCar->_trkPos.seg->side[mOnLeftSide] != NULL)
    {
        int style = oCar->_trkPos.seg->side[mOnLeftSide]->style;
        if (style < 2)
        {
            mWallToMiddleAbs += oCar->_trkPos.seg->side[mOnLeftSide]->width;

            if (oCar->_trkPos.seg->side[mOnLeftSide]->side[mOnLeftSide] != NULL)
            {
                mWallToMiddleAbs += oCar->_trkPos.seg->side[mOnLeftSide]->side[mOnLeftSide]->width;
            }
        }
    }

    mWalldist = mWallToMiddleAbs - fabs(mToMiddle);
    mGlobalCarPos.x = oCar->_pos_X;
    mGlobalCarPos.y = oCar->_pos_Y;
    mTrackType = oCar->_trkPos.seg->type;
    mTrackRadius = oCar->_trkPos.seg->radius;

    if (mTrackRadius == 0.0)
    {
        mTrackRadius = 1000.0;
    }

    mCurvature = 1 / mPath[mDrvPath].carpos.radius;
    mTargetOnCurveInside = false;
    int type = mPath[mDrvPath].tarpos.type;

    if ((type == TR_LFT && mTargetOnLeftSide) || (type == TR_RGT && !mTargetOnLeftSide))
    {
        mTargetOnCurveInside = true;
    }

    mAngleToTrack = RtTrackSideTgAngleL(&(oCar->_trkPos)) - oCar->_yaw;
    NORM_PI_PI(mAngleToTrack);
    mAngleToLeft = mAngleToTrack < 0.0 ? true : false;

    if (oCar->_gear == -1)
    {
        mPointingToWall = (mAngleToLeft != mOnLeftSide) ? true : false;
    }
    else
    {
        mPointingToWall = (mAngleToLeft == mOnLeftSide) ? true : false;
    }

    mMu = oCar->_trkPos.seg->surface->kFriction;
    mFriction = mMu * (mCARMASS * GRAVITY + mCA * mSpeed * mSpeed);
    mCentrifugal = mCARMASS * mSpeed * mSpeed / mPath[mDrvPath].carpos.radius;
    mBrakeFriction = sqrt(MAX(0.1, mFriction * mFriction - mCentrifugal * mCentrifugal));
    mBrakeforce = MAX(mBRAKEFORCEMIN, mBRAKEFORCEFACTOR * mBrakeFriction / mBRAKEFORCE_MAX);
    mBrakeforce = MIN(1.0, mBrakeforce);

    if (!mCatchedRaceLine)
    {
        mPathChangeTime += RCM_MAX_DT_ROBOTS;
    }

    mDamageDiff = oCar->_dammage - mLastDamage;
    mLastDamage = oCar->_dammage;
    mRacePosChange = mPrevRacePos - oCar->_pos;
    mPrevRacePos = oCar->_pos;

#ifdef DRIVER_PRINT_RACE_EVENTS
    if (mDamageDiff > 20 || (mDrvState == STATE_OFFTRACK && mStateChange == true) || mRacePosChange != 0) {
        char* str = GfTime2Str(oCurrSimTime, 0);
        LogDANDROID.debug("%s %s: Damage=%d Offtrack=%d RacePosChange=%d\n", str, oCar->_name, mDamageDiff, mDrvState == STATE_OFFTRACK, mRacePosChange);
        free(str);
    }
#endif

    updateSector();
    learnSpeedFactors();
    getBrakedistfactor();
    getSpeedFactors();
    updateStuck();
    updateAttackAngle();
    updateCurveAhead();
    mPit.update(mFromStart);
}

void TDriver::updateOpponents()
{
    mOpponents.update(oSituation, oCar);
    mOppNear = mOpponents.oppNear();
    mOppNear2 = mOpponents.oppNear2();
    mOppBack = mOpponents.oppBack();
    mOppLetPass = mOpponents.oppLetPass();
    mOpp = mOppNear;
    // Second Oppenent near?
    mBackmarkerInFrontOfTeammate = false;
    mTwoOppsAside = false;
    mOppComingFastBehind = mOpponents.oppComingFastBehind;

    if (mOppNear2 != NULL)
    {
        // Watch for backmarkers in front of teammate
        if (mOppNear2->backmarker && mOpp->teammate
            && mOpp->speed > 15.0
            && mOpp->mDist > 1.0
            && mOppNear2->mDist < 2.0 * mOVT_FRONTSPACE)
        {
            mBackmarkerInFrontOfTeammate = true;
        }
        // Check if 2. Opponent aside
        if (mOppNear2->mAside)
        {
            mTwoOppsAside = true;
        }
    }
    // Distances
    mOppDist = DBL_MAX;
    mOppSidedist = DBL_MAX;
    mOppAside = false;

    if (mOpp != NULL)
    {
        mOppDist = mOpp->mDist;

        if (mOpp->mAside && mOpp->borderdist > -3.0)
        {
            mOppSidedist = mOpp->sidedist;
            mOppAside = true;
        }

        mOppLeft = (mOpp->toMiddle > 0.0) ? true : false;
        mOppLeftHyst = hysteresis(mOppLeftHyst, mOpp->toMiddle, 0.5);
        mOppLeftOfMe = (mOpp->toMiddle - mToMiddle > 0.0) ? true : false;
        mOppLeftOfMeHyst = hysteresis(mOppLeftOfMeHyst, mOpp->toMiddle - mToMiddle, 0.3);
        mOppInFrontspace = (mOppDist < mOVT_FRONTSPACE&& mOppDist >= 0.0) ? true : false;
    }
}

void TDriver::updatePath()
{
    for (int path = 0; path < 3; path++)
    {
        updatePathCar(path);
        updatePathTarget(path);
        updatePathOffset(path);
        updatePathSpeed(path);
    }
}

void TDriver::updateUtils()
{
    updateDrivingFast();
    updateFrontCollFactor();
    updateLetPass();
    mOvertakePath = overtakeStrategy();
}

void TDriver::calcTarget()
{
    calcTargetToMiddle();
    calcGlobalTarget();
    calcTargetAngle();
}

void TDriver::setControls()
{
    oCar->_steerCmd = (tdble)getSteer();
    oCar->_gearCmd = getGear();
    oCar->_clutchCmd = (tdble)getClutch();  // must be after gear
    oCar->_brakeCmd = (tdble)filterABS(getBrake(mMaxspeed));
    mAccel = filterTCLSideSlip(filterTCL(getAccel(mMaxspeed)));  // must be after brake
    oCar->_accelCmd = (tdble)mAccel;
    oCar->_lightCmd = RM_LIGHT_HEAD1 | RM_LIGHT_HEAD2;
}

void TDriver::printChangedVars()
{
    if (mDriverMsgLevel || mLearning)
    {
        if (mStateChange)
        {
            driverMsgValue(1, "mDrvState:", mDrvState);
        }

        if (mPathChange)
        {
            driverMsgValue(1, "mDrvPath:", mDrvPath);
        }

        if (prev_mCurveAhead != mCurveAhead)
        {
            driverMsgValue(1, "mCurveAhead:", mCurveAhead);
        }

        if (prev_mDrivingFast != mDrivingFast)
        {
            driverMsgValue(1, "mDrivingFast:", mDrivingFast);
        }
        if (prev_mOvertake != mOvertake)
        {
            driverMsgValue(1, "mOvertake:", mOvertake);
        }

        if (prev_mLetPass != mLetPass)
        {
            driverMsgValue(1, "mLetPass:", mLetPass);
        }

        if (prev_mOppComingFastBehind != mOppComingFastBehind)
        {
            driverMsgValue(1, "mOppComingFastBehind:", mOppComingFastBehind);
        }

        if (prev_mCatchedRaceLine != mCatchedRaceLine)
        {
            driverMsgValue(1, "mCatchedRaceLine:", mCatchedRaceLine);
        }

        if (prev_mMaxSteerAngle != mMaxSteerAngle)
        {
            driverMsgValue(2, "mMaxSteerAngle:", mMaxSteerAngle);
        }

        if (prev_mBumpSpeed != mBumpSpeed)
        {
            driverMsgValue(2, "mBumpSpeed:", mBumpSpeed);
        }

        if (prev_mSector != mSector)
        {
            driverMsgValue(2, "mSector: ", mSector);

            if (mSector == 0)
            {
                LogDANDROID.debug("time: %g\n", oCar->_lastLapTime);
            }
        }

        if (prev_mControlAttackAngle != mControlAttackAngle)
        {
            driverMsgValue(3, "mControlAttackAngle:", mControlAttackAngle);
        }

        if (prev_mControlYawRate != mControlYawRate)
        {
            driverMsgValue(3, "mControlYawRate:", mControlYawRate);
        }

        driverMsgValue(4, "mPathOffs:", mPathOffs);
        driverMsgValue(4, "vmax:", 3.6 * mMaxspeed);
    }
}

void TDriver::setPrevVars()
{
    prev_mDrvState = mDrvState;
    prev_mSector = mSector;
    prev_mCurveAhead = mCurveAhead;
    prev_mDrivingFast = mDrivingFast;
    prev_mOvertake = mOvertake;
    prev_mLetPass = mLetPass;
    prev_mOppComingFastBehind = mOppComingFastBehind;
    prev_mCatchedRaceLine = mCatchedRaceLine;
    prev_mControlAttackAngle = mControlAttackAngle;
    prev_mControlYawRate = mControlYawRate;
    prev_mMaxSteerAngle = mMaxSteerAngle;
    prev_mBumpSpeed = mBumpSpeed;
}

void TDriver::initVars()
{
    mDrvState = STATE_RACE;
    mDrvPath = PATH_O;
    mDrvPath_prev = PATH_O;
    mSector = 0;
    mCurveAhead = false;
    mDrivingFast = false;
    mOvertake = false;
    mLetPass = false;
    mOppComingFastBehind = false;
    mCatchedRaceLine = false;
    mControlAttackAngle = false;
    mControlYawRate = false;
    mMaxSteerAngle = false;
    mBumpSpeed = false;
}

double TDriver::getPitSpeed()
{
    double speedEntryExit = mPit.getPitstop() ? mPITENTRYSPEED : mPITEXITSPEED;
    double pitlimitdist = fromStart(mPit.getLimitEntry() - mFromStart);
    double maxspeed = speedEntryExit;

    if (pitlimitdist < brakeDist(mSpeed, mPit.getSpeedlimit())
        || mPit.isPitlimit(mFromStart))
    {
        maxspeed = mPit.getSpeedlimit();
    }
    double pitdist = mPit.getDist();
    double brakespeed;

    if (pitdist < 20.0)
    {
        brakespeed = 0.6 * brakeSpeed(pitdist, 0.0);

        if (IS_DANDROID_TORCS)
            brakespeed = 0.8 * brakeSpeed(pitdist, 0.0);
    }
    else
    {
        brakespeed = 0.6 * brakeSpeed(pitdist, 0.0);

        if (IS_DANDROID_TORCS)
            brakespeed = 0.8 * brakeSpeed(pitdist, 0.0);
    }

    maxspeed = MIN(maxspeed, brakespeed);

    return maxspeed;
}

double TDriver::getMaxSpeed(DanPoint danpoint)
{
    double maxlookaheaddist = MIN(500.0, brakeDist(mSpeed, 0.0));
    double nextdist = 0.0;
    double nextradius;
    double nextspeed;
    double bumpspeed;
    double maxspeed = DBL_MAX;
    double lowest = DBL_MAX;
    double radius = fabs(danpoint.radius);;
    double curv_z = danpoint.curv_z;
    while (nextdist < maxlookaheaddist)
    {
        danpoint = mDanPath.nextPos(danpoint);
        nextdist = fromStart(danpoint.fromstart - mFromStart);
        nextradius = fabs(danpoint.radius);
        nextspeed = curveSpeed(nextradius);
        bumpspeed = bumpSpeed(danpoint.curv_z, nextspeed);

        if (bumpspeed < nextspeed)
        {
            maxspeed = brakeSpeed(nextdist, bumpspeed);
        }
        else
        {
            maxspeed = brakeSpeed(nextdist, nextspeed);
        }

        if (lowest > maxspeed)
        {
            lowest = maxspeed;
        }
    }

    double cs = curveSpeed(radius);
    double bs = bumpSpeed(curv_z, cs);

    if (bs < cs)
    {
        mBumpSpeed = true;
    }
    else
    {
        mBumpSpeed = false;
    }

    maxspeed = MIN(1000, MIN(lowest, MIN(cs, bs)));

    return maxspeed;
}

double TDriver::curveSpeed(double radius)
{
    radius = fabs(radius);

    return mSectSpeedfactor * sqrt(mMu * GRAVITY * radius / (1.0 - MIN(0.99, radius * mCA * mMu / mMass)));
}

double TDriver::bumpSpeed(double curv_z, double curvespeed)
{
    double speed_z = DBL_MAX;
    double bumpspeedfactor = mBUMPSPEEDFACTOR;

    if (curv_z < -0.015)
    {
        if (mOpp != NULL)
        {
            if (mBUMPSPEEDFACTOR > 3.0 && mColl)
            {
                bumpspeedfactor = 3.0;
            }
        }

        speed_z = bumpspeedfactor * sqrt(GRAVITY / -(curv_z));
        // What should we do, if the bump is in a curve?
        if (fabs(curvespeed - speed_z) < 10.0)
        {
            speed_z *= 0.75;
        }
    }

    return speed_z;
}

double TDriver::brakeSpeed(double nextdist, double nextspeed)
{
    double brakespeed = DBL_MAX;
    double bd = brakeDist(mSpeed, nextspeed);

    if (bd > nextdist)
    {
        brakespeed = nextspeed;
    }

    return brakespeed;
}

double TDriver::brakeDist(double speed, double allowedspeed)
{
    if (speed <= allowedspeed)
    {
        return -1000.0;
    }

    double v1sqr;
    double v2sqr;
    double brakedist = 0.0;
    const int step = 10;
    int diff = (speed - allowedspeed) / step;
    double rest = (speed - allowedspeed) - diff * step;
    for (int i = 0; i < diff; i++)
    {
        v1sqr = (speed - i * step) * (speed - i * step);
        v2sqr = (speed - i * step - step) * (speed - i * step - step);
        brakedist += mBrakedistfactor * mMass * (v1sqr - v2sqr) / (2.0 * (mMu * GRAVITY * mMass + v2sqr * (mCA * mMu + mCW)));
    }

    v1sqr = (allowedspeed + rest) * (allowedspeed + rest);
    v2sqr = allowedspeed * allowedspeed;
    brakedist += mBrakedistfactor * mMass * (v1sqr - v2sqr) / (2.0 * (mMu * GRAVITY * mMass + v2sqr * (mCA * mMu + mCW)));
    brakedist *= 0.65 + 20.0 * fabs(getCurvature(brakedist));
    LogDANDROID.debug("bdist=%g \n", brakedist);

    return brakedist;
}

double TDriver::getBrake(double maxspeed)
{
    double brakeforce = 0.0;

    if (mSpeed > maxspeed)
    {
        brakeforce = mBrakeforce;
    }

    if (mDrvState == STATE_OFFTRACK)
    {
        brakeforce *= 0.2;
    }

    if (mDrvState == STATE_PITLANE)
    {
        // Pit speed limiter
        if (mSpeed > maxspeed)
        {
            brakeforce = mBrakeforce;
        }
        else if (mSpeed > maxspeed - 0.1)
        {
            brakeforce = 0.05;
        }
    }

    if (mDrvState == STATE_PITSTOP)
    {
        brakeforce = mBrakeforce;
    }

    double collbrakeforce = 0.0;
    if (onCollision())
    {
        collbrakeforce = mBrakeforce + 0.05 + mCollOvershooting / 10.0;
    }

    brakeforce = MAX(collbrakeforce, brakeforce);
    brakeforce = MIN(1.0, brakeforce);

    if (mDrvState == STATE_STUCK)
    {
        //brakeforce = 0.0;
    }

    return brakeforce;
}

double TDriver::getAccel(double maxspeed)
{
    double accel;

    if (oCar->ctrl.brakeCmd > 0.0
        || fabs(mAttackAngle) > 0.3
        || (mMaxSteerAngle && mDrivingFast))
    {
        accel = 0.0;
        mAccel = 0.5;
    }
    else
    {
        controlSpeed(mAccel, maxspeed);

        if (mLetPass)
        {
            mAccel *= 0.5;
        }

        accel = mSkillDriver * mAccel;
    }

    if (oCurrSimTime < 0.0)
    {
        if (oCar->_enginerpm / oCar->_enginerpmRedLine > 0.7)
        {
            accel = 0.0;
        }
    }

    return accel;
}

double TDriver::getSteer()
{
    if (mDrvState == STATE_STUCK)
    {
        if (fabs(mAngleToTrack) < 1.0)
        {
            mTargetAngle = -mAngleToTrack / 4.0;
        }
        else
        {
            mTargetAngle = -0.5 * SIGN(mAngleToTrack);
        }
    }

    limitSteerAngle(mTargetAngle);
    controlAttackAngle(mTargetAngle);

    return mTargetAngle / oCar->_steerLock;
}

int TDriver::getGear()
{
    const double SHIFT_UP = 0.99;          // [-] (% of rpmredline)
    const double SHIFT_DOWN_MARGIN = 130.0;    // [rad/s] down from rpmredline
    int shifttime = 5;
    const int MAX_GEAR = oCar->_gearNb - 1;

    if (oCurrSimTime < 0.0)
    {
        return mGear = 0;
    }

    if (oCurrSimTime < 0.5)
    {
        // For the start
        shifttime = 0;
    }

    if (mTenthTimer)
    {
        if (mShiftTimer < shifttime)
        {
            mShiftTimer++;
        }
    }

    if (mShiftTimer < shifttime)
    {
        return mGear;
    }

    if (mDrvState == STATE_STUCK)
    {
        return mGear = -1;
    }

    if (oCar->_gear <= 0)
    {
        return mGear = 1;
    }

    if (oCar->_gear < MAX_GEAR && oCar->_enginerpm / oCar->_enginerpmRedLine > SHIFT_UP)
    {
        mShiftTimer = 0;

        return mGear++;
    }
    else
    {
        double ratiodown = oCar->_gearRatio[oCar->_gear + oCar->_gearOffset - 1] / oCar->_gearRatio[oCar->_gear + oCar->_gearOffset];

        if (oCar->_gear > 1 && (oCar->_enginerpmRedLine - SHIFT_DOWN_MARGIN) / oCar->_enginerpm > ratiodown)
        {
            mShiftTimer = 0;

            return mGear--;
        }
    }

    return mGear;
}

double TDriver::getClutch()
{
    if (oCar->_gear > 1 || mSpeed > 15.0)
    {
        if (oCar->_gear > mPrevgear)
        {
            mClutchtime = 0.3;
        }

        if (mClutchtime > 0.0)
        {
            mClutchtime -= 1.0 * RCM_MAX_DT_ROBOTS;
        }

        if (oCar->_gear < mPrevgear)
        {
            mClutchtime = 0.0;
        }
    }
    else if (oCar->_gear == 1)
    {
        mClutchtime -= mSTARTCLUTCHRATE;

        if (fabs(mAngleToTrack) > 1.0 || mDrvState == STATE_OFFTRACK)
        {
            mClutchtime = 0.0;
        }
    }
    else if (oCar->_gear == -1)
    {
        // For the reverse gear.
        if (oCar->_enginerpm > 500.0)
        {
            mClutchtime -= 0.01;
        }
        else
        {
            mClutchtime += 0.01;
        }
    }
    else if (oCar->_gear == 0)
    {
        // For a good start
        mClutchtime = 0.7;
    }

    mPrevgear = oCar->_gear;
    mClutchtime = MIN(MAX(0.0, mClutchtime), 1.0);

    return mClutchtime;
}

bool TDriver::stateStuck()
{
    if (mStuck)
    {
        return true;
    }

    return false;
}

bool TDriver::stateOfftrack()
{
    if (mDrvState != STATE_PITLANE && mDrvState != STATE_PITSTOP)
    {
        if (mBorderdist < -2.2 || (mSpeed < 15.0 && mBorderdist < -1.8))
        {
            return true;
        }
    }

    return false;
}

bool TDriver::statePitstop()
{
    if (mDrvState == STATE_PITLANE && !mLeavePit)
    {
        float dl, dw;
        RtDistToPit(oCar, mTrack, &dl, &dw);

        if (fabs(dw) < 1.5 && dl > mTrack->length - 1.0)
        {
            return true;
        }
    }
    else if (mDrvState == STATE_PITSTOP)
    {
        // Traffic in the way when leaving?
        if (mOppBack != NULL)
        {
            if (mOppBack->mDist > -20.0 && mOppBack->speed > 5.0 && mOppBack->speed < 25.0)
            {
                return true;
            }
        }

        mLeavePit = true;
    }
    else if (mDrvState == STATE_RACE)
    {
        mLeavePit = false;
    }

    return false;
}

bool TDriver::statePitlane()
{
    if (mPit.getPitOffset(mFromStart))
    {
        return true;
    }

    return false;
}

void TDriver::updateLetPass()
{
    if (mOppLetPass == NULL || mDrvState != STATE_RACE || oCurrSimTime < 60.0)
    {
        mLetPass = false;

        return;
    }
    // Check range
    if (mOppLetPass->mDist < -50.0 || mOppLetPass->mDist > 0.0)
    {
        mLetPass = false;

        return;
    }
    // Check for other opponent between behind
    if (mOppBack != NULL)
    {
        if (mOppBack != mOppLetPass && mOppBack->mDist > mOppLetPass->mDist)
        {
            mLetPass = false;

            return;
        }
    }
    // Check for other opponent aside
    if (mOppNear != NULL)
    {
        if (mOppNear != mOppLetPass)
        {
            if (fabs(mOppNear->mDist) < 3.0)
            {
                mLetPass = false;

                return;
            }
        }
    }
    // Check for bad conditions
    if (!mLetPass)
    {
        if (mDrivingFast || mSpeed > mOppLetPass->speed + 5.0)
        {
            if (mOppLetPass->mDist < -20.0 || mOppLetPass->mDist > 0.0)
            {
                return;
            }
        }
    }

    mLetPass = true;
}

void TDriver::setDrvState(int state)
{
    mDrvState = state;

    // Update state changes
    mStateChange = false;

    if (prev_mDrvState != mDrvState)
    {
        mStateChange = true;
    }
}

double TDriver::pathOffs(int path)
{
    double offs = 0.0;
    if (mDrvState == STATE_RACE)
    {
        offs = mPath[path].offset;
    }

    return offs;
}

void TDriver::setDrvPath(int path)
{
    mPathChange = false;
    // Check the conditions
    if (mDrvPath != path || mStateChange)
    {
        // Don't change when dangerous or speed on limits
        if (mDrivingFast && fabs(pathOffs(path)) > 2.0 && !mOvertake && !mTestLine)
        {
            return;
        }
        // Don't change when opponent comes fast from behind
        if (mOppComingFastBehind)
        {
            return;
        }
        // Returning to track from excursion or pits
        if (mDrvState == STATE_OFFTRACK || mDrvState == STATE_PITLANE)
        {
            if (fabs(mPath[PATH_L].offset) < fabs(mPath[PATH_R].offset))
            {
                path = PATH_L;
            }
            else
            {
                path = PATH_R;
            }
        }

        // Make the path change
        mDrvPath_prev = mDrvPath;
        mDrvPath = path;
        mPathChange = true;
        mPathChangeTime = 0.0;
    }
    // Update path infos
    mPathOffs = pathOffs(mDrvPath);
    updateCatchedRaceLine();
}

void TDriver::calcDrvState()
{
    int path = PATH_O;
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
        if (mLetPass)
        {
            if (mTargetToMiddle > 0.0)
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
#if 1
        if (mTestLine == 1)
        {
            path = PATH_L;
        }

        if (mTestLine == 2)
        {
            path = PATH_R;
        }

        if (mTestLine == 3)
        {
            if ((mDrvPath != PATH_L && mCatchedRaceLine) || (mDrvPath == PATH_L && !mCatchedRaceLine))
            {
                path = PATH_L;
            }
            else if ((mDrvPath != PATH_R && mCatchedRaceLine) || (mDrvPath == PATH_R && !mCatchedRaceLine))
            {
                path = PATH_R;
            }
        }
#endif
    }
    setDrvPath(path);
}

void TDriver::calcTargetToMiddle()
{
    double prevtargettomiddle = mTargetToMiddle;
    mNormalTargetToMiddle = mPath[mDrvPath].tarpos.tomiddle;
    mTargetToMiddle = mNormalTargetToMiddle;

    switch (mDrvState)
    {
    case STATE_RACE:
    {
        // Path changes
        if (!mCatchedRaceLine)
        {
            double rate = 2.0;

            if (!mDrivingFast)
            {
                rate = 4.0;
            }

            double dist = fabs(mNormalTargetToMiddle - mPath[mDrvPath_prev].tarpos.tomiddle);
            double time = dist / rate;
            double part = 1.0;
            if (mPathChangeTime < time)
            {
                part = mPathChangeTime / time;
            }

            mTargetToMiddle = part * mNormalTargetToMiddle + (1.0 - part) * mPath[mDrvPath_prev].tarpos.tomiddle;
            // Start straight
            if (oCurrSimTime < 4.0)
            {
                mTargetToMiddle = prevtargettomiddle = mToMiddle;
                mPathChangeTime = 0.0;
            }
            // In case we change path in the middle of a path change
            if (fabs(prevtargettomiddle - mTargetToMiddle) > 0.5)
            {
                if (fabs(prevtargettomiddle - mNormalTargetToMiddle) < dist)
                {
                    part = 1.0 - fabs(prevtargettomiddle - mNormalTargetToMiddle) / dist;
                    mPathChangeTime = part * time;
                }
                else
                {
                    part = 0.0;
                    mPathChangeTime = 0.0;
                }

                mTargetToMiddle = part * mNormalTargetToMiddle + (1.0 - part) * mPath[mDrvPath_prev].tarpos.tomiddle;
            }
        }
        // Special cases
        if (mDrvPath == PATH_L || mDrvPath == PATH_R)
        {
            if (mSpeed < 10.0 && fabs(mOppSidedist) < 3.5)
            {
                mTargetToMiddle = SIGN(mTargetToMiddle) * (mTrack->width / 2.0);
            }
        }
        if (fabs(mOppSidedist) < 3.0)
        {
            if (mBorderdist > 1.5)
            {
                mTargetToMiddle -= 1.0 * SIGN(mOppSidedist) * (3.0 - fabs(mOppSidedist));
            }
            else
            {
                mTargetToMiddle = SIGN(mTargetToMiddle) * ((mTrack->width / 2.0) - 1.5);
            }
        }
        if (mWalldist < mTARGETWALLDIST + 1.0)
        {
            mTargetToMiddle = mTargetToMiddle - SIGN(mTargetToMiddle) * mTARGETWALLDIST; // needed for Corkscrew pit wall
        }

        break;
    }
    case STATE_STUCK:
    {
        break;
    }
    case STATE_OFFTRACK:
    {
        mTargetToMiddle = SIGN(mToMiddle) * ((mTrack->width / 2.0) - 1.0);
        if (mWalldist < 0.0)
        {
            mTargetToMiddle = SIGN(mToMiddle) * (mWallToMiddleAbs + 2.0); // we are on the wrong side of the pit wall
        }

        break;
    }
    case STATE_PITLANE:
    {
        mTargetToMiddle = mPit.getPitOffset(mTargetFromstart);

        if (fabs(mTargetToMiddle) < mTrack->width / 2.0)
        {
            double pitentrydist = fromStart(mPit.getPitEntry() - mFromStart);

            if (pitentrydist > 0.0 && pitentrydist < mPITENTRYMARGIN)
            {
                mTargetToMiddle = mToMiddle + (mTargetToMiddle - mToMiddle) * (mPITENTRYMARGIN - pitentrydist) / mPITENTRYMARGIN;
            }
        }

        break;
    }
    }
}

bool TDriver::overtakeOpponent()
{
    if (mOpp == NULL)
    {
        mOvertake = false;

        return mOvertake;
    }

    // Stay the course for some time
    if (mOvertake)
    {
        if (mTenthTimer)
        {
            if (mOvertakeTimer < 5)
            {
                mOvertakeTimer++;

                return mOvertake;
            }
        }
    }
    else
    {
        mOvertakeTimer = 0;
    }

    // Overtake conditions
    double maxdist = MIN(50, mFRONTCOLL_MARGIN + 5.0 + mSpeed);

    if (mOppDist < maxdist && mOppDist > 1.0
        // Watch for cars coming out of the pitlane
        && (mOpp->borderdist > -3.0 || (mOpp->borderdist <= -3.0 && mOpp->speed > 25 && fabs(mOpp->sidedist) < 5.0)))
    {
        if (mOpp->mCatchtime < 2.0 || (mOppDist < mFRONTCOLL_MARGIN + 2.0 && !mDrivingFast)) mCatchingOpp = true;
        if (mOpp->mCatchtime > 10.0) mCatchingOpp = false;
        if (((mCatchingOpp || (mOpp->backmarker && mOppDist < mFRONTCOLL_MARGIN + 3.0 && mAccelAvg < 1.0)) && !mOpp->teammate && !mDrivingFast)
            || (mCatchingOpp && mOppDist < 10.0 && mOvertakePath != mDrvPath && mSpeed < 0.85 * mPath[mOvertakePath].maxspeed && fabs(pathOffs(mOvertakePath)) < 2.0 && mOpp->backmarker)
            || (mOvertake && mOppDist < mFRONTCOLL_MARGIN + 5.0 && !mDrivingFast)
            || (mOvertake && mOppDist < mFRONTCOLL_MARGIN + 10.0 && mOpp->backmarker)
            || mSpeed < 8.0
            || (mOpp->speed < 5.0 && mOppDist < mFRONTCOLL_MARGIN + 8.0)
            )
        {
            mOvertake = true;
        }
        else
        {
            mOvertake = false;
            mCatchingOpp = false;
        }
    }
    else
    {
        mOvertake = false;
    }
    // If aside always overtake
    if (mOppDist > -2.0 && mOppDist <= 1.0)
    {
        mOvertake = true;
    }
    // Special case: if in front and on raceline stay there
    if ((mOppDist < 0.0 && mDrvPath == PATH_O && mCatchedRaceLine))
    {
        mOvertake = false;
    }

    return mOvertake;
}

int TDriver::overtakeStrategy()
{
    if (mOpp == NULL)
    {
        return mDrvPath;
    }

    // Predict side of opponent
    int predict_catchtime_opp_path = PATH_O;
    if (mOpp->mCatchtime < 10.0)
    {
        double opptomiddle_prediction = mOpp->toMiddle + mOpp->toMiddleChangeRate * mOpp->mCatchtime;

        if (fabs(opptomiddle_prediction) > 1.0)
        {
            predict_catchtime_opp_path = opptomiddle_prediction > 0.0 ? PATH_L : PATH_R;
        }
    }

    int path = mDrvPath;
    // Normal overtaking
    if (mOpp->mDist > 1.0)
    {
        if (predict_catchtime_opp_path)
        {
            if (predict_catchtime_opp_path == PATH_L)
            {
                path = PATH_R;
            }
            else
            {
                path = PATH_L;
            }
        }
        else
        {
            // Generally drive on the side with more space
            if (fabs(mPath[PATH_R].carpos.tomiddle - mOpp->toMiddle) - fabs(mPath[PATH_L].carpos.tomiddle - mOpp->toMiddle) > 0.0)
            {
                path = PATH_R;
            }
            else
            {
                path = PATH_L;
            }
            // But stay on your side when there is enough space
            if (mOppLeftOfMeHyst)
            {
                if (fabs(mPath[PATH_R].carpos.tomiddle - mOpp->toMiddle) > 4.0)
                {
                    path = PATH_R;
                }
            }
            else
            {
                if (fabs(mPath[PATH_L].carpos.tomiddle - mOpp->toMiddle) > 4.0)
                {
                    path = PATH_L;
                }
            }
        }
    }
    else {
        // Always stay on your side if opponent aside
        if (mOppLeftOfMe)
        {
            path = PATH_R;
        }
        else
        {
            path = PATH_L;
        }
    }

    return path;
}

void TDriver::updateStuck()
{
    if (mTenthTimer)
    {
        if (mWait || mDrvState == STATE_PITSTOP)
        {
            mStuckcount = 0;
        }

        if (mStuck)
        {
            if (fabs(mSpeed) < 7.0)
            {
                if (mStuckcount++ > 50)
                {
                    mStuckcount = 0;
                    mStuck = false;
                }
            }
            else
            {
                mStuckcount = 0;
                mStuck = false;
            }
        }
        else if (fabs(mSpeed) < 1.5)
        {
            if (mStuckcount++ > 20)
            {
                mStuckcount = 0;
                mStuck = true;
            }
        }
        else
        {
            mStuckcount = 0;
        }
    }
}

bool TDriver::onCollision()
{
    mWait = false;
    mColl = false;
    mCollDist = 1000.0;
    mCollBrakeDist = 1000.0;
    // check opponents
    for (int i = 0; i < mOpponents.nopponents; i++)
    {
        Opponent* opp = &mOpponents.opponent[i];

        if (opp->mRacing && opp->mDist > -5.0 && opp->mDist < 200.0 && !(opp->mAside))
        {
            if ((opp->mInDrivingDirection && oppInCollisionZone(opp)) || mBackmarkerInFrontOfTeammate)
            {
                double brakedist = brakeDist(mSpeed, opp->speed);

                if (mSpeed < 0.0)
                {
                    brakedist = brakeDist(-mSpeed, 0.0);
                }

                double colldist = fabs(opp->mDist) - mFrontCollFactor * mFRONTCOLL_MARGIN;

                if (brakedist > colldist)
                {
                    if (colldist < mCollDist)
                    {
                        mCollDist = colldist;
                        mCollBrakeDist = brakedist;
                        mCollOvershooting = mCollBrakeDist - mCollDist;
                    }

                    mColl = true;
                }
            }
        }
    }

    if (mColl && mCarIndex == mDriverMsgCarIndex)
    {
        LogDANDROID.debug("fs=%g colldist=%g brakedist=%g\n", mFromStart, mCollDist, mCollBrakeDist);
    }

    // is track free to enter
    if (mOppComingFastBehind
        && mBorderdist < -2.0 && mBorderdist > -5.0
        && mSpeed < 9.0
        && !mPointingToWall)
    {
        mWait = true;
        mColl = true;
    }
    // check for wall
    if (mPointingToWall && fabs(mAngleToTrack) > 0.7)
    {
        if (mWalldist - 2.5 < brakeDist(mSpeed, 0.0) && !mStuck)
        {
            mColl = true;
        }
    }

    return mColl;
}

bool TDriver::oppInCollisionZone(Opponent* opp)
{
    double diffspeedmargin = diffSpeedMargin(opp);

    if (opp->mDistToStraight < diffspeedmargin || oppOnMyLine(opp, diffspeedmargin))
    {
        return true;
    }

    return false;
}

bool TDriver::oppOnMyLine(Opponent* opp, double margin)
{
    if (mDrvState != STATE_RACE)
    {
        return false;
    }

    double oppfs = fromStart(opp->fromStart);
    DanPoint oppdp;
    mDanPath.getDanPos(mDrvPath, oppfs, oppdp);

    if (fabs(oppdp.tomiddle - opp->toMiddle) < margin)
    {
        return true;
    }

    return false;
}

double TDriver::diffSpeedMargin(Opponent* opp)
{
    double speeddiff = MAX(0.0, mSpeed - opp->speed);
    double oppangle = opp->mAngle;
    double angle = 0.0;
    if ((oppangle < 0.0 && mOppLeftOfMe) || (oppangle > 0.0 && !mOppLeftOfMe))
    {
        angle = MIN(0.3, fabs(oppangle));
    }

    double factor = MAX(0.05, 0.5 * angle);
    double diffspeedmargin = MIN(15.0, 2.0 + sin(fabs(oppangle)) + factor * speeddiff);

    if (mSpeed < 5.0 || oppNoDanger(opp))
    {
        diffspeedmargin = 2.0 + sin(fabs(oppangle));
    }

    if (mDrivingFast)
    {
        diffspeedmargin += 1.0 + 0.2 * speeddiff;
    }

    return diffspeedmargin;
}

bool TDriver::oppNoDanger(Opponent* opp)
{
    if ((opp->borderdist < -3.0 && fabs(opp->speed) < 0.5 && mBorderdist > 0.0 && fabs(opp->mDist) > 1.0))
    {
        return true;
    }

    return false;
}

void TDriver::initCa(PCarSettings CarParmHandle)
{
    char* WheelSect[4] = { (char*)SECT_FRNTRGTWHEEL, (char*)SECT_FRNTLFTWHEEL, (char*)SECT_REARRGTWHEEL, (char*)SECT_REARLFTWHEEL };
    double frontwingarea = GfParmGetNum(CarParmHandle, SECT_FRNTWING, PRM_WINGAREA, (char*)NULL, 0.0);
    double rearwingarea = GfParmGetNum(CarParmHandle, SECT_REARWING, PRM_WINGAREA, (char*)NULL, 0.0);
    double frontclift = GfParmGetNum(CarParmHandle, SECT_AERODYNAMICS, PRM_FCL, (char*)NULL, 0.0);
    double rearclift = GfParmGetNum(CarParmHandle, SECT_AERODYNAMICS, PRM_RCL, (char*)NULL, 0.0);
    double frntwingca = 1.23 * frontwingarea * sin(mFRONTWINGANGLE);
    double rearwingca = 1.23 * rearwingarea * sin(mREARWINGANGLE);
    double cl = frontclift + rearclift;
    double h = 0.0;

    for (int i = 0; i < 4; i++)
    {
        h += GfParmGetNum(CarParmHandle, WheelSect[i], PRM_RIDEHEIGHT, (char*)NULL, 0.20f);
    }

    h *= 1.5; h = h * h; h = h * h; h = 2.0 * exp(-3.0 * h);
    mCA = h * cl + 4.0 * (frntwingca + rearwingca);
}

void TDriver::initCw(PCarSettings CarParmHandle)
{
    double cx = GfParmGetNum(CarParmHandle, SECT_AERODYNAMICS, PRM_CX, (char*)NULL, 0.0);
    double frontarea = GfParmGetNum(CarParmHandle, SECT_AERODYNAMICS, PRM_FRNTAREA, (char*)NULL, 0.0);
    mCW = 0.645 * cx * frontarea;
}

void TDriver::initBrakes()
{
    double maxf = 2.0 * mBRAKEREPARTITION * mBRAKEPRESS * oCar->_brakeDiskRadius(0) * mBRAKEPISTONAREA_FRONT * mBRAKEDISKMU_FRONT / oCar->_wheelRadius(0);
    double maxr = 2.0 * (1 - mBRAKEREPARTITION) * mBRAKEPRESS * oCar->_brakeDiskRadius(2) * mBRAKEPISTONAREA_REAR * mBRAKEDISKMU_REAR / oCar->_wheelRadius(2);
    mBRAKEFORCE_MAX = maxf + maxr;
}

void TDriver::readConstSpecs(PCarHandle CarHandle)
{
    mCARMASS = GfParmGetNum(CarHandle, SECT_CAR, PRM_MASS, NULL, 1000.0);
    mTANKVOL = GfParmGetNum(CarHandle, SECT_CAR, PRM_TANK, (char*)NULL, 50);
    mWHEELBASE = GfParmGetNum(CarHandle, SECT_FRNTAXLE, PRM_XPOS, (char*)NULL, 0.0) - GfParmGetNum(CarHandle, SECT_REARAXLE, PRM_XPOS, (char*)NULL, 0.0);

    mBRAKEPISTONAREA_FRONT = GfParmGetNum(CarHandle, SECT_FRNTRGTBRAKE, PRM_BRKAREA, (char*)NULL, 0.002f);
    mBRAKEPISTONAREA_REAR = GfParmGetNum(CarHandle, SECT_REARRGTBRAKE, PRM_BRKAREA, (char*)NULL, 0.002f);

    mBRAKEDISKMU_FRONT = GfParmGetNum(CarHandle, SECT_FRNTRGTBRAKE, PRM_MU, (char*)NULL, 0.30f);
    mBRAKEDISKMU_REAR = GfParmGetNum(CarHandle, SECT_REARRGTBRAKE, PRM_MU, (char*)NULL, 0.30f);

    const char *enabling;
    enabling = GfParmGetStr(CarHandle, SECT_FEATURES, PRM_TIRETEMPDEG, VAL_NO);

    if (strcmp(enabling, VAL_YES) == 0)
    {
      mHASTYC = true;
      LogDANDROID.info("#Car has TYC yes\n");
    }
    else
      LogDANDROID.info("#Car has TYC no\n");

    enabling = GfParmGetStr(CarHandle, SECT_FEATURES, PRM_ABSINSIMU, VAL_NO);

    if (strcmp(enabling, VAL_YES) == 0)
    {
      mHASABS = true;
      LogDANDROID.info("#Car has ABS yes\n");
    }
    else
      LogDANDROID.info("#Car has ABS no\n");

    enabling = GfParmGetStr(CarHandle, SECT_FEATURES, PRM_ESPINSIMU, VAL_NO);

    if (strcmp(enabling, VAL_YES) == 0)
    {
      mHASESP = true;
      LogDANDROID.info("#Car has ESP yes\n");
    }
    else
      LogDANDROID.info("#Car has ESP no\n");

    enabling = GfParmGetStr(CarHandle, SECT_FEATURES, PRM_TCLINSIMU, VAL_NO);

    if (strcmp(enabling, VAL_YES) == 0)
    {
      mHASTCL = true;
      LogDANDROID.info("#Car has TCL yes\n");
    }
    else
      LogDANDROID.info("#Car has TCL no\n");
}

void TDriver::readVarSpecs(PCarSettings CarParmHandle)
{
    mBRAKEPRESS = GfParmGetNum(CarParmHandle, SECT_BRKSYST, PRM_BRKPRESS, (char*)NULL, 20000.0);
    mBRAKEREPARTITION = GfParmGetNum(CarParmHandle, SECT_BRKSYST, PRM_BRKREP, (char*)NULL, 0.5);
    mFRONTWINGANGLE = GfParmGetNum(CarParmHandle, SECT_FRNTWING, PRM_WINGANGLE, (char*)NULL, 0.0);
    mREARWINGANGLE = GfParmGetNum(CarParmHandle, SECT_REARWING, PRM_WINGANGLE, (char*)NULL, 0.0);
}

void TDriver::readPrivateSection(PCarSettings* CarParmHandle)
{
    mBRAKEFORCEFACTOR = GfParmGetNum(*CarParmHandle, "private", "brakeforcefactor", (char*)NULL, 1.0);
    mBRAKEFORCEMIN = GfParmGetNum(*CarParmHandle, "private", "brakeforcemin", (char*)NULL, 0.0);
    mMUSCALE = GfParmGetNum(*CarParmHandle, "private", "muscale", (char*)NULL, 0.9);
    mBRAKESCALE = GfParmGetNum(*CarParmHandle, "private", "brakescale", (char*)NULL, 1.9);
    mBUMPSPEEDFACTOR = GfParmGetNum(*CarParmHandle, "private", "bumpspeedfactor", (char*)NULL, 3.0);
    mFUELPERMETER = GfParmGetNum(*CarParmHandle, "private", "fuelpermeter", (char*)NULL, 0.001f);
    mFUELWEIGHTFACTOR = GfParmGetNum(*CarParmHandle, "private", "fuelweightfactor", (char*)NULL, 1.0);
    mPITDAMAGE = (int)GfParmGetNum(*CarParmHandle, "private", "pitdamage", (char*)NULL, 5000);
    mPITENTRYMARGIN = GfParmGetNum(*CarParmHandle, "private", "pitentrymargin", (char*)NULL, 200.0);
    mPITENTRYSPEED = GfParmGetNum(*CarParmHandle, "private", "pitentryspeed", (char*)NULL, 25.0);
    mPITEXITSPEED = GfParmGetNum(*CarParmHandle, "private", "pitexitspeed", (char*)NULL, 25.0);
    mTARGETFACTOR = GfParmGetNum(*CarParmHandle, "private", "targetfactor", (char*)NULL, 0.3f);
    mTARGETWALLDIST = GfParmGetNum(*CarParmHandle, "private", "targetwalldist", (char*)NULL, 0.0);
    mTRACTIONCONTROL = GfParmGetNum(*CarParmHandle, "private", "tractioncontrol", (char*)NULL, 1.0) != 0;
    mMAXLEFT = GfParmGetNum(*CarParmHandle, "private", "maxleft", (char*)NULL, 10.0);
    mMAXRIGHT = GfParmGetNum(*CarParmHandle, "private", "maxright", (char*)NULL, 10.0);
    mMARGININSIDE = GfParmGetNum(*CarParmHandle, "private", "margininside", (char*)NULL, 1.0);
    mMARGINOUTSIDE = GfParmGetNum(*CarParmHandle, "private", "marginoutside", (char*)NULL, 1.5);
    mCLOTHFACTOR = GfParmGetNum(*CarParmHandle, "private", "clothoidfactor", (char*)NULL, 1.005f);
    mSEGLEN = GfParmGetNum(*CarParmHandle, "private", "seglen", (char*)NULL, 3.0);
}

void TDriver::printSetup()
{
    LogDANDROID.debug("%s: Learning=%d\n", oCar->_name, mLearning);
    LogDANDROID.debug("%s: Testpitstop=%d\n", oCar->_name, mTestpitstop);
    LogDANDROID.debug("%s: TestLine=%d\n", oCar->_name, mTestLine);
    LogDANDROID.debug("%s: DriverMsgLevel=%d\n", oCar->_name, mDriverMsgLevel);
    LogDANDROID.debug("%s: DriverMsgCarIndex=%d\n", oCar->_name, mDriverMsgCarIndex);
    LogDANDROID.debug("%s: FRONTCOLL_MARGIN=%g\n", oCar->_name, mFRONTCOLL_MARGIN);

    LogDANDROID.debug("%s: BRAKEPRESS=%g\n", oCar->_name, mBRAKEPRESS);
    LogDANDROID.debug("%s: BRAKE_REPARTITION=%g\n", oCar->_name, mBRAKEREPARTITION);
    LogDANDROID.debug("%s: FRONTWINGANGLE=%g\n", oCar->_name, mFRONTWINGANGLE * 360 / (2 * PI));
    LogDANDROID.debug("%s: REARWINGANGLE=%g\n", oCar->_name, mREARWINGANGLE * 360 / (2 * PI));
    LogDANDROID.debug("%s: CA=%g\n", oCar->_name, mCA);
    LogDANDROID.debug("%s: CW=%g\n", oCar->_name, mCW);
    LogDANDROID.debug("%s: WHEELBASE=%g\n", oCar->_name, mWHEELBASE);
    LogDANDROID.debug("%s: CARMASS=%g\n", oCar->_name, mCARMASS);
    LogDANDROID.debug("%s: BRAKEPISTON_AREA_FRONT=%g\n", oCar->_name, mBRAKEPISTONAREA_FRONT);
    LogDANDROID.debug("%s: BRAKEPISTON_AREA_REAR=%g\n", oCar->_name, mBRAKEPISTONAREA_REAR);
    LogDANDROID.debug("%s: BRAKEDISK_MU_FRONT=%g\n", oCar->_name, mBRAKEDISKMU_FRONT);
    LogDANDROID.debug("%s: BRAKEDISK_MU_REAR=%g\n", oCar->_name, mBRAKEDISKMU_REAR);

    LogDANDROID.debug("%s: brakeforcefactor=%g\n", oCar->_name, mBRAKEFORCEFACTOR);
    LogDANDROID.debug("%s: brakeforcemin=%g\n", oCar->_name, mBRAKEFORCEMIN);
    LogDANDROID.debug("%s: bumpspeedfactor=%g\n", oCar->_name, mBUMPSPEEDFACTOR);
    LogDANDROID.debug("%s: fuelpermeter=%g\n", oCar->_name, mFUELPERMETER);
    LogDANDROID.debug("%s: fuelweightfactor=%g\n", oCar->_name, mFUELWEIGHTFACTOR);
    LogDANDROID.debug("%s: pitdamage=%d\n", oCar->_name, mPITDAMAGE);
    LogDANDROID.debug("%s: pitentrymargin=%g\n", oCar->_name, mPITENTRYMARGIN);
    LogDANDROID.debug("%s: pitentryspeed=%g\n", oCar->_name, mPITENTRYSPEED);
    LogDANDROID.debug("%s: pitexitspeed=%g\n", oCar->_name, mPITEXITSPEED);
    LogDANDROID.debug("%s: targetfactor=%g\n", oCar->_name, mTARGETFACTOR);
    LogDANDROID.debug("%s: targetwalldist=%g\n", oCar->_name, mTARGETWALLDIST);
    LogDANDROID.debug("%s: tractioncontrol=%d\n", oCar->_name, mTRACTIONCONTROL);

    LogDANDROID.debug("%s: maxleft=%g\n", oCar->_name, mMAXLEFT);
    LogDANDROID.debug("%s: maxright=%g\n", oCar->_name, mMAXRIGHT);
    LogDANDROID.debug("%s: margininside=%g\n", oCar->_name, mMARGININSIDE);
    LogDANDROID.debug("%s: marginoutside=%g\n", oCar->_name, mMARGINOUTSIDE);
    LogDANDROID.debug("%s: clothoidfactor=%g\n", oCar->_name, mCLOTHFACTOR);
    LogDANDROID.debug("%s: seglen=%g\n", oCar->_name, mSEGLEN);

    LogDANDROID.debug("%s: skill level global=%g\n", oCar->_name, mSkillGlobal);
    LogDANDROID.debug("%s: skill level driver=%g\n", oCar->_name, mSkillDriver);
}

double TDriver::filterABS(double brake)
{
    double ABS_MINSPEED = 3.0;      // [m/s]
    double ABS_SLIP = 0.87;
    double slip = 0.0;

    if (mSpeed < ABS_MINSPEED)
        return brake;

    int i;

    for (i = 0; i < 4; i++)
    {
        slip += oCar->_wheelSpinVel(i) * oCar->_wheelRadius(i) / mSpeed;
    }

    slip = slip / 4.0;

    if (slip < ABS_SLIP)
    {
        if (mAbsFactor > 0.4) mAbsFactor -= 0.1;
        brake *= mAbsFactor;
    }
    else
    {
        if (mAbsFactor < 0.9) mAbsFactor += 0.1;
        brake *= mAbsFactor;
    }

    return brake;
}

double TDriver::filterTCL(double accel)
{
    if ((!mTRACTIONCONTROL && mDrvPath == PATH_O && mSpeed > 25) || (!mTRACTIONCONTROL && oCurrSimTime < 6.0))
    {
        return accel;
    }

    const double TCL_SLIP = 3.0;
    double slipfront = filterTCL_FWD() - mSpeed;
    double sliprear = filterTCL_RWD() - mSpeed;

    if (slipfront > TCL_SLIP || sliprear > TCL_SLIP)
    {
        if (mTclFactor > 0.1) mTclFactor -= 0.1;
        accel *= mTclFactor;
    }
    else
    {
        if (mTclFactor < 0.9) mTclFactor += 0.1;
    }

    return accel;
}

double TDriver::filterTCL_FWD()
{
    return (oCar->_wheelSpinVel(FRNT_RGT) + oCar->_wheelSpinVel(FRNT_LFT)) *
        oCar->_wheelRadius(FRNT_LFT) / 2.0f;
}

double TDriver::filterTCL_RWD()
{
    return (oCar->_wheelSpinVel(REAR_RGT) + oCar->_wheelSpinVel(REAR_LFT)) *
        oCar->_wheelRadius(REAR_LFT) / 2.0f;
}

double TDriver::filterTCLSideSlip(double accel)
{
    if (!mTRACTIONCONTROL && mDrvPath == PATH_O && mSpeed > 25)
    {
        return accel;
    }

    double sideslip = (oCar->_wheelSlipSide(FRNT_RGT) + oCar->_wheelSlipSide(FRNT_LFT) + oCar->_wheelSlipSide(REAR_RGT) + oCar->_wheelSlipSide(REAR_LFT)) / 4.0f;

    if (sideslip > 2.0 && mSpeed < 50)
    {
        return accel *= 0.8;
    }

    return accel;
}

double TDriver::fromStart(double fromstart)
{
    if (fromstart > -mTrack->length && fromstart < 2.0 * mTrack->length)
    {
        if (fromstart > mTrack->length)
        {
            return fromstart - mTrack->length;
        }
        else if (fromstart < 0.0)
        {
            return fromstart + mTrack->length;
        }

        return fromstart;
    }
    else
    {
        LogDANDROID.debug("!!!!!!!!!!!!!There is  a bug in %s, 'fromstart'=%g is out of range !!!!!!!!!!!!!!!", oCar->_name, fromstart);

        return 0.0;
    }
}

double TDriver::getCurvature(double dist)
{
    DanPoint p;
    double fs = fromStart(mFromStart + dist);
    mDanPath.getDanPos(mDrvPath, fs, p);

    return 1 / p.radius;
}

void TDriver::updateSector()
{
    for (int i = 0; i < (int)mSect.size(); i++)
    {
        if (mFromStart > mSect[i].fromstart
            && mFromStart < mSect[i].fromstart + 3.0)
        {
            mSector = i;
            break;
        }
    }
}

void TDriver::learnSpeedFactors()
{
    if (!mLearning || !mNewFile)
    {
        return;
    }

    double delta = 0.05;

    nextLearnSector(0);
    if (mLearnedAll && mNewFile)
    {
        writeSectorSpeeds();
        mNewFile = false;
        driverMsg("saved learning data in csv file");

        return;
    }

    if (oCar->_laps >= 2)
    {
        // Detecting offtrack situations and barrier collisions
        if (offtrack())
        {
            mOfftrackInSector = true;
        }
        // Get sector times
        for (int i = 0; i < (int)mSect.size(); i++)
        {
            if (mFromStart > mSect[i].fromstart && mFromStart < mSect[i].fromstart + 3.0)
            {
                if (mLearnSectTime)
                {
                    mLearnSectTime = false;
                    int prev_i = (i > 0) ? i - 1 : (int)mSect.size() - 1;
                    mSect[prev_i].time = oCurrSimTime - mSectorTime;

                    if (mOfftrackInSector)
                    {
                        mSect[prev_i].time = 10000;
                        driverMsgValue(0, "offtrack sector: ", prev_i);
                    }

                    mSectorTime = oCurrSimTime;
                    LogDANDROID.debug("sec: %d time: %g\n", prev_i, mSect[prev_i].time);
                    driverMsgValue(1, "sector: ", i);
                }
            }
            else if (mFromStart > mSect[i].fromstart + 3.0
                && mFromStart < mSect[i].fromstart + 6.0)
            {
                mLearnSectTime = true;
                mOfftrackInSector = false;
            }
        }
    }

    // Get lap time
    if (mFromStart > 3.0 && mFromStart < 6.0 && oCar->_laps >= 3 && mGetLapTime)
    {
        mGetLapTime = false;

        for (int i = 0; i < (int)mSect.size(); i++)
        {
            mLastLapTime += mSect[i].time;
            mBestLapTime += mSect[i].besttime;
        }
    }

    // First timing lap
    if (mFromStart > 3.0 && mFromStart < 6.0 && oCar->_laps == 3 && mLearnLap)
    {
        mLearnLap = false;
        // Reset best times to current times, fuel load dependent
        for (int i = 0; i < (int)mSect.size(); i++)
        {
            if (mSect[i].time < 10000)
            {
                mSect[i].besttime = mSect[i].time;
            }
            else
            {
                mSect[i].besttime += 1.0;
                mSect[i].speedfactor = mSect[i].bestspeedfactor - 0.1;
                mSect[i].bestspeedfactor = mSect[i].speedfactor;
            }
        }
        // All speedfactors the same?
        if (equalSpeedFactors())
        {
            for (int i = 0; i < (int)mSect.size(); i++)
            {
                increaseSpeedFactor(i, delta);
            }
        }
        else
        {
            mLearnSingleSector = true;
            mLearnSector = nextLearnSector(mLearnSector);
            increaseSpeedFactor(mLearnSector, delta);
        }
    }

    // Update speed factors
    if (mFromStart > 3.0 && mFromStart < 6.0 && oCar->_laps >= 4 && mLearnLap)
    {
        mLearnLap = false;

        if (!mLearnSingleSector)
        {
            mAllSectorsFaster = allSectorsFaster();

            if (mAllSectorsFaster)
            {
                // All speed factors still the same and still gaining time
                LogDANDROID.debug("lap: %d speedfactor: %g time gained: %g\n", oCar->_laps - 1, mSect[0].speedfactor, mBestLapTime - mLastLapTime);

                for (int i = 0; i < (int)mSect.size(); i++)
                {
                    mSect[i].bestspeedfactor = mSect[i].speedfactor;
                    mSect[i].besttime = mSect[i].time;
                }
            }
            else
            {
                // All speed factors still the same and reached the limit
                LogDANDROID.debug("lap: %d speedfactor: %g not all sectors faster\n", oCar->_laps - 1, mSect[0].speedfactor);
                for (int i = 0; i < (int)mSect.size(); i++)
                {
                    mSect[i].speedfactor -= delta;
                }

                mLearnSingleSector = true;
            }
        }
        else
        {
            // Changing only one sector speed factor per lap
            if (mLastLapTime < mBestLapTime)
            {
                LogDANDROID.debug("lap: %d sec: %d sf: %g gained: %g\n", oCar->_laps - 1, mLearnSector, mSect[mLearnSector].speedfactor, mBestLapTime - mLastLapTime);
                mSect[mLearnSector].bestspeedfactor = mSect[mLearnSector].speedfactor;
                // Update all best times
                for (int i = 0; i < (int)mSect.size(); i++)
                {
                    mSect[i].besttime = mSect[i].time;
                }
            }
            else
            {
                LogDANDROID.debug("lap: %d sec: %d sf: %g lost: %g\n", oCar->_laps - 1, mLearnSector, mSect[mLearnSector].speedfactor, mLastLapTime - mBestLapTime);
                mSect[mLearnSector].speedfactor = mSect[mLearnSector].bestspeedfactor;
                mSect[mLearnSector].time = mSect[mLearnSector].besttime;
                mSect[mLearnSector].learned = 1;
                driverMsgValue(0, "learned: ", mLearnSector);
            }
        }

        LogDANDROID.debug("lap: %d time total: %g best: %g\n", oCar->_laps - 1, mLastLapTime, mBestLapTime);
    }

    // Setup for the next lap
    if (mFromStart > 6.0 && mFromStart < 9.0 && oCar->_laps < oSituation->_totLaps && !mLearnLap)
    {
        // Increase speed factors
        if (oCar->_laps >= 4)
        {
            if (mAllSectorsFaster)
            {
                for (int i = 0; i < (int)mSect.size(); i++)
                {
                    increaseSpeedFactor(i, delta);
                }
            }
            else
            {
                mLearnSector = nextLearnSector(mLearnSector);
                increaseSpeedFactor(mLearnSector, delta);
            }
        }
        // Reset flags
        mAllSectorsFaster = false;
        mGetLapTime = true;
        mLastLapTime = 0.0;
        mBestLapTime = 0.0;
        mLearnLap = true;
    }
}

bool TDriver::offtrack()
{
    // Offtrack situations
    double offtrackmargin = 1.5;

    if (mLearnSingleSector && mSector != mLearnSector)
    {
        offtrackmargin += 0.3;
    }

    if (mBorderdist < -offtrackmargin)
    {
        LogDANDROID.debug("offtrack: %g\n", mBorderdist);
        return true;
    }
    // Barrier collisions
    if (mDamageDiff > 0 && mWalldist - oCar->_dimension_y / 2.0 < 0.5)
    {
        LogDANDROID.debug("barrier coll damage: %d\n", mDamageDiff);

        return true;
    }
    return false;
}

bool TDriver::equalSpeedFactors()
{
    for (int i = 0; i < (int)mSect.size(); i++)
    {
        if (mSect[i].speedfactor != mSect[0].speedfactor)
        {
            return false;
        }
    }

    return true;
}

bool TDriver::allSectorsFaster()
{
    for (int i = 0; i < (int)mSect.size(); i++)
    {
        if (mSect[i].time > mSect[i].besttime)
        {
            return false;
        }
    }

    return true;
}

int TDriver::nextLearnSector(int sect)
{
    sect = (sect < (int)mSect.size() - 1) ? sect + 1 : 0;

    for (int i = 0; i < (int)mSect.size(); i++)
    {
        if (!mSect[sect].learned)
        {
            break;
        }
        sect = (sect < (int)mSect.size() - 1) ? sect + 1 : 0;

        if (i == (int)mSect.size() - 1)
        {
            mLearnedAll = true;
        }
    }

    return sect;
}

void TDriver::increaseSpeedFactor(int sect, double inc)
{
    if (!mLearnedAll)
    {
        mSect[sect].speedfactor += inc;
    }

    if (mSect[sect].speedfactor >= 2.0)
    {
        mSect[sect].learned = 1;
    }
}

void TDriver::getBrakedistfactor()
{
    mBrakedistfactor = mSect[mSector].brakedistfactor;
    if (mCatchedRaceLine && mDrvPath == PATH_O)
    {
        mBrakedistfactor *= 1.0;
    }
    else if (mCatchedRaceLine)
    {
        if (mTargetOnCurveInside)
        {
            mBrakedistfactor *= 1.0;
        }
        else
        {
            mBrakedistfactor *= 2.0;
        }
    }
    else
    {
        if (mTargetOnCurveInside)
        {
            mBrakedistfactor *= 1.5;
        }
        else
        {
            mBrakedistfactor *= 2.5;
        }
    }
}

void TDriver::getSpeedFactors()
{
    mSectSpeedfactor = mSect[mSector].speedfactor;
}

void TDriver::updatePathCar(int path)
{
    if (!mDanPath.getDanPos(path, mFromStart, mPath[path].carpos))
    {
        driverMsg("error dandroid TDriver::updatePathCar");
    }
}

void TDriver::updatePathTarget(int path)
{
    if (mDrvState == STATE_RACE && path == PATH_O && mCatchedRaceLine)
    {
        mTargetFromstart = fromStart(mFromStart + mLOOKAHEAD_CONST + mTARGETFACTOR * mSpeed);
    }
    else if (mDrvState == STATE_PITLANE)
    {
        mTargetFromstart = fromStart(mFromStart + 2.0 + 0.3 * mSpeed);
    }
    else
    {
        mTargetFromstart = fromStart(mFromStart + mLOOKAHEAD_CONST + 0.3 * mSpeed);
    }
    if (!mDanPath.getDanPos(path, mTargetFromstart, mPath[path].tarpos)) {
        driverMsg("error dandroid TDriver::updatePathTarget");
    }
}

void TDriver::updatePathOffset(int path)
{
    mPath[path].offset = mPath[path].carpos.tomiddle - mToMiddle;
}

void TDriver::updatePathSpeed(int path)
{
    mPath[path].maxspeed = getMaxSpeed(mPath[path].carpos);
}

void TDriver::updateCurveAhead()
{
    if (mFromStart > mCurveAheadFromStart)
    {
        mCurveAhead = false;
    }

    if (!mCurveAhead)
    {
        if (mTrackType != TR_STR && mTrackRadius < 200.0) {
            mCurveAheadFromStart = fromStart(mFromStart + 5);
            mCurveAhead = true;
        }
        else
        {
            double fs = fromStart(mFromStart + 1.5 * mSpeed);
            DanPoint dp;
            mDanPath.getDanPos(mDrvPath, fs, dp);

            if (dp.type != TR_STR && fabs(dp.radius) < 300.0)
            {
                mCurveAheadFromStart = fs;
                mCurveAhead = true;
            }
        }
    }
}

void TDriver::updateDrivingFast()
{
    double maxspeed = mPath[mDrvPath].maxspeed;
    mDrivingFast = (mSpeed > 0.85 * maxspeed
        || (mTrackRadius < 200 && maxspeed > 100.0 && mSpeed > 40.0)
        || (mCurveAhead && mSpeed > 40.0)
        || mControlAttackAngle
        || mBumpSpeed)
        && mSpeed > 5.0;
    // Delay state change for 0.5 second (25 x 20ms)
    if (prev_mDrivingFast && !mDrivingFast)
    {
        if (mDrivingFastCount < 25)
        {
            mDrivingFastCount++;
            mDrivingFast = true;
        }
        else
        {
            mDrivingFastCount = 0;
            mDrivingFast = false;
        }
    }
    else
    {
        mDrivingFastCount = 0;
    }
}

void TDriver::updateCatchedRaceLine()
{
    if (mDrvState == STATE_RACE && !mPathChange)
    {
        if (fabs(mPathOffs) < 1.0)
        {
            if (mCatchedRaceLineTime > 1.0)
            {
                mCatchedRaceLine = true;
            }
            else if (mTenthTimer)
            {
                mCatchedRaceLineTime += 0.1;
            }
        }
        else if (!mCatchedRaceLine)
        {
            mCatchedRaceLineTime = 0.0;
        }
        else if (fabs(mPathOffs) > 4.5)
        {
            mCatchedRaceLine = false;
            mCatchedRaceLineTime = 0.0;
        }
    }
    else
    {
        mCatchedRaceLine = false;
        mCatchedRaceLineTime = 0.0;
    }
}

void TDriver::updateFrontCollFactor()
{
    mFrontCollFactor = 1.0;

    if (mBackmarkerInFrontOfTeammate || mDrivingFast)
    {
        mFrontCollFactor = 1.5;
    }

    if (fabs(mSpeed) < 5.0)
    {
        mFrontCollFactor = 0.2;
    }

    if (mOpp != NULL)
    {
        if (fabs(mOpp->mAngle) > 1.5)
        {
            mFrontCollFactor = 2.0;
        }
    }
}

void TDriver::calcMaxspeed()
{
    double maxspeed = mPath[mDrvPath].maxspeed;
    switch (mDrvState)
    {
    case STATE_RACE:
    {
        if (mCatchedRaceLine && mDrvPath == PATH_O)
        {
            mMaxspeed = maxspeed;
        }
        else if (mCatchedRaceLine)
        {
            if (mTargetOnCurveInside)
            {
                mMaxspeed = 0.98 * maxspeed;
            }
            else
            {
                mMaxspeed = (0.95 - 0.01 * fabs(mToMiddle)) * maxspeed;
            }
        }
        else
        {
            if (mTargetOnCurveInside)
            {
                mMaxspeed = 0.93 * maxspeed;
            }
            else
            {
                mMaxspeed = (0.9 - 0.01 * fabs(mToMiddle)) * maxspeed;
            }
        }

        mMaxspeed = mSkillGlobal * mMaxspeed;

        if(mHASTYC)
        {
            mMaxspeed *= TyreCondition();
        }

        // Special cases
        if (mLetPass)
        {
            mMaxspeed = 0.85 * maxspeed;
        }

        if (fabs(mAngleToTrack) > 1.0)
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
        mMaxspeed = 10.0;
        break;
    }
    case STATE_PITLANE:
    {
        mMaxspeed = MIN(getPitSpeed(), 1.0 * maxspeed);
        break;
    }
    default:
    {
        break;
    }
    }


}

void TDriver::limitSteerAngle(double& targetangle)
{
    double v2 = mSpeed * mSpeed;
    double rmax = v2 / (mMu * GRAVITY + v2 * mCA * mMu / mMass);
    double maxangle = atan(mWHEELBASE / rmax);
    double maxanglefactor;

    if (mDrvState == STATE_OFFTRACK)
    {
        maxanglefactor = 1.0;
    }
    else if (!mCatchedRaceLine)
    {
        maxanglefactor = 10.0;
    }
    else
    {
        maxanglefactor = 100.0;
    }
    maxangle *= maxanglefactor;
    mMaxSteerAngle = false;

    if (fabs(targetangle) > maxangle)
    {
        targetangle = SIGN(targetangle) * maxangle;
        NORM_PI_PI(targetangle);
        mMaxSteerAngle = true;
    }
}

void TDriver::calcGlobalTarget()
{
    if (mTargetToMiddle == mNormalTargetToMiddle)
    {
        mGlobalTarget = mPath[mDrvPath].tarpos.pos;
    }
    else
    {
        tTrkLocPos target_local;
        RtTrackGlobal2Local(oCar->_trkPos.seg, (tdble)mPath[mDrvPath].tarpos.pos.x, (tdble)mPath[mDrvPath].tarpos.pos.y, &target_local, TR_LPOS_MAIN);
        target_local.toMiddle = (tdble)mTargetToMiddle;
        tdble x, y;
        RtTrackLocal2Global(&target_local, &x, &y, TR_TOMIDDLE);
        mGlobalTarget.x = x;
        mGlobalTarget.y = y;
    }
}

void TDriver::calcTargetAngle()
{
    mTargetAngle = Utils::VecAngle(mGlobalTarget - mGlobalCarPos) - oCar->_yaw;
    NORM_PI_PI(mTargetAngle);
}

void TDriver::controlSpeed(double& accelerator, double maxspeed)
{
    // Set parameters
    mSpeedController.m_p = 0.02;
    mSpeedController.m_d = 0.0;
    // Run controller
    double speeddiff = maxspeed - mSpeed;
    accelerator += mSpeedController.sample(speeddiff);

    if (accelerator > 1.0)
    {
        accelerator = 1.0;
    }
    if (accelerator < 0.0)
    {
        accelerator = 0.0;
    }
}

void TDriver::updateAttackAngle()
{
    double velAng = atan2(oCar->_speed_Y, oCar->_speed_X);
    mAttackAngle = velAng - oCar->_yaw;
    NORM_PI_PI(mAttackAngle);

    if (mSpeed < 1.0)
    {
        mAttackAngle = 0.0;
    }
}

bool TDriver::controlAttackAngle(double& targetangle)
{
    if (fabs(mAttackAngle) > 0.1 || mDrvState == STATE_OFFTRACK)
    {
        mAttackAngleController.m_d = 4.0;
        mAttackAngleController.m_p = 0.3;
        targetangle += mAttackAngleController.sample(mAttackAngle);
        NORM_PI_PI(targetangle);
        mControlAttackAngle = true;
    }
    else
    {
        mAttackAngleController.sample(mAttackAngle);
        mControlAttackAngle = false;
    }

    return mControlAttackAngle;
}

void TDriver::controlYawRate(double& targetangle)
{
    mControlYawRate = false;
    if (mDrvState == STATE_RACE)
    {
        double AvgK = 1 / mPath[mDrvPath].carpos.radius;
        // Control rotational velocity.
        double Omega = mSpeed * AvgK;
        double yawratediff = Omega - oCar->_yaw_rate;

        if (fabs(yawratediff) > 0.2)
        {
            mControlYawRate = true;
            targetangle += 0.09 * (Omega - oCar->_yaw_rate);
            NORM_PI_PI(targetangle);
        }
    }
}

bool TDriver::hysteresis(bool lastout, double in, double hyst)
{
    if (lastout == false)
    {
        if (in > hyst)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (in < -hyst)
        {
            return false;
        }
        else {
            return true;
        }
    }
}

double TDriver::getFuel(double dist)
{
    double fuel = dist * mFUELPERMETER;
    if (mTestpitstop) {
        fuel = 1.0 * mTrack->length * mFUELPERMETER;
    }
    return fuel = MAX(MIN(fuel, mTANKVOL), 0.0);
}

void TDriver::writeSectorSpeeds()
{
    char dirname[256];
    sprintf(dirname, "%s/drivers/%s/%s/learned/", GetLocalDir(), MyBotName, mCarType.c_str());
#ifdef DANDROID_TORCS
    if (GfCreateDir(strdup(dirname)) == GF_DIR_CREATED) {
#else
    if (GfDirCreate(strdup(dirname)) == GF_DIR_CREATED) {
#endif
        char filename[256];
        sprintf(filename, "%sdrivers/%s/%s/learned/%s.csv", GetLocalDir(), MyBotName, mCarType.c_str(), mTrack->internalname);
        std::ofstream myfile;
        myfile.open(filename);

        for (int i = 0; i < (int)mSect.size(); i++)
        {
            myfile << mSect[i].sector << std::endl;
            myfile << mSect[i].fromstart << std::endl;
            myfile << mSect[i].brakedistfactor << std::endl;
            myfile << mSect[i].speedfactor << std::endl;
        }

        myfile.close();
    }
    else
    {
        driverMsg("Error saveFile: unable to create user dir");
    }
}

bool TDriver::readSectorSpeeds()
{
    char filename[256];
    if (mLearning)
    {
        sprintf(filename, "%sdrivers/%s/%s/learned/%s.csv", GetLocalDir(), MyBotName, mCarType.c_str(), mTrack->internalname);
    }
    else
    {
        sprintf(filename, "%sdrivers/%s/%s/learned/%s.csv", GetDataDir(), MyBotName, mCarType.c_str(), mTrack->internalname);
    }

    DanSector sect;
    std::ifstream myfile(filename);

    if (myfile.is_open())
    {
        while (myfile >> sect.sector
            >> sect.fromstart
            >> sect.brakedistfactor
            >> sect.speedfactor)
        {
            if (mLearning)
            {
                LogDANDROID.debug("S:%d l:%d fs:%g bdf:%g t:%g bt:%g sf:%g bsf:%g\n", sect.sector, sect.learned, sect.fromstart, sect.brakedistfactor, sect.time, sect.besttime, sect.speedfactor, sect.bestspeedfactor);
            }

            mSect.push_back(sect);
        }

        myfile.close();
        return true;
    }
    else
    {
        driverMsg("readSectorSpeeds(): no csv file found");
        return false;
    }
}

void TDriver::driverMsg(std::string desc)
{
    LogDANDROID.debug("%s %s\n", oCar->_name, desc.c_str());
}

void TDriver::driverMsgValue(int priority, std::string desc, double value)
{
    if (priority <= mDriverMsgLevel && mCarIndex == mDriverMsgCarIndex)
    {
        LogDANDROID.debug("%dm %s s:%d p:%d %s %g\n", (int)mFromStart, oCar->_name, mDrvState, mDrvPath, desc.c_str(), value);
    }
}

double TDriver::TyreConditionFront()
{
    return MIN(oCar->_tyreCondition(0), oCar->_tyreCondition(1));
}

double TDriver::TyreConditionRear()
{
    return MIN(oCar->_tyreCondition(2), oCar->_tyreCondition(3));
}

double TDriver::TyreCondition()
{
    LogDANDROID.debug("# DanDroid (%i) Tyre condition = %.8f - Tyre temperature = %.8f\n", mCarIndex, MIN(TyreConditionFront(), TyreConditionRear()), oCar->_tyreT_mid(0));
    return MIN(TyreConditionFront(), TyreConditionRear());
}

double TDriver::TyreTreadDepthFront()
{
    double Right = (oCar->_tyreTreadDepth(0) - oCar->_tyreCritTreadDepth(0));
    double Left = (oCar->_tyreTreadDepth(1) - oCar->_tyreCritTreadDepth(1));

    return 100 * MIN(Right,Left);
}

double TDriver::TyreTreadDepthRear()
{
    double Right = (oCar->_tyreTreadDepth(2) - oCar->_tyreCritTreadDepth(2));
    double Left = (oCar->_tyreTreadDepth(3) - oCar->_tyreCritTreadDepth(3));

    return 100 * MIN(Right,Left);
}

double TDriver::TyreTreadDepth()
{
    return MIN(TyreTreadDepthFront(), TyreTreadDepthRear());
}
