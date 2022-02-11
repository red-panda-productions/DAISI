/***************************************************************************

    file        : Path.cpp
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

// Path.cpp: implementation of the Path class.
//
//////////////////////////////////////////////////////////////////////

#include "Path.h"
#include "Utils.h"
#include "ParametricCubic.h"

#include <robottools.h>

using namespace std;

// The "SHADOW" logger instance.
extern GfLogger* PLogSHADOW;
#define LogSHADOW (*PLogSHADOW)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Path::Path()
:	NSEG(0),
    m_pTrack(0),
    m_pts(0),
    m_estimatedTime(0)
{
}

Path::~Path()
{
}

void	Path::Clear()
{
    NSEG = 0;
    m_pTrack = 0;
    m_pts.clear();
    m_estimatedTime = 0;
}

Path&	Path::operator=( const Path& path )
{
    NSEG = path.NSEG;
    m_pTrack = path.m_pTrack;
    m_pts = path.m_pts;
    m_estimatedTime = path.m_estimatedTime;
    m_maxL = path.m_maxL;
    m_maxR = path.m_maxR;

    return *this;
}

bool	Path::ContainsPos( double trackPos ) const
{
    return true;
}

bool	Path::GetPtInfo( double trackPos, PtInfo& pi ) const
{
    trackPos = m_pTrack->NormalisePos(trackPos);

    int		idx0 = m_pTrack->IndexFromPos(trackPos);
    int		idxp = (idx0 - 1 + NSEG) % NSEG;
    int		idx1 = (idx0 + 1) % NSEG;
    int		idx2 = (idx0 + 2) % NSEG;

    double	dist0 = m_pts[idx0].Dist();
    double	dist1 = m_pts[idx1].Dist();
    double	dist2 = m_pts[idx2].Dist();
    if( dist1 < dist0 )
        dist1 += m_pTrack->GetLength();
    if( dist2 < dist0 )
        dist2 += m_pTrack->GetLength();

    Vec3d	p0 = m_pts[idxp].CalcPt();
    Vec3d	p1 = m_pts[idx0].CalcPt();
    Vec3d	p2 = m_pts[idx1].CalcPt();
    Vec3d	p3 = m_pts[idx2].CalcPt();

    double	k1 = Utils::CalcCurvatureXY(p0, p1, p2);
    double	k2 = Utils::CalcCurvatureXY(p1, p2, p3);

#if 0   // dead code
    Vec3d	v02 = p2 - p0;
    Vec3d	v13 = p3 - p1;
#endif

    ParametricCubic	cubic;

//	cubic.SetPoints( p1.GetXY(), v02.GetXY(), p2.GetXY(), v13.GetXY() );
    cubic.SetPoints( p0.GetXY(), p1.GetXY(), p2.GetXY(), p3.GetXY() );

//	double	tx = (pt - p1) * v1 / (v1 * v1);
    double	tx = (trackPos - dist0) / (dist1 - dist0);

    Vec2d	pp = cubic.Calc(tx);
    Vec2d	pv = cubic.CalcGradient(tx);
    // double	ck = cubic.CalcCurvature(tx);
    double	k = Utils::InterpCurvatureLin(k1, k2, tx);

//	LogSHADOW.debug( "*** tx=%.3f/%d (%.3f,%.3f) (%.3f,%.3f) k=%.5f %.5f\n", tx, idx0, pp.x, pp.y, pv.x, pv.y, k, k - ck );

    tTrkLocPos	pos;
    const tTrackSeg*	pSeg = m_pTrack->GetAt(idx0).pSeg;
    RtTrackGlobal2Local((tTrackSeg*)pSeg, tdble(pp.x), tdble(pp.y), &pos, 0);
    double	ty = -pos.toMiddle;

    pi.idx = idx0;
    pi.k = k;
    pi.offs = ty;
    pi.t = tx;
    pi.oang = Utils::VecAngle(pv);

    if( pi.t < 0 || pi.t >= 1 )
        LogSHADOW.debug( "*** t out of range %g  tl %g  tp %g  d0 %g  d1 %g\n",
            pi.t, m_pTrack->GetLength(), trackPos, dist0, dist1 );


    pi.spd = m_pts[pi.idx].spd + (m_pts[idx1].spd - m_pts[pi.idx].spd) * pi.t;

    // v^2 - u^2 = 2 a s
    double acc0 = (m_pts[idx1].spd * m_pts[idx1].spd - m_pts[idx0].spd * m_pts[idx0].spd) / (2 * (dist1 - dist0));
    double acc1 = (m_pts[idx2].spd * m_pts[idx2].spd - m_pts[idx1].spd * m_pts[idx1].spd) / (2 * (dist2 - dist1));
    pi.acc = acc0 + (acc1 - acc0) * pi.t;

    pi.toL = m_pts[pi.idx].Wl();
    pi.toR = m_pts[pi.idx].Wr();
    pi.extL = m_pts[pi.idx].Extl();
    pi.extR = m_pts[pi.idx].Extr();

    return true;
}

bool	Path::GotPath() const
{
    return !m_pts.empty();
}

void	Path::Initialise( const MyTrack* pTrack, double maxL, double maxR )
{
    m_maxL = maxL;
    m_maxR = maxR;

    if( m_pTrack == pTrack )
        return;

    NSEG = pTrack->GetSize();

    m_pTrack = pTrack;
    m_pts.resize( NSEG );

    {for( int i = 0; i < NSEG; i++ )
    {
        m_pts[i].pSeg	= &(*pTrack)[i];
        m_pts[i].k		= 0;
        m_pts[i].kz		= 0;
        m_pts[i].kv		= 0;
        m_pts[i].offs	= m_pts[i].pSeg->midOffs;
        m_pts[i].pt		= m_pts[i].CalcPt();
        m_pts[i].ap		= 0;
        m_pts[i].ar		= 0;
        m_pts[i].maxSpd	= 10;
        m_pts[i].spd	= 10;
        m_pts[i].accSpd	= 10;
        m_pts[i].h		= 0;
        m_pts[i].lBuf	= 0;
        m_pts[i].rBuf	= 0;
        m_pts[i].fixed	= false;
    }}

    CalcAngles();
    CalcCurvaturesXY();
    CalcCurvaturesZ();
    CalcCurvaturesV();
    CalcCurvaturesH();
}

int		Path::GetSize() const
{
    return NSEG;
}

const Path::PathPt&	Path::GetAt( int idx ) const
{
    return m_pts[idx];
}

Path::PathPt&	Path::GetAt( int idx )
{
    return m_pts[idx];
}

const MyTrack*   Path::GetTrack() const
{
    return m_pTrack;
}

void	Path::CalcCurvaturesXY( int start, int len, int step )
{
    {for( int count = 0; count < NSEG; count++ )
    {
        int		i  = (start + count) % NSEG;
        int		ip = (i - step + NSEG) % NSEG;
        int		in = (i + step) % NSEG;

        m_pts[i].k = Utils::CalcCurvatureXY( m_pts[ip].CalcPt(),
                                               m_pts[i ].CalcPt(),
                                               m_pts[in].CalcPt() );
    }}
}

void	Path::CalcCurvaturesZ( int start, int len, int step )
{
    {for( int count = 0; count < NSEG; count++ )
    {
        int		i  = (start + count) % NSEG;
        int		ip = (i - 3 * step + NSEG) % NSEG;
        int		in = (i + 3 * step) % NSEG;

        // TODO: question the '6 *' in the expression below...
        // NOTE: I think this is a holdover from the time where the angle was measured??
        // Also the normal KZ_SCALE value to use is around 0.1 - 0.2, which make the value
        // kz * KZ_SCALE be approximately 1.0, which is more expected.
        // However there are some places in the code where the PathPt::kz value is used
        // without multiplying by the KZ_SCALE ... is this at all correct?
        m_pts[i].kz = 6 * Utils::CalcCurvatureZ( m_pts[ip].CalcPt(),
                                                   m_pts[i ].CalcPt(),
                                                   m_pts[in].CalcPt() );
    }}
}

void	Path::CalcCurvaturesV( int start, int len, int step )
{
    {for( int count = 0; count < NSEG; count++ )
    {
        int     i  = (start + count) % NSEG;
        int     ip = (i - 1 * step + NSEG) % NSEG;
        int		in = (i + 1 * step) % NSEG;
#if 1
        t3Dd	tn;
        tTrkLocPos	pos;
        RtTrackGlobal2Local(m_pts[i].pSeg->pSeg, (float)m_pts[i].pt.x, (float)m_pts[i].pt.y, &pos, TR_LPOS_MAIN);
        RtTrackSurfaceNormalL(&pos, &tn);
        Vec3d	track_normal(tn);
#else
        double  sn_pitch = sin(m_pts[i].ap);
        double  cs_pitch = cos(m_pts[i].ap);
        double  sn_roll  = sin(m_pts[i].ar);
        double  cs_roll  = cos(m_pts[i].ar);

        Vec3d   track_normal(sn_pitch, cs_pitch * sn_roll, cs_pitch * cs_roll);
        //track_normal.normalize();
#endif
        Vec3d   curr_pt = m_pts[i].pt;

        // work out tangent in the horizontal plane
        Vec2d   tangent2d;
        Utils::CalcTangent( m_pts[ip].pt.GetXY(), curr_pt.GetXY(), m_pts[in].pt.GetXY(), tangent2d );

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
        double  a_height = m_pTrack->CalcHeightAbovePoint(prev_pt, track_normal, m_pts[i].pSeg);
        double  b_height = m_pTrack->CalcHeightAbovePoint(curr_pt, track_normal, m_pts[i].pSeg);
        double  c_height = m_pTrack->CalcHeightAbovePoint(next_pt, track_normal, m_pts[i].pSeg);

        // calculate the curvature normal to the track segment in the direction of the tangent
        // to the racing line.
        m_pts[i].kv = Utils::CalcCurvature(-delta_x, a_height, 0, b_height, delta_x, c_height);
    }}
}

void	Path::CalcCurvaturesH( int start, int len, int step )
{
    {for( int count = 0; count < NSEG; count++ )
    {
        int     i  = (start + count) % NSEG;
        int     ip = (i - 1 * step + NSEG) % NSEG;
        int		in = (i + 1 * step) % NSEG;

        // if( i == 376 )
        //    int f = 2;
#if 1
        t3Dd	tn;
        tTrkLocPos	pos;
        RtTrackGlobal2Local(m_pts[i].pSeg->pSeg, (float)m_pts[i].pt.x, (float)m_pts[i].pt.y, &pos, TR_LPOS_MAIN);
        RtTrackSurfaceNormalL(&pos, &tn);
        Vec3d	track_normal(tn);
#else
        double  sn_pitch = sin(m_pts[i].ap);
        double  cs_pitch = cos(m_pts[i].ap);
        double  sn_roll  = sin(m_pts[i].ar);
        double  cs_roll  = cos(m_pts[i].ar);

        Vec3d   track_normal(sn_pitch, cs_pitch * sn_roll, cs_pitch * cs_roll);
        //track_normal.normalize();
#endif

#if 0
        // project the points on to the plain of the track segment.
        Vec3d   prev_pt = m_pts[ip].pt - (m_pts[ip].pt * track_normal) * track_normal;
        Vec3d   curr_pt = m_pts[i ].pt - (m_pts[i ].pt * track_normal) * track_normal;
        Vec3d   next_pt = m_pts[in].pt - (m_pts[in].pt * track_normal) * track_normal;

        // work out coordinate system in the plane.
        Vec3d	x = Vec3d(next_pt - prev_pt).to_unit_vector();
        Vec3d	y = track_normal % x;
#else
        // project the points on to the horizontal plain.
        Vec3d   prev_pt = m_pts[ip].pt.SetZ(0);
        Vec3d   curr_pt = m_pts[i ].pt.SetZ(0);
        Vec3d   next_pt = m_pts[in].pt.SetZ(0);

        // work out coordinate system in the track plane.
        Vec3d	x = (track_normal % m_pts[i].Norm()).to_unit_vector();
        Vec3d	y = track_normal % x;
#endif
        // calculate the curvature in the plane of the track segment.
        m_pts[i].kh = Utils::CalcCurvature(x * prev_pt, y * prev_pt,
                                           x * curr_pt, y * curr_pt,
                                           x * next_pt, y * next_pt);
    }}
}

void	Path::CalcAngles( int start, int len, int step )
{
    {for( int count = 0; count < NSEG; count++ )
    {
        int		i  = (start + count) % NSEG;
        int		ip = (i - step + NSEG) % NSEG;
        int		in = (i + step) % NSEG;

        // estimate pitch using the points ahead and behind along the path.
        Vec3d  pitchVec = m_pts[in].pt - m_pts[ip].pt;
        double pathPitchAngle = atan2(pitchVec.z, pitchVec.GetXY().len());

        // TODO: calculate the path roll angle instead of the track roll angle...
        double pathRollAngle = atan2(m_pts[i].Norm().z, 1);

        m_pts[i].ap = pathPitchAngle;
        m_pts[i].ar = pathRollAngle;
    }}
}

void	Path::CalcLoadRatios( int start, int len, const CarModel& cm, int step )
{
    const double	recip_op_load = 1 / (cm.MASS * G);
    // const double	mass_load = (cm.MASS + cm.FUEL) * G;
    const double	Ca = cm.CA;

    {for( int count = 0; count < NSEG; count++ )
    {
        int				i  = (start + count) % NSEG;
        const PathPt&	pp = m_pts[i];

        double	load = cm.calcPredictedLoad(pp.accSpd, 1.0, Ca, pp.k, pp.kz, pp.kv, sin(pp.ar), cos(pp.ar), cos(pp.ap));
        double	load_ratio = load * recip_op_load;

        m_pts[i].loadRatio = load_ratio;
    }}
}

void	Path::CalcMaxSpeeds( int start, int len, const CarModel& carModel, int step )
{
    int     prev_i = start - step;
    if( prev_i < 0 )
        prev_i = ((NSEG - 1) / step) * step;

    // [experimental] limit in order to give the car a chance to adjust to
    // a change of curvature.  rates over this limit are penalised in speed
    // to push the optimiser to avoid them.
    // const double RATE_OF_CHANGE_OF_K_LIMIT = 0.05;

    {for( int count = 0; count < len; count += step )
    {
        int	 i = (start + count) % NSEG;

        // if( i == 387 && step == 1 )
        //    int ff = 5;

        // use friction from outer set of wheels.
        double  frictionOffset = m_pts[i].offs + SGN(m_pts[i].k) * 0.75;

        double	spd = carModel.CalcMaxSpeed(
                            m_pts[i].k, m_pts[i].kz, m_pts[i].kv,
                            m_pTrack->GetFriction(i, frictionOffset),
                            GetRollAngle(i), GetPitchAngle(i));
/*
        // add a penalty on the speed if the rate of change of curvature is too high.
        if( spd != 0 )
        {
            double	dist = Utils::VecLenXY(m_pts[prev_i].pt - m_pts[i].pt);
            double  time = dist / spd;
            double  delta_k = m_pts[prev_i].k - m_pts[i].k;
            if( fabs(delta_k) > time * RATE_OF_CHANGE_OF_K_LIMIT )
                spd *= 0.5;
        }
*/
        prev_i = i;

        m_pts[i].maxSpd = spd;
        m_pts[i].spd    = spd;
        m_pts[i].accSpd = spd;
    }}
}

