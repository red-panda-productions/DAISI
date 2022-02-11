/***************************************************************************

    file        : ClothoidPath.cpp
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

// ClothoidPath.cpp: implementation of the ClothoidPath class.
//
//////////////////////////////////////////////////////////////////////

#include "ClothoidPath.h"
#include "Utils.h"
#include "LinearRegression.h"

#include <vector>

using namespace std;

// The "SHADOW" logger instance.
extern GfLogger* PLogSHADOW;
#define LogSHADOW (*PLogSHADOW)

int Blah::ccount = 0;
int Blah::dcount = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ClothoidPath::ClothoidPath()
//:	m_safetyLimit(1.5),
//	m_safetyMultiplier(100)
{
    //	m_factors.push_back( 1.005 );
    CalcCachedFactors();
}

ClothoidPath::~ClothoidPath()
{
}

ClothoidPath&	ClothoidPath::operator=( const Path& path )
{
    const ClothoidPath* pClothoidPath = dynamic_cast<const ClothoidPath*>(&path);
    if( pClothoidPath )
        return ClothoidPath::operator=( *pClothoidPath );

    Path::operator=(path);
    m_options = PathOptions();
    CalcCachedFactors();
    return *this;
}

ClothoidPath&	ClothoidPath::operator=( const ClothoidPath& path )
{
    Path::operator=(path);
    m_options = path.m_options;
    CalcCachedFactors();
    return *this;
}

const PathOptions&	ClothoidPath::GetOptions() const
{
    return m_options;
}
/*
void	ClothoidPath::ClearFactors()
{
    m_options.factors.clear();
}

void	ClothoidPath::AddFactor( double factor )
{
    m_options.factors.push_back( factor );
}

void	ClothoidPath::SetFactors( const vector<double>& factors )
{
    m_options.factors = factors;
}

const vector<double>&	ClothoidPath::GetFactors() const
{
    return m_options.factors;
}
*/
void	ClothoidPath::MakeSmoothPath(
        const MyTrack*      pTrack,
        const CarModel&		cm,
        const PathOptions&	opts )
{
    m_options = opts;

    Path::Initialise( pTrack, opts.maxL, opts.maxR );

    CalcCachedFactors();

    const int	NSEG = pTrack->GetSize();

    CalcAngles();
    CalcCurvaturesZ();
    CalcCurvaturesV();
    CalcCurvaturesH();
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
            OptimisePath( cm, step, delta, 0 );
        }
    }
    while( step > 1 );

    OptimisePath( cm, 1, 5, 0 );
    /*
    for( int i = 0; i < opts.quadSmoothIters; i++ )
    {
        for( int j = 0; j < NSEG; j++ )
            QuadraticFilter( j );
    }
*/
    if( opts.bumpMod )
    {
        CalcAngles();
        CalcCurvaturesZ();
        CalcCurvaturesV();
        CalcCurvaturesH();
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
                OptimisePath( cm, step, delta, opts.bumpMod );
                CalcFwdAbsK( fwdRange );
                CalcMaxSpeeds( cm, step );
                PropagateBraking( cm, step );
                PropagateAcceleration( cm, step );
            }
        }
        while( step > 1 );
    }

    CalcAngles();
    CalcCurvaturesZ();
    CalcCurvaturesV();
    CalcCurvaturesH();
}

void    ClothoidPath::CalcCachedFactors()
{
    /*	const int   NSEG = m_pTrack == NULL ? 0 : m_pTrack->GetSize();
    m_factors.resize( NSEG );
    for( int idx = 0; idx < NSEG; idx++ )
    {
        int		fIndex = m_options.factors.size() * idx / NSEG;
        double	factor = m_options.factors[fIndex];
        m_factors[idx] = factor;
    }*/
}

