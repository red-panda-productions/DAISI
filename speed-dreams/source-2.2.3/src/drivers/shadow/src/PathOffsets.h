/***************************************************************************

    file        : PathOffsets.h
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

#ifndef _PATHOFFSETS_H_
#define _PATHOFFSETS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>

#include "Vec2d.h"
#include "MyTrack.h"

class PathOffsets
{
private:
    std::string         _baseFilename;
    std::vector<double> _offsets;
    std::vector<double>	_times;
    int			        _lastSeg;
    Vec2d		        _lastPt;
    double		        _lastTime;

public:
    PathOffsets();
    ~PathOffsets();

    void    setBaseFilename( const char* pBaseFilename );
    void    update( const MyTrack& track, const tCarElt* pCar );
    void    save_springs( const MyTrack& track, int lap );
};

#endif // _PATHOFFSETS_H_
