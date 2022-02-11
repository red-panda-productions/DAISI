/***************************************************************************

    file        : LinePath.cpp
    created     : 9 Apr 2006
    copyright   : (C) 2006 Tim Foden

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// LinePath.cpp: implementation of the LinePath class.
//
//////////////////////////////////////////////////////////////////////

#include <robottools.h>

#include "LinePath.h"
#include "Utils.h"
#include "ParametricCubic.h"

// The "SHADOW" logger instance.
extern GfLogger* PLogSHADOW;
#define LogSHADOW (*PLogSHADOW)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LinePath::LinePath() :
    NSEG(0),
    m_pTrack(0),
    m_pPath(0),
    m_estimatedTime(0)
{
}

LinePath::~LinePath()
{
    //m_pPath = NULL;
    NSEG = 0;
}

LinePath& LinePath::operator=( const LinePath& path )
{
    m_pTrack = path.m_pTrack;

    NSEG = path.NSEG;

    //delete [] m_pPath;
    m_pPath = path.m_pPath;
    m_estimatedTime = path.m_estimatedTime;
    //memcpy( m_pPath, path.m_pPath, NSEG * sizeof(PathPt) );

    return *this;
}

bool LinePath::ContainsPos( double trackPos ) const
{
    return true;
}

bool LinePath::GetPtInfo( double trackPos, PtInfo& pi ) const
{
    int		idx0 = m_pTrack->IndexFromPos(trackPos);

    int		idxp = (idx0 - 1 + NSEG) % NSEG;
    int		idx1 = (idx0 + 1) % NSEG;
    int		idx2 = (idx0 + 2) % NSEG;

    double	dist0 = m_pPath[idx0].Dist();
    double	dist1 = m_pPath[idx1].Dist();
    if( idx1 == 0 )
        dist1 = m_pTrack->GetLength();

    Vec3d	p0 = m_pPath[idxp].CalcPt();
    Vec3d	p1 = m_pPath[idx0].CalcPt();
    Vec3d	p2 = m_pPath[idx1].CalcPt();
    Vec3d	p3 = m_pPath[idx2].CalcPt();

    double	k1 = Utils::CalcCurvatureXY(p0, p1, p2);
    double	k2 = Utils::CalcCurvatureXY(p1, p2, p3);

    double k1z = Utils::CalcCurvatureZ(p0, p1, p2);
    double k2z = Utils::CalcCurvatureZ(p1, p2, p3);

    double	tx = (trackPos - dist0) / (dist1 - dist0);

    pi.idx = idx0;
    pi.k   = (1.0 - tx) * k1 + tx * k2;
    pi.kz  = (1.0 - tx) * k1z + tx * k2z;

    pi.t = tx;
    pi.offs = (m_pPath[idx0].offs) + tx * (m_pPath[idx1].offs - m_pPath[idx0].offs);

    double Ang0 = Utils::VecAngXY(m_pPath[idx1].CalcPt() - m_pPath[idx0].CalcPt());
    double Ang1 = Utils::VecAngXY(m_pPath[idx2].CalcPt() - m_pPath[idx1].CalcPt());

    double DeltaAng = Ang1 - Ang0;
    NORM_PI_PI(DeltaAng);
    pi.oang = Ang0 + pi.t * DeltaAng;

    Vec2d Tang1, Tang2;
    Utils::CalcTangent(p0.GetXY(), p1.GetXY(), p2.GetXY(), Tang1);
    Utils::CalcTangent(p1.GetXY(), p2.GetXY(), p3.GetXY(), Tang2);

    Ang0 = Utils::VecAngle(Tang1);
    Ang1 = Utils::VecAngle(Tang2);
    DeltaAng = Ang1 - Ang0;
    NORM_PI_PI(DeltaAng);

    pi.spd = m_pPath[pi.idx].spd + (m_pPath[idx1].spd - m_pPath[pi.idx].spd) * pi.t;
    pi.accSpd = m_pPath[pi.idx].accSpd + (m_pPath[idx1].accSpd - m_pPath[pi.idx].accSpd) * pi.t;

    return true;

/*    Vec3d	v1 = p2 - p1;
    Vec3d	v2 = p3 - p2;
    //	double	tx = (pt - p1) * v1 / (v1 * v1);


    Vec3d	n = Utils::VecNormXY(v1);
    Vec3d	np = p1 + v1 * tx;
    double	ty = 0;
    double	oldTy = 1;

    int		count = 0;
    while( fabs(ty - oldTy) > 0.00001 )
    {
        oldTy = ty;
        Vec3d	pp = np + n * ty;
        double	len1 = Utils::VecLenXY(np - p1);
        double	len2 = Utils::VecLenXY(np - p2);
        double	kappa = (k1 * len2 + k2 * len1) / (len1 + len2);
        double	k = Utils::CalcCurvatureXY(p1, pp, p2);

        if( kappa != 0 )
        {
            double	delta = 0.0001;
            double	deltaK = Utils::CalcCurvatureXY(p1, pp + n * delta, p2) - k;
            ty += delta * (kappa - k) / deltaK;
        }

        if( ++count > 100 )
            break;
    }

    Vec3d	pp = np + n * ty;
    //	double	k = Utils::CalcCurvatureXY(p1, pp, p2);
    //	double	k = k1 + (k2 - k1) * tx;
    double	k = Utils::InterpCurvature(k1, k2, tx);

    pi.idx = idx0;
    pi.k = k;
    pi.offs = ty;
    pi.t = tx;

    //	if( pi.t < 0 || pi.t >= 1 )
    //		GfOut( "*** t out of range %g  tl %g  tp %g  d0 %g  d1 %g\n",
    //			pi.t, m_pTrack->GetLength(), trackPos, dist0, dist1 );

    //	pi.offs0 = m_pPath[idx0].offs;
    //	pi.offs1 = m_pPath[idx1].offs;
    pi.offs = m_pPath[idx0].offs + tx *
            (m_pPath[idx1].offs - m_pPath[idx0].offs);

    //	pi.oang  = Utils::VecAngXY(m_pPath[idx1].CalcPt() -
    //							   m_pPath[pi.idx].CalcPt());

    double	ang0  = Utils::VecAngXY(m_pPath[idx1].CalcPt() -
                                    m_pPath[idx0].CalcPt());
    double	ang1  = Utils::VecAngXY(m_pPath[idx2].CalcPt() -
                                    m_pPath[idx1].CalcPt());

    double	deltaAng = ang1 - ang0;
    NORM_PI_PI( deltaAng );
    pi.oang = ang0 + pi.t * deltaAng;

    {
        // this seems good (-0.3s) on alpine 1 only!!
        Vec2d	tan1, tan2;
        Utils::CalcTangent( p0.GetXY(), p1.GetXY(), p2.GetXY(), tan1 );
        Utils::CalcTangent( p1.GetXY(), p2.GetXY(), p3.GetXY(), tan2 );
        Vec2d	dir = Utils::VecUnit(tan1) * (1 - tx) + Utils::VecUnit(tan2) * tx;

        //		pi.offs += -ty;
        //		pi.oang = Utils::VecAngle(dir);
        //		pi.k = Utils::CalcCurvatureXY(p0, pp, p3);

        double	ang0  = Utils::VecAngle(tan1);
        double	ang1  = Utils::VecAngle(tan2);
        double	deltaAng = ang1 - ang0;
        NORM_PI_PI( deltaAng );
        //		pi.oang = ang0 + tx * deltaAng;
    }


    {
        // this seems good (-0.3s) on alpine 1 only!!
        Vec2d	tan;
        Utils::CalcTangent( p0.GetXY(), pp.GetXY(), p3.GetXY(), tan );
        pi.oang = Utils::VecAngle(tan);
    }

    //	pi.k = m_pPath[pi.idx].k;

    pi.spd = m_pPath[pi.idx].spd + (m_pPath[idx1].spd - m_pPath[pi.idx].spd) * pi.t;
    //	pi.spd = m_pPath[pi.idx].spd;

    pi.toL = m_pPath[pi.idx].Wl();
    pi.toR = m_pPath[pi.idx].Wr();

    return true;*/
}

