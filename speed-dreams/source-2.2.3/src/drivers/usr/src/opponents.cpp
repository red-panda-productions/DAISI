/***************************************************************************

    file                 : opponents.cpp
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

#include "opponents.h"
#include "Vec2d.h"
#include "Utils.h"

Opponents::Opponents()
{
}

void Opponents::init( tTrack* track,  const tSituation* situation,  MyCar* mycar,  Path* mypath)
{
    mOpp.clear();
    int i, j = 0;
    for (i = 0; i < situation->_ncars; i++)
    {
        if (situation->cars[i] != mycar->car())
        {
            mOpp.push_back(Opponent(track, situation->cars[i], mycar, mypath));
            j++;
        }
    }
}

void Opponents::update()
{
    mOppNear = NULL;
    double mindist = 1000.0;
    double minside = 1000.0;
    mOppLetPass = NULL;
    double minletpass = -1000.0;
    mOppBack = NULL;
    double minback = -1000.0;
    mOppComingFastBehind = false;

    for (int i = 0; i < (int)mOpp.size(); i++)
    {
        // Update opponent
        mOpp[i].update();

        if (!mOpp[i].racing())
        {
            continue;
        }

        // Is opponent coming fast from behind
        if (mOpp[i].dist() > -100.0 && mOpp[i].dist() < 0.0)
        {
            if (mOpp[i].fastBehind())
            {
                mOppComingFastBehind = true;
            }
        }

        // Get nearest opponent
        double dist = mOpp[i].dist();
        double sidedist = mOpp[i].sideDist();

        if (mOpp[i].aside())
        {
            if (fabs(sidedist) < fabs(minside))
            {
                minside = sidedist;
                mindist = 0.0;
                mOppNear = &mOpp[i];
            }
        }
        else if (dist > -2.0 && fabs(dist) < fabs(mindist) && fabs(sidedist) < 15.0)
        {
            mindist = dist;
            mOppNear = &mOpp[i];
        }

        // Get opponent to let pass
        if (mOpp[i].letPass()) {
            if (dist <= 0.0 && dist > minletpass)
            {
                minletpass = dist;
                mOppLetPass = &mOpp[i];
            }
        }

        // Get nearest opponent behind
        if (dist < 0.0 && dist > minback)
        {
            minback = dist;
            mOppBack = &mOpp[i];
        }
    }

    mMateFrontAside = false;

    if (mOppNear != NULL)
    {
        if (mOppNear->aside() && mOppNear->teamMate() && !mOppNear->behind() && !mOppNear->backMarker() && mOppNear->borderDist() > -3.0) {
            mMateFrontAside = true;
        }
    }
}

Opponent* Opponents::opp(int idx)
{
    return &mOpp[idx];
}