void	Path::PropagateBraking( int start, int len, const CarModel& cm, int step )
{
    for( int count = step * ((len - 1) / step); count >= 0; count -= step )
    {
        int		i = (start + count) % NSEG;
        int		j = (i + step) % NSEG;

        if( m_pts[i].spd > m_pts[j].spd )
        {
            // see if we need to adjust spd[i] to make it possible
            //	to slow to spd[j] by the next seg.

            Vec3d	delta = m_pts[i].CalcPt() - m_pts[j].CalcPt();
            double	dist = Utils::VecLenXY(delta);
            double	k = (m_pts[i].k + m_pts[j].k) * 0.5;
            if( fabs(k) > 0.0001 )
                dist = 2 * asin(0.5 * dist * k) / k;
            // use friction from outer set of wheels.
            double  frictionOffset = m_pts[i].offs + SGN(m_pts[i].k) * 0.75;
            double	u = cm.CalcBraking(
                            m_pts[i].k, m_pts[i].kz, m_pts[i].kv,
                            m_pts[j].k, m_pts[j].kz, m_pts[j].kv,
                            m_pts[j].spd,
                            dist,
                            m_pTrack->GetFriction(i, frictionOffset),
                            GetRollAngle(i), GetPitchAngle(i));

            if( m_pts[i].spd > u )
                m_pts[i].spd = m_pts[i].accSpd = u;

            if( m_pts[i].h > 0.1 )
                m_pts[i].spd = m_pts[j].spd;

//			LogSHADOW.debug( "%4d  K %7.4f      u %7.3f   v %7.3f\n", i, K, u, v );
        }
    }
}

