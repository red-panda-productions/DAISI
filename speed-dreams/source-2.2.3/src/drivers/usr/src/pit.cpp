/***************************************************************************

    file                 : pit.cpp
    created              : Thu Aug 31 01:21:49 UTC 2006
    copyright            : (C) 2006 Daniel Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "pit.h"
#include "Utils.h"

#include <iostream>

Pit::Pit() :
    mTrack(NULL),
    mMyCar(NULL),
    mCar(NULL),
    mTeamCar(NULL),
    mPit(NULL),
    mPitInfo(NULL),
    mTiresChangeTime(30.0),
    mMaxDamage(7000.0),
    mMaxDamageDist(30000.0),
    mPreEntryMargin(100.0)
{
}

void Pit::init(const tTrack* track, const tSituation* situation, MyCar* car, int pitdamage, double pitgripfactor, double pitentrymargin)
{
    // Get tires change time
    //void* handle = NULL;
    /*std::string tmpstr = std::string(GetLocalDir()) + "config/raceman/endrace.xml";
    handle = GfParmReadFile(tmpstr.c_str(), GFPARM_RMODE_STD);
    if (handle)
    {
        mTiresChangeTime = GfParmGetNum(handle, "Race", "all tires change time", (char*)NULL, 30.0);
        GfParmReleaseHandle(handle);
    }*/

    if (pitdamage)
    {
        mPitDamage = pitdamage;
    }
    else
    {
        mPitDamage = 5000;
    }

    if (pitgripfactor)
    {
        mPitGripFactor = pitgripfactor;
    }
    else
    {
        mPitGripFactor = 0.80;
    }

    mEntryMargin = pitentrymargin;
    mTrack = track;
    mMyCar = car;
    mCar = (tCarElt*)mMyCar->car();
    mPit = mCar->_pit;
    mPitInfo = &mTrack->pits;
    mPitstop = mInPitLane = false;
    mFuelChecked = false;
    mStopChecked = false;
    mTotalFuel = 0.0;
    mFuelLapsCounted = 0;
    mAvgFuelPerLap = 0.0;
    mLastPitFuel = 0.0;
    mLastFuel = 0.0;
    mPenalty = 0;
    // Get teammates car
    int i;

    for (i = 0; i < situation->_ncars; i++)
    {
        if (situation->cars[i] != mCar)
        {
            if (!strncmp(mCar->_teamname, situation->cars[i]->_teamname, 10))
            {
                mTeamCar = situation->cars[i];
            }
        }
    }

    if (mPit != NULL)
    {
        mSpeedLimit = mPitInfo->speedLimit - 0.5;
        // Compute pit spline points along the track
        mPitp[3].x = mPit->pos.seg->lgfromstart + mPit->pos.toStart;
        mPitp[2].x = mPitp[3].x - mPitInfo->len;
        mPitp[4].x = mPitp[3].x + mPitInfo->len;
        mPitp[0].x = mPitInfo->pitEntry->lgfromstart;
        mPitp[1].x = mPitInfo->pitStart->lgfromstart - mPitInfo->len;
        mPitp[5].x = mPitInfo->pitEnd->lgfromstart + mPitInfo->pitEnd->length + mPitInfo->len;
        //mPitp[5].x = mPitInfo->pitEnd->lgfromstart + mPitInfo->nPitSeg * mPitInfo->len;
        mPitp[6].x = mPitInfo->pitExit->lgfromstart + mPitInfo->pitExit->length;
        //mPitp[6].x = mPitInfo->pitExit->lgfromstart;
        mDtp[0].x = mPitp[0].x;
        mDtp[1].x = mPitp[1].x;
        mDtp[2].x = mPitp[5].x;
        mDtp[3].x = mPitp[6].x;
        mPitEntry = mPitp[0].x;
        mPitExit = mPitp[6].x;
        mLimitEntry = mPitp[1].x;
        mLimitExit = mPitp[5].x;
        // Normalizing spline segments to <= 0.0
        int i;

        for (i = 0; i < PITPOINTS; i++)
        {
            mPitp[i].s = 0.0;
            mPitp[i].x = toSplineCoord(mPitp[i].x);
        }

        for (i = 0; i < DTPOINTS; i++)
        {
            mDtp[i].s = 0.0;
            mDtp[i].x = toSplineCoord(mDtp[i].x);
        }

        // Fix broken pit exit.
        if (mPitp[6].x < mPitp[5].x)
        {
            //fprintf(stderr,"bt: Pitexit broken on track %s.\n", track->name);fflush(stderr);
            mPitp[6].x = mPitp[5].x;// +50.0f;
        }

        mPitp[1].x = std::min(mPitp[1].x, mPitp[2].x);
        mPitp[5].x = std::max(mPitp[4].x, mPitp[5].x);

        mPitOnLeftSide = mPitInfo->side == TR_LFT;
        double sign = mPitOnLeftSide ? 1.0 : -1.0;
        mPitp[0].y = sign * (mTrack->width / 2.0 - 2.0);
        mPitp[6].y = sign * (mTrack->width / 2.0 - 2.0);
        //mPitp[0].y = 0.0;
        //mPitp[6].y = 0.0;
        mDtp[0].y = mPitp[0].y;
        mDtp[3].y = mPitp[6].y;

        for (i = 1; i < PITPOINTS - 1; i++)
        {
            mPitp[i].y = sign * (fabs(mPitInfo->driversPits->pos.toMiddle) - 0.3 * mPitInfo->width - 2.2);
            //mPitp[i].y = sign * (fabs(mPitInfo->driversPits->pos.toMiddle) - mPitInfo->width);
        }

        for (i = 1; i < DTPOINTS - 1; i++)
        {
            mDtp[i].y = sign * (fabs(mPitInfo->driversPits->pos.toMiddle) - 0.3 * mPitInfo->width - 2.2);
            //mDtp[i].y = sign * (fabs(mPitInfo->driversPits->pos.toMiddle) - mPitInfo->width);
        }

        mPitp[3].y = sign * (fabs(mPitInfo->driversPits->pos.toMiddle) + 1.0);
        mPitSpline.init(PITPOINTS, mPitp);
        mDtSpline.init(DTPOINTS, mDtp);
    }
}

