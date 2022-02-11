/***************************************************************************

    file        : SpringsPath.cpp
    created     : 4 Feb 2018
    copyright   : (C) 2018 Tim Foden

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// SpringsPath.cpp: implementation of the SpringsPath class.
//
//////////////////////////////////////////////////////////////////////

#include "SpringsPath.h"
#include "Utils.h"
#include "LinearRegression.h"

#include <vector>

using namespace std;

// The "SHADOW" logger instance.
extern GfLogger* PLogSHADOW;
#define LogSHADOW (*PLogSHADOW)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SpringsPath::SpringsPath()
//:	m_safetyLimit(1.5),
//	m_safetyMultiplier(100)
{
//	m_factors.push_back( 1.005 );
}

SpringsPath::~SpringsPath()
{
}

SpringsPath&	SpringsPath::operator=( const Path& path )
{
    const SpringsPath* pSpringsPath = dynamic_cast<const SpringsPath*>(&path);
    if( pSpringsPath )
        return SpringsPath::operator=( *pSpringsPath );

    Path::operator=(path);
    m_options = PathOptions();
    return *this;
}

SpringsPath&	SpringsPath::operator=( const SpringsPath& path )
{
    Path::operator=(path);
    m_options = path.m_options;
    return *this;
}

const PathOptions&	SpringsPath::GetOptions() const
{
    return m_options;
}
/*
void	SpringsPath::ClearFactors()
{
    m_options.factors.clear();
}

void	SpringsPath::AddFactor( double factor )
{
    m_options.factors.push_back( factor );
}

void	SpringsPath::SetFactors( const vector<double>& factors )
{
    m_options.factors = factors;
}

const vector<double>&	SpringsPath::GetFactors() const
{
    return m_options.factors;
}
*/
void	SpringsPath::MakeSmoothPath(
    const MyTrack*      pTrack,
    const CarModel&	    cm,
    const PathOptions&	opts )
{
    m_options = opts;

    Path::Initialise( pTrack, opts.maxL, opts.maxR );

    const int	NSEG = pTrack->GetSize();

    CalcCurvaturesZ();
    int	fwdRange = 110;
    CalcFwdAbsK( fwdRange );

    const int delta = 25;
    const int n = (150 + delta - 1) / delta;

    int		step = 1;
    while( step * 4 < NSEG )
        step *= 2;

    do
    {
        step = (step + 1) / 2;
//		int n = 100 * int(sqrt(step));
        for( int i = 0; i < n; i++ )
        {
//			OptimisePathTopLevel( cm, step, delta, 0 );
            OptimisePathSprings( cm, step, delta, 0 );
        }
    }
    while( step > 1 );

//	OptimisePathTopLevel( cm, 1, 5, 0 );
    OptimisePathSprings( cm, 1, 5, 0 );

    for( int i = 0; i < opts.quadSmoothIters; i++ )
    {
        for( int j = 0; j < NSEG; j++ )
            QuadraticFilter( j );
    }

    if( opts.bumpMod )
    {
        CalcCurvaturesZ();
        CalcFwdAbsK( fwdRange );
//		AnalyseBumps( cm, true );
        AnalyseBumps( cm, false );

        step = 8;
//		while( step * 4 < NSEG )
//			step *= 2;

        do
        {
            step = (step + 1) / 2;
//			int n = 100 * int(sqrt(step));
            for( int i = 0; i < n; i++ )
            {
                OptimisePathTopLevel( cm, step, delta, opts.bumpMod );
                CalcFwdAbsK( fwdRange );
                CalcMaxSpeeds( cm, step );
                PropagateBraking( cm, step );
                PropagateAcceleration( cm, step );
            }
        }
        while( step > 1 );
    }

    CalcCurvaturesZ();
}