void	Path::PropagateAcceleration( int start, int len, const CarModel& cm, int step )
{
    for( int count = 0; count < len; count += step )
    {
        int		j = (start + count) % NSEG;
        int		i = (j - step + NSEG) % NSEG;

        if( m_pts[i].accSpd < m_pts[j].accSpd )
        {
            // see if we need to adjust spd[j] to make it possible
            //	to speed up to spd[i] from spd[j].

            double	dist = Utils::VecLenXY(m_pts[i].CalcPt() -
                                           m_pts[j].CalcPt());
            double	k = (m_pts[i].k + m_pts[j].k) * 0.5;
            if( fabs(k) > 0.0001 )
                dist = 2 * asin(0.5 * dist * k) / k;
//			double	acc = 5;
//			double	u = m_pts[i].accSpd;
//			double	v = sqrt(2 * acc * dist + u * u);
            // use friction from outer set of wheels.
            double  frictionOffset = m_pts[i].offs + SGN(m_pts[i].k) * 0.75;
            double	v = cm.CalcAcceleration(
                            m_pts[i].k, m_pts[i].kz, m_pts[i].kv,
                            m_pts[j].k, m_pts[j].kz, m_pts[j].kv,
                            m_pts[i].accSpd,
                            dist,
                            m_pTrack->GetFriction(i, frictionOffset),
                            GetRollAngle(i), GetPitchAngle(i));

//			if( v2 > v )
//				int fff = 5;

            if( m_pts[j].accSpd > v )
                m_pts[j].accSpd = v;
        }
    }
}

void	Path::CalcCurvaturesXY( int step )
{
    CalcCurvaturesXY( 0, NSEG, step );
}