// Transforms track coordinates to spline parameter coordinates
double Pit::toSplineCoord(double x) const
{
    x -= mPitEntry;

    while (x < 0.0)
    {
        x += mTrack->length;
    }

    return x;
}

// Computes offset to track middle for trajectory
double Pit::pathToMiddle(double fromstart) const
{
    if (mPit != NULL)
    {
        if (mInPitLane || ((mPitstop || mPenalty == RM_PENALTY_DRIVETHROUGH) && isBetween(fromstart)))
        {
            fromstart = toSplineCoord(fromstart);

            if (mPenalty == RM_PENALTY_DRIVETHROUGH)
            {
                return mDtSpline.evaluate(fromstart);
            }
            else
            {
                return mPitSpline.evaluate(fromstart);
            }
        }
        else if ((mPitstop || mPenalty == RM_PENALTY_DRIVETHROUGH) && isBetween(fromstart + mEntryMargin))
        {
            return mPitp[0].y;
        }
    }

    return 0.0;
}

void Pit::setPitstop(bool pitstop)
{
    if (mPit == NULL)
    {
        return;
    }

    if (!isBetween(mCar->_distFromStartLine) && !isBetween(mCar->_distFromStartLine + mEntryMargin + mPreEntryMargin))
    {
        if (mTeamCar != NULL && !(mTeamCar->_state & RM_CAR_STATE_OUT))
        {
            if (mTeamCar->_raceCmd == RM_CMD_PIT_ASKED || mTeamCar->_state & RM_CAR_STATE_PIT)
            {
                return;
            }
        }

        mCar->_raceCmd = RM_CMD_PIT_ASKED;
        mPitstop = pitstop;
    }
    else if (pitstop == false)
    {
        mPitstop = pitstop;
    }
}

