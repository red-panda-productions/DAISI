/***************************************************************************

    file        : Opponent.h
    created     : 18 Apr 2017
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

// Opponent.h: interface for the Opponent class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _OPPONENT_H_
#define _OPPONENT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PathRecord.h"
#include "TeamInfo.h"
#include "Utils.h"

#include <car.h>
#include <raceman.h>

class Driver;

class Opponent
{
public:
    enum	// flags
    {
        F_LEFT			= 0x000001,
        F_RIGHT			= 0x000002,
        F_FRONT			= 0x000004,
        F_REAR			= 0x000008,

        F_AHEAD			= 0x000010,
        F_TO_SIDE		= 0x000020,
        F_BEHIND		= 0x000040,

        F_TRK_LEFT		= 0x000100,
        F_TRK_RIGHT		= 0x000200,

        F_CATCHING		= 0x001000,
        F_CATCHING_ACC	= 0x002000,
        F_COLLIDE		= 0x004000,
        F_TRAFFIC		= 0x008000,
        F_CLOSE			= 0x010000,
        F_TEAMMATE		= 0x020000,
        F_LAPPER		= 0x040000,	// it's lapping us.
        F_BEING_LAPPED	= 0x080000,	// we're lapping it.
        F_DANGEROUS		= 0x100000,
    };

    struct Sit
    {
        Sit()
        :   spd(0.0),
            stuckTime(0.0),
            tVX(0.0),
            tVY(0.0),
            tYaw(0.0),
            rdPX(0.0),
            rdPY(0.0),
            rdVX(0.0),
            rdVY(0.0),
            agVX(0.0),
            agVY(0.0),
            ragVX(0.0),
            ragVY(0.0),
            arAX(0.0),
            arAY(0.0),
            agAX(0.0),
            agAY(0.0),
            rAX(0.0),
            rAY(0.0),
            ragAX(0.0),
            ragAY(0.0),
            minDXa(0.0),
            minDXb(0.0),
            minDY(0.0),
            decDX(0.0),
            accDX(0.0),
            relPos(0.0),
            offs(0.0)
        {
        }

        // t = local track relative.
        // r = relative to velocity frame of my car.
        // g = global torcs coordinate frame.
        // d = delta or difference.
        // a = average.

        double	spd;
        double	stuckTime;

        double	tVX;	// along track.
        double	tVY;	// normal to track.
        double	tYaw;

        double	rdPX;
        double	rdPY;
        double	rdVX;
        double	rdVY;

        double	agVX;
        double	agVY;
        double	ragVX;
        double	ragVY;

        double	arAX;
        double	arAY;
        double	agAX;
        double	agAY;
        double	rAX;
        double	rAY;
        double	ragAX;
        double	ragAY;

        double	minDXa;
        double	minDXb;
        double	minDY;
        double	decDX;
        double	accDX;

        double	relPos;
        double	offs;

        PtInfo	pi;	// from my path.

        static int compareRelPos( const Sit* a, const Sit* b ) { return SGN(a->relPos - b->relPos); }
    };

    struct PassInfo
    {
        PassInfo()
        :   isSpace(false),
            offset(0.0),
            mySpeed(0.0),
            goodPath(false),
            bestU(0.0),
            bestV(0.0),
            myOffset(0.0)
        {
        }

        bool	isSpace;
        double	offset;
        double	mySpeed;
        bool	goodPath;
        double	bestU;
        double	bestV;
        double	myOffset;
    };

    struct Info
    {
        Info()
        :   flags(0),
            avoidLatchTime(0.0),
            dangerousLatchTime(0.0),
            closeAheadTime(0.0),
            closeBehindTime(0.0),
            catchTime(0.0),
            catchY(0.0),
            catchSpd(0.0),
            catchDecel(0.0),
            catchAccTime(0.0),
            catchAccY(0.0),
            catchAccSpd(0.0),
            newCatching(false),
            newCatchSpd(0.0),
            newCatchTime(0.0),
            newAheadTime(0.0),
            newMidPos(0.0),
            newBestOffset(0.0),
            tmDamage(0.0)
        {
        }

        bool		GotFlags( int f ) const	{ return (flags & f) == f; }

        int			flags;		// flags from above (cOPP_XXX).

        Sit			sit;

        double		avoidLatchTime;
        double		dangerousLatchTime;

        double		closeAheadTime;
        double		closeBehindTime;

        double		catchTime;
        double		catchY;
        double		catchSpd;
        double		catchDecel;

        double		catchAccTime;
        double		catchAccY;
        double		catchAccSpd;

        bool		newCatching;
        double		newCatchSpd;
        double		newCatchTime;
        double		newAheadTime;
        double		newMidPos;
        double		newBestOffset;
        PassInfo	newPiL;
        PassInfo	newPiR;

        double		tmDamage;
    };

public:
    Opponent();
    ~Opponent();

    void		Initialise( MyTrack* pTrack, CarElt* pCar );

    CarElt*			GetCar();
    const CarElt*	GetCar() const;
    const Info&		GetInfo() const;
    Info&			GetInfo();

    void			UpdatePath();
//	void			ProcessMyCar( const Situation* s, const TeamInfo* pTeamInfo,
//								  const CarElt* myCar, double myDirX, double myDirY,
//								  int idx );
    void			UpdateSit( const CarElt* myCar, const Situation* s, const TeamInfo* pTeamInfo,
                               double myDirX, double myDirY, const PtInfo& oppPi );
    void			ProcessMyCar(const Situation* s, const TeamInfo* pTeamInfo,
                                  const CarElt* myCar, const Sit& mySit,
                                  const Driver &me, double myMaxAccX, int idx );

    static bool compareSitRelPos( const Opponent* a, const Opponent* b ) { return a->m_info.sit.relPos < b->m_info.sit.relPos; }

private:
    Info			m_info;
    PathRecord		m_path;		// info about path of this opponent.
};

#endif // _OPPONENT_H_
