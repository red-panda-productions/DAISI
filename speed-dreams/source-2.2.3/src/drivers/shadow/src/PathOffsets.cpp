/***************************************************************************

    file        : PathOffsets.cpp
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

#include "PathOffsets.h"
#include "Utils.h"

#include <portability.h>
#include <robottools.h>

PathOffsets::PathOffsets()
:	_lastSeg(-1),
    _lastPt(0, 0),
    _lastTime(0)
{
}

PathOffsets::~PathOffsets()
{
}

void    PathOffsets::setBaseFilename( const char* pBaseFilename )
{
    _baseFilename = pBaseFilename;
}

void    PathOffsets::update( const MyTrack& track, const tCarElt* pCar )
{
    const size_t NSEG = track.GetSize();

    if (_offsets.size() != NSEG)
    {
        _offsets.clear();
        _offsets.resize( NSEG );
        _times.clear();
        _times.resize( NSEG );
    }

    // work out which slice the car is in.
    int		last_seg    = _lastSeg;
    double	track_pos   = RtGetDistFromStart((tCarElt*)pCar);
    int		cur_seg     = track.IndexFromPos(track_pos);

    // work out current position.
    Vec2d	cur_pt(pCar->pub.DynGCg.pos.x, pCar->pub.DynGCg.pos.y);
    double	cur_time(pCar->race.curLapTime);

    if( last_seg >= 0 && last_seg != cur_seg )
    {
        int		next_seg = (last_seg + 1) % NSEG;
        while( last_seg != cur_seg )
        {
            if( next_seg == 0 && pCar->race.laps > 1 )
            {
                save_springs( track, pCar->race.laps - 1 );
            }

            const Seg&	s0 = track.GetAt(next_seg);
            double		t, w;
            if( Utils::LineCrossesLine(_lastPt, cur_pt - _lastPt,
                                        s0.pt.GetXY(), s0.norm.GetXY(), t, w) &&
                t >= 0.0 && t <= 1.0 )
            {
                _offsets[next_seg] = w;
                _times[next_seg] = _lastTime + t * (cur_time - _lastTime);
            }

            last_seg = next_seg;
            next_seg = (next_seg + 1) % NSEG;
        }
    }

    _lastSeg	= cur_seg;
    _lastPt		= cur_pt;
    _lastTime	= cur_time;
}

void    PathOffsets::save_springs( const MyTrack& track, int lap )
{
    char    filename[256];
    sprintf( filename, "%s-recorded-lap=%d.spr", _baseFilename.c_str(), lap );

    fprintf( stderr, "Saving \"springs\" data file %s\n", filename );
    fflush( stderr );

    FILE*	pFile = fopen(filename, "w");
    if( pFile == 0 )
    {
//		DEBUGF( "Failed to open data file\n" );
        return;
    }

    fprintf( pFile, "SPRINGS-PATH\n" );
    fprintf( pFile, "0\n" );

    fprintf( pFile, "TRACK-LEN\n" );
    fprintf( pFile, "%g\n", track.GetLength() );

    fprintf( pFile, "BEGIN-POINTS\n" );
#if defined(_MSC_VER) && _MSC_VER < 1800
    fprintf( pFile, "%Iu\n", _offsets.size() );
#else
    fprintf( pFile, "%zu\n", _offsets.size() );
#endif
    for( size_t i = 0; i < _offsets.size(); i++ )
    {
        fprintf( pFile, "%.20g %g\n", _offsets[i], _times[i] );
    }
    fprintf( pFile, "END-POINTS\n" );

    fclose( pFile );
}