void	ClothoidPath::AnalyseBumps( const CarModel& cm, bool dumpInfo )
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
    //double	dt = 0.1;
    {for( int count = 0; count < 2; count++ )
        {
            int		pi = NSEG - 1;

            for( int i = 0; i < NSEG; i++ )
            {
                //double	oldSz = sz;
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

    for( int count = 0; count < 3; count++ )
    {
        for( int i = 0; i < NSEG; i++ )
        {
            int	j = (i + 1) % NSEG;
            if( m_pts[i].h < m_pts[j].h )
                m_pts[i].h = m_pts[j].h;
        }
    }
}

void	ClothoidPath::SmoothBetween( int step )
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

        Vec3d	p0 = l0->pt;
        Vec3d	p1 = l1->pt;
        Vec3d	p2 = l2->pt;
        Vec3d	p3 = l3->pt;

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

double	ClothoidPath::LimitOffset( const CarModel& cm, double k, double t, const PathPt* l3 ) const
{
    double	marg = cm.WIDTH / 2 + 0.02;//1.0;//1.1
    double	wl  = -MN(m_maxL, l3->Wl()) + marg;
    double	wr  =  MN(m_maxR, l3->Wr()) - marg;
    double	buf = MN(m_options.safetyLimit, m_options.safetyMultiplier * fabs(k));	// a = v*v/r;

    if( k >= 0 )
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
    else
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

    return t;
}

void	ClothoidPath::SetOffset(
        const CarModel&	cm,
        double			k,
        double			t,
        PathPt*			l3,
        const PathPt*	l1,
        const PathPt*	l2,
        const PathPt*	l4,
        const PathPt*	l5 )
{
    double	newT = LimitOffset(cm, k, t, l3);

    l3->offs = newT;
    //	l3->k = k;
    l3->pt = l3->CalcPt();

    if( m_options.apexFactor > 0 )
    {
        double	delta = newT - t;
        if( fabs(delta) < 0.1 && delta * l3->k > 0 )
        {
            delta *= m_options.apexFactor;
            const_cast<PathPt*>(l2)->offs = LimitOffset(cm, l2->k, l2->offs + delta, l2);
            const_cast<PathPt*>(l2)->pt = l2->CalcPt();
            const_cast<PathPt*>(l4)->offs = LimitOffset(cm, l4->k, l4->offs + delta, l4);
            const_cast<PathPt*>(l4)->pt = l4->CalcPt();

            const_cast<PathPt*>(l2)->k = Utils::CalcCurvatureXY(l1->pt, l2->pt, l3->pt);
            const_cast<PathPt*>(l4)->k = Utils::CalcCurvatureXY(l3->pt, l4->pt, l5->pt);
        }
    }

    l3->k = Utils::CalcCurvatureXY(l2->pt, l3->pt, l4->pt);
}

void	ClothoidPath::OptimiseLine(
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

    SetOffset( cm, 0, t, l3, NULL, l2, l4, NULL );
}

void ClothoidPath::Search( const CarModel& cm )
{
    Search( cm, EstimateTimeFunc() );
}

void ClothoidPath::Search( const CarModel& cm, const ICalcTimeFunc& calcTimeFunc )
{
    /*const double LaneMinInt = 1.0;
    const double LaneMinExt = 1.0;
    const double LaneMaxInt = 1.0;
    const double LaneMaxExt = 1.0;*/

    int IndexStep = 128;
    double LaneStep = 0.02;

    const int NSEG = GetSize();

    vector<int> Tries(NSEG);

    for (int Pass = 0; Pass <= 4; Pass++, LaneStep /= 2, IndexStep /= 2)
    {
        int i;

        for (i = NSEG ; --i >= 0;)
            Tries[i] = 0;

        for (i = 0; i < NSEG - IndexStep; i += IndexStep)
        {
            MakeSmoothPath(m_pTrack, cm, PathOptions());
            CalcMaxSpeeds( cm );
            PropagateBraking( cm );
            PropagateAcceleration( cm );

            PathPt* pp = &m_pts[i];
            //double s0 = EstimateSpeed();
            //double tm0 = CalcEstimatedLapTime();
            double tm0 = calcTimeFunc(*this);
            //LogSHADOW.info("Optimizing lane number " << i << " / " << NSEG << " (pass " << Pass << ")");
            //LogSHADOW.info("Reference estimated lap speed = " << s0 << " mph");
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

                if( l < -(pp->Wl() - pp->lBuf) || l > (pp->Wr() + pp->rBuf) )
                    break;

                pp->offs = l;
                pp->pt = pp->CalcPt();

                MakeSmoothPath(m_pTrack, cm, PathOptions());
                CalcMaxSpeeds( cm );
                PropagateBraking( cm );
                PropagateAcceleration( cm );

                double tm = calcTimeFunc(*this);

                if( tm < tm0 )
                {
                    BestLane = pp->offs;
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

            pp->offs = BestLane;
            pp->pt = pp->CalcPt();
            Tries[i]++;

            if (BestLane == RefLane)
                pp->fixed = OldfConst != 0;
            else if (Tries[i] <= 5 && i > 0)
                i -= 2 * IndexStep;
        }
    }
}

void	ClothoidPath::Optimise(
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
            factor = Map(1.0 / fabs(l3->fwdK), 20, 100, 1.003, 1.05);
    }

    Vec3d	p0 = l0->pt;
    Vec3d	p1 = l1->pt;
    Vec3d	p2 = l2->pt;
    Vec3d	p3 = l3->pt;
    Vec3d	p4 = l4->pt;
    Vec3d	p5 = l5->pt;
    Vec3d	p6 = l6->pt;

    double	k1 = Utils::CalcCurvatureXY(p1, p2, p3);
    double	k2 = Utils::CalcCurvatureXY(p3, p4, p5);

    double	length1 = hypot(p3.x - p2.x, p3.y - p2.y);
    double	length2 = hypot(p4.x - p3.x, p4.y - p3.y);

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
    double	maxSpdK = 0.0015;

    if( k1 * k2 >= 0 && fabs(k1) < maxSpdK && fabs(k2) < maxSpdK )
    {
        k *= 0.9;
    }

    double sc = 1.0;
    double	t = l3->offs;

    Utils::LineCrossesLineXY( l3->Pt(), l3->Norm(), p2, p4 - p2, t );

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
    }

    SetOffset( cm, k, t, l3, l1, l2, l4, l5 );
}

void	ClothoidPath::OptimisePath(
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
                //				double  factor = m_factors[idx];

                if( bumpMod == 2 && l3->h > 0.1 )
                    OptimiseLine( cm, idx, step, 0.1, l3, l2, l4 );
                else
                    Optimise( cm, factor, idx, l3, l0, l1, l2, l4, l5, l6, bumpMod );
            }

            if( (i += step) >= NSEG )
                i = 0;//i -= m_nSegs;
        }
    }

    // now smooth the values between steps
    if( step > 1 )
        SmoothBetween( step );
}