/*
bool	LinePath::GetPtInfo( double trackPos, PtInfo& pi ) const
{
    const int	NSEG = m_pTrack->GetSize();

    int		idx0 = m_pTrack->IndexFromPos(trackPos);
    int		idxp = (idx0 - 1 + NSEG) % NSEG;
    int		idx1 = (idx0 + 1) % NSEG;
    int		idx2 = (idx0 + 2) % NSEG;

    double	dist0 = m_pPath[idx0].Dist();
    double	dist1 = m_pPath[idx1].Dist();
    if( idx1 == 0 )
        dist1 = m_pTrack->GetLength();

    Vec3d	p0 = m_pPath[idxp].CalcPt();
    Vec3d	p1 = m_pPath[idx0].CalcPt();
    Vec3d	p2 = m_pPath[idx1].CalcPt();
    Vec3d	p3 = m_pPath[idx2].CalcPt();

    double	k1 = Utils::CalcCurvatureXY(p0, p1, p2);
    double	k2 = Utils::CalcCurvatureXY(p1, p2, p3);

    Vec3d	v02 = p2 - p0;
    Vec3d	v13 = p3 - p2;

    ParametricCubic	cubic;

//	cubic.Set( p1.GetXY(), v02.GetXY(), p2.GetXY(), v13.GetXY() );
    cubic.Set( p0.GetXY(), p1.GetXY(), p2.GetXY(), p3.GetXY() );

//	double	tx = (pt - p1) * v1 / (v1 * v1);
    double	tx = (trackPos - dist0) / (dist1 - dist0);

    Vec2d	pp = cubic.Calc(tx);
    Vec2d	pv = cubic.CalcGradient(tx);
//	double	k = cubic.CalcCurvature(tx);
    double	k = Utils::InterpCurvatureLin(k1, k2, tx);

    tTrkLocPos	pos;
    const tTrackSeg*	pSeg = m_pTrack->GetAt(idx0).pSeg;
    RtTrackGlobal2Local((tTrackSeg*)pSeg, pp.x, pp.y, &pos, 0);
    double	ty = -pos.toMiddle;

    pi.idx = idx0;
    pi.k = k;
    pi.offs = ty;
    pi.t = tx;
    pi.oang = Utils::VecAngle(pv);

    if( pi.t < 0 || pi.t >= 1 )
        GfOut( "*** t out of range %g  tl %g  tp %g  d0 %g  d1 %g\n",
            pi.t, m_pTrack->GetLength(), trackPos, dist0, dist1 );


    pi.spd = m_pPath[pi.idx].spd + (m_pPath[idx1].spd - m_pPath[pi.idx].spd) * pi.t;

    pi.toL = m_pPath[pi.idx].Wl();
    pi.toR = m_pPath[pi.idx].Wr();

    return true;
}
*/
void LinePath::Set( const LinePath& path )
{
    m_pTrack = path.m_pTrack;

    //delete [] m_pPath;
    m_pPath = path.m_pPath;

    m_maxL = path.m_maxL;
    m_maxR = path.m_maxR;

    //memcpy( m_pPath, path.m_pPath, NSEG * sizeof(*m_pPath) );
}

