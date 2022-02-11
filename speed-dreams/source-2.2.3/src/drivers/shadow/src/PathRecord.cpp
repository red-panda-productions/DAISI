/***************************************************************************

    file        : PathRecord.cpp
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

// PathRecord.cpp: implementation of the PathRecord class.
//
//////////////////////////////////////////////////////////////////////



#include <robottools.h>

#include "Utils.h"
#include "PathRecord.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PathRecord::PathRecord()
:	m_pTrack(0),
    m_pCar(0),
    m_pData(0),
    m_lastSeg(0),
    m_lastPt(0, 0),
    m_lastSpd(0)
{
}

PathRecord::~PathRecord()
{
    delete [] m_pData;
}

void	PathRecord::Initialise( MyTrack* pTrack, tCarElt* pCar )
{
//	DEBUGF( "pr::init\n" );
    const int	NSEG = pTrack->GetSize();

    m_pTrack = pTrack;
    m_pCar   = pCar;

    delete [] m_pData;
    m_pData = new Rec[NSEG];

    for( int i = 0; i < NSEG; i++ )
    {
        m_pData[i].pSeg = &pTrack->GetAt(i);
        m_pData[i].avgV = 20;
        m_pData[i].avgW = pCar->_trkPos.toMiddle;
    }

    double	pos = RtGetDistFromStart(pCar);
    m_lastSeg = pTrack->IndexFromPos(pos);
    m_lastPt.x = pCar->pub.DynGCg.pos.x;
    m_lastPt.y = pCar->pub.DynGCg.pos.y;
    m_lastSpd = hypot(pCar->_speed_X, pCar->_speed_Y);
//	DEBUGF( "pr::init done\n" );
}

void	PathRecord::Update()
{
    const int	NSEG = m_pTrack->GetSize();

    // work out which slice the car is in.
    int		last_s = m_lastSeg;
    double	dist = RtGetDistFromStart(m_pCar);
    int		cur_s = m_pTrack->IndexFromPos(dist);

    // work out current speed and position.
    double	spd = hypot(m_pCar->_speed_X, m_pCar->_speed_Y);
    Vec2d	pt(m_pCar->pub.DynGCg.pos.x, m_pCar->pub.DynGCg.pos.y);

    if( //!m_pCar->On_pit_lane &&
        last_s >= 0 && last_s != cur_s )
    {
        // we have crossed at least one line boundary, so we need to calculate
        //	the crossing point(s), and the speed(s) at those points.

        int		next_s = (last_s + 1) % NSEG;
        while( last_s != cur_s )
        {
            const Seg&	s0 = m_pTrack->GetAt(next_s);
            double		t, w;
            if( Utils::LineCrossesLine(m_lastPt, pt - m_lastPt,
                                        s0.pt.GetXY(), s0.norm.GetXY(), t, w) &&
                t >= 0.0 && t <= 1.0 )
            {
                double	v = m_lastSpd + (spd - m_lastSpd) * t;

                Rec&	rec = m_pData[next_s];
                const double	gamma = 0.8;
                rec.avgW	= rec.avgW * (1 - gamma) + w * gamma;
                rec.avgV	= rec.avgV * (1 - gamma) + v * gamma;
                rec.statW.add( w );
                rec.statV.add( v );
            }

            last_s = next_s;
            next_s = (next_s + 1) % NSEG;
        }
    }

    m_lastSeg = cur_s;
    m_lastPt  = pt;
    m_lastSpd = spd;
}

tCarElt*	PathRecord::GetCar()
{
    return m_pCar;
}

const tCarElt*	PathRecord::GetCar() const
{
    return m_pCar;
}

MyTrack*	PathRecord::GetTrack()
{
    return m_pTrack;
}

void	PathRecord::GetPredictionForPos( double pos, double& w, double& v ) const
{
    int		i = m_pTrack->IndexFromPos(pos);
    int		j = (i + 1) % m_pTrack->GetSize();

    const Seg&	s0 = m_pTrack->GetAt(i);
    const Seg&	s1 = m_pTrack->GetAt(j);
    double	t = (pos - s0.segDist) / (s1.segDist - s0.segDist);

    w = m_pData[i].avgW + (m_pData[j].avgW - m_pData[i].avgW) * t;
    v = m_pData[i].avgV + (m_pData[j].avgV - m_pData[i].avgV) * t;
}

void	PathRecord::GetPrediction( double& w, double& v ) const
{
    double	pos = RtGetDistFromStart(m_pCar);
    GetPredictionForPos( pos, w, v );
}

double	PathRecord::CalcConfidence( double w, double v ) const
{
    // calculate the confidence that the car is to the position (w) and
    //	speed (v) passed in.
    double	spd = hypot(m_pCar->_speed_X, m_pCar->_speed_Y);
    double	tw = 1 - fabs(tanh(w + m_pCar->_trkPos.toMiddle));
    double	tv = 1 - fabs(tanh(v - spd));

    return tw * tv;
}

double	PathRecord::CalcConfidence() const
{
    double	w, v;
    GetPrediction( w, v );
    return CalcConfidence(w, v);
}

const PathRecord::Rec&	PathRecord::GetAt( int index ) const
{
    return m_pData[index];
}
