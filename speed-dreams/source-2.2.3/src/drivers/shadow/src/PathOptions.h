/***************************************************************************

    file        : PathOptions.h
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

#ifndef _PATHOPTIONS_H_
#define _PATHOPTIONS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

struct PathOptions
{
    int					bumpMod;
    double				safetyLimit;
    double				safetyMultiplier;
    int					quadSmoothIters;
    double				maxL;
    double				maxR;
    double				apexFactor;
    std::vector<double>	factors;

    PathOptions()
    :	bumpMod(0), safetyLimit(1.5), safetyMultiplier(100), quadSmoothIters(0), maxL(999), maxR(999), apexFactor(0), factors(1, 1.005) {}

    PathOptions( int bm, double limit = 1.5, double mult = 100, double ml = 999, double mr = 999, double af = 0 )
    :	bumpMod(bm), safetyLimit(limit), safetyMultiplier(mult), quadSmoothIters(0), maxL(ml), maxR(mr), apexFactor(af), factors(1, 1.005) {}

    PathOptions&	operator=( const PathOptions& other )
    {
        bumpMod				= other.bumpMod;
        safetyLimit			= other.safetyLimit;
        safetyMultiplier	= other.safetyMultiplier;
        quadSmoothIters		= other.quadSmoothIters;
        maxL				= other.maxL;
        maxR				= other.maxR;
        apexFactor			= other.apexFactor;
        factors				= other.factors;
        return *this;
    }
};

#endif // _PATHOPTIONS_H_