void LinePath::Initialise( MyTrack* pTrack, double maxL, double maxR )
{
    NSEG = pTrack->GetSize();
    m_pTrack = pTrack;
    //delete [] m_pPath;
    m_pPath.resize( NSEG );
    //m_pPath = new PathPt[NSEG];

    m_maxL = maxL;
    m_maxR = maxR;

    for( int i = 0; i < NSEG; i++ )
    {
        m_pPath[i].pSeg		= &(*pTrack)[i];
        m_pPath[i].k		= 0;
        m_pPath[i].kz		= 0;
        m_pPath[i].kv		= 0;
        m_pPath[i].pt		= m_pPath[i].CalcPt();
        m_pPath[i].ap		= 0;
        m_pPath[i].ar		= 0;
        m_pPath[i].maxSpd	= 10;
        m_pPath[i].spd	= 10;
        m_pPath[i].accSpd	= 10;
        m_pPath[i].h		= 0;
        m_pPath[i].lBuf	= 0;
        m_pPath[i].rBuf	= 0;
        m_pPath[i].fixed	= false;
    }

    CalcAngles();
    CalcCurvaturesXY();
    CalcCurvaturesZ();
    CalcCurvaturesV();
    CalcCurvaturesH();
}

const LinePath::PathPt&	LinePath::GetAt( int idx ) const
{
    return m_pPath[idx];
}

LinePath::PathPt& LinePath::GetAt( int idx )
{
    return m_pPath[idx];
}

void LinePath::CalcCurvaturesXY( int start, int len, int step )
{
    const int	NSEG = m_pTrack->GetSize();

    for( int count = 0; count < NSEG; count++ )
    {
        int		i  = (start + count) % NSEG;
        int		ip = (i - step + NSEG) % NSEG;
        int		in = (i + step) % NSEG;

        m_pPath[i].k = Utils::CalcCurvatureXY( m_pPath[ip].CalcPt(),
                                               m_pPath[i ].CalcPt(),
                                               m_pPath[in].CalcPt() );
    }
}

void LinePath::CalcCurvaturesZ( int start, int len, int step )
{
    for( int count = 0; count < NSEG; count++ )
    {
        int		i  = (start + count) % NSEG;
        int		ip = (i - 3 * step + NSEG) % NSEG;
        int		in = (i + 3 * step) % NSEG;

        m_pPath[i].kz = 6 * Utils::CalcCurvatureZ( m_pPath[ip].CalcPt(),
                                                   m_pPath[i ].CalcPt(),
                                                   m_pPath[in].CalcPt() );

    }
}

void LinePath::CalcCurvaturesV( int start, int len, int step )
{
    for( int count = 0; count < NSEG; count++ )
    {
        int     i  = (start + count) % NSEG;
        int     ip = (i - 1 * step + NSEG) % NSEG;
        int		in = (i + 1 * step) % NSEG;

        t3Dd	tn;
        tTrkLocPos	pos;
        RtTrackGlobal2Local(m_pPath[i].pSeg->pSeg, (float)m_pPath[i].pt.x, (float)m_pPath[i].pt.y, &pos, TR_LPOS_MAIN);
        RtTrackSurfaceNormalL(&pos, &tn);
        Vec3d	track_normal(tn);
        Vec3d   curr_pt = m_pPath[i].pt;

        // work out tangent in the horizontal plane
        Vec2d   tangent2d;
        Utils::CalcTangent( m_pPath[ip].pt.GetXY(), curr_pt.GetXY(), m_pPath[in].pt.GetXY(), tangent2d );

        // project tangent onto the track segment plane, and re-normalise it.
        Vec3d	tangent(tangent2d);
        Vec3d	projection((tangent * track_normal) * track_normal);
        tangent -= projection;
        tangent.normalize();

        // work out points ahead and behind of the current point, along the tangent.
        const int delta_x = 10;
        Vec3d   prev_pt(curr_pt - tangent * delta_x);
        Vec3d   next_pt(curr_pt + tangent * delta_x);

        // measure the relative track heights of the 3 points along the tangent line.
        // (note the b_height should always be zero.)
        double  a_height = m_pTrack->CalcHeightAbovePoint(prev_pt, track_normal, m_pPath[i].pSeg);
        double  b_height = m_pTrack->CalcHeightAbovePoint(curr_pt, track_normal, m_pPath[i].pSeg);
        double  c_height = m_pTrack->CalcHeightAbovePoint(next_pt, track_normal, m_pPath[i].pSeg);

        // calculate the curvature normal to the track segment in the direction of the tangent
        // to the racing line.
        m_pPath[i].kv = Utils::CalcCurvature(-delta_x, a_height, 0, b_height, delta_x, c_height);
    }
}