void	SpringsPath::AnalyseBumps( const CarModel& cm, bool dumpInfo )
{
    // here we look at the bumps on the track, to try to estimate the height
    // of the car off the ground (h.)

    // get an estimate of speeds.
    CalcMaxSpeeds( cm );
    PropagateBraking( cm );
    PropagateAcceleration( cm );

    const int		NSEG = m_pTrack->GetSize();
    const double	g = -9.81;

    double	sz = m_pts[0].pt.z;
    double	vz = 0;
    double	pz = sz;
    // double	dt = 0.1;
    {for( int count = 0; count < 2; count++ )
    {
        int		pi = NSEG - 1;

        for( int i = 0; i < NSEG; i++ )
        {
            // double	oldSz = sz;
            double	oldPz = pz;

            double	v = (m_pts[i].accSpd + m_pts[pi].accSpd) * 0.5;
            double	s = Utils::VecLenXY(m_pts[i].pt - m_pts[pi].pt);
            double	dt = s / v;

            pz = m_pts[i].pt.z;
            sz += vz * dt + 0.5 * g * dt * dt;
            vz += g * dt;

            if( sz <= pz )
            {
                double	newVz = (pz - oldPz) / dt;
                if( vz < newVz )
                    vz = newVz;
                sz = pz;
            }

            double	h = sz - pz;
            m_pts[i].h = h;

            if( count == 1 && dumpInfo )
            {
                LogSHADOW.debug( "###  %4d  spd %3.0f k %7.4f dt %.3f pz %5.2f sz %5.2f vz %5.2f -> h %5.2f\n",
                        i, m_pts[i].accSpd * 3.6, m_pts[i].k, dt,
                        pz, sz, vz, m_pts[i].h );
            }

            pi = i;
        }
    }}

    {for( int count = 0; count < 3; count++ )
    {
        {for( int i = 0; i < NSEG; i++ )
        {
            int	j = (i + 1) % NSEG;
            if( m_pts[i].h < m_pts[j].h )
                m_pts[i].h = m_pts[j].h;
        }}
/*
        {for( int i = NSEG - 1; i >= 0; i-- )
        {
            int	j = (i + NSEG - 1) % NSEG;
            if( m_pts[i].h < m_pts[j].h )
                m_pts[i].h = m_pts[j].h;
        }}*/
    }}
}

void	SpringsPath::SmoothBetween( int step )
{
    const int	NSEG = m_pTrack->GetSize();

    // now smooth the values between steps
    PathPt*	l0 = 0;
    PathPt*	l1 = &m_pts[((NSEG - 1) / step) * step];
    PathPt*	l2 = &m_pts[0];
    PathPt*	l3 = &m_pts[step];

    int		j = 2 * step;
    for( int i = 0; i < NSEG; i += step )
    {
        l0 = l1;
        l1 = l2;	// l1 represents m_pLines[i];
        l2 = l3;
        l3 = &m_pts[j];

        j += step;
        if( j >= NSEG )
            j = 0;

        Vec3d	p0 = l0->pt;//CalcPt();
        Vec3d	p1 = l1->pt;//CalcPt();
        Vec3d	p2 = l2->pt;//CalcPt();
        Vec3d	p3 = l3->pt;//CalcPt();

        double	k1 = Utils::CalcCurvatureXY(p0, p1, p2);
        double	k2 = Utils::CalcCurvatureXY(p1, p2, p3);

        if( i + step > NSEG )
            step = NSEG - i;

        for( int k = 1; k < step; k++ )
        {
            double	t;
            PathPt&	l = m_pts[(i + k) % NSEG];
            Utils::LineCrossesLineXY( l.Pt(), l.Norm(), p1, p2 - p1, t );
            l.offs = t;

            double	len1 = (l.CalcPt() - p1).len();
            double	len2 = (l.CalcPt() - p2).len();
            double	kappa = (k1 * len2 + k2 * len1) / (len1 + len2);

            if( kappa != 0 )
            {
                double	delta = 0.0001;
                double	deltaK = Utils::CalcCurvatureXY(
                                        p1, l.Pt() + l.Norm() * (t + delta), p2);
                t += delta * kappa / deltaK;
            }

            const double buf = 1.0;//1.25;
            if( t < -l.Wl() + l.lBuf + buf )
                t = -l.Wl() + l.lBuf + buf;
            else if( t > l.Wr() - l.rBuf - buf )
                t = l.Wr() - l.rBuf - buf;

            if( t < -m_maxL + l.lBuf + buf )
                t = -m_maxL + l.lBuf + buf;
            else if( t > m_maxR - l.rBuf - buf )
                t = m_maxR - l.rBuf - buf;

            l.offs = t;
            l.pt = l.CalcPt();
        }
    }
}

static double Map( double value, double fromLo, double fromHi, double toLo, double toHi )
{
    double	newValue = (value - fromLo) / (fromHi - fromLo) * (toHi - toLo) + toLo;
    if( newValue < toLo )
        newValue = toLo;
    else if( newValue > toHi )
        newValue = toHi;
    return newValue;
}

