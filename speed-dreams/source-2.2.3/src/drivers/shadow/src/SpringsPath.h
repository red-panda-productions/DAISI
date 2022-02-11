/***************************************************************************

    file        : SpringsPath.h
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

// SpringsPath.h: interface for the SpringsPath class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SPRINGSPATH_H_
#define _SPRINGSPATH_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "Path.h"
#include "PathOptions.h"
#include "MyTrack.h"

class SpringsPath : public Path
{
public:
    enum
    {
        FLAG_FLYING		= 0x01,
    };

    class ICalcTimeFunc
    {
    public:
        virtual double operator()( const Path& path ) const = 0;
    };

    class EstimateTimeFunc : public ICalcTimeFunc
    {
    public:
        virtual double operator()( const Path& path ) const
        {
            return path.CalcEstimatedLapTime();
        }
    };

public:
    SpringsPath();
    virtual ~SpringsPath();

    virtual SpringsPath&	operator=( const Path& other );
    SpringsPath&			operator=( const SpringsPath& other );

    const PathOptions&	GetOptions() const;

    void	MakeSmoothPath( const MyTrack* pTrack, const CarModel& cm,
                            const PathOptions& opts );

    void	Search( const CarModel& cm );
    void	Search( const CarModel& cm, const ICalcTimeFunc& calcTimeFunc );

private:
    void	AnalyseBumps( const CarModel& cm, bool dumpInfo = false );
    void	SmoothBetween( int step );
    using Path::SetOffset;
    void	SetOffset( const CarModel& cm, double k, double t,
                       PathPt* l3, const PathPt* l2, const PathPt* l4 );
    void	SetOffset( const CarModel&	cm, int index, double t );
    void	OptimisePtLinear( const CarModel& cm, int idx, int step, double hLimit,
                              PathPt* l3, const PathPt* l2, const PathPt* l4 );
    void	OptimisePtClothoid(	const CarModel& cm, double factor,
                                int idx, PathPt* l3,
                                const PathPt* l0, const PathPt* l1,
                                const PathPt* l2, const PathPt* l4,
                                const PathPt* l5, const PathPt* l6,
                                int	bumpMod );
public:
    void	ResetSpringVelocities();
    void	OptimisePathSprings( const CarModel&	cm,
                                 int				step,
                                 int				nIterations,
                                 int				bumpMod );

    void	OptimisePathTopLevel(	const CarModel& cm,
                                    int step, int nIterations, int bumpMod );

private:
    PathOptions	m_options;

    struct PathCalc
    {
        Vec2d	pnorm;
        double	rlen;
        double	old_offs;
        Vec2d	force;
    };

    std::vector<PathCalc>	m_temp;
};

#endif // _SRINGSPATH_H_