void LinePath::CalcCurvaturesH( int start, int len, int step )
{
    for( int count = 0; count < NSEG; count++ )
    {
        int     i  = (start + count) % NSEG;
        int     ip = (i - 1 * step + NSEG) % NSEG;
        int		in = (i + 1 * step) % NSEG;

        if( i == 376 )
            int f = 2;

        t3Dd	tn;
        tTrkLocPos	pos;
        RtTrackGlobal2Local(m_pPath[i].pSeg->pSeg, (float)m_pPath[i].pt.x, (float)m_pPath[i].pt.y, &pos, TR_LPOS_MAIN);
        RtTrackSurfaceNormalL(&pos, &tn);
        Vec3d	track_normal(tn);

        // project the points on to the horizontal plain.
        Vec3d   prev_pt = m_pPath[ip].pt.SetZ(0);
        Vec3d   curr_pt = m_pPath[i ].pt.SetZ(0);
        Vec3d   next_pt = m_pPath[in].pt.SetZ(0);

        // work out coordinate system in the track plane.
        Vec3d	x = (track_normal % m_pPath[i].Norm()).to_unit_vector();
        Vec3d	y = track_normal % x;

        // calculate the curvature in the plane of the track segment.
        m_pPath[i].kh = Utils::CalcCurvature(x * prev_pt, y * prev_pt,
                                           x * curr_pt, y * curr_pt,
                                           x * next_pt, y * next_pt);
    }
}

void LinePath::CalcAngles( int start, int len, int step )
{
    for( int count = 0; count < NSEG; count++ )
    {
        int		i  = (start + count) % NSEG;
        int		ip = (i - step + NSEG) % NSEG;
        int		in = (i + step) % NSEG;

        // estimate pitch using the points ahead and behind along the path.
        Vec3d  pitchVec = m_pPath[in].pt - m_pPath[ip].pt;
        double pathPitchAngle = atan2(pitchVec.z, pitchVec.GetXY().len());

        // TODO: calculate the path roll angle instead of the track roll angle...
        double pathRollAngle = atan2(m_pPath[i].Norm().z, 1);
        LogSHADOW.debug("Track Pitch = %.3f - Track Roll = %.3f\n", pathPitchAngle, pathRollAngle);

        m_pPath[i].ap = pathPitchAngle;
        m_pPath[i].ar = pathRollAngle;
    }
}

void LinePath::CalcLoadRatios( int start, int len, const CarModel& cm, int step )
{
    const double	recip_op_load = 1 / (cm.MASS * G);
    const double	mass_load = (cm.MASS + cm.FUEL) * G;
    const double	Ca = cm.CA;

    for( int count = 0; count < NSEG; count++ )
    {
        int				i  = (start + count) % NSEG;
        const PathPt&	pp = m_pPath[i];

        double	load = cm.calcPredictedLoad(pp.accSpd, 1.0, Ca, pp.k, pp.kz, pp.kv, sin(pp.ar), cos(pp.ar), cos(pp.ap));
        double	load_ratio = load * recip_op_load;

        m_pPath[i].loadratio = load_ratio;
    }
}

void LinePath::CalcMaxSpeeds( int start, int len, const CarModel& carModel, int step )
{
    for( int count = 0; count < len; count += step )
    {
        int	i = (start + count) % NSEG;
        int j = (i + 1) % NSEG;

        Vec3d Delta = m_pPath[i].CalcPt() - m_pPath[j].CalcPt();
        double Dist = Utils::VecLenXY(Delta);
        // use friction from outer set of wheels.
        double  frictionOffset = m_pPath[i].offs + SGN(m_pPath[i].k) * 0.75;
        double	trackRollAngle = atan2(m_pPath[i].Norm().z, 1);
        double  trackTiltAngle = 1.1 * atan2(Delta.z, Dist);
        //double	spd = carModel.CalcMaxSpeed( m_pPath[i].k, m_pPath[j].k, m_pPath[j].kz, m_pTrack->GetFriction(i, frictionOffset), trackRollAngle, trackTiltAngle);
        double	spd = carModel.CalcMaxSpeed( m_pPath[j].k, m_pPath[j].kz, m_pPath[j].kv, m_pTrack->GetFriction(i, frictionOffset), trackRollAngle, trackTiltAngle);
        double TrackTurnangle = CalcTrackTurnangle(i, (i + 50) % NSEG);
        LogSHADOW.debug("Track Turn Angle = %.3f\n", TrackTurnangle);

        if (TrackTurnangle > 0.7)
            spd *= 0.75;

        if (TrackTurnangle < 0.2)
            spd *= 1.05;

        if (spd < 5)
            spd = 5.0;

        m_pPath[i].maxSpd = spd;
        m_pPath[i].spd    = spd;
        m_pPath[i].accSpd = spd;
    }
}

void LinePath::PropagateBreaking( int start, int len, const CarModel& cm, int step )
{
    for( int count = step * ((len - 1) / step); count >= 0; count -= step )
    {
        int		i = (start + count) % NSEG;
        int		j = (i + step) % NSEG;

        if( m_pPath[i].spd > m_pPath[j].spd )
        {
            // see if we need to adjust spd[i] to make it possible
            //	to slow to spd[j] by the next seg.

            Vec3d	delta = m_pPath[i].CalcPt() - m_pPath[j].CalcPt();
            double	dist = Utils::VecLenXY(delta);
            double	k = (m_pPath[i].k + m_pPath[j].k) * 0.5;

            if( fabs(k) > 0.0001 )
                dist = 2 * asin(0.5 * dist * k) / k;

            double	trackRollAngle = atan2(m_pPath[i].Norm().z, 1);
            double  trackTiltAngle = 1.1 * atan2(delta.z, dist);
            double  frictionOffset = m_pPath[i].offs + SGN(m_pPath[i].k) * 0.75;
            double brakeFactor = -1;
            double kz = -1, mu = -1;

            double	u = cm.CalcBreaking( m_pPath[i].k, m_pPath[i].kz, m_pPath[j].k, m_pPath[j].kz, m_pPath[j].spd, dist, m_pTrack->GetFriction(i, frictionOffset),
                        trackRollAngle, trackTiltAngle);

            if( m_pPath[i].spd > u )
                m_pPath[i].spd = m_pPath[i].accSpd = u;

            if( m_pPath[i].h > 0.1 )
                m_pPath[i].spd = m_pPath[j].spd;

            //			GfOut( "%4d  K %7.4f      u %7.3f   v %7.3f\n", i, K, u, v );
        }
    }
}

