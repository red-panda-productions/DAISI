/***************************************************************************

    file                 : opponent.cpp
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

#include "opponent.h"
#include "Vec2.h"
#include "Vec2d.h"
#include "Utils.h"

#include <portability.h>

Opponent::Opponent(tTrack* track, CarElt* car, MyCar* mycar, Path* mypath)
    : mLeftOfMe(false),
      mOppCar(car),
      mCar(mycar->car()),
      mTrack(track),
      mMyPath(mypath),
      mTeamMate(!strncmp(mOppCar->_teamname, mCar->_teamname, 20)),
      mCarsDim(mOppCar->_dimension_x / 2.0 + mCar->_dimension_x / 2.0),
      mFrontRange(200.0),
      mBackRange(100.0)
{
}

void Opponent::update()
{
    // Init state
    mBackMarker = false;
    mLetpass    = false;
    mDamaged    = false;
    mRacing     = true;

    // Check for cars out
    if (mOppCar->_state & RM_CAR_STATE_NO_SIMU || mOppCar->_state & RM_CAR_STATE_PIT)
    {
        mRacing = false;

        return;
    }

    updateBasics();
    updateDist();

    // Is opponent in relevant range
    if (mDist > -mBackRange && mDist < mFrontRange)
    {
        updateSpeed();
        // Get tire temperature
        double tiretempdiff = mCar->priv.wheel[0].condition - mOppCar->priv.wheel[0].condition;

        // Detect backmarkers
        if ((mOppCar->_distRaced + mTrack->length / 2.0 < mCar->_distRaced)
                || (mTeamMate && (mOppCar->_dammage > mCar->_dammage + 1000 || tiretempdiff > 20.0)))
        {
            mBackMarker = true;
        }

        // Let opponent pass
        if ((mOppCar->_distRaced - mTrack->length / 2.0 > mCar->_distRaced  && tiretempdiff < 25.0)
                || (mTeamMate && (mOppCar->_dammage < mCar->_dammage - 1000 || tiretempdiff < -20.0) && !mBackMarker))
        {
            mLetpass = true;
        }

        // Damaged opponent?
        if (mOppCar->_dammage > mCar->_dammage + 2000)
        {
            mDamaged = true;
        }

        // Update special data
        mDistToStraight = calcDistToStraight();
        mBehind = calcBehind();
        mInDrivingDirection = calcInDrivingDirection();
        mCatchTime = calcCatchTime();
        mFastBehind = calcFastBehind();
        mLeftOfMe = Utils::hysteresis(mLeftOfMe, toMiddle() - mCar->_trkPos.toMiddle, 0.3);
    }
}

void Opponent::updateBasics()
{
    mAngle = calcAngle();
    mToMiddle = mOppCar->_trkPos.toMiddle;
    mSideDist = mToMiddle - mCar->_trkPos.toMiddle;
    mBorderDist = mOppCar->_trkPos.seg->width / 2.0 - fabs(mToMiddle);
}

void Opponent::updateSpeed()
{
    mSpeed = speedInYawDir(mMyPath->yaw(mOppCar->_distFromStartLine));

    if (fabs(mDist) < 20.0)
    {
        if (fabs(mAngle) > 0.5)
        {
            mSpeed = speedInYawDir(mCar->_yaw);
        }
    }
}

double Opponent::speedInYawDir(double yaw) const
{
    Vec2d speed(mOppCar->_speed_X, mOppCar->_speed_Y);
    Vec2d dir(cos(yaw), sin(yaw));

    return speed * dir;
}

void Opponent::updateDist()
{
    // Distance on ideal race path
    mDist = mMyPath->distOnPath(mCar->_distFromStartLine, mOppCar->_distFromStartLine);

    if (fabs(mDist) < 30.0)
    {
        // More precise when near
        double fraction = std::max(0.0, (fabs(mDist) - 15.0) / 15.0);
        double dX = mOppCar->_pos_X - mCar->_pos_X;
        double dY = mOppCar->_pos_Y - mCar->_pos_Y;
        mDist = fraction * mDist + (1.0 - fraction) * sqrt(dX * dX + dY * dY - mSideDist * mSideDist) * copysign(1.0, mDist);

        // If not certainly aside display a minimal dist
        if (fabs(mDist) < mCarsDim && fabs(mSideDist) < 0.9 * mOppCar->_dimension_y)
        {
            mDist = (mCarsDim + 0.001) * copysign(1.0, mDist);
        }
    }

    mAside = false;
    if (mDist >= mCarsDim)
    {
        // Opponent in front
        mDist -= mCarsDim;
    }
    else if (mDist <= -mCarsDim)
    {
        // Opponent behind
        mDist += mCarsDim;
    }
    else
    {
        // Opponent aside
        if (mCar->_speed_x < 8.0)
        {
            // Precise at slow speed
            mDist = cornerDist();
        }
        else
        {
            mDist = 0.0;
        }
    }

    if (mDist == 0.0)
    {
        mAside = true;
    }
}

double Opponent::cornerDist() const
{
    Vec2d frontlft(mCar->_corner_x(FRNT_LFT), mCar->_corner_y(FRNT_LFT));
    Vec2d frontrgt(mCar->_corner_x(FRNT_RGT), mCar->_corner_y(FRNT_RGT));
    Vec2d rearlft(mCar->_corner_x(REAR_LFT), mCar->_corner_y(REAR_LFT));
    Vec2d rearrgt(mCar->_corner_x(REAR_RGT), mCar->_corner_y(REAR_RGT));

    double mindist = 1000.0;
    bool left[4];
    bool right[4];

    for (int i = 0; i < 4; i++)
    {
        Vec2d corner(mOppCar->_corner_x(i), mOppCar->_corner_y(i));
        double frontdist = Utils::distPtFromLine(frontlft, frontrgt, corner);
        double reardist = Utils::distPtFromLine(rearlft, rearrgt, corner);
        double leftdist = Utils::distPtFromLine(frontlft, rearlft, corner);
        double rightdist = Utils::distPtFromLine(frontrgt, rearrgt, corner);

        bool front = frontdist < reardist && reardist > mCar->_dimension_x ? true : false;
        bool rear = reardist < frontdist && frontdist > mCar->_dimension_x ? true : false;
        left[i] = leftdist < rightdist && rightdist > mCar->_dimension_y ? true : false;
        right[i] = rightdist < leftdist && leftdist > mCar->_dimension_y ? true : false;

        double dist = 1000.0;
        if (front)
        {
            dist = frontdist;
        }
        else if (rear)
        {
            dist = -reardist;
        }

        if (fabs(dist) < fabs(mindist))
        {
            mindist = dist;
        }
    }

    if (fabs(mindist) > 3.0)
    {
        mindist -= copysign(1.0, mindist) * 2.99;
    }
    else
    {
        mindist = copysign(1.0, mindist) * 0.01;
    }

    bool lft = true;
    bool rgt = true;

    for (int j = 0; j < 4; j++)
    {
        if (!left[j])
        {
            lft = false;
        }
    }

    for (int k = 0; k < 4; k++)
    {
        if (!right[k])
        {
            rgt = false;
        }
    }

    if (lft || rgt)
    {
        // Opponent aside
        mindist = 0.0;
    }

    return mindist;
}

double Opponent::calcDistToStraight() const
{
    Vec2d opppos(mOppCar->_pos_X, mOppCar->_pos_Y);
    Vec2d mypos(mCar->_pos_X, mCar->_pos_Y);
    Vec2d mydir(cos(mCar->_yaw), sin(mCar->_yaw));
    return Utils::distPtFromLine(mypos, mypos + mydir, opppos);
}

bool Opponent::calcBehind() const
{
    Vec2d opppos(mOppCar->_pos_X - mCar->_pos_X, mOppCar->_pos_Y - mCar->_pos_Y);
    double alpha = Utils::normPiPi(mCar->_yaw - opppos.angle());

    if (fabs(alpha) > PI / 2.0)
    {
        return true;
    }

    return false;
}

double Opponent::calcAngle() const
{
    return Utils::normPiPi(mOppCar->_yaw - mCar->_yaw);
}

bool Opponent::calcInDrivingDirection() const
{
    if ((mBehind && mCar->_speed_x < -0.01) || (!mBehind && mCar->_speed_x > 0.01))
    {
        return true;
    }

    return false;
}

double Opponent::calcCatchTime() const
{
    double diffspeed = mCar->_speed_x - mSpeed;

    if (fabs(diffspeed) < 0.01)
    {
        diffspeed = 0.01;
    }

    double catchtime = mDist / diffspeed;

    if (catchtime < 0.0 || catchtime > 1000.0)
    {
        catchtime = 1000.0;
    }
    return catchtime;
}

bool Opponent::calcFastBehind() const
{
    if (mDist > -1.0 || mSpeed < 20.0)
    {
        return false;
    }

    if (catchTime() < 1.0)
    {
        return true;
    }

    return false;
}
