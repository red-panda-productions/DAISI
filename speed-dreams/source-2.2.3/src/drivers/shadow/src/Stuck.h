/***************************************************************************

    file        : Stuck.h
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

#ifndef _STUCK_H
#define _STUCK_H

#include <vector>
#include <queue>

#include <car.h>
#include <robot.h>

#include "Utils.h"
#include "MyTrack.h"
#include "Opponent.h"

class Stuck
{
public:
    Stuck();
    ~Stuck();

    bool	execute( const MyTrack& track, const tSituation* s, tCarElt* me, const Opponent::Sit& mySit );

private:
    void	executeRacing(  const MyTrack& track, const tSituation* s, const tCarElt* me, const Opponent::Sit& mySit );
    void	executeReorient(const MyTrack& track, const tSituation* s, tCarElt* me, const Opponent::Sit& mySit );
    void	executeInit( const MyTrack& track, const tSituation* s, tCarElt* me );
    void	executeSolving( const MyTrack& track, const tSituation* s, tCarElt* me );
    void	executePlan(    const MyTrack& track, const tSituation* s, tCarElt* me );

private:
    enum { N_ANGLES = 64 };
    enum { HALF_ANGLE = N_ANGLES / 2 };
    enum { ANGLE_MASK = N_ANGLES - 1 };
    enum { OCTANT = N_ANGLES / 8 };
    enum { HALF_OCTANT = OCTANT / 2 };

    enum { GRID_RAD = 50 };
    enum { GRID_SIZE = 2 * GRID_RAD + 1 };
    enum { CAR_RAD = 4 };

    enum { SPD = 4 };	// speed of car while getting unstuck.

    enum State { RACING, REORIENT_FORWARDS, REORIENT_BACKWARDS, REINIT, SOLVING, EXEC_PLAN };

    static inline int fwang( int iang, bool fw ) { return (iang << 1) | int(fw); }

    struct GridPoint
    {
        unsigned int	pt;			// current position.
        float			est_time;	// estimated time to finish.
        float			time;		// current time.

        GridPoint()
        :	pt(0), est_time(0), time(0)
        {
        }

        GridPoint( int p )
        :	pt(p), est_time(0), time(0)
        {
        }

        GridPoint( int x, int y, int iang, bool fw, float time, float est )
        {
            set(x, y, iang, fw, time, est);
        }

        GridPoint( int x, int y, float ang, bool fw, float time, float est )
        {
            set(x, y, ang, fw, time, est);
        }

        GridPoint( const Stuck& stuck, const tCarElt* car, bool fw, float est )
        {
            float dx = car->pub.DynGCg.pos.x - stuck._gridOrigin.x;
            float dy = car->pub.DynGCg.pos.y - stuck._gridOrigin.y;
            int x = (int)floor(dx + 0.5);
            int y = (int)floor(dy + 0.5);
            set(x, y, (float)car->pub.DynGCg.pos.az, fw, 0.0f, est);
        }

        bool operator<( const GridPoint& other ) const
        {
            return est_time > other.est_time;
        }

        bool isValid() const
        {
            return false;
        }

        void set( int x, int y, int iang, bool fw, float tm, float est )
        {
            x = x & 0xFF;
            y = y & 0xFF;
            iang = iang & ANGLE_MASK;
            pt = (fw << 24) | (x << 16) | (y << 8) | (iang);
            time = tm;
            est_time = est;
        }

        void set( int x, int y, float ang, bool fw, float time, float est )
        {
            int iang = to_iang(ang);
            set(x, y, iang, fw, time, est);
        }

        void set_fw( bool fw )
        {
            pt = (fw << 24) | (0x00FFFFFF & pt);
        }

        double dist( const GridPoint& other ) const
        {
            int dx = x() - other.x();
            int dy = y() - other.y();
            int da = iang() - other.iang();
            if( da > 32 )
                da -= 64;
            else if( da < -32 )
                da += 64;

            return dx*dx + dy*dy + da*da*0.001;
        }

        bool fw() const		{ return (pt >> 24) != 0; }
        bool bw() const		{ return (pt >> 24) == 0; }
        int x() const		{ return (pt >> 16) & 0xFF; }
        int y() const		{ return ((pt >> 8) & 0xFF); }
        int iang() const	{ return pt & 0xFF; }
        int fwang() const	{ return (iang() << 1) | int(fw()); }
    };

    struct Cell
    {
        enum { EDGE_MASK = 0x80000000 };
        unsigned int	occupied_mask;
        float			est_time_to_car;
        float			est_time_to_dest;
        int				dist_from_walls;
        float			times[N_ANGLES * 2];	// indexed by fwang
        int				from[N_ANGLES * 2];		// indexed by fwang
        char			solution[N_ANGLES * 2];	// indexed by fwang


        Cell() { clear(); }

        void clear()
        {
            occupied_mask = EDGE_MASK;	// track edges...
            //occupied_mask = 0;
            est_time_to_car = -1;
            est_time_to_dest = -1;
            dist_from_walls = 0;
            for( int i = 0; i < N_ANGLES * 2; i++ )
            {
                times[i] = 9e9f;
                from[i] = -1;
                solution[i] = 0;
            }
        }

        void addCarMask( int carIdx )		{ occupied_mask |=  (1u << carIdx); };
        void removeCarMask( int carIdx )	{ occupied_mask &= ~(1u << carIdx); };
        void clearAllCarMasks()				{ occupied_mask &=  EDGE_MASK; };

        void addEdgeMask()					{ occupied_mask |=  EDGE_MASK; };
        void removeEdgeMask()				{ occupied_mask &= ~EDGE_MASK; };

        bool isAvailable() const			{ return occupied_mask == 0; }
        bool isAvailable( int fwang ) const	{ return occupied_mask == 0 && times[fwang] >= 0; }

        void setSolution( int fwang )		{ solution[fwang] = 1; times[fwang] = 9e9f; }
        bool isSolution( int fwang ) const	{ return solution[fwang] == 1; }
    };

    struct Edge
    {
        int sy;    // starting y value
        int ey;    // ending y value
        float sx;    // starting x value
        float dX;    // for a step of size 1 in y
        int	x;		// current x value

        Edge( float x1, float y1, float x2, float y2 )
        {
            if( y1 > y2 )
            {
                std::swap( x1, x2 );
                std::swap( y1, y2 );
            }

            sy = (int)ceil(y1);
            ey = (int)floor(y2);
            dX = y1 < y2 ? (x2 - x1) / (y2 - y1) : 0;
            sx = x1 + (sy - y1) * dX;
            x  = 0;
        }

        bool operator<( const Edge& other ) const
        {
            return x < other.x;
        }

        int calcX(int y) { return (int)floor(sx + (y - sy) * dX); }
    };

    struct OppInfo
    {
        double			x;
        double			y;
        int				ix;
        int				iy;
        const tCarElt*	car;

        OppInfo() : x(0), y(0), ix(0), iy(0), car(0) {}
        OppInfo( double X, double Y, const tCarElt* CAR )
        :	x(X), y(Y), car(CAR)
        {
            ix = (int)floor(x + 0.5);
            iy = (int)floor(x + 0.5);
        }

        bool operator==( const OppInfo& other ) const
        {
            return ix == other.ix && iy == other.iy && car == other.car;
        }

        bool operator!=( const OppInfo& other ) const
        {
            return !operator==(other);
        }
    };

    static const int	delta8_x[8];
    static const int	delta8_y[8];
    static const float	delta8_t[8];
    static const float	delta64_t[64];

private:
    void	updateStuckTime( const tCarElt* me, const tSituation* s );
    void	makeOpponentsList( const tSituation* s, const tCarElt* me,
                               std::vector<OppInfo>* opponents );
    bool	clearAhead( const MyTrack& track, const tSituation* s, const tCarElt* me ) const;
    bool	opponentsChanged( const tSituation* s, const tCarElt* me );

    bool	isInitialised() const { return _me != 0; }
    void	reorient( const tCarElt* me, double dirAng );
    void	init( const MyTrack& track, const tSituation* s, const tCarElt* me );
    void	fillCarCells( int carI, double carX, double carY, double carAng, double dx, double dy, double rad, bool addMask );
    void	fillTrackCells( const MyTrack& track );
    bool	solve( const tCarElt* me );
    bool	solveR( const tCarElt* me );
    void	dumpGrid() const;
    void	getUnstuck( const MyTrack& track, tCarElt* me, const tSituation* s );

private:
    const Cell& at( int x, int y ) const		{ return _grid[x][y]; }
    Cell&       at( int x, int y )				{ return _grid[x][y]; }

    const Cell& at( const GridPoint& pt ) const	{ return _grid[pt.x()][pt.y()]; }
    Cell&		at( const GridPoint& pt )		{ return _grid[pt.x()][pt.y()]; }

    bool		isValid( int x, int y ) const	{ return x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE; }

    const Cell& operator[]( const GridPoint& pt ) const { return at(pt); }

    double	calcCarDist( bool fw, double maxDist, const tCarElt* me, const tSituation* s ) const;

    void	generateSuccessorsN( const GridPoint& from, std::vector<GridPoint>& succs ) const;
    void	generateSuccessorsR( const GridPoint& from, std::vector<GridPoint>& succs ) const;

    static int	to_iang( double ang )
    {
        const float ang_step = float(N_ANGLES / (2 * 3.14159265));
        int iang = (int)floor(ang * ang_step + 0.5);
        return iang & ANGLE_MASK;
    }

    static int		reverse( int iang ) { return (iang + N_ANGLES / 2) & ANGLE_MASK; }

    static void		sort( std::vector<Edge>& row, int y );

private:
    const tCarElt*						_me;
    Vec2d								_gridOrigin;
    std::vector<std::vector<Cell> >		_grid;
    State								_stuckState;
    double								_stuckTime;
    int									_stuckCount;

    // track points for collision detection.
    std::vector<Vec2d>					_leftPoints;
    std::vector<Vec2d>					_rightPoints;

    // intialisation variables
    std::vector<OppInfo>				_opponents;
    GridPoint							_origCarPt;

    // for debugging
    std::vector<GridPoint>				_destinations;

    // solver variables
    int									_expansionsN;
    std::priority_queue<GridPoint>		_pqN;
    int									_expansionsR;
    std::priority_queue<GridPoint>		_pqR;
    float								_bestTime;
    GridPoint							_bestPt;

    // plan variables
    std::vector<GridPoint>				_plan;
    int									_planIndex;
};

#endif	// _STUCK_H