void LinePath::PropagateAcceleration( int start, int len, const CarModel& cm, int step )
{
    for( int count = 0; count < len; count += step )
    {
        int		j = (start + count) % NSEG;
        int		i = (j - step + NSEG) % NSEG;

        if( m_pPath[i].accSpd < m_pPath[j].accSpd )
        {
            // see if we need to adjust spd[j] to make it possible
            //	to speed up to spd[i] from spd[j].
            Vec3d delta = m_pPath[i].CalcPt() - m_pPath[j].CalcPt();
            double	dist = Utils::VecLenXY(m_pPath[i].CalcPt() - m_pPath[j].CalcPt());
            double	k = (m_pPath[i].k + m_pPath[j].k) * 0.5;

            if( fabs(k) > 0.0001 )
                dist = 2 * asin(0.5 * dist * k) / k;

            //			double	acc = 5;
            //			double	u = m_pPath[i].accSpd;
            //			double	v = sqrt(2 * acc * dist + u * u);
            double	trackRollAngle = atan2(m_pPath[i].Norm().z, 1);
            double  trackTiltAngle = 1.1 * atan2(delta.z, dist);
            double  frictionOffset = m_pPath[i].offs + SGN(m_pPath[i].k) * 0.75;
            double	v = cm.CalcAcceleration( m_pPath[i].k, m_pPath[i].kz, m_pPath[j].k, m_pPath[j].kz, m_pPath[i].accSpd, dist, m_pTrack->GetFriction(i, frictionOffset),
                        trackRollAngle, trackTiltAngle);

            //			if( v2 > v )
            //				int fff = 5;

            if( m_pPath[j].accSpd > v )
                m_pPath[j].accSpd = v;
        }
    }
}

void LinePath::CalcCurvaturesXY( int step )
{
    CalcCurvaturesXY( 0, NSEG, step );
}

void LinePath::CalcCurvaturesZ( int step )
{
    CalcCurvaturesZ( 0, NSEG, step );
}

void LinePath::CalcCurvaturesV( int step )
{
    CalcCurvaturesV( 0, NSEG, step );
}

void LinePath::CalcCurvaturesH( int step )
{
    CalcCurvaturesH( 0, NSEG, step );
}

void LinePath::CalcAngles( int step )
{
    CalcAngles( 0, NSEG, step );
}

void LinePath::CalcLoadRatios( const CarModel& cm, int step )
{
    CalcLoadRatios( 0, NSEG, cm, step );
}


void LinePath::CalcMaxSpeeds( const CarModel& carModel, int step )
{
    CalcMaxSpeeds( 0, NSEG, carModel, step );
}

void LinePath::PropagateBreaking( const CarModel& cm, int step )
{
    PropagateBreaking( 0, NSEG, cm, step );
    PropagateBreaking( 0, NSEG, cm, step );
}

void LinePath::PropagateAcceleration( const CarModel& cm, int step )
{
    PropagateAcceleration( 0, NSEG, cm, step );
    PropagateAcceleration( 0, NSEG, cm, step );
}

void LinePath::CalcFwdAbsK( int range, int step )
{
    int		count = range / step;
    int		i = count * step;
    int		j = i;
    double	totalK = 0;

    while( i > 0 )
    {
        totalK += m_pPath[i].k;
        i -= step;
    }

    m_pPath[0].fwdK = totalK / count;
    totalK += fabs(m_pPath[0].k);
    totalK -= fabs(m_pPath[j].k);

    i = (NSEG / step) * step;
    j -= step;
    if( j < 0 )
        j = (NSEG / step) * step;

    while( i > 0 )
    {
        m_pPath[i].fwdK = totalK / count;
        //		GfOut( "***** i %d, k %7.4f fdwK %g  %6.1f\n",
        //				i, m_pPath[i].k, m_pPath[i].fwdK, 1 / m_pPath[i].fwdK );
        totalK += fabs(m_pPath[i].k);
        totalK -= fabs(m_pPath[j].k);

        i -= step;
        j -= step;
        if( j < 0 )
            j = (NSEG / step) * step;
    }
}

void LinePath::SetOffset( const CarModel& cm, double offset, PathPt* l )
{
    double	marg = cm.WIDTH / 2 + 0.02;//1.0;//1.1
    double	wl  = -MN(m_maxL, l->Wl()) + marg;
    double	wr  =  MN(m_maxR, l->Wr()) - marg;

    if( offset < wl )
        offset = wl;
    else if( offset > wr )
        offset = wr;

    l->offs = offset;
    l->pt = l->CalcPt();
}