void	SpringsPath::SetOffset(
    const CarModel&	cm,
    double			k,
    double			t,
    PathPt*			l3,
    const PathPt*	l2,
    const PathPt*	l4 )
{
    double	marg = cm.WIDTH / 2 + 0.02;//1.0;//1.1
    double	wl  = -MN(m_maxL, l3->Wl()) + marg;
    double	wr  =  MN(m_maxR, l3->Wr()) - marg;
//	double	buf = MN(1.5, 100 * fabs(k));	// a = v*v/r;
//	double	buf = MN(2.5, 200 * fabs(k));	// a = v*v/r;
    double	buf = MN(m_options.safetyLimit, m_options.safetyMultiplier * fabs(k));	// a = v*v/r;
//	double	buf = 0.5;//MN(1.5, 100 * fabs(k));	// a = v*v/r;

    if( k >= 0 )// 0.00001 )
    {
        if( t < wl )
            t = wl;
        else if( t > wr - l3->rBuf - buf )
        {
            if( l3->offs > wr - l3->rBuf - buf )
                t = MN(t, l3->offs);
            else
                t = wr - l3->rBuf - buf;
            t = MN(t, wr);
        }
    }
    else //if( k < -0.00001 )
    {
        if( t > wr )
            t = wr;
        else if( t < wl + l3->lBuf + buf )
        {
            if( l3->offs < wl + l3->lBuf + buf )
                t = MX(t, l3->offs);
            else
                t = wl + l3->lBuf + buf;
            t = MX(t, wl);
        }
    }

//	if( t < -m_maxL + lMargin )
//		t = -m_maxL + lMargin;
//	else if( t > m_maxR - rMargin )
//		t = m_maxR - rMargin;

    l3->offs = t;
//	l3->k = k;
    l3->pt = l3->CalcPt();
    l3->k = Utils::CalcCurvatureXY(l2->pt, l3->pt, l4->pt);
}

void	SpringsPath::SetOffset(
    const CarModel&	cm,
    int				index,
    double			t )
{
    PathPt* pPt = &m_pts[index];

    double	marg = cm.WIDTH / 2 + 0.02;//1.0;//1.1
    double	wl   = -MN(m_maxL, pPt->Wl()) + marg;
    double	wr   =  MN(m_maxR, pPt->Wr()) - marg;
    double	buf  = MN(m_options.safetyLimit, m_options.safetyMultiplier * fabs(pPt->k));	// a = v*v/r;

    if( pPt->k >= 0 )// 0.00001 )
    {
        if( t < wl )
            t = wl;
        else if( t > wr - pPt->rBuf - buf )
            t = wr - pPt->rBuf - buf;
    }
    else //if( k < -0.00001 )
    {
        if( t > wr )
            t = wr;
        else if( t < wl + pPt->lBuf + buf )
            t = wl + pPt->lBuf + buf;
    }

    pPt->offs = t;
    pPt->pt = pPt->CalcPt();
}

void	SpringsPath::OptimisePtLinear(
    const CarModel& cm,
    int				idx,
    int				step,
    double			hLimit,
    PathPt*			l3,
    const PathPt*	l2,
    const PathPt*	l4 )
{
    LinearRegression	l;

    const int NSEG = m_pTrack->GetSize();

    int i = (idx + NSEG - step) % NSEG;
    while( m_pts[i].h > hLimit )
    {
        l.Sample( m_pts[i].pt.GetXY() );
        i = (i + NSEG - step) % NSEG;
    }

    l.Sample( m_pts[i].pt.GetXY() );

    i = idx;
    while( m_pts[i].h > hLimit )
    {
        l.Sample( m_pts[i].pt.GetXY() );
        i = (i + step) % NSEG;
    }

    l.Sample( m_pts[i].pt.GetXY() );

    LogSHADOW.debug( "%4d  ", idx );
    Vec2d	p, v;
    l.CalcLine( p, v );

    double	t;
    Utils::LineCrossesLine( l3->Pt().GetXY(), l3->Norm().GetXY(), p, v, t );

    SetOffset( cm, 0, t, l3, l2, l4 );
}

void SpringsPath::Search( const CarModel& cm )
{
    Search( cm, EstimateTimeFunc() );
}

