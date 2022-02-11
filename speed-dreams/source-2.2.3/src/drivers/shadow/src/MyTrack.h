/***************************************************************************

    file        : MyTrack.h
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

// MyTrack.h: interface for the MyTrack class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _MYTRACK_H_
#define _MYTRACK_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <track.h>
#include <car.h>

#include <vector>

#include "Seg.h"
#include "PtInfo.h"

class MyTrack
{
public:
    struct SideMod
    {
        SideMod() : side(-1), start(0), end(0) {}

        int		side;
        int		start;
        int		end;
    };

public:
    MyTrack();
    ~MyTrack();

    void	Clear();
    void	NewTrack( tTrack* pNewTrack, const std::vector<double>* pInnerMod = NULL, bool pit = false, SideMod* pSideMod = NULL,
                      int pitStartBufSegs = 0);
    void    setWidth( double width ) { m_width = width; }

    tTrack*			GetTrack();
    const tTrack*	GetTrack() const;

    double	GetLength() const;
    int		GetSize() const;
    double	GetWidth() const;

    double	NormalisePos( double trackPos ) const;
    bool	PosInRange( double pos, double rangeStart, double rangeLength ) const;
    int		IndexFromPos( double trackPos ) const;

    const Seg&	operator[]( int index ) const;
    const Seg&	GetAt( int index ) const;

    double	GetDelta() const;
    double	CalcPos( const tTrkLocPos& trkPos, double offset = 0 ) const;
    double	CalcPos( const tCarElt* car, double offset = 0 ) const;
    double	CalcPos( double x, double y, const Seg* hint = 0, bool sides = false ) const;

    double	CalcHeightAbovePoint( const Vec3d& start_point, const Vec3d& direction, const Seg* hint = 0 ) const;

    double	CalcForwardAngle( double trackPos ) const;
    Vec2d	CalcNormal( double trackPos ) const;

    double	GetFriction( int index, double offset ) const;

    void	CalcPtAndNormal( const tTrackSeg* pSeg, double toStartOfSeg,
                             double& t, Vec3d& pt, Vec3d& norm ) const;

private:
    int					NSEG;
    double				m_delta;
    Seg*				m_pSegs;
    tTrack*				m_pCurTrack;
    SideMod				m_sideMod;
    std::vector<double>	m_innerMod;
    int					m_nBends;
    double              m_width;
};
#endif // _MYTRACK_H_
