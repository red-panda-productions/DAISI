/***************************************************************************

    file                 : pidcontroller.cpp
    created              : Apr 09 01:21:49 UTC 2006
    copyright            : (C) 2006 Tim Foden, 2018 Daniel Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "pidcontroller.h"

#include <algorithm>

PidController::PidController() :
    mPrevError(0),
    mAccum(0),
    mMaxAccum(100),
    mP(1),
    mD(0),
    mI(0)
{
}

double PidController::sample(double error, double dt)
{
    double deriv = (error - mPrevError) / dt;
    mPrevError = error;

    if (mI != 0)
    {
        mAccum += error * dt;
        mAccum = std::max(-mMaxAccum, std::min(mAccum, mMaxAccum));
    }

    return mP * error + mD * deriv + mI * mAccum;
}