void SpringsPath::Search( const CarModel& cm, const ICalcTimeFunc& calcTimeFunc )
{
    // const double LaneMinInt = 1.0;//SideDistInt / Track.width;
    // const double LaneMinExt = 1.0;//SideDistExt / Track.width;
    // const double LaneMaxInt = 1.0;//1 - LaneMinInt;
    // const double LaneMaxExt = 1.0;//1 - LaneMinExt;

    int IndexStep = 128;
    double LaneStep = 0.02;

    const int NSEG = GetSize();

    vector<int> Tries(NSEG);

    for (int Pass = 0; Pass <= 4; Pass++, LaneStep /= 2, IndexStep /= 2)
    {
//		int Tries[MaxDivs];
        int i;

        for (i = NSEG ; --i >= 0;)
            Tries[i] = 0;

        for (i = 0; i < NSEG - IndexStep; i += IndexStep)
        {
            //ComputeFullPath(CK1999Path::K1999);
            MakeSmoothPath(m_pTrack, cm, PathOptions());
            CalcMaxSpeeds( cm );
            PropagateBraking( cm );
            PropagateAcceleration( cm );

            PathPt* pp = &m_pts[i];
            //double s0 = EstimateSpeed();
            //double tm0 = CalcEstimatedLapTime();
            double tm0 = calcTimeFunc(*this);
            //OUTPUT("Optimizing lane number " << i << " / " << NSEG << " (pass " << Pass << ")");
            //OUTPUT("Reference estimated lap speed = " << s0 << " mph");
            int OldfConst = pp->fixed;
            pp->fixed = 1;
            double RefLane = pp->offs;//tLane[i];
            double BestLane = pp->offs;//tLane[i];

            int Dir = -1;
            int j = 0;
            while(1)
            {
                j++;
                double l = RefLane + j * Dir * LaneStep;
                //if ((tCurvature[i] > 0 && (l < LaneMinInt || l > LaneMaxExt)) ||
                //	(tCurvature[i] < 0 && (l > LaneMaxInt || l < LaneMinInt)))
                if( l < -(pp->Wl() - pp->lBuf) || l > (pp->Wr() + pp->rBuf) )
                    break;
                //tLane[i] = l;
                pp->offs = l;
                //UpdateTxTy(i);
                pp->pt = pp->CalcPt();
                //ComputeFullPath(CK1999Path::K1999);
                MakeSmoothPath(m_pTrack, cm, PathOptions());
                CalcMaxSpeeds( cm );
                PropagateBraking( cm );
                PropagateAcceleration( cm );

                //double s = EstimateSpeed();
                //double tm = CalcEstimatedLapTime();
                double tm = calcTimeFunc(*this);
                //OUTPUT("Lane = " << tLane[i] << " Estimated speed = " << s << " mph");
                //if (s > s0)
                if( tm < tm0 )
                {
                    BestLane = pp->offs;//tLane[i];
                    //s0 = s;
                    tm0 = tm;
                }
                else
                {
                    if (j == 1 && Dir < 0)
                        Dir = 1;
                    else
                        break;
                    j = 0;
                }
            }
            //tLane[i] = BestLane;
            pp->offs = BestLane;
            //UpdateTxTy(i);
            pp->pt = pp->CalcPt();
            Tries[i]++;

            if (BestLane == RefLane)
                //tfConst[i] = OldfConst;
                pp->fixed = OldfConst != 0;
            else if (Tries[i] <= 5 && i > 0)
                i -= 2 * IndexStep;
        }
    }
}

