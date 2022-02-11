/***************************************************************************

    file        : PathRecord.h
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

// PathRecord.h: interface for the PathRecord class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _PATHRECORD_H_
#define _PATHRECORD_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>

#include "MyTrack.h"

class PathRecord
{
public:
    struct Stats
    {
        double		sum_x;
        double		sum_x2;
        double		avg_x;
        int			n;

        Stats() : sum_x(0), sum_x2(0), avg_x(0), n(0) {}
        void add( double x ) { sum_x += x; sum_x2 += x * x; n++; avg_x = sum_x / n; }
        double var() const { return (sum_x2 - 2 * avg_x * sum_x + avg_x) / n; }
        double dev() const { return sqrt(var()); }
//		double conf( double x ) { return 1 - erf(fabs(x - avg_x) / (dev() * 1.41421));	}
    };

    class Rec
    {
    public:
        const Seg*	pSeg;	// MyTrack segment this info applies to.
        double		avgW;	// moving average position
        Stats		statW;	// stats about position.
        double		avgV;	// moving average speed
        Stats		statV;	// stats about speed.
    };

public:
    PathRecord();
    ~PathRecord();

    void			Initialise( MyTrack* pTrack, CarElt* pCar );
    void			Update();

    tCarElt*		GetCar();
    const tCarElt*	GetCar() const;
    MyTrack*		GetTrack();
    void			GetPredictionForPos( double pos, double& w, double& v ) const;
    void			GetPrediction( double& w, double& v ) const;
    double			CalcConfidence( double w, double v ) const;
    double			CalcConfidence() const;

    const Rec&		GetAt( int index ) const;

public:
    MyTrack*		m_pTrack;
    tCarElt*		m_pCar;

    Rec*			m_pData;
    int				m_lastSeg;
    Vec2d			m_lastPt;
    double			m_lastSpd;
};

#endif // _PATHRECORD_H_
