/***************************************************************************

    file        : PitPath.cpp
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

// PitPath.cpp: implementation of the PitPath class.
//
//////////////////////////////////////////////////////////////////////

#include "PitPath.h"

#include "Utils.h"
#include "CubicSpline.h"
#include "ParametricCubicSpline.h"
#include "Strategy.h"

#include <robottools.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PitPath::PitPath()
{
}

PitPath::~PitPath()
{
}

PitPath& PitPath::operator=( const PitPath& other )
{
    Path::operator=( other );

    m_pitEntryPos	= other.m_pitEntryPos;
    m_pitExitPos	= other.m_pitExitPos;
    m_pitStartPos	= other.m_pitStartPos;
    m_pitEndPos		= other.m_pitEndPos;
    m_stopIdx		= other.m_stopIdx;
    m_stopPos		= other.m_stopPos;

    return *this;
}

PitPath& PitPath::operator=( const Path& other )
{
    const PitPath* pPitPath = dynamic_cast<const PitPath*>(&other);
    if( pPitPath )
    {
        return PitPath::operator=(*pPitPath);
    }
    else
    {
        Path::operator=(other);
        m_pitEntryPos	= 0;
        m_pitExitPos	= 0;
        m_pitStartPos	= 0;
        m_pitEndPos		= 0;
        m_stopIdx		= 0;
        m_stopPos		= 0;
        return *this;
    }
}

void	PitPath::MakePath(
    const tTrackOwnPit*	pPit,
    Path*			pBasePath,
    const CarModel&	cm,
    int				pitType,
    double			entryOffset,
    double			exitOffset )
{
    operator=( *pBasePath );

    if( pPit == NULL )
        return;

    const tTrackPitInfo*	pPitInfo = &m_pTrack->GetTrack()->pits;

    const int	NPOINTS = 7;
    double	x[NPOINTS];
    double	y[NPOINTS];
    double	s[NPOINTS];

    // Compute pit spline points along the track.
    x[3] = pPit->pos.seg->lgfromstart + pPit->pos.toStart;
    x[2] = x[3] - pPitInfo->len;
    x[4] = x[3] + pPitInfo->len;
    x[0] = pPitInfo->pitEntry->lgfromstart + entryOffset;
    x[1] = pPitInfo->pitStart->lgfromstart;
//	x[5] = x[3] + (pPitInfo->nMaxPits - driverIndex) * pPitInfo->len;
    x[5] = pPitInfo->pitStart->lgfromstart + (pPitInfo->nMaxPits) * pPitInfo->len;
    x[6] = pPitInfo->pitExit->lgfromstart + pPitInfo->pitExit->length + exitOffset;

    m_pitEntryPos = x[0];
    m_pitStartPos = x[1];
    m_pitEndPos   = x[5];
    m_pitExitPos  = x[6];

    // Normalizing spline segments to >= 0.0.
    {for( int i = 0; i < NPOINTS; i++ )
    {
        x[i] = ToSplinePos(x[i]);
        s[i] = 0.0;
    }}

    // Fix broken pit exit.
    if( x[6] < x[5] )
    {
        x[6] = x[5] + 50.0;
        m_pitExitPos  = x[6];
    }

    // Fix point for first pit if necessary.
    if( x[1] > x[2] )
        x[1] = x[2];

    // Fix point for last pit if necessary.
    if( x[5] < x[4] )
        x[5] = x[4];

    // splice entry/exit of pit path into the base path provided.
    PtInfo	pi;
    pBasePath->GetPtInfo(m_pitEntryPos, pi);
    y[0] = pi.offs;
    s[0] = -tan(pi.oang - m_pTrack->CalcForwardAngle(m_pitEntryPos));

    pBasePath->GetPtInfo(m_pitExitPos, pi);
    y[6] = pi.offs;
    s[6] = -tan(pi.oang - m_pTrack->CalcForwardAngle(m_pitExitPos));

    double sign = (pPitInfo->side == TR_LFT) ? -1.0 : 1.0;
    {for( int i = 1; i < NPOINTS - 1; i++ )
    {
        y[i] = fabs(pPitInfo->driversPits->pos.toMiddle) - pPitInfo->width;
        y[i] *= sign;
    }}

    if( pitType == Strategy::PT_NORMAL )
        y[3] = (fabs(pPitInfo->driversPits->pos.toMiddle) + 1.0) * sign;

    //CubicSpline	spline(NPOINTS, x, y, s);

    // x,y,s is in track centre relative coords... convert to global coords.
    Vec2d   gp[NPOINTS];
    Vec2d   gv[NPOINTS];
    {for( int i = 0; i < NPOINTS; i++ )
    {
        LocalToGlobalXY( x[i], y[i], s[i], &gp[i], &gv[i] );
    }}

    ParametricCubicSpline   pspline(NPOINTS, gp, gv);

    // modify points in line path for pits...
    int		idx0 = (m_pTrack->IndexFromPos(m_pitEntryPos) + 1) % NSEG;
    int		idx1 = m_pTrack->IndexFromPos(m_pitExitPos);
    {for( int i = idx0; i != idx1; i = (i + 1) % NSEG )
    {
        //double	x = ToSplinePos(m_pTrack->GetAt(i).segDist);
        //double	y = spline.CalcY(x);
        Vec2d   linePt  = GetAt(i).Pt().GetXY();
        Vec2d   lineTan = GetAt(i).Norm().GetXY();
        double  t;
        if( pspline.CalcLineCrossingPt(linePt, lineTan, &t) )
        {
            m_pts[i].offs = -t;
            m_pts[i].pt = m_pts[i].CalcPt();
        }
    }}

    CalcCurvaturesXY();
    CalcMaxSpeeds( cm );

    idx0 = (m_pTrack->IndexFromPos(m_pitStartPos) + NSEG - 8) % NSEG;
    idx1 = (m_pTrack->IndexFromPos(m_pitEndPos) + 2) % NSEG;
    double	spd = MN(m_pts[idx0].spd, pPitInfo->speedLimit - 2);
    m_pts[idx0].maxSpd = m_pts[idx0].spd = spd;
    {for( int i = idx0; i != idx1; i = (i + 1) % NSEG )
    {
        spd = MN(m_pts[i].spd, pPitInfo->speedLimit - 0.1);
        m_pts[i].maxSpd = m_pts[i].spd = spd;
    }}

    if( pitType == Strategy::PT_NORMAL )
    {
        double	stopPos = pPit->pos.seg->lgfromstart + pPit->pos.toStart + 2;
        idx0 = m_pTrack->IndexFromPos(stopPos);
        idx1 = (idx0 + 1) % NSEG;
        int idxp = (idx0 - 1 + NSEG) % NSEG;
        // int idxp2 = (idx0 - 2 + NSEG) % NSEG;
        //m_pts[idxp2].maxSpd = m_pts[idxp2].spd = 1.5f;
        m_pts[idxp].maxSpd = m_pts[idxp].spd = 3.0f;
        m_pts[idx0].maxSpd = m_pts[idx0].spd = 3.0f;
        m_pts[idx1].maxSpd = m_pts[idx1].spd = 3.0f;

        m_stopPos	= m_pTrack->NormalisePos(stopPos);
    }

    m_stopIdx = idx0;

    PropagateBraking( cm );

    idx0 = (m_pTrack->IndexFromPos(m_pitEntryPos) + 1) % NSEG;
    while( m_pts[idx0].spd < pBasePath->GetAt(idx0).spd )
        idx0 = (idx0 + NSEG - 1) % NSEG;
    m_pitEntryPos = m_pts[idx0].Dist();
}

bool	PitPath::InPitSection( double trackPos ) const
{
    trackPos = ToSplinePos(trackPos);
    double	pitExitPos = ToSplinePos(m_pitExitPos);
    return m_pitEntryPos <= trackPos && trackPos <= pitExitPos;
}

bool	PitPath::CanStop( double trackPos ) const
{
//	return m_pTrack->IndexFromPos(trackPos) == m_stopIdx;
    return m_pTrack->PosInRange(trackPos, m_stopPos, 4);
}

double	PitPath::EntryToPitDistance() const
{
//	double dist = GetAt(m_stopIdx).Dist() - m_pitEntryPos;
    double dist = m_stopPos - m_pitEntryPos;
    if( dist < 0 )
        dist += m_pTrack->GetLength();
    return dist;
}

double	PitPath::ToSplinePos( double trackPos ) const
{
    if( trackPos < m_pitEntryPos )
        trackPos += m_pTrack->GetLength();
    return trackPos;
}

void    PitPath::LocalToGlobalXY( double dist, double offs, double slope, Vec2d* p, Vec2d* v )
{
    dist = m_pTrack->NormalisePos(dist);
    int idx = m_pTrack->IndexFromPos(dist);
    const tTrackSeg* pSeg = m_pTrack->GetAt(idx).pSeg;

    double dummy;
    Vec3d trackNormal, trackPoint;
    m_pTrack->CalcPtAndNormal(pSeg, dist - pSeg->lgfromstart, dummy, trackPoint, trackNormal);
    Vec2d trackTangent = trackNormal.GetXY().GetNormal();

    *p = Vec3d(trackPoint + trackNormal * offs).GetXY();
    *v = Vec2d(trackTangent + trackNormal.GetXY() * slope).GetUnit();
}