bool Pit::isBetween(double fromstart) const
{
    if (fromstart > mTrack->length)
    {
        fromstart -= mTrack->length;
    }

    if (mPitEntry <= mPitExit)
    {
        if (fromstart >= mPitEntry && fromstart <= mPitExit)
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
        if ((fromstart >= 0.0 && fromstart <= mPitExit) || (fromstart >= mPitEntry && fromstart <= mTrack->length))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool Pit::isPitLimit(double fromstart)
{
    if (mLimitEntry <= mLimitExit)
    {
        if (fromstart >= mLimitEntry && fromstart <= mLimitExit)
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
        if ((fromstart >= 0.0 && fromstart <= mLimitExit) || (fromstart >= mLimitEntry && fromstart <= mTrack->length))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

void Pit::update()
{
    if (mPit == NULL)
    {
        return;
    }

    double fs = fabs(mCar->_distFromStartLine); // TORCS problem with negative _distFromStartLine
    updateInPitLane(fs);
    updateFuel(fs);
    int remaininglaps = mCar->_remainingLaps - mCar->_lapsBehindLeader;

    if (mPitstop || remaininglaps == 0)
    {
        return;
    }

    mPenalty = 0; // fuel, damage and tires served before penalty
    bool pittyres = false;
    // Check for fuel, damage and tyres
    bool pitfuel = mCar->_fuel < (mAvgFuelPerLap + 2.0);
    bool pitdamage = (mCar->_dammage > mPitDamage && remaininglaps * mTrack->length > mMaxDamageDist && mLastFuel > 15.0) || (mCar->_dammage > mMaxDamage);
    //bool pittyres = (mMyCar->tires()->distLeft() < 1.0 * mTrack->length && mMyCar->tires()->gripFactor() < mPitGripFactor && remaininglaps * mTrack->length > 10000.0);

    if(mMyCar->HASTYC)
        pittyres = (mMyCar->tires()->gripFactor() < mPitGripFactor && remaininglaps * mTrack->length > 10000.0 && mMyCar->tires()->distLeft() < 1000);
    else {
        pittyres = false;
    }
    if ((fs > mPitEntry - mEntryMargin - mPreEntryMargin - 3.0) && (fs < mPitEntry - mEntryMargin - mPreEntryMargin) && !mStopChecked)
    {
        if (pitBeforeTeammate(remaininglaps))
        {
            setPitstop(true);
        }
        else if (pitfuel || pitdamage || pittyres)
        {
            setPitstop(true);
        }
        else if (pitForPenalty())
        {
            setPitstop(true);
        }

        mStopChecked = true;
    }
    else if ((fs >= mPitEntry - mEntryMargin) && (fs < mPitEntry - mEntryMargin + 3.0))
    {
        mStopChecked = false;
    }
}

bool Pit::pitBeforeTeammate(int remaininglaps) const
{
    bool teamcarrunning = (mTeamCar != NULL) && !(mTeamCar->_state & RM_CAR_STATE_OUT) && (mTeamCar->_bestLapTime > 0.0);
    double teamcarfuel;

    if (teamcarrunning)
    {
        teamcarfuel = mTeamCar->_fuel;
    }

    if (teamcarrunning && (mCar->_fuel < teamcarfuel) && (mCar->_fuel < remaininglaps * mAvgFuelPerLap))
    {
        double matelaps = teamcarfuel / mAvgFuelPerLap;
        double matetoentry;

        if ((mPitEntry - mEntryMargin) > mTeamCar->_distFromStartLine)
        {
            matetoentry = (mPitEntry - mEntryMargin) - mTeamCar->_distFromStartLine;
        }
        else
        {
            matetoentry = mTrack->length - (mTeamCar->_distFromStartLine + (mPitEntry - mEntryMargin));
        }

        double matefulllaps = floor(matelaps - matetoentry / mTrack->length);
        double matetime = (matefulllaps + matetoentry / mTrack->length) * mTeamCar->_bestLapTime;
        double maxstoptime = 15.0 + 0.007 * mCar->_dammage + mTiresChangeTime;
        double mytime = mCar->_bestLapTime + maxstoptime + 60.0;
        bool matepitasked = mTeamCar->_raceCmd == RM_CMD_PIT_ASKED;
        bool matestop = mTeamCar->_state & RM_CAR_STATE_PIT;

        if (matetime < mytime && !(matepitasked || matestop))
        {
            return true;
        }
    }

    return false;
}

bool Pit::pitForPenalty()
{
    // Do we need to serve a penalty
    tCarPenalty *penalty = GF_TAILQ_FIRST(&(mCar->_penaltyList));

    if (penalty)
    {
        if (penalty->penalty == RM_PENALTY_DRIVETHROUGH || penalty->penalty == RM_PENALTY_STOPANDGO)
        {
            // Rudimentary strategy here - always serving the penalty straightaway.
            // there's almost certainly more clever ways of doing this, as there's
            // five laps after the penalty in which to serve it before the car is
            // eliminated.
            mPenalty = penalty->penalty;

            return true;
        }
    }

    return false;
}

void Pit::updateInPitLane(double fromstart)
{
    if (isBetween(fromstart))
    {
        if (mPitstop || mPenalty == RM_PENALTY_DRIVETHROUGH)
        {
            mInPitLane = true;
        }
    }
    else
    {
        mInPitLane = false;
    }
}

void Pit::updateFuel(double fromstart)
{
    if (fromstart <= 3.0 && !mFuelChecked)
    {
        if (mCar->race.laps > 1)
        {
            mTotalFuel += mLastFuel + mLastPitFuel - mCar->priv.fuel;
            mFuelLapsCounted++;
            mAvgFuelPerLap = mTotalFuel / mFuelLapsCounted;
        }

        mLastFuel = mCar->priv.fuel;
        mLastPitFuel = 0.0;
        mFuelChecked = true;
    }
    else if (fromstart > 3.0 && fromstart < 6.0)
    {
        mFuelChecked = false;
    }
}

double Pit::calcRefuel()
{
    double laps = mCar->_remainingLaps + (mTrack->length - mCar->_distFromStartLine) / mTrack->length;

    // Calc fuel pitstops
    double fueltoend = (laps - mCar->_lapsBehindLeader) * mAvgFuelPerLap;
    int fuelpitstops = int(floor(fueltoend / mCar->_tank));

    // Calc tires pitstops
    double disttoend = (laps - mCar->_lapsBehindLeader) * mTrack->length;
    double tiresstintdist = 1.0 / mMyCar->tires()->avgWearPerMeter() + 10000.0;
    int tirespitstops = int(floor(disttoend / tiresstintdist));

    // Need fuel or tires stops?
    int pitstops = std::max(fuelpitstops, tirespitstops);

    // Calc the stint fuel
    double stintfuel = fueltoend / (pitstops + 1) +2.0;

    if (pitstops)
    {
        // Stay out a little longer, if possible
        stintfuel += 0.5 * mAvgFuelPerLap;
    }

    stintfuel = Utils::clip(stintfuel, 0.0, (double)(mCar->_tank));

    // Tire strategy
    double tiresdist = mMyCar->tires()->distLeft() - 1000.0;
    double stintdist = stintfuel * (mTrack->length / mAvgFuelPerLap);

    if (tiresdist < stintdist || (mMyCar->tires()->wear() > 1.03 - ((double)mCar->_remainingLaps / 10)))
    {
        mTireChange = false;
    }
    else
    {
        mTireChange = false;
    }

    // Print infos
    //std::cout << "Fuel pitstops " << fuelpitstops << std::endl;
    //std::cout << "Fuel per meter " << mAvgFuelPerLap / mTrack->length << std::endl;
    //std::cout << "Tire pitstops " << tirespitstops << std::endl;
    //std::cout << "Tire wear per meter " << mMyCar->tires()->avgWearPerMeter() << std::endl;

    return stintfuel - mCar->_fuel;
}

int Pit::calcRepair() const
{
    if ((mCar->_remainingLaps - mCar->_lapsBehindLeader) * mTrack->length < mMaxDamageDist)
    {
        return (int)(0.5 * mCar->_dammage);
    }

    return mCar->_dammage;
}

double Pit::dist()
{
    if (mPitstop)
    {
        float dl, dw;
        RtDistToPit(mCar, (tTrack*)mTrack, &dl, &dw);

        return dl;
    }

    return DBL_MAX;
}

// Callback from driver
void Pit::pitCommand()
{
    if (mPenalty == RM_PENALTY_STOPANDGO)
    {
        // No repairs or fuel as we're serving a penalty
        mCar->pitcmd.stopType = RM_PIT_STOPANDGO;
    }
    else
    {
        mCar->_pitRepair = calcRepair();
        mLastPitFuel = calcRefuel();
        mCar->_pitFuel = (tdble) mLastPitFuel;

        if (mTireChange)
        {
            mCar->pitcmd.tireChange = (tCarPitCmd::TireChange) 1;
        }
        else
        {
            mCar->pitcmd.tireChange = (tCarPitCmd::TireChange) 0;
        }

        mCar->pitcmd.stopType = RM_PIT_REPAIR;
    }

    setPitstop(false);
}
