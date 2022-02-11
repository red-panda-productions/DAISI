/***************************************************************************

    file        : ClothoidPath.h
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

// ClothoidPath.h: interface for the ClothoidPath class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CLOTHOIDPATH_H_
#define _CLOTHOIDPATH_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Path.h"
#include "PathOptions.h"
#include "MyTrack.h"

#include <vector>

struct Blah
{
    static int ccount;
    static int dcount;
    int dummy;
    int* pDummy;
//	std::vector<int> vect;
    Blah() : dummy(123456789), pDummy(new int[1024])//, vect(2, 5)
    {
        pDummy[0] = 1234;
        ccount++;
    }
    ~Blah()
    {
        pDummy[0] = 4321;
        delete [] pDummy;
        dcount++;
    }

private:
    Blah( const Blah& blah );
    Blah& operator=( const Blah& blah );
};

class ClothoidPath : public Path
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
    ClothoidPath();
private:
    ClothoidPath( const ClothoidPath& other );
public:
    virtual ~ClothoidPath();

    virtual ClothoidPath&	operator=( const Path& other ) override;
    virtual ClothoidPath&	operator=( const ClothoidPath& other );

    const PathOptions&	GetOptions() const;

    void	MakeSmoothPath( const MyTrack* pTrack, const CarModel& cm,
                            const PathOptions& opts );

    void	Search( const CarModel& cm );
    void	Search( const CarModel& cm, const ICalcTimeFunc& calcTimeFunc );

//private:
    void	AnalyseBumps( const CarModel& cm, bool dumpInfo = false );
    void	SmoothBetween( int step );
    using Path::SetOffset;
    double	LimitOffset( const CarModel& cm, double k, double t, const PathPt* l3 ) const;
    void	SetOffset( const CarModel& cm, double k, double t,
                       PathPt* l3, const PathPt* l1, const PathPt* l2, const PathPt* l4, const PathPt* l5 );
    void	OptimiseLine( const CarModel& cm, int idx, int step, double hLimit,
                       PathPt* l3, const PathPt* l2, const PathPt* l4 );
    void	Optimise(	const CarModel& cm, double factor,
                        int idx, PathPt* l3,
                        const PathPt* l0, const PathPt* l1,
                        const PathPt* l2, const PathPt* l4,
                        const PathPt* l5, const PathPt* l6,
                        int	bumpMod );
    void	OptimisePath(	const CarModel& cm,
                            int step, int nIterations, int bumpMod );
    void	OptimisePathSection(	const CarModel& cm, int start, int len,
                                    int step, const PathOptions& options );

    void    CalcCachedFactors();

private:
    PathOptions	m_options;
    Blah				m_blah;
//	std::vector<double> m_factors;  // cached factors
};

#endif // _CLOTHOIDPATH_H_