void	SpringsPath::OptimisePtClothoid(
    const CarModel&	cm,
    double			factor,
    int				idx,
    PathPt*			l3,
    const PathPt*	l0,
    const PathPt*	l1,
    const PathPt*	l2,
    const PathPt*	l4,
    const PathPt*	l5,
    const PathPt*	l6,
    int				bumpMod )
{
    if( factor == 0 )
    {
        if( fabs(l3->fwdK) < 1 / 100 )
            factor = 1.05;
        else
//			factor = 1.003 + ((1.0 / fabs(l3->k)) - 35) * 0.00035;
//			factor = 1.003 + ((1.0 / fabs(l3->k)) - 25) * 0.0006;
//			factor = Map(fabs(l3->k), 1.0/25, 1.0/200, 1.003, 1.03);
//			factor = Map(1.0 / fabs(l3->k), 34, 100, 1.003, 1.03);// alpine 1:55.87
//			factor = Map(1.0 / fabs(l3->k), 40, 100, 1.004, 1.03);// alpine 1:55.87
//			factor = Map(1.0 / fabs(l3->k), 20, 80, 1.003, 1.03);// generally good
            factor = Map(1.0 / fabs(l3->fwdK), 20, 100, 1.003, 1.05);
//			factor = Map(fabs(l3->k), 1.0/30, 1.0/100, 1.003, 1.03);
    }

    Vec3d	p0 = l0->pt;//CalcPt();
    Vec3d	p1 = l1->pt;//CalcPt();
    Vec3d	p2 = l2->pt;//CalcPt();
    Vec3d	p3 = l3->pt;//CalcPt();
    Vec3d	p4 = l4->pt;//CalcPt();
    Vec3d	p5 = l5->pt;//CalcPt();
    Vec3d	p6 = l6->pt;//CalcPt();

    double	k1 = Utils::CalcCurvatureXY(p1, p2, p3);
    double	k2 = Utils::CalcCurvatureXY(p3, p4, p5);

    double	length1 = hypot(p3.x - p2.x, p3.y - p2.y);
    double	length2 = hypot(p4.x - p3.x, p4.y - p3.y);
/*
    {
        // adjust length to arc length...
        double	KK = l3->k;//Utils::CalcCurvatureXY(p2, p3, p4);
        double	K1 = fabs(KK + k1) * 0.5;
        double	K2 = fabs(KK + k2) * 0.5;
        if( K1 > 0.001 )
            length1 = 2 * asin(length1 * K1 / 2) / K1;
        if( K2 > 0.001 )
            length2 = 2 * asin(length2 * K2 / 2) / K2;
    }
*/
    if( k1 * k2 > 0 )
    {
        double	k0 = Utils::CalcCurvatureXY(p0, p1, p2);
        double	k3 = Utils::CalcCurvatureXY(p4, p5, p6);
        if( k0 * k1 > 0 && k2 * k3 > 0 )
        {
            if( fabs(k0) < fabs(k1) && fabs(k1) * 1.02 < fabs(k2) )
            {
                k1 *= factor;
                k0 *= factor;
            }
            else if( fabs(k0) > fabs(k1) * 1.02 && fabs(k1) > fabs(k2) )
            {
                k1 *= factor;
                k0 *= factor;
            }
        }
    }
    else if( k1 * k2 < 0 )
    {
        double	k0 = Utils::CalcCurvatureXY(p0, p1, p2);
        double	k3 = Utils::CalcCurvatureXY(p4, p5, p6);
        if( k0 * k1 > 0 && k2 * k3 > 0 )
        {
            if( fabs(k1) < fabs(k2) && fabs(k1) < fabs(k3) )
            {
                k1 = (k1 * 0.25 + k2 * 0.75);
                k0 = (k0 * 0.25 + k3 * 0.75);
            }
            else if( fabs(k2) < fabs(k1) && fabs(k2) < fabs(k0) )
            {
                k2 = (k2 * 0.25 + k1 * 0.75);
                k3 = (k3 * 0.25 + k0 * 0.75);
            }
        }
    }

    double	k = (length2 * k1 + length1 * k2) / (length1 + length2);

//	double	maxSpdK = cm.CalcMaxSpdK();
    double	maxSpdK = 0.00175;//60 / (50 * 50);	// a = vv/r; r = vv/a; k = a/vv;
    if( k1 * k2 >= 0 && fabs(k1) < maxSpdK && fabs(k2) < maxSpdK )
    {
//		k = 0;//(fabs(k1) < fabs(k2) ? k1 : k2);
        k *= 0.9;
    }

    double sc = 1.0;
/*
    if( idx > 1240 && idx < 1280 && fabs(k) > 0.013 )
    {
        k = SGN(k) * 0.013;
        sc = 1.08;
    }
*/
    double	t = l3->offs;
    // double	oldT = t;
    Utils::LineCrossesLineXY( l3->Pt(), l3->Norm(), p2, p4 - p2, t );
//	if( l3->h < 0.1 )
    {
        double	delta = 0.0001;
        double	deltaK = Utils::CalcCurvatureXY(p2, l3->Pt() + l3->Norm() * (t + delta), p4);

        if( bumpMod == 1 )
        {
            double	f = l3->h <= 0.07 ? 1.00 :
                        l3->h <= 0.10 ? 0.97 :
                        l3->h <= 0.20 ? 0.90 :
                        l3->h <= 0.30 ? 0.80 : 0.70;
            delta *= f;
        }

        t += sc * delta * k / deltaK;
        //t += k * 0.05;
        //t += k * 0.03;
        //t += k * 0.01;
        //t += k * 0.1;
        //t += k / 15;
    }
/*
    double	marg = cm.WIDTH / 2 + 0.02;//1.0;//1.1
    double	wl  = -MN(m_maxL, l3->Wl()) + marg;
    double	wr  =  MN(m_maxR, l3->Wr()) - marg;
    double	buf = MN(1.5, 100 * fabs(k));	// a = v*v/r;
//	double	buf = 0.5;//MN(1.5, 100 * fabs(k));	// a = v*v/r;

    if( k >= 0 )// 0.00001 )
    {
        if( t < wl )
            t = wl;
        else if( t > wr - l3->rBuf - buf )
        {
            if( oldT > wr - l3->rBuf - buf )
                t = MN(t, oldT);
            else
                t = wr - l3->rBuf - buf;
            t = MN(t, wr);
        }
    }
    else //if( k < -0.00001 )
    {
        if( t > wr )
            t = wr;
        else if( t < wl + l3->lBuf + buf )
        {
            if( oldT < wl + l3->lBuf + buf )
                t = MX(t, oldT);
            else
                t = wl + l3->lBuf + buf;
            t = MX(t, wl);
        }
    }

//	if( t < -m_maxL + lMargin )
//		t = -m_maxL + lMargin;
//	else if( t > m_maxR - rMargin )
//		t = m_maxR - rMargin;

    l3->offs = t;
    l3->pt = l3->CalcPt();
//	l3->k = k;
    l3->k = Utils::CalcCurvatureXY(p2, l3->pt, p4);*/
    SetOffset( cm, k, t, l3, l2, l4 );
}