void	Path::CalcCurvaturesZ( int step )
{
    CalcCurvaturesZ( 0, NSEG, step );
}

void	Path::CalcCurvaturesV( int step )
{
    CalcCurvaturesV( 0, NSEG, step );
}

void	Path::CalcCurvaturesH( int step )
{
    CalcCurvaturesH( 0, NSEG, step );
}

void	Path::CalcAngles( int step )
{
    CalcAngles( 0, NSEG, step );
}

void	Path::CalcLoadRatios( const CarModel& cm, int step )
{
    CalcLoadRatios( 0, NSEG, cm, step );
}

void	Path::CalcMaxSpeeds( const CarModel& carModel, int step )
{
    CalcMaxSpeeds( 0, NSEG, carModel, step );
}

void	Path::PropagateBraking( const CarModel& cm, int step )
{
    PropagateBraking( 0, NSEG, cm, step );
    PropagateBraking( 0, NSEG, cm, step );
}

void	Path::PropagateAcceleration( const CarModel& cm, int step )
{
    PropagateAcceleration( 0, NSEG, cm, step );
    PropagateAcceleration( 0, NSEG, cm, step );
}

void	Path::CalcPathInfo( const CarModel& cm )
{
    CalcAngles();
    CalcCurvaturesXY();
    CalcCurvaturesZ();
    CalcCurvaturesV();
    CalcCurvaturesH();
    CalcMaxSpeeds(cm);
    PropagateBraking(cm);
    PropagateAcceleration(cm);
    CalcLoadRatios(cm);
    SetEstimatedTime( CalcEstimatedLapTime() );
}

void	Path::CalcFwdAbsK( int range, int step )
{
    int		count = range / step;
    int		i = count * step;
    int		j = i;
    double	totalK = 0;

    while( i > 0 )
    {
        totalK += m_pts[i].k;
        i -= step;
    }

    m_pts[0].fwdK = totalK / count;
    totalK += fabs(m_pts[0].k);
    totalK -= fabs(m_pts[j].k);

    i = ((NSEG - 1) / step) * step;
    j -= step;
    if( j < 0 )
        j = ((NSEG - 1) / step) * step;

    while( i > 0 )
    {
        m_pts[i].fwdK = totalK / count;
//		LogSHADOW.debug( "***** i %d, k %7.4f fdwK %g  %6.1f\n",
//				i, m_pts[i].k, m_pts[i].fwdK, 1 / m_pts[i].fwdK );
        totalK += fabs(m_pts[i].k);
        totalK -= fabs(m_pts[j].k);

        i -= step;
        j -= step;
        if( j < 0 )
            j = ((NSEG - 1) / step) * step;
    }
}

void	Path::SetOffset( const CarModel& cm, double offset, PathPt* l )
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

void	Path::InterpolateBetweenLinear( const CarModel& cm, int step )
{
    // now smooth the values between steps
    PathPt*	l0 = 0;
    PathPt*	l1 = &m_pts[0];

    for( int i = 0; i < NSEG; i += step )
    {
        int		j = (i + step) % NSEG;
        l0 = l1;
        l1 = &m_pts[j];

        for( int k = 1; k < step; k++ )
        {
            double	t;
            PathPt&	l = m_pts[(i + k) % NSEG];
            Utils::LineCrossesLine(l.Pt().GetXY(), l.Norm().GetXY(), l0->pt.GetXY(), l1->pt.GetXY() - l0->pt.GetXY(), t);

            SetOffset( cm, t, &l );
        }
    }
}

void	Path::InterpolateBetweenLinearSection( const CarModel& cm, int start, int len, int step )
{
    // now smooth the values between steps
    PathPt*	l0 = 0;
    PathPt*	l1 = &m_pts[start];

    for( int index = 0; index < len; index += step )
    {
        int		i = (start + index) % NSEG;
        int		j = (start + MN(len, index + step)) % NSEG;

        l0 = l1;
        l1 = &m_pts[j];

        for( int k = (i + 1) % NSEG; k != j; k = (k + 1) % NSEG )
        {
            double	t;
            PathPt&	l = m_pts[k];
            Utils::LineCrossesLine(l.Pt().GetXY(), l.Norm().GetXY(), l0->pt.GetXY(), l1->pt.GetXY() - l0->pt.GetXY(), t);
            SetOffset( cm, t, &l );
        }
    }
}

void	Path::InterpolateBetweenLaneLinear( const CarModel& cm, int step )
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

        double	startT = m_pts[i].offs;
        double	endT   = m_pts[j].offs;
        for( int k = 1; k < step; k++ )
        {
            double	t = startT + (endT - startT) * k / step;
            PathPt&	l = m_pts[(i + k) % NSEG];
            SetOffset( cm, t, &l );
        }
    }
}