void LinePath::InterpolateBetweenLinear( const CarModel& cm, int step )
{
    // now smooth the values between steps
    PathPt*	l0 = 0;
    PathPt*	l1 = &m_pPath[0];

    for( int i = 0; i < NSEG; i += step )
    {
        int		j = (i + step) % NSEG;
        l0 = l1;
        l1 = &m_pPath[j];

        for( int k = 1; k < step; k++ )
        {
            double	t;
            PathPt&	l = m_pPath[(i + k) % NSEG];
            Utils::LineCrossesLine(l.Pt().GetXY(), l.Norm().GetXY(), l0->pt.GetXY(), l1->pt.GetXY() - l0->pt.GetXY(), t);

            SetOffset( cm, t, &l );
        }
    }
}

void LinePath::InterpolateBetweenLinearSection( const CarModel& cm, int start, int len, int step )
{
    // now smooth the values between steps
    PathPt*	l0 = 0;
    PathPt*	l1 = &m_pPath[start];

    for( int index = 0; index < len; index += step )
    {
        int		i = (start + index) % NSEG;
        int		j = (start + MN(len, index + step)) % NSEG;

        l0 = l1;
        l1 = &m_pPath[j];

        for( int k = (i + 1) % NSEG; k != j; k = (k + 1) % NSEG )
        {
            double	t;
            PathPt&	l = m_pPath[k];
            Utils::LineCrossesLine(l.Pt().GetXY(), l.Norm().GetXY(), l0->pt.GetXY(), l1->pt.GetXY() - l0->pt.GetXY(), t);
            SetOffset( cm, t, &l );
        }
    }
}

void LinePath::InterpolateBetweenLaneLinear( const CarModel& cm, int step )
{
    // now smooth the values between steps
    for( int i = 0; i < NSEG; i += step )
    {
        int		j = i + step;
        if( j >= NSEG )
        {
            step = NSEG - i;
            j = 0;
        }

        double	startT = m_pPath[i].offs;
        double	endT   = m_pPath[j].offs;

        for( int k = 1; k < step; k++ )
        {
            double	t = startT + (endT - startT) * k / step;
            PathPt&	l = m_pPath[(i + k) % NSEG];
            SetOffset( cm, t, &l );
        }
    }
}

void LinePath::GenShortest( const CarModel& cm )
{
    // just run an averaging alg over whole track repeatedly
    for( int step = 128; step > 0; step /= 2 )
    {
        for( int j = 0; j < 5; j++ )
        {
            PathPt*	l0 = 0;
            PathPt*	l1 = &m_pPath[((NSEG - step - 1) / step) * step];
            PathPt*	l2 = &m_pPath[((NSEG - 1) / step) * step];

            Vec2d	p0;
            Vec2d	p1 = l1->pt.GetXY();
            Vec2d	p2 = l2->pt.GetXY();

            for( int i = 0; i < NSEG; i += step )
            {
                l0 = l1;
                l1 = l2;
                l2 = &m_pPath[i];

                p0 = p1;
                p1 = p2;
                p2 = l2->pt.GetXY();

                double	t;

                if( Utils::LineCrossesLine(l1->Pt().GetXY(), l1->Norm().GetXY(), p0, p2 - p0, t) )
                {
                    SetOffset( cm, t, l1 );
                    p1 = l1->pt.GetXY();
                }
            }
        }

        if( step > 1 )
            InterpolateBetweenLinear( cm, step );
    }

    CalcAngles();
    CalcCurvaturesXY();
    CalcCurvaturesZ();
    CalcCurvaturesV();
    CalcCurvaturesH();
}

void LinePath::GenMiddle()
{
    for( int i = 0; i < NSEG; i++ )
    {
        m_pPath[i].offs = 0;
        m_pPath[i].pt = m_pPath[i].Pt();
    }

    CalcAngles();
    CalcCurvaturesXY();
    CalcCurvaturesZ();
    CalcCurvaturesV();
    CalcCurvaturesH();
}

void LinePath::Average( const CarModel& cm )
{
    AverageSection( cm, 0, NSEG );
}

void LinePath::AverageSection( const CarModel& cm, int from, int len )
{
    // run an averaging alg
    PathPt*	l0 = 0;
    PathPt*	l1 = &m_pPath[(from - 1 + NSEG) % NSEG];
    PathPt*	l2 = &m_pPath[from];

    Vec2d	p0;
    Vec2d	p1 = l1->pt.GetXY();
    Vec2d	p2 = l2->pt.GetXY();

    for( int i = 0; i < NSEG; i++ )
    {
        int		j = (from + 1 + i) % NSEG;

        l0 = l1;
        l1 = l2;
        l2 = &m_pPath[j];

        p0 = p1;
        p1 = p2;
        p2 = l2->pt.GetXY();

        double	t;

        if( Utils::LineCrossesLine(l1->Pt().GetXY(), l1->Norm().GetXY(), p0, p2 - p0, t) )
        {
            t = l1->offs * 0.9 + t * 0.1;
            SetOffset( cm, t, l1 );
            p1 = l1->pt.GetXY();
        }
    }
}