static int nextInSection( int* current, int start, int len, int step, int NSEG )
{
    int next = (*current + step) % NSEG;

    int curr_relative_to_start = (*current - start + NSEG) % NSEG;
    int next_relative_to_start = (next     - start + NSEG) % NSEG;
    if( curr_relative_to_start < len && next_relative_to_start > len )
    {
        next = (start + len) % NSEG;
    }

    *current = next;
    return next;
}

// optimise a section EXCLUDING the start point and the (start+len) point, using the given step.
void	ClothoidPath::OptimisePathSection(
        const CarModel&		cm,
        int					start,
        int					len,
        int					step,
        const PathOptions&	options )
{
    m_options = options;

    const int	NSEG = m_pTrack->GetSize();

    // -2 (as opposed to -3) as 1st point to be modified is NOT start, but start+step
    int current = (start + NSEG - 2 * step) % NSEG;

    PathPt*	l0 = 0;
    PathPt*	l1 = &m_pts[current];
    PathPt*	l2 = &m_pts[nextInSection(&current, start, len, step, NSEG)];
    PathPt*	l3 = &m_pts[nextInSection(&current, start, len, step, NSEG)];
    PathPt*	l4 = &m_pts[nextInSection(&current, start, len, step, NSEG)];
    PathPt*	l5 = &m_pts[nextInSection(&current, start, len, step, NSEG)];
    PathPt*	l6 = &m_pts[nextInSection(&current, start, len, step, NSEG)];

    // go forwards
    int		n = (len - 1) / step;

    for( int count = 0; count < n; count++ )
    {
        l0 = l1;
        l1 = l2;
        l2 = l3;
        l3 = l4;
        l4 = l5;
        l5 = l6;
        l6 = &m_pts[nextInSection(&current, start, len, step, NSEG)];

        int i = current;

        if( !l3->fixed )
        {
            int		idx = (i + NSEG - 3 * step) % NSEG;
            int		fIndex = m_options.factors.size() * idx / NSEG;
            double	factor = m_options.factors[fIndex];
            //			double  factor = m_factors[idx];

            if( m_options.bumpMod == 2 && l3->h > 0.1 )
                OptimiseLine( cm, idx, step, 0.1, l3, l2, l4 );
            else
                Optimise( cm, factor, idx, l3, l0, l1, l2, l4, l5, l6, m_options.bumpMod );
        }
    }

    // now smooth the values between steps
    if( step > 1 )
        InterpolateBetweenLinearSection( cm, start, len, step );
}
