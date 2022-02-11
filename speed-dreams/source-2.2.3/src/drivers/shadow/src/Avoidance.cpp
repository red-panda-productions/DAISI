/***************************************************************************

    file        : Avoidance.cpp
    created     : (C) 2006
    copyright   : (C) 2017 Tim Foden

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "Avoidance.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Avoidance::Avoidance()
{
}

Avoidance::~Avoidance()
{
}

Vec2d	Avoidance::calcTarget( const Info& ai, const CarElt* pCar, const Driver& me )
{
    Vec2d	target(0, 0);

    if( ai.avoidToSide )
    {
        int	avoid = ai.avoidToSide;
        target.x = (avoid & Opponent::F_LEFT) ? 1 : -1;
        target.y = 1;

        if( avoid == (Opponent::F_LEFT | Opponent::F_RIGHT) )
        {
            // cars on both sides... aim down the middle.
            //	offs is a fair estimate of where this is.
            double	offs = (ai.minRSideDist - ai.minLSideDist) * 0.5 -
                                pCar->_trkPos.toMiddle;
            target = me.CalcPathTarget2(pCar->_distFromStartLine, offs);
        }
        else if( ai.avoidAhead )
        {
            // try to choose path that has potential also of overtaking
            //	the car ahead.
            if( ai.avoidAhead == Opponent::F_LEFT &&
                avoid == Opponent::F_RIGHT )
            {
                double	offs = -ai.minLDist - pCar->_trkPos.toMiddle + 0.5;
                target = me.CalcPathTarget2(pCar->_distFromStartLine, offs);
            }
            else if( ai.avoidAhead == Opponent::F_RIGHT &&
                     avoid == Opponent::F_LEFT )
            {
                double	offs = ai.minRDist - pCar->_trkPos.toMiddle - 0.5;
                target = me.CalcPathTarget2(pCar->_distFromStartLine, offs);
            }
        }

//		double	toL, toR;
//		me.GetPathToLeftAndRight(pCar, toL, toR);
//
//		if( toL > ai.minLSideDist )
//			toL = ai.minLSideDist;
//		if( toR > ai.minRSideDist )
//			toR = ai.minRSideDist;
//
//		double	offs = (toR - toL) * 0.5 - pCar->_trkPos.toMiddle;
//		target = me.CalcPathTarget(pCar->_distFromStartLine, offs);
    }
    else if( ai.avoidLapping )
    {
//		DEBUGF( "****** AVOID LAPPING ***** %d\n", avoidLapping );
        int	avoid = ai.avoidLapping;
        if( avoid == (Opponent::F_LEFT | Opponent::F_RIGHT) )
        {
            // lapping cars on both sides behind... avoid inside of bend.
            avoid = (ai.k < 0) ? Opponent::F_LEFT : Opponent::F_RIGHT;
        }
        target.x = (avoid & Opponent::F_LEFT) ? 1 : -1;
        target.y = 1;
    }
    else if( ai.avoidAhead == (Opponent::F_LEFT | Opponent::F_RIGHT) )
    {
        // cars ahead on both sides ahead... avoid closest (or slowest) car...
        int	avoid = ai.minLDist < ai.minRDist ? Opponent::F_LEFT : Opponent::F_RIGHT;
        target.x = (avoid & Opponent::F_LEFT) ? 1 : -1;
        target.y = 1;
    }
    else if( ai.avoidAhead )
    {
        target.x = (ai.avoidAhead & Opponent::F_LEFT) ? 1 : -1;
        target.y = 1;
    }

    return target;
}