void LinePath::ModifySection( int from, int len, double delta, int important, double lBuf, double rBuf )
{
    // find distances...
    double*	pDist = new double[len];
    pDist[0] = 0;

    for( int i = 1; i < len; i++ )
    {
        int		j = (from + i - 1) % NSEG;
        int		k = (j + 1) % NSEG;
        double	length = (GetAt(j).pt.GetXY() - GetAt(k).pt.GetXY()).len();
        pDist[i] = pDist[i - 1] + length;
    }

    int		newFrom = from;
    int		newTo = from + len;

    // dry run to find limits, and a better estimate of distance
    double	totalDist = pDist[len - 1];

    Vec3d	p0 = GetAt(from).pt;

    for( int i = 0; i < len; i++ )
    {
        int		j = (from + i) % NSEG;
        const PathPt&	l0 = GetAt((j - 1 + NSEG) % NSEG);
        const PathPt&	l1 = GetAt(j);
        const PathPt&	l2 = GetAt((j + 1) % NSEG);

        double	dist = pDist[i];
        double	angle = PI * dist / totalDist;
        double	offset = (1 - cos(angle)) * 0.5 * delta;

        Vec2d	tan = Vec3d(l2.pt - l0.pt).GetXY().GetUnit().GetNormal();
        double	dot = tan * l1.Norm().GetXY();
        offset /= fabs(dot);

        double	offs = l1.offs + offset;
        if( offset < 0 && offs < -l1.Wl() + lBuf ||
            offset > 0 && offs > l1.Wr() - rBuf )
        {
            if( i < (important - from + NSEG) % NSEG)
                newFrom = j;
            else
            {
                newTo = j;
                break;
            }
        }

        Vec3d	p1 = l1.CalcPt(offs);

        if( i > 0 )
        {

            double	length = (p1.GetXY() - p0.GetXY()).len();
            pDist[i] = pDist[i - 1] + length;
        }

        p0 = p1;
    }

    int		oldFrom = from;
    from = newFrom;
    len = (newTo - newFrom + NSEG) % NSEG;

    if( len < 5 )
    {
        delete [] pDist;
        return;
    }

    int		newI = (from - oldFrom + NSEG) % NSEG;
    totalDist = pDist[newI + len - 1] - pDist[newI];

    p0 = GetAt((from - 1 + NSEG) % NSEG).pt;

    for( int i = 0; i < len; i++ )
    {
        int				j = (from + i) % NSEG;
        PathPt&			l1 = GetAt(j);
        const PathPt&	l2 = GetAt((j + 1) % NSEG);

        double	dist = pDist[i + newI] - pDist[newI];
        double	angle = PI * dist / totalDist;
        double	offset = (1 - cos(angle)) * 0.5 * delta;

        Vec2d	tan = Vec3d(l2.pt - p0).GetXY().GetUnit().GetNormal();
        double	dot = tan * l1.Norm().GetXY();
        offset /= fabs(dot);

        p0 = l1.pt;

        l1.offs += offset;
        l1.pt = l1.CalcPt();
    }

    delete [] pDist;
}

void LinePath::SetEstimatedTime( double time )
{
    m_estimatedTime = time;
}

double LinePath::GetEstimatedTime() const
{
    return m_estimatedTime;
}

double LinePath::GetRollAngle( int idx ) const
{
    return m_pPath[idx].ar;
}

double LinePath::GetPitchAngle( int idx ) const
{
    return m_pPath[idx].ap;
}

double LinePath::CalcEstimatedTime( int start, int len ) const
{
    double	totalTime = 0;

    for( int s = 0; s < len; s++ )
    {
        int		i = (s + start) % NSEG;
        int		j = (i + 1) % NSEG;
        double	dist = Utils::VecLenXY(m_pPath[i].CalcPt() -
                                       m_pPath[j].CalcPt());
        double	spd = (m_pPath[i].accSpd + m_pPath[j].accSpd) * 0.5;
        double	time = dist / spd;
        totalTime += time;
    }

    return totalTime;
}

double LinePath::CalcEstimatedLapTime() const
{
    double	lapTime = 0;

    for( int i = 0; i < NSEG; i++ )
    {
        int		j = (i + 1) % NSEG;
        double	dist = Utils::VecLenXY(m_pPath[i].CalcPt() -
                                       m_pPath[j].CalcPt());
        double	spd = (m_pPath[i].accSpd + m_pPath[j].accSpd) * 0.5;
        double	time = dist / spd;
        lapTime += time;
    }

    return lapTime;
}

//==========================================================================*
// Calculate Track Turnangle
//--------------------------------------------------------------------------*
double LinePath::CalcTrackTurnangle(int i, int j)
{
    double TotalCrv = 0;
    while (i < j)
        TotalCrv += m_pPath[i++].k;

    return fabs(TotalCrv);
}