void SpringsPath::ResetSpringVelocities()
{
    if( m_temp.size() != NSEG )
    {
        m_temp.resize( NSEG );
    }

    for( int i = 0; i < NSEG; i += 1 )
    {
        m_temp[i].old_offs = m_pts[i].offs;
    }
}

//static const double SPRING_STRENGTH = 150;
//static const double SPRING_STRENGTH = 60;
//static const double SPRING_STRENGTH = 30;
static const double SPRING_STRENGTH = 15;
static const double MASS = 1.0;
static const double DT = 0.1; //0.2 //0.1 //0.0333
static const double CD = 500.0; //0.1

void SpringsPath::OptimisePathSprings(
    const CarModel&	cm,
    int				step,
    int				nIterations,
    int				bumpMod )
{
    const int	NSEG = m_pTrack->GetSize();

    if( m_temp.size() != NSEG )
    {
        m_temp.resize( NSEG );
        ResetSpringVelocities();
    }

    for( int iterCount = 0; iterCount < nIterations; iterCount++ )
    {
        for( int i = 0; i < NSEG; i += step )
        {
            int p = i - step >= 0    ? i - step : (NSEG - 1) / step * step;
            int n = i + step <  NSEG ? i + step : 0;
            m_pts[i].k = Utils::CalcCurvatureXY(m_pts[p].pt, m_pts[i].pt, m_pts[n].pt);
            Vec2d v = m_pts[i].pt.GetXY() - m_pts[n].pt.GetXY();
            m_temp[i].rlen = 1.0 / v.len();
            m_temp[i].pnorm = Vec2d(v * m_temp[i].rlen).GetNormal();
            m_temp[i].force = Vec2d();
        }

        for( int i = 0; i < NSEG; i += step )
        {
            int p = i - step >= 0    ? i - step : (NSEG - 1) / step * step;
            int n = i + step <  NSEG ? i + step : 0;
            double torque = m_pts[i].k * SPRING_STRENGTH;
//															   street-1
//			torque = SGN(torque) * pow(abs(torque), 1.8);	// 74.718
//			torque = SGN(torque) * pow(abs(torque), 1.9);	// 74.681
//			torque = SGN(torque) * pow(abs(torque), 2.0);	// 74.601
            torque = SGN(torque) * pow(abs(torque), 2.1);	// 74.544
//			torque = SGN(torque) * pow(abs(torque), 2.2);	// 74.497
//			torque = SGN(torque) * pow(abs(torque), 2.3);	// 74.496
//			torque = SGN(torque) * pow(abs(torque), 2.4);	// 74.501
//			torque = torque + SGN(torque) * pow(abs(torque), 3);
//			torque = torque + 5 * SGN(torque) * pow(abs(torque), 1.5);
            double pf = torque * m_temp[p].rlen;
            double nf = torque * m_temp[i].rlen;
            m_temp[p].force += m_temp[p].pnorm * pf;
            m_temp[n].force += m_temp[i].pnorm * nf;
            m_temp[i].force -= m_temp[p].pnorm * pf + m_temp[i].pnorm * nf;
        }

        for( int i = 0; i < NSEG; i += step )
        {
            int p = i - step >= 0    ? i - step : (NSEG - 1) / step * step;
            int n = i + step <  NSEG ? i + step : 0;

            double v_offs = m_pts[i].offs - m_temp[i].old_offs;
            double force = m_temp[i].force * m_pts[i].Norm().GetXY();
            force -= v_offs * abs(v_offs) * CD;
            double accel = force / MASS;

            double new_offs = 2 * m_pts[i].offs - m_temp[i].old_offs + accel * DT * DT;
    //		double new_offs = m_pts[i].offs + accel * DT * DT;

            if( new_offs < -m_pts[i].Wl() )
                new_offs = -m_pts[i].Wl();
            else if( new_offs > m_pts[i].Wr() )
                new_offs = m_pts[i].Wr();

            m_temp[i].old_offs = m_pts[i].offs;
            m_pts[i].offs = new_offs;
            m_pts[i].pt = m_pts[i].CalcPt();
            m_pts[i].k = Utils::CalcCurvatureXY(m_pts[p].pt, m_pts[i].pt, m_pts[n].pt);
            SetOffset( cm, i, new_offs );
        }
    }
}

