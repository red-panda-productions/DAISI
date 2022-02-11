/***************************************************************************

    file        : MyTrack.cpp
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

// MyTrack.cpp: implementation of the MyTrack class.
//
//////////////////////////////////////////////////////////////////////

#include "MyTrack.h"
#include "Utils.h"

#include <robottools.h>

using namespace std;

// The "SHADOW" logger instance.
extern GfLogger* PLogSHADOW;
#define LogSHADOW (*PLogSHADOW)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MyTrack::MyTrack()
    :	NSEG(0),
      m_delta(3),
      m_pSegs(0),
      m_pCurTrack(0),
      m_nBends(0),
      m_width(2.0)
{
}

MyTrack::~MyTrack()
{
    delete [] m_pSegs;
}

void	MyTrack::Clear()
{
    delete [] m_pSegs;
    NSEG = 0;
    m_pSegs = 0;
    m_pCurTrack = 0;
    m_innerMod.clear();
    m_nBends = 0;
    m_width = 0;
}

void	MyTrack::NewTrack( tTrack* pNewTrack, const vector<double>* pInnerMod, bool pit, SideMod* pSideMod, int pitStartBufSegs)
{
    if( m_pCurTrack != pNewTrack )
    {
        delete [] m_pSegs;
        m_pSegs = 0;
        NSEG = 0;
    }

    m_pCurTrack = pNewTrack;

    if( pInnerMod )
        m_innerMod = *pInnerMod;
    else
    {
        m_innerMod.clear();
    }

    if( pSideMod )
        m_sideMod = *pSideMod;

    if( m_pSegs == 0 )
    {
        // make new segs ... roughly every NOMINAL_SEG_LEN metres apart.
        const double	NOMINAL_SEG_LEN = 3;//10;
        NSEG = int(floor(pNewTrack->length / NOMINAL_SEG_LEN));
        m_pSegs = new Seg[NSEG];
        m_delta = pNewTrack->length / NSEG;

        LogSHADOW.debug( "   ### NSEG %d\n", NSEG );

        tTrackSeg*	pseg = pNewTrack->seg;
        while( pseg->lgfromstart > pNewTrack->length / 2 )
            pseg = pseg->next;
        double		tsend = pseg->lgfromstart + pseg->length;
        LogSHADOW.debug( "   ### tsend %g len %g fromstart %g\n",
                            tsend, pseg->length, pseg->lgfromstart );

        int	pitEntry = -1;
        int pitStart = -1;
        int pitEnd   = -1;
        int	pitExit  = -1;
        int pitSide  = pNewTrack->pits.side == TR_LFT ? TR_SIDE_LFT : TR_SIDE_RGT;

        for( int i = 0; i < NSEG; i++ )
        {
            double	segDist = i * m_delta;
            while( segDist >= tsend )
            {
                pseg = pseg->next;
                tsend = pseg->lgfromstart + pseg->length;
            }

            m_pSegs[i].segDist = segDist;
            m_pSegs[i].pSeg = pseg;
            m_pSegs[i].wl = pseg->width / m_width;
            m_pSegs[i].wr = pseg->width / m_width;
            m_pSegs[i].midOffs = 0;
            m_pSegs[i].bendId = -1;

            LogSHADOW.debug(" # .........m_width = %.2f\n", m_width);

            if( pitEntry < 0 && (pseg->raceInfo & TR_PITENTRY) )
                pitEntry = i;
            if( pitStart < 0 && (pseg == pNewTrack->pits.pitStart || (pseg->raceInfo & TR_PITSTART) != 0 ))
                pitStart = (i - 1 - pitStartBufSegs + NSEG) % NSEG;
            if( pseg == pNewTrack->pits.pitEnd || (pseg->raceInfo & TR_PITEND) != 0 )
                pitEnd  = (i + 1) % NSEG;
            if( (pseg->raceInfo & TR_PITEXIT) )
                pitExit  = i;
        }

        int		lastStart	= 0;
        double	lastK		= 0;
        int		lastSign	= 1;

        vector<int>	bends;

        for( int i = 0; i < NSEG; i++ )
        {
            const tTrackSeg* pseg = m_pSegs[i].pSeg;
            double k =  pseg->type == TR_LFT ?  1.0 / pseg->radius :
                                                pseg->type == TR_RGT ? -1.0 / pseg->radius : 0;

            if( k != lastK )
            {
                if( lastSign * lastK > 0 && lastSign * k < lastSign * lastK )
                {
                    LogSHADOW.debug("bend[%d..%d] r=%g, k=%g, s=%d\n", lastStart, i, pseg->radius, lastK, lastSign );
                    bends.push_back( (lastStart + i) / 2 );
                }

                lastStart	= i;
                lastSign	= k < lastK ? -1 : 1;
                lastK		= k;
            }
        }

        int nBends = bends.size();
        LogSHADOW.debug( "number of bends identified: %d\n", nBends );

        for( int bend = 0; bend < nBends; bend++ )
        {
            int begin	= bends[bend];
            int end		= bends[(bend + 1) % nBends];
            int len		= (end - begin + NSEG) % NSEG;
            int half	= len / 2;

            int type	= m_pSegs[begin].pSeg->type;
            int nextId  = (1 + bend * 2) % (nBends * 2);
            for( int j = 0; j < half; j++ )
            {
                int index = (begin + j + NSEG) % NSEG;
                if( type != m_pSegs[index].pSeg->type )
                {
                    LogSHADOW.debug( "[%db] end=%d\n", nextId / 2, index );
                    break;
                }
                m_pSegs[index].bendId = nextId;
            }

            type	= m_pSegs[end].pSeg->type;
            nextId  = (1 + bend * 2 + 1) % (nBends * 2);

            for( int j = 0; j < half; j++ )
            {
                int index = (end - j + NSEG) % NSEG;

                if( type != m_pSegs[index].pSeg->type )
                {
                    LogSHADOW.debug( "[%da] begin=%d\n", nextId / 2, index );
                    break;
                }

                m_pSegs[index].bendId = nextId;
            }
        }

        for( int i = 0; i < NSEG; i++ )
        {
            pseg = m_pSegs[i].pSeg;

            double	segDist = m_pSegs[i].segDist;
            double	t = (segDist - pseg->lgfromstart) / pseg->length;

            bool	inPitMain  = ((pitStart < pitEnd  && pitStart <= i && i <= pitEnd)  ||
                                  (pitStart > pitEnd  && (i <= pitEnd  || i >= pitStart)));
            bool	inPitTotal = ((pitEntry < pitExit && pitEntry <= i && i <= pitExit) ||
                                  (pitEntry > pitExit && (i <= pitExit || i >= pitEntry)));

            const double	MIN_MU = pseg->surface->kFriction * 0.9;
            const double	MAX_ROUGH = MX(0.0025, pseg->surface->kRoughness * 1.2);
            const double	MAX_RESIST = MX(0.02, pseg->surface->kRollRes * 1.2);
            const double	SLOPE = pseg->Kzw;
            const double    WALL_MARGIN = 0.5;

            for( int s = 0; s < 2; s++ )
            {
                tTrackSeg*	pSide = pseg->side[s];

                double	w_so_far = 0;
                double  extra_w = 9999;

                while( pSide )
                {
                    double	w = pSide->startWidth +
                            (pSide->endWidth - pSide->startWidth) * t;
                    float slope = pSide->height/pSide->width;
                    bool outer = ((s == TR_SIDE_LFT) && (pseg->type == TR_RGT))
                            || ((s == TR_SIDE_RGT) && (pseg->type == TR_LFT));

                    if (pSide->style == TR_CURB)
                    {
                        if( s == m_sideMod.side &&
                                i >= m_sideMod.start &&
                                i <= m_sideMod.end )
                            ;
                        else
                        {
                            extra_w = MN(extra_w, 1.2);

                            // never go up a curb on the outside of a corner that has lower friction
                            // than the main track.
                            if( ((s == TR_SIDE_LFT && pseg->type == TR_RGT) ||
                                 (s == TR_SIDE_RGT && pseg->type == TR_LFT)) &&
                                    pSide->surface->kFriction  < pseg->surface->kFriction )
                            {
                                extra_w = MN(extra_w, w_so_far);
                            }

                            // don't go up raised curbs.
                            if( pSide->height > 0 )
                            {
                                extra_w = MN(extra_w, w_so_far);
                            }
                        }
                    }
                    else if( pSide->style == TR_PLAN )
                    {
                        // don't go into the main pit area.
                        if( inPitTotal && pitSide == s )
                        {
                            extra_w = MN(extra_w, w_so_far);
                        }

                        bool	inner = (s == TR_SIDE_LFT && pseg->type == TR_LFT) ||
                                (s == TR_SIDE_RGT && pseg->type == TR_RGT);
                        bool    pitEntryOrExit = pitSide == s && inPitTotal && !inPitMain;
                        if( inner && !pitEntryOrExit )
                        {
                            // on the inside of a bend always keep a wheel on the main track.
                            extra_w = MN(extra_w, 1.5);
                        }

                        if( pSide->surface->kFriction  < MIN_MU		||  // too little grip
                                pSide->surface->kRoughness > MAX_ROUGH	||  // too rough
                                pSide->surface->kRollRes   > MAX_RESIST	||  // too high a rolling resistance
                                fabs(pSide->Kzw - SLOPE)   > 0.005 )        // too large a slope difference
                        {
                            extra_w = MN(extra_w, w_so_far);
                        }

                        if (((s == TR_SIDE_LFT && pseg->type == TR_RGT) ||
                             (s == TR_SIDE_RGT && pseg->type == TR_LFT)) &&
                                pSide->surface->kFriction < pseg->surface->kFriction)
                        {
                            extra_w = MN(extra_w, w_so_far);
                        }
                    }
                    else
                    {
                        // wall of some sort.
                        if( pSide->style == TR_WALL )
                            extra_w = MN(extra_w, w_so_far - WALL_MARGIN);
                        else
                            extra_w = MN(extra_w, w_so_far);
                    }

                    w_so_far += w;

                    pSide = pSide->side[s];
                }

                extra_w = MN(extra_w, w_so_far - WALL_MARGIN);	// there's always a wall/fence at the edge of the track.

                bool	inner = (s == TR_SIDE_LFT && pseg->type == TR_LFT) ||
                        (s == TR_SIDE_RGT && pseg->type == TR_RGT);
                if( inner )
                {
                    int innerId = m_pSegs[i].bendId;
                    if( innerId >= 0 && innerId < (int)m_innerMod.size() )
                    {
                        double innerMod = m_innerMod[innerId];
                        extra_w += innerMod;
                    }
                }

                if( s == TR_SIDE_LFT )
                    m_pSegs[i].wl += extra_w;
                else
                    m_pSegs[i].wr += extra_w;

                double extent = pseg->width / 2;
                pSide = pseg->side[s];

                while( pSide )
                {
                    if( pSide->style >= TR_WALL )
                        break;

                    extent += pSide->width;

                    pSide = pSide->side[s];
                }

                if( s == TR_SIDE_LFT )
                    m_pSegs[i].el = extent;
                else
                    m_pSegs[i].er = extent;
            }

            CalcPtAndNormal( pseg, segDist - pseg->lgfromstart,
                             m_pSegs[i].t,
                             m_pSegs[i].pt, m_pSegs[i].norm );

            LogSHADOW.debug( "%4d  p(%7.2f, %7.2f, %7.2f)  n(%7.4f, %7.4f, %7.4f)\n",
                                i, m_pSegs[i].pt.x, m_pSegs[i].pt.y, m_pSegs[i].pt.z,
                                m_pSegs[i].norm.x, m_pSegs[i].norm.y, m_pSegs[i].norm.z );
        }

        for( int i = 0; i < NSEG; i++ )
        {
            int j = (i + 1) % NSEG;

            for( int s = 0; s < 2; s++ )
            {
                double  currW = s == TR_SIDE_LFT ? m_pSegs[i].el : m_pSegs[i].er;
                double  nextW = s == TR_SIDE_LFT ? m_pSegs[j].el : m_pSegs[j].er;

                if( currW > nextW + 1 )
                {
                    if( s == TR_SIDE_LFT )
                    {
                        m_pSegs[i].el = nextW;
                    }
                    else
                    {
                        m_pSegs[i].er = nextW;
                    }
                }
            }
        }

        for( int i = NSEG - 1; i >= 0; i-- )
        {
            int j = (i + NSEG - 1) % NSEG;

            for( int s = 0; s < 2; s++ )
            {
                double  currW = s == TR_SIDE_LFT ? m_pSegs[i].el : m_pSegs[i].er;
                double  prevW = s == TR_SIDE_LFT ? m_pSegs[j].el : m_pSegs[j].er;

                if( currW > prevW + 1 )
                {
                    if( s == TR_SIDE_LFT )
                    {
                        m_pSegs[i].el = prevW;
                    }
                    else
                    {
                        m_pSegs[i].er = prevW;
                    }
                }
            }
        }
    }
}

tTrack*	MyTrack::GetTrack()
{
    return m_pCurTrack;
}

const tTrack*	MyTrack::GetTrack() const
{
    return m_pCurTrack;
}

double	MyTrack::GetLength() const
{
    return m_pCurTrack->length;
}

int		MyTrack::GetSize() const
{
    return NSEG;
}

double	MyTrack::GetWidth() const
{
    return m_pCurTrack->width;
}

double	MyTrack::NormalisePos( double trackPos ) const
{
    while( trackPos < 0 )
        trackPos += m_pCurTrack->length;

    while( trackPos >= m_pCurTrack->length )
        trackPos -= m_pCurTrack->length;

    return trackPos;
}

bool	MyTrack::PosInRange( double pos, double rangeStart, double rangeLength ) const
{
    double delta = NormalisePos(pos - rangeStart);
    return delta < rangeLength;
}

int		MyTrack::IndexFromPos( double trackPos ) const
{
    int	idx = (int(floor(trackPos / m_delta)) + NSEG) % NSEG;

    return idx;
}

const Seg&	MyTrack::operator[]( int index ) const
{
    return m_pSegs[index];
}

const Seg&	MyTrack::GetAt( int index ) const
{
    return m_pSegs[index];
}

double	MyTrack::GetDelta() const
{
    return m_delta;
}

double	MyTrack::CalcPos( const tTrkLocPos& trkPos, double offset ) const
{
    double	pos = RtGetDistFromStart2(const_cast<tTrkLocPos*>(&trkPos)) + offset;

    return NormalisePos(pos);
}

double	MyTrack::CalcPos( const tCarElt* car, double offset ) const
{
    double	pos = RtGetDistFromStart(const_cast<tCarElt*>(car)) + offset;

    return NormalisePos(pos);
}

double	MyTrack::CalcPos( double x, double y, const Seg* hint, bool sides ) const
{
    tTrackSeg*	pTrackSeg = m_pSegs[0].pSeg;

    if( hint != 0 )
        pTrackSeg = hint->pSeg;

    tTrkLocPos	pos;
    RtTrackGlobal2Local( pTrackSeg, tdble(x), tdble(y), &pos, sides );
    double	dist = RtGetDistFromStart2(&pos);

    return dist;
}

double	MyTrack::CalcHeightAbovePoint( const Vec3d& start_point, const Vec3d& direction, const Seg* hint ) const
{
    tTrkLocPos	pos;
    pos.seg = hint ? hint->pSeg : m_pSegs[0].pSeg;

    Vec3d	point(start_point);

    for(int i = 0; i < 10; i++ )
    {
        RtTrackGlobal2Local( pos.seg, tdble(point.x), tdble(point.y), &pos, TR_LPOS_MAIN );
        double h = RtTrackHeightL(&pos);
        double delta_h = h - point.z;
        if( fabs(delta_h) < 0.0001 )
            break;

        point.x += direction.x * delta_h;
        point.y += direction.y * delta_h;
        point.z += direction.z * delta_h;
    }

    double dot = (point - start_point) * direction;

    return dot;
}

double	MyTrack::CalcForwardAngle( double trackPos ) const
{
    int					idx = IndexFromPos(trackPos);
    const tTrackSeg*	pSeg = m_pSegs[idx].pSeg;

    double	t;
    Vec3d	pt;
    Vec3d	norm;
    CalcPtAndNormal( pSeg, NormalisePos(trackPos - pSeg->lgfromstart), t, pt, norm );

    return Utils::VecAngXY(norm) + PI / 2;
}

Vec2d	MyTrack::CalcNormal( double trackPos ) const
{
    int					idx = IndexFromPos(trackPos);
    const tTrackSeg*	pSeg = m_pSegs[idx].pSeg;

    double	t;
    Vec3d	pt;
    Vec3d	norm;
    CalcPtAndNormal( pSeg, NormalisePos(trackPos - pSeg->lgfromstart), t, pt, norm );

    return norm.GetXY();
}

double	MyTrack::GetFriction( int index, double offset ) const
{
    const tTrackSeg*	pSeg = m_pSegs[index].pSeg;

    // TODO: cope with varying track widths (perhaps just call the function in torcs itself?)

    if( offset < 0 )
    {
        double x = pSeg->width / 2 - offset;
        while( pSeg->lside != NULL && x > pSeg->width )
        {
            x -= pSeg->width;
            pSeg = pSeg->lside;
        }
    }
    else
    {
        double x = pSeg->width / 2 + offset;
        while( pSeg->rside != NULL && x > pSeg->width )
        {
            x -= pSeg->width;
            pSeg = pSeg->rside;
        }
    }

    double	friction = pSeg->surface->kFriction;

    return friction;
}

void	MyTrack::CalcPtAndNormal(
        const tTrackSeg*	pSeg,
        double				toStart,
        double&				t,
        Vec3d&				pt,
        Vec3d&				norm ) const
{
    if( pSeg->type == TR_STR )
    {
        Vec3d	s = (Vec3d(pSeg->vertex[TR_SL]) + Vec3d(pSeg->vertex[TR_SR])) / 2;
        Vec3d	e = (Vec3d(pSeg->vertex[TR_EL]) + Vec3d(pSeg->vertex[TR_ER])) / 2;
        t = toStart / pSeg->length;
        pt = s + (e - s) * t;

        double hl = pSeg->vertex[TR_SL].z +
                (pSeg->vertex[TR_EL].z - pSeg->vertex[TR_SL].z) * t;
        double hr = pSeg->vertex[TR_SR].z +
                (pSeg->vertex[TR_ER].z - pSeg->vertex[TR_SR].z) * t;
        norm = -Vec3d(pSeg->rgtSideNormal);
        norm.z = (hr - hl) / pSeg->width;
    }
    else
    {
        double d = pSeg->type == TR_LFT ? 1 : -1;
        double deltaAng = d * toStart / pSeg->radius;
        double ang = pSeg->angle[TR_ZS] - PI / 2 + deltaAng;
        double c = cos(ang);
        double s = sin(ang);
        double r = d * pSeg->radius;
        t = toStart / pSeg->length;
        double hl = pSeg->vertex[TR_SL].z +
                (pSeg->vertex[TR_EL].z - pSeg->vertex[TR_SL].z) * t;
        double hr = pSeg->vertex[TR_SR].z +
                (pSeg->vertex[TR_ER].z - pSeg->vertex[TR_SR].z) * t;
        pt = Vec3d(pSeg->center.x + c * r, pSeg->center.y + s * r, (hl + hr) / 2);
        norm = Vec3d(c, s, (hr - hl) / pSeg->width);
    }
}