bool LinePath::LoadPath( const char* pDataFile )
{
    GfOut( "Loading \"springs\" data file %s\n", pDataFile );

    FILE*	pFile = fopen(pDataFile, "r");
    if( pFile == 0 )
    {
        //		GfOut( "Failed to open data file\n" );
        return false;
    }

    char	buf[1024];
    if( fgets(buf, sizeof(buf), pFile) == NULL || strncmp(buf, "SPRINGS-PATH", 12) != 0 )
    {
        //		GfOut( "Failed to open data file -- SPRINGS-PATH\n" );
        fclose( pFile );
        return false;
    }

    int		version = 0;
    if( fgets(buf, sizeof(buf), pFile) == NULL || sscanf(buf, "%d", &version) != 1 || (version != 1 && version != 2) )
    {
        //		GfOut( "Failed to open data file -- version\n" );
        fclose( pFile );
        return false;
    }

    // versions:
    //	1	- dist/offset
    //	2	- global (x, y)

    if( fgets(buf, sizeof(buf), pFile) == NULL || strncmp(buf, "TRACK-LEN", 9) != 0 )
    {
        //		GfOut( "Failed to open data file -- TRACK-LEN\n" );
        fclose( pFile );
        return false;
    }

    double	trackLen = 0;
    if( fgets(buf, sizeof(buf), pFile) == NULL || sscanf(buf, "%lf", &trackLen) != 1 || fabs(trackLen - m_pTrack->GetLength()) > 0.01 )
    {
        GfOut( "Failed to open data file -- length %g %g\n",
               trackLen, m_pTrack->GetLength() );
        //		fclose( pFile );
        //		return false;
    }

    if( fgets(buf, sizeof(buf), pFile) == NULL || strncmp(buf, "BEGIN-POINTS", 12) != 0 )
    {
        //		GfOut( "Failed to open data file -- BEGIN-POINTS\n" );
        fclose( pFile );
        return false;
    }

    int		nPoints = 0;
    if( fgets(buf, sizeof(buf), pFile) == NULL || sscanf(buf, "%d", &nPoints) != 1 )
    {
        //		GfOut( "Failed to open data file -- nPoints\n" );
        fclose( pFile );
        return false;
    }

    Vec2d*	pPoints = new Vec2d[nPoints];

    for( int i = 0; i < nPoints; i++ )
        {
            if( fgets(buf, sizeof(buf), pFile) == NULL || sscanf(buf, "%lf %lf", &pPoints[i].x, &pPoints[i].y) != 2 )
            {
                //			GfOut( "Failed to open data file -- point data\n" );
                delete [] pPoints;
                fclose( pFile );
                return false;
            }
        }

    fclose( pFile );

    if( version == 1 )
    {
        //
        //	interpolate points to make path offsets here...
        //

        Vec2d	inP0 = pPoints[0];
        Vec2d	inP1 = pPoints[1];
        int		inP = 1;

        for( int i = 0; i < NSEG; i++ )
            {
                PathPt*	pp = &m_pPath[i];
                while( pp->Dist() > inP1.x )
                {
                    inP0 = inP1;
                    inP++;
                    if( inP < nPoints )
                        inP1 = pPoints[inP];
                    else
                    {
                        //				ASSERT( inP == nPoints );
                        inP1 = pPoints[0];
                        inP1.x = trackLen;
                    }
                }

                double	t = (pp->Dist() - inP0.x) / (inP1.x - inP0.x);
                double	w = inP0.y + (inP1.y - inP0.y) * t;

                pp->offs = -w;
                pp->pt   = pp->CalcPt();
            }
    }
    else	// version == 2
    {
        //
        //	points are global x, y coords that need to be interpolated.
        //

        Vec2d	origin(0, 0);

        {
            //
            tTrkLocPos	pos;
            pos.seg = m_pTrack->GetAt(0).pSeg;
            pos.type = 0;
            pos.toStart = 0;
            pos.toRight = (tdble)(m_pTrack->GetWidth() / 2);
            pos.toMiddle = 0;
            pos.toLeft = (tdble)(m_pTrack->GetWidth() / 2);

            float	x, y;
            RtTrackLocal2Global( &pos, &x, &y, 0 );
            //GfOut( "global start coords (%g, %g)\n", x, y );

            origin.x = x;
            origin.y = y;
        }

        // work out which slice the last point is in.
        //GfOut( "nPoints %d\n", nPoints );
        Vec2d	lastPt = pPoints[nPoints - 1] + origin;
        //GfOut( "lastPt (%g, %g)\n", lastPt.x, lastPt.y );
        double	dist = m_pTrack->CalcPos(lastPt.x, lastPt.y);
        //GfOut( "dist %g\n", dist );
        int		last_s = m_pTrack->IndexFromPos(dist);

        for( int i = 0; i < nPoints; i++ )
        {
            // work out position.
            Vec2d	pt = pPoints[i] + origin;

            // work out which slice this point is in.
            dist = m_pTrack->CalcPos(pt.x, pt.y, &m_pTrack->GetAt(last_s));
            int	cur_s = m_pTrack->IndexFromPos(dist);

            //tTrackSeg*	pSeg = m_pTrack->GetAt(cur_s).pSeg;
            /*GfOut( "%4d  (%8g,%8g)  seg %4d/%3d%c %d\n",
                   i, pt.x, pt.y, cur_s, pSeg->id,
                   pSeg->type == TR_RGT ? 'R' : pSeg->type == TR_LFT ? 'L' : '-',
                   pSeg->raceInfo );*/

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
                    if( Utils::LineCrossesLine(lastPt, pt - lastPt,
                                               s0.pt.GetXY(), s0.norm.GetXY(), t, w) &&
                            t >= 0.0 && t <= 1.0 )
                    {
                        //						Rec&	rec = m_pData[next_s];
                        //						const double	gamma = 0.8;
                        //						rec.avgW	= rec.avgW * (1 - gamma) + w * gamma;
                        //GfOut( "%%%%  w[%d] = %g (was %g)\n", next_s, w, m_pPath[next_s].offs );
                        m_pPath[next_s].offs = w;
                        m_pPath[next_s].pt   = m_pPath[next_s].CalcPt();
                    }

                    last_s = next_s;
                    next_s = (next_s + 1) % NSEG;
                }
            }

            last_s = cur_s;
            lastPt = pt;
        }
    }

    CalcCurvaturesZ();

    //GfOut( "\"springs\" data file loaded OK\n" );

    // take some of the "kinks" out of the data.
    //	OptimisePath( 1, 100 );
    //	OptimisePath( 1, 2 );

    // all done.
    delete [] pPoints;
    return true;
}

//const LinePath::PathPt&	LinePath::GetAt( int index ) const
//{
//	return m_pPath[index];
//}