void	SpringsPath::OptimisePathTopLevel(
    const CarModel&	cm,
    int				step,
    int				nIterations,
    int				bumpMod )
{
    const int	NSEG = m_pTrack->GetSize();

    for( int j = 0; j < nIterations; j++ )
    {
        PathPt*	l0 = 0;
        PathPt*	l1 = &m_pts[NSEG - 3 * step];
        PathPt*	l2 = &m_pts[NSEG - 2 * step];
        PathPt*	l3 = &m_pts[NSEG - step];
        PathPt*	l4 = &m_pts[0];
        PathPt*	l5 = &m_pts[step];
        PathPt*	l6 = &m_pts[2 * step];

        // go forwards
        int		i = 3 * step;
        int		n = (NSEG + step - 1) / step;
        for( int count = 0; count < n; count++ )
        {
            l0 = l1;
            l1 = l2;
            l2 = l3;
            l3 = l4;
            l4 = l5;
            l5 = l6;
            l6 = &m_pts[i];

            if( !l3->fixed )
            {
                int		idx = (i + NSEG - 3 * step) % NSEG;
                int		fIndex = m_options.factors.size() * idx / NSEG;
                double	factor = m_options.factors[fIndex];


                if( bumpMod == 2 && l3->h > 0.1 )
                    OptimisePtLinear( cm, idx, step, 0.1, l3, l2, l4 );
                else
                    OptimisePtClothoid( cm, factor, idx, l3, l0, l1, l2, l4, l5, l6, bumpMod );
/*
                //<attstr name="line 0" val="620,11,-820,1283.3,0.53705">
                if( idx >= 620 && idx <= 630 )
                {
                    //OptimisePtLinear( cm, idx, step, l3, l2, l4 );
                    double	ang = 0.76920668422857;
                    Vec2d	p(-738.63190589505,1346.5);
//	p.x = -738.63190589505;
//	p.y = 1346.5;
//	ang = 0.76920668422857;
                    Vec2d	v(cos(ang), sin(ang));
                    double	t;
                    Utils::LineCrossesLine( l3->Pt().GetXY(), l3->Norm().GetXY(), p, v, t );
                    SetOffset( cm, 0, t, l3, l2, l4 );
                }
                else
                    OptimisePtClothoid( cm, factor, idx, l3, l0, l1, l2, l4, l5, l6, bumpMod );
*/
            }

            if( (i += step) >= NSEG )
                i = 0;//i -= m_nSegs;
        }
    }

    // now smooth the values between steps
    if( step > 1 )
        SmoothBetween( step );
}