void	Path::GenShortest( const CarModel& cm )
{
    // just run an averaging alg over whole track repeatedly
    {for( int step = 128; step > 0; step /= 2 )
    {
        {for( int j = 0; j < 5; j++ )
        {
#if 0   // dead code
            PathPt*	l0 = 0;
#endif
            PathPt*	l1 = &m_pts[((NSEG - step - 1) / step) * step];
            PathPt*	l2 = &m_pts[((NSEG - 1) / step) * step];

            Vec2d	p0;
            Vec2d	p1 = l1->pt.GetXY();
            Vec2d	p2 = l2->pt.GetXY();

            for( int i = 0; i < NSEG; i += step )
            {
#if 0   // dead code
                l0 = l1;
#endif
                l1 = l2;
                l2 = &m_pts[i];

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
        }}

        if( step > 1 )
            InterpolateBetweenLinear( cm, step );
    }}

    CalcAngles();
    CalcCurvaturesXY();
    CalcCurvaturesZ();
    CalcCurvaturesV();
    CalcCurvaturesH();
}

void	Path::GenMiddle()
{
    {for( int i = 0; i < NSEG; i++ )
    {
        m_pts[i].offs = 0;
        m_pts[i].pt = m_pts[i].Pt();
    }}

    CalcAngles();
    CalcCurvaturesXY();
    CalcCurvaturesZ();
    CalcCurvaturesV();
    CalcCurvaturesH();
}

void	Path::Average( const CarModel& cm )
{
    AverageSection( cm, 0, NSEG );
}

void	Path::AverageSection( const CarModel& cm, int from, int len )
{
    // run an averaging alg
#if 0   // dead code
    PathPt*	l0 = 0;
#endif
    PathPt*	l1 = &m_pts[(from - 1 + NSEG) % NSEG];
    PathPt*	l2 = &m_pts[from];

    Vec2d	p0;
    Vec2d	p1 = l1->pt.GetXY();
    Vec2d	p2 = l2->pt.GetXY();

    for( int i = 0; i < NSEG; i++ )
    {
        int		j = (from + 1 + i) % NSEG;

#if 0   // dead code
        l0 = l1;
#endif
        l1 = l2;
        l2 = &m_pts[j];

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

void	Path::ModifySection( int from, int len, double delta, int important, double lBuf, double rBuf )
{
    // find distances...

    double*	pDist = new double[len];
    pDist[0] = 0;
    {for( int i = 1; i < len; i++ )
    {
        int		j = (from + i - 1) % NSEG;
        int		k = (j + 1) % NSEG;
        double	length = (GetAt(j).pt.GetXY() - GetAt(k).pt.GetXY()).len();
        pDist[i] = pDist[i - 1] + length;
    }}

    int		newFrom = from;
    int		newTo = from + len;

    // dry run to find limits, and a better estimate of distance

    double	totalDist = pDist[len - 1];

    Vec3d	p0 = GetAt(from).pt;
    {for( int i = 0; i < len; i++ )
    {
        int		j = (from + i) % NSEG;
        const PathPt&	l0 = GetAt((j - 1 + NSEG) % NSEG);
        const PathPt&	l1 = GetAt(j);
        const PathPt&	l2 = GetAt((j + 1) % NSEG);

        double	dist = pDist[i];
//		double	angle = PI * i / len;
        double	angle = PI * dist / totalDist;
        double	offset = (1 - cos(angle)) * 0.5 * delta;

        Vec2d	tan = Vec3d(l2.pt - l0.pt).GetXY().GetUnit().GetNormal();
        double	dot = tan * l1.Norm().GetXY();
        offset /= fabs(dot);

        double	offs = l1.offs + offset;
        if ((offset < 0 && offs < -l1.Wl() + lBuf) ||
            (offset > 0 && offs > l1.Wr() - rBuf))
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
    }}

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
    {for( int i = 0; i < len; i++ )
    {
        int				j = (from + i) % NSEG;
        PathPt&			l1 = GetAt(j);
        const PathPt&	l2 = GetAt((j + 1) % NSEG);

        double	dist = pDist[i + newI] - pDist[newI];
//		double	angle = PI * i / len;
        double	angle = PI * dist / totalDist;
        double	offset = (1 - cos(angle)) * 0.5 * delta;

        Vec2d	tan = Vec3d(l2.pt - p0).GetXY().GetUnit().GetNormal();
        double	dot = tan * l1.Norm().GetXY();
        offset /= fabs(dot);

        p0 = l1.pt;

        l1.offs += offset;
        l1.pt = l1.CalcPt();
    }}
/*
    {for( int i = 0; i < len; i++ )
    {
        int	j = (from + i) % NSEG;
        QuadraticFilter( j );
    }}
*/
    delete [] pDist;
}

/*

y = A x^2 + B x + C


sum(x^2) A + sum(x)   B + N        C = sum(y)			...(1)
sum(x^3) A + sum(x^2) B + sum(x)   C = sum(y x)			...(2)
sum(x^4) A + sum(x^3) B + sum(x^2) C = sum(y x^2)		...(3)


// from (1)

A = [sum(y) - (sum(x) B + N C)] / sum(x^2)
A = [sum(y) - sum(x) B - N C] / sum(x^2)				...(4)


// substitute (4) into (2) and (3)...

(2)...
sum(x^3) A                                                      + sum(x^2) B + sum(x) C = sum(y x)
sum(x^3) [sum(y) -          sum(x) B -          N C] / sum(x^2) + sum(x^2) B + sum(x) C = sum(y x)
[sum(x^3) sum(y) - sum(x^3) sum(x) B - sum(x^3) N C] / sum(x^2) + sum(x^2) B +          sum(x) C =          sum(y x)
 sum(x^3) sum(y) - sum(x^3) sum(x) B - sum(x^3) N C +  sum(x^2)   sum(x^2) B + sum(x^2) sum(x) C = sum(x^2) sum(y x)

   sum(x^2) sum(x^2) B - sum(x^3) sum(x) B +    sum(x^2) sum(x) C - sum(x^3) N C + sum(x^3) sum(y) = sum(x^2) sum(y x)
B [sum(x^2) sum(x^2)   - sum(x^3) sum(x)] +  C [sum(x^2) sum(x)   - sum(x^3) N]  + sum(x^3) sum(y) = sum(x^2) sum(y x)
B [sum(x^2) sum(x^2)   - sum(x^3) sum(x)] = -C [sum(x^2) sum(x)   - sum(x^3) N]  - sum(x^3) sum(y) + sum(x^2) sum(y x)

B   [sum(x^2) sum(x^2  ) - sum(x^3) sum(x)] = sum(x^2) sum(y x) - C [sum(x^2) sum(x)   - sum(x^3) N]  - sum(x^3) sum(y)

B = [sum(x^2) sum(y x) - C [sum(x^2) sum(x) - sum(x^3) N] - sum(x^3) sum(y)] / [sum(x^2) sum(x^2) - sum(x^3) sum(x)]		...(5)

(3)...
sum(x^4) A                                                      + sum(x^3) B + sum(x^2) C = sum(y x^2)
sum(x^4) [sum(y) -          sum(x) B -          N C] / sum(x^2) + sum(x^3) B + sum(x^2) C = sum(y x^2)
[sum(x^4) sum(y) - sum(x^4) sum(x) B - sum(x^4) N C] / sum(x^2) + sum(x^3) B +          sum(x^2) C =          sum(y x^2)
 sum(x^4) sum(y) - sum(x^4) sum(x) B - sum(x^4) N C +  sum(x^2)   sum(x^3) B + sum(x^2) sum(x^2) C = sum(x^2) sum(y x^2)

   sum(x^2) sum(x^3) B - sum(x^4) sum(x) B +    sum(x^2) sum(x^2) C - sum(x^4) N C +  sum(x^4) sum(y) = sum(x^2) sum(y x^2)
B [sum(x^2) sum(x^3)   - sum(x^4) sum(x)] +  C [sum(x^2) sum(x^2)   - sum(x^4) N]  +  sum(x^4) sum(y) = sum(x^2) sum(y x^2)
B [sum(x^2) sum(x^3)   - sum(x^4) sum(x)] = -C [sum(x^2) sum(x^2)   - sum(x^4) N]  -  sum(x^4) sum(y) + sum(x^2) sum(y x^2)

B [sum(x^2) sum(x^3)   - sum(x^4) sum(x)] = sum(x^2) sum(y x^2) - C [sum(x^2) sum(x^2)   - sum(x^4) N]  -  sum(x^4) sum(y)

B = [sum(x^2) sum(y x^2) - C [sum(x^2) sum(x^2) - sum(x^4) N] - sum(x^4) sum(y)] / [sum(x^2) sum(x^3) - sum(x^4) sum(x)]	...(6)

// now equate the Bs of (5) and (6)

  [sum(x^2) sum(y x)   - C [sum(x^2) sum(x)   - sum(x^3) N] - sum(x^3) sum(y)] / [sum(x^2) sum(x^2) - sum(x^3) sum(x)]
= [sum(x^2) sum(y x^2) - C [sum(x^2) sum(x^2) - sum(x^4) N] - sum(x^4) sum(y)] / [sum(x^2) sum(x^3) - sum(x^4) sum(x)]

  [sum(x^2) sum(y x)   - C [sum(x^2) sum(x)   - sum(x^3) N] - sum(x^3) sum(y)] * [sum(x^2) sum(x^3) - sum(x^4) sum(x)]
= [sum(x^2) sum(y x^2) - C [sum(x^2) sum(x^2) - sum(x^4) N] - sum(x^4) sum(y)] * [sum(x^2) sum(x^2) - sum(x^3) sum(x)]

  [sum(x^2) sum(y x)   - sum(x^3) sum(y)] [sum(x^2) sum(x^3) - sum(x^4) sum(x)] - C [sum(x^2) sum(x)   - sum(x^3) N] [sum(x^2) sum(x^3) - sum(x^4) sum(x)]
= [sum(x^2) sum(y x^2) - sum(x^4) sum(y)] [sum(x^2) sum(x^2) - sum(x^3) sum(x)] - C [sum(x^2) sum(x^2) - sum(x^4) N] [sum(x^2) sum(x^2) - sum(x^3) sum(x)]

  C [sum(x^2) sum(x^2) - sum(x^4) N] [sum(x^2) sum(x^2) - sum(x^3) sum(x)]    - C [sum(x^2) sum(x)   - sum(x^3) N] [sum(x^2) sum(x^3) - sum(x^4) sum(x)]
= [sum(x^2) sum(y x^2) - sum(x^4) sum(y)] [sum(x^2) sum(x^2) - sum(x^3) sum(x)] - [sum(x^2) sum(y x) - sum(x^3) sum(y)] [sum(x^2) sum(x^3) - sum(x^4) sum(x)]

  C <[sum(x^2) sum(x^2) - sum(x^4) N] [sum(x^2) sum(x^2) - sum(x^3) sum(x)]  - [sum(x^2) sum(x)   - sum(x^3) N] [sum(x^2) sum(x^3) - sum(x^4) sum(x)]>
= [sum(x^2) sum(y x^2) - sum(x^4) sum(y)] [sum(x^2) sum(x^2) - sum(x^3) sum(x)] - [sum(x^2) sum(y x) - sum(x^3) sum(y)] [sum(x^2) sum(x^3) - sum(x^4) sum(x)]

C = [sum(x^2) sum(y x^2) - sum(x^4) sum(y)] [sum(x^2) sum(x^2) - sum(x^3) sum(x)] - [sum(x^2) sum(y x) - sum(x^3) sum(y)] [sum(x^2) sum(x^3) - sum(x^4) sum(x)]
  / <[sum(x^2) sum(x^2) - sum(x^4) N] [sum(x^2) sum(x^2) - sum(x^3) sum(x)]  - [sum(x^2) sum(x)   - sum(x^3) N] [sum(x^2) sum(x^3) - sum(x^4) sum(x)]>
*/

void	Path::QuadraticFilter( int idx )
{
    double	su  = 0;
    double	su2 = 0;
    double	su3 = 0;
    double	su4 = 0;

    double	sux  = 0;
    double	su2x = 0;
    double	suy  = 0;
    double	su2y = 0;

    double	sx = 0;
    double	sy = 0;

    const int N = 7;			// must be odd so that:
    const int half_N = N / 2;	// N == half_N * 2 + 1

    int from = (idx + NSEG - half_N) % NSEG;
    double	u[N] = {0};
    Vec3d last = GetAt(from).pt;
    for( int i = 1; i < N; i++ )
    {
        Vec3d	curr = GetAt((from + i) % NSEG).pt;
        u[i] = u[i - 1] + (last - curr).len();
        last = curr;
    }

    double midU = u[half_N];
    for( int i = 0; i < N; i++ )
        u[i] -= midU;

    for( int i = 0; i < N; i++ )
    {
        Vec3d	curr = GetAt((from + i) % NSEG).pt;

        double u2 = u[i] * u[i];

        su   += u[i];
        su2  += u2;
        su3  += u2 * u[i];
        su4  += u2 * u2;

        sux  += u[i] * curr.x;
        su2x += u2   * curr.x;
        suy  += u[i] * curr.y;
        su2y += u2   * curr.y;

        sx   += curr.x;
        sy   += curr.y;
    }

//	C = <[sum(x^2) sum(y x^2) - sum(x^4) sum(y)] [sum(x^2) sum(x^2) - sum(x^3) sum(x)] - [sum(x^2) sum(y x) - sum(x^3) sum(y)] [sum(x^2) sum(x^3) - sum(x^4) sum(x)]>
//	  / <[sum(x^2) sum(x^2)   - sum(x^4) N]      [sum(x^2) sum(x^2) - sum(x^3) sum(x)] - [sum(x^2) sum(x)   - sum(x^3) N]      [sum(x^2) sum(x^3) - sum(x^4) sum(x)]>

    double xnum = (su2 * su2x - su4 * sx) * (su2 * su2 - su3 * su) - (su2 * sux - su3 * sx) * (su2 * su3 - su4 * su);
    double  den = (su2 * su2  - su4 * N)  * (su2 * su2 - su3 * su) - (su2 * su  - su3 * N)  * (su2 * su3 - su4 * su);
    double x = xnum / den;

    double ynum = (su2 * su2y - su4 * sy) * (su2 * su2 - su3 * su) - (su2 * suy - su3 * sy) * (su2 * su3 - su4 * su);
//	double yden = (su2 * su2  - su4 * N)  * (su2 * su2 - su3 * su) - (su2 * su  - su3 * N)  * (su2 * su3 - su4 * su);
    double y = ynum / den;

    Path::PathPt& pathPt = GetAt(idx);
    pathPt.offs = -Utils::ClosestPtOnLine(x, y, pathPt.Pt().x, pathPt.Pt().y, pathPt.Norm().x, pathPt.Norm().y);
    pathPt.pt = pathPt.CalcPt();
}

void	Path::FirFilter()
{
    // from http://t-filter.engineerjs.com/
    // using 0..400Hz gain=1 5db, 500..1000Hz gain=0 -40db. (this is the default.)
    static const double taps[] =
    {
        -0.002383400601468984,
        -0.002138757089494907,
         0.013317429587663769,
         0.057514322213710257,
         0.127298456724532580,
         0.194840026760914750,
         0.223103844808285020,
         0.194840026760914750,
         0.127298456724532580,
         0.057514322213710257,
         0.013317429587663769,
        -0.002138757089494907,
        -0.002383400601468984
    };
#if 0   // dead code
    static const double xtaps[] =
    {
        -0.02010411882885732,
        -0.05842798004352509,
        -0.061178403647821976,
        -0.010939393385338943,
         0.05125096443534972,
         0.033220867678947885,
        -0.05655276971833928,
        -0.08565500737264514,
         0.0633795996605449,
         0.31085440365663597,
         0.4344309124179415,
         0.31085440365663597,
         0.0633795996605449,
        -0.08565500737264514,
        -0.05655276971833928,
         0.033220867678947885,
         0.05125096443534972,
        -0.010939393385338943,
        -0.061178403647821976,
        -0.05842798004352509,
        -0.02010411882885732,
    };
#endif
    const int N_TAPS = sizeof(taps) / sizeof(taps[0]);

    vector<double>  values(NSEG);
    for( int i = 0; i < NSEG; i++ )
    {
        int index = i;
        double total = 0;
        for( int t = 0; t < N_TAPS; t++ )
        {
            total += m_pts[index].offs * taps[t];
            if( ++index >= NSEG )
                index = 0;
        }

        values[(i + N_TAPS / 2) % NSEG] = total;
    }

    for( int i = 0; i < NSEG; i++ )
    {
        m_pts[i].offs = values[i];
        m_pts[i].pt = m_pts[i].CalcPt();
    }
}

void	Path::SetEstimatedTime( double time )
{
    m_estimatedTime = time;
}

double	Path::GetEstimatedTime() const
{
    return m_estimatedTime;
}

double	Path::CalcEstimatedTime( int start, int len ) const
{
    double	totalTime = 0;

    for( int s = 0; s < len; s++ )
    {
        int		i = (s + start) % NSEG;
        int		j = (i + 1) % NSEG;
        double	dist = Utils::VecLenXY(m_pts[i].CalcPt() -
                                       m_pts[j].CalcPt());
        double	spd = (m_pts[i].accSpd + m_pts[j].accSpd) * 0.5;
        double	time = dist / spd;
        totalTime += time;
    }

    return totalTime;
}

double	Path::CalcEstimatedLapTime() const
{
    double	lapTime = 0;

    for( int i = 0; i < NSEG; i++ )
    {
        int		j = (i + 1) % NSEG;
        double	dist = Utils::VecLenXY(m_pts[i].CalcPt() -
                                       m_pts[j].CalcPt());
        double	spd = (m_pts[i].accSpd + m_pts[j].accSpd) * 0.5;
        double	time = dist / spd;
        lapTime += time;
    }

    return lapTime;
}

double  Path::GetRollAngle( int idx ) const
{
    return m_pts[idx].ar;
}

double  Path::GetPitchAngle( int idx ) const
{
    return m_pts[idx].ap;
}

bool	Path::LoadPath( const char* pDataFile )
{
    LogSHADOW.debug( "Loading \"springs\" data file %s\n", pDataFile );

    FILE*	pFile = fopen(pDataFile, "r");
    if( pFile == 0 )
    {
//		LogSHADOW.debug( "Failed to open data file\n" );
        return false;
    }

    char	buf[1024];
    if( fgets(buf, sizeof(buf), pFile) == NULL ||
        strncmp(buf, "SPRINGS-PATH", 12) != 0 )
    {
//		LogSHADOW.debug( "Failed to open data file -- SPRINGS-PATH\n" );
        fclose( pFile );
        return false;
    }

    int		version = -1;
    if (fgets(buf, sizeof(buf), pFile) == NULL ||
        sscanf(buf, "%d", &version) != 1 ||
        (version != 0 && version != 1 && version != 2))
    {
//		LogSHADOW.debug( "Failed to open data file -- version\n" );
        fclose( pFile );
        return false;
    }

    // versions:
    //	0	- offsets only, assumes slices already known.
    //	1	- dist/offset
    //	2	- global (x, y)

    if( fgets(buf, sizeof(buf), pFile) == NULL ||
        strncmp(buf, "TRACK-LEN", 9) != 0 )
    {
//		LogSHADOW.debug( "Failed to open data file -- TRACK-LEN\n" );
        fclose( pFile );
        return false;
    }

    double	trackLen = 0;
    if( fgets(buf, sizeof(buf), pFile) == NULL ||
        sscanf(buf, "%lf", &trackLen) != 1 ||
        fabs(trackLen - m_pTrack->GetLength()) > 0.01 )
    {
        LogSHADOW.debug( "Failed to open data file -- length %g %g\n",
                trackLen, m_pTrack->GetLength() );
        fclose( pFile );
        return false;
    }

    if( fgets(buf, sizeof(buf), pFile) == NULL ||
        strncmp(buf, "BEGIN-POINTS", 12) != 0 )
    {
        LogSHADOW.debug( "Failed to open data file -- BEGIN-POINTS\n" );
        fclose( pFile );
        return false;
    }

    int		nPoints = 0;
    if( fgets(buf, sizeof(buf), pFile) == NULL ||
        sscanf(buf, "%d", &nPoints) != 1 )
    {
        LogSHADOW.debug( "Failed to open data file -- nPoints\n" );
        fclose( pFile );
        return false;
    }

    vector<Vec2d>	points(nPoints);

    const int required = version == 0 ? 1 : 2;

    {for( int i = 0; i < nPoints; i++ )
    {
        if( fgets(buf, sizeof(buf), pFile) == NULL ||
            sscanf(buf, "%lf %lf", &points[i].x, &points[i].y) < required )
        {
//			LogSHADOW.debug( "Failed to open data file -- point data\n" );
            fclose( pFile );
            return false;
        }
    }}

    fclose( pFile );

    if( version == 0 )
    {
        //
        //	path offsets are saved directly.
        //

        {for( int i = 0; i < NSEG; i++ )
        {
            PathPt*	pp = &m_pts[i];
            if( pp->offs != points[i].x )
            {
                pp->offs = points[i].x;
                pp->pt   = pp->CalcPt();
            }
        }}
    }
    else if( version == 1 )
    {
        //
        //	interpolate points to make path offsets here...
        //

        Vec2d	inP0 = points[0];
        Vec2d	inP1 = points[1];
        int		inP = 1;

        {for( int i = 0; i < NSEG; i++ )
        {
            PathPt*	pp = &m_pts[i];
            while( pp->Dist() > inP1.x )
            {
                inP0 = inP1;
                inP++;
                if( inP < nPoints )
                    inP1 = points[inP];
                else
                {
    //				ASSERT( inP == nPoints );
                    inP1 = points[0];
                    inP1.x = trackLen;
                }
            }

            double	t = (pp->Dist() - inP0.x) / (inP1.x - inP0.x);
            double	w = inP0.y + (inP1.y - inP0.y) * t;

            pp->offs = -w;
            pp->pt   = pp->CalcPt();
        }}
    }
    else	// version == 2
    {
        //
        //	points are global x, y coords that need to be interpolated.
        //

        Vec2d	origin(0, 0);
/*
        {
            //
            tTrkLocPos	pos;
            pos.seg = m_pTrack->GetAt(0).pSeg;
            pos.type = 0;
            pos.toStart = 0;
            pos.toRight = tdble(m_pTrack->GetWidth() / 2);
            pos.toMiddle = 0;
            pos.toLeft = tdble(m_pTrack->GetWidth() / 2);

            float	x, y;
            RtTrackLocal2Global( &pos, &x, &y, 0 );
            LogSHADOW.debug( "global start coords (%g, %g)\n", x, y );

            origin.x = x;
            origin.y = y;
        }
*/
        // work out which slice the last point is in.
        LogSHADOW.debug( "nPoints %d\n", nPoints );
        Vec2d	lastPt = points[nPoints - 1] + origin;
        LogSHADOW.debug( "lastPt (%g, %g)\n", lastPt.x, lastPt.y );
        double	dist = m_pTrack->CalcPos(lastPt.x, lastPt.y);
        LogSHADOW.debug( "dist %g\n", dist );
        int		last_s = m_pTrack->IndexFromPos(dist);

        for( int i = 0; i < nPoints; i++ )
        {
            // work out position.
            Vec2d	pt = points[i] + origin;

            // work out which slice this point is in.
            dist = m_pTrack->CalcPos(pt.x, pt.y, &m_pTrack->GetAt(last_s));
            int	cur_s = m_pTrack->IndexFromPos(dist);

            // if( cur_s == 258 )
            //     int f = 1;

            tTrackSeg*	pSeg = m_pTrack->GetAt(cur_s).pSeg;
            LogSHADOW.debug( "%4d  (%8g,%8g)  seg %4d/%3d%c %d\n",
                    i, pt.x, pt.y, cur_s, pSeg->id,
                    pSeg->type == TR_RGT ? 'R' : pSeg->type == TR_LFT ? 'L' : '-',
                    pSeg->raceInfo );

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
                        t >= 0.0 && t <= 1.0001 )
                    {
//						Rec&	rec = m_pData[next_s];
//						const double	gamma = 0.8;
//						rec.avgW	= rec.avgW * (1 - gamma) + w * gamma;
                        LogSHADOW.debug( "%%%%  w[%d] = %g (was %g)\n", next_s, w, m_pts[next_s].offs );
                        m_pts[next_s].offs = w;
                        m_pts[next_s].pt   = m_pts[next_s].CalcPt();
                    }

                    last_s = next_s;
                    next_s = (next_s + 1) % NSEG;
                }
            }

            last_s = cur_s;
            lastPt = pt;
        }
    }

    CalcAngles();
    CalcCurvaturesXY();
    CalcCurvaturesZ();
    CalcCurvaturesV();
    CalcCurvaturesH();

    LogSHADOW.debug( "\"springs\" data file loaded OK\n" );

    // take some of the "kinks" out of the data.
//	OptimisePath( 1, 100 );
//	OptimisePath( 1, 2 );

    // all done.
    return true;
}

bool	Path::SavePath( const char* pDataFile ) const
{
    LogSHADOW.debug( "Saving \"springs\" data file %s\n", pDataFile );

    FILE*	pFile = fopen(pDataFile, "w");
    if( pFile == 0 )
    {
//		LogSHADOW.debug( "Failed to open data file\n" );
        return false;
    }

    fprintf( pFile, "SPRINGS-PATH\n" );
    fprintf( pFile, "0\n" );

    fprintf( pFile, "TRACK-LEN\n" );
    fprintf( pFile, "%g\n", m_pTrack->GetLength() );

    fprintf( pFile, "BEGIN-POINTS\n" );
#if defined(_MSC_VER) && _MSC_VER < 1800
    fprintf( pFile, "%Iu\n", m_pts.size() );
#else
    fprintf( pFile, "%zu\n", m_pts.size() );
#endif
    for( size_t i = 0; i < m_pts.size(); i++ )
    {
        fprintf( pFile, "%.20g\n", m_pts[i].offs );
    }
    fprintf( pFile, "END-POINTS\n" );

    fclose( pFile );

    // all done.
    return true;
}
