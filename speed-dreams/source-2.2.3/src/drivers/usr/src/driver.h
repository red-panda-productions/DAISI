/***************************************************************************

    file                 : driver.h
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

#ifndef _DRIVER_H_
#define _DRIVER_H_

#include "datalog.h"
//#include "message.h"
#include "MuFactors.h"
#include "MyCar.h"
#include "opponents.h"
#include "PathState.h"
#include "PathType.h"
#include "pidcontroller.h"
#include "pit.h"
#include "Path.h"
#include "PathMargins.h"
#include "tires.h"

#include <string>
#include <vector>

// The "USR" logger instance.
extern GfLogger* PLogUSR;
#define LogUSR (*PLogUSR)

class Driver
{
public:
    Driver(int index);

    void InitTrack(tTrack *Track, void* carHandle, void** carParmHandle, tSituation* situation);
    void NewRace(tCarElt* car, tSituation* situation);
    void Drive();
    int  PitCmd();
    void EndRace();
    void Shutdown();

    const char* mDriverName;
    int         mTeamIndex;                              // Index of car in Teams arrays;

private:
    enum   DriveState {STATE_RACE, STATE_STUCK, STATE_OFFTRACK, STATE_PITLANE, STATE_PITSTOP};
    void   TeamInfo(tCarElt* car, tSituation* situation);
    void   updateTime();
    void   updateBasics();
    void   updateOpponents();
    void   updatePathState();
    void   setControls();
    void   printInfos();
    void   initVars();
    void   setPrevVars();
    double pitSpeed();
    double brakeDist(PathType path, double speed, double allowedspeed);
    double getBrake(double maxspeed) const;
    double getAccel(double maxspeed);
    double getSteer(double steerlock);
    bool   stateStuck();
    bool   stateOfftrack() const;
    bool   statePitstop() const;
    bool   statePitlane();
    void   setDrvState(DriveState state);
    void   setDrvPath(PathType path);
    void   calcStateAndPath();
    bool   overtakeOpponent();
    void   updateOvertakePath();
    bool   stuck();
    bool   updateOnCollision();
    bool   oppInCollisionZone(Opponent* opp) const;
    bool   oppFast(Opponent* opp) const;
    double diffSpeedMargin(Opponent* opp) const;
    bool   oppNoDanger(Opponent* opp) const;
    double frontCollFactor(Opponent* opp);
    double fromStart(double fromstart) const;
    double pathOffs(PathType path) const;
    double pathSpeed(PathType path) const;
    double pathAcceleration(PathType path) const;
    void   updateDrivingFast();
    void   updateLetPass();
    void   calcMaxspeed();
    void   calcOffsetAndYaw();
    double getSteerAngle(double steerlock);
    double controlSpeed(double accelerator, double maxspeed);

    void	Meteorology(const tTrack *t);
    unsigned int GetWeather(const tTrack *t);

    bool    CheckPitSharing(tCarElt *car);

    // Name, dirs
    int INDEX;
    std::string mDataDir;
    std::string mLocalDir;

    // Flags
    enum Flags
    {
        STATE_CHANGE, DRIVING_FAST, FRICT_LR,
        COLL, WAIT,
        LET_PASS, CATCH, OVERTAKE,
        FAST_BEHIND
    };

    std::vector<std::string> mFlagNames; // names assigned in constructor for VS 2013 compatibility
    std::vector<bool> m;                 // flags vector
    std::vector<bool> mPrev;

    // Basics
    double mDeltaTime;
    double mSimTime;
    double mLapSimTime;
    tSituation* mSituation;
    MyCar mCar;
    MyTrack mTrack;

    // Paths
    std::vector<Path> mPath;
    std::vector<PathState> mPathState;
    PathType mDrvPath;
    double   mPathOffs;
    double   mPrevPathOffs;
    double   mPathOffsDeriv;
    double   mPathYaw;
    double   mPathCurvature;
    double   mPathYaw_carSpeedYaw_diff;
    PathType mDrvPathOld;
    PathType mOvertakePath;
    double   mLRTargetPortion; // where we want to be between L and R (1..-1)
    double   mLRTargetStep;
    double   mVMaxK;
    double   mVMaxKFactor;

    // Mu factors
    MuFactors mMuFactors;
    double   mCurrMu;

    // Pit
    Pit      mPit;

    // Opponents
    Opponents mOpps;
    bool     mOppAsideAtPitEntry;

    // States
    DriveState mDrvState;
    double     mFromStart;
    double     mStuckTime;

    // Test
    PathType   mTestPath;

    // Speed
    double     mMaxspeed;
    PidController mSpeedController;

    // Controls
    double mBrakePedalRace;
    double mBrakePedal;
    double mAccelPedalRace;
    double mAccelPedal;
    double mSteer;
    double mYawSteer;
    double mYawRateSteer;
    double mCurvSteer;
    double mOffsSteer;
    double mOffsDerivSteer;

    // Message, Log
    //Message mMsg;
    //int mMsgOn;
    DataLog mDataLog;
    int mDataLogOn;

    // Configuration file constants
    double mFuelperMeter;
    int    mPitDamage;
    double mPitGripFactor;
    double mPitEntryMargin;
    double mPitEntrySpeed;
    double mPitExitSpeed;
    double mSegLen;
    double mClothFactor;
    int    mPitTest;

    bool   rain;
    double rainintensity;
    unsigned int weathercode;            // Track specific weather

    double driver_aggression;
    double SkillGlobal;
    double SkillDriver;
    bool   pitsharing;
    bool   mGarage;

    double simtime;                // how long since the race started

    unsigned int    random_seed;

    // Constants
    double mFrontCollMargin;
    double mOvtMargin;

    tTrack* track;
    tCarElt* car;
};

#endif // _DRIVER_H_
