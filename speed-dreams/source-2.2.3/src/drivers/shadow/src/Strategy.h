/***************************************************************************

    file        : Strategy.h
    created     : 3 Jan 2020
    copyright   : (C) 2020 Xavier Bertaux

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// PitControl.h: interface for the PitControl class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _STRATEGY_H_
#define _STRATEGY_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyTrack.h"
#include "PitPath.h"
#include "TeamInfo.h"

class Strategy
{
public:
    enum
    {
        PT_NORMAL,
        PT_DRIVE_THROUGH,
    };

public:
    Strategy( const MyTrack& track, const PitPath& pitPath );
    ~Strategy();

    void	SetDamageLimits( int warnDamage, int dangerDamage, bool tyc );
    void    SetTyreLimits( double warnTyre, double dangerTyre);
    void	Process( CarElt* pCar, TeamInfo::Item* pMyInfo );

    bool	WantToPit() const;
    int		PitType() const;		// type of pit requested.

private:
    enum
    {
        PIT_NONE,
        PIT_ENABLED,
        PIT_ENTER,
        PIT_ASKED,
        PIT_EXIT,
    };

private:
    const MyTrack&	m_track;
    const PitPath&	m_pitPath;

    double          m_warnTyreLimit;
    double          m_dangerTyreLimit;

    bool            m_HasTYC;

    int				m_state;

    double			m_lastTyreWear;
    double			m_totalTyreWear;
    int				m_lastLap;
    int				m_totalLaps;
    int				m_pitType;
};

#endif // _STRATEGY_H_
