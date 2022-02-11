/***************************************************************************

    file        : Stuck.cpp
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

#include <queue>
#include <set>
#include <algorithm>

#include "Stuck.h"
#include "CarBounds2d.h"

using namespace std;

// The "SHADOW" logger instance.
extern GfLogger* PLogSHADOW;
#define LogSHADOW (*PLogSHADOW)

const int	Stuck::delta8_x[8] = {1, 1, 0, -1, -1, -1,  0,  1};
const int	Stuck::delta8_y[8] = {0, 1, 1,  1,  0, -1, -1, -1};
const float	Stuck::delta8_t[8] = {1.0f/Stuck::SPD, 1.0f/Stuck::SPD * 1.414f,
                                  1.0f/Stuck::SPD, 1.0f/Stuck::SPD * 1.414f,
                                  1.0f/Stuck::SPD, 1.0f/Stuck::SPD * 1.414f,
                                  1.0f/Stuck::SPD, 1.0f/Stuck::SPD * 1.414f};

const float	Stuck::delta64_t[64] =
{
    1.0f/Stuck::SPD * 1.000f, 1.0f/Stuck::SPD * 1.050f, 1.0f/Stuck::SPD * 1.100f, 1.0f/Stuck::SPD * 1.150f,
    1.0f/Stuck::SPD * 1.500f, 1.0f/Stuck::SPD * 1.480f, 1.0f/Stuck::SPD * 1.460f, 1.0f/Stuck::SPD * 1.440f,
    1.0f/Stuck::SPD * 1.414f, 1.0f/Stuck::SPD * 1.440f, 1.0f/Stuck::SPD * 1.460f, 1.0f/Stuck::SPD * 1.480f,
    1.0f/Stuck::SPD * 1.500f, 1.0f/Stuck::SPD * 1.150f, 1.0f/Stuck::SPD * 1.100f, 1.0f/Stuck::SPD * 1.050f,

    1.0f/Stuck::SPD * 1.000f, 1.0f/Stuck::SPD * 1.050f, 1.0f/Stuck::SPD * 1.100f, 1.0f/Stuck::SPD * 1.150f,
    1.0f/Stuck::SPD * 1.500f, 1.0f/Stuck::SPD * 1.480f, 1.0f/Stuck::SPD * 1.460f, 1.0f/Stuck::SPD * 1.440f,
    1.0f/Stuck::SPD * 1.414f, 1.0f/Stuck::SPD * 1.440f, 1.0f/Stuck::SPD * 1.460f, 1.0f/Stuck::SPD * 1.480f,
    1.0f/Stuck::SPD * 1.500f, 1.0f/Stuck::SPD * 1.150f, 1.0f/Stuck::SPD * 1.100f, 1.0f/Stuck::SPD * 1.050f,

    1.0f/Stuck::SPD * 1.000f, 1.0f/Stuck::SPD * 1.050f, 1.0f/Stuck::SPD * 1.100f, 1.0f/Stuck::SPD * 1.150f,
    1.0f/Stuck::SPD * 1.500f, 1.0f/Stuck::SPD * 1.480f, 1.0f/Stuck::SPD * 1.460f, 1.0f/Stuck::SPD * 1.440f,
    1.0f/Stuck::SPD * 1.414f, 1.0f/Stuck::SPD * 1.440f, 1.0f/Stuck::SPD * 1.460f, 1.0f/Stuck::SPD * 1.480f,
    1.0f/Stuck::SPD * 1.500f, 1.0f/Stuck::SPD * 1.150f, 1.0f/Stuck::SPD * 1.100f, 1.0f/Stuck::SPD * 1.050f,

    1.0f/Stuck::SPD * 1.000f, 1.0f/Stuck::SPD * 1.050f, 1.0f/Stuck::SPD * 1.100f, 1.0f/Stuck::SPD * 1.150f,
    1.0f/Stuck::SPD * 1.500f, 1.0f/Stuck::SPD * 1.480f, 1.0f/Stuck::SPD * 1.460f, 1.0f/Stuck::SPD * 1.440f,
    1.0f/Stuck::SPD * 1.414f, 1.0f/Stuck::SPD * 1.440f, 1.0f/Stuck::SPD * 1.460f, 1.0f/Stuck::SPD * 1.480f,
    1.0f/Stuck::SPD * 1.500f, 1.0f/Stuck::SPD * 1.150f, 1.0f/Stuck::SPD * 1.100f, 1.0f/Stuck::SPD * 1.050f,
};

Stuck::Stuck()
:	_me(0),
    _stuckState(RACING),
    _stuckTime(0),
    _stuckCount(0)
{
    _grid.resize( GRID_SIZE );

    for( int x = 0; x < (int)_grid.size(); x++ )
        _grid[x].resize( GRID_SIZE );
}

Stuck::~Stuck()
{
}

// racing.
//	normal racing.  no control applied here, just monitoring the
//	car's situation to decide if its stuck or needs re-orientation.
//
//	if angle not OK, go to reorient.
//	if halted for too long, go to stuck.
//
// reorient.
//	car angle more than 30 degrees to racing line.  use forwards and backwards movements
//	with steering to try to reorient the car.  if halted for too long go to stuck.
//
// stuck.
//	car has come to a halt.  start rearching for a plan to get unstuck.
//	once a plan is found, start to execute it.
//
// execute plan.
//	follow plan, monitoring the car situation.  if stuck again, go back to stuck.
//	once free, go back to racing.
//

bool Stuck::execute( const MyTrack& track, const tSituation* s, tCarElt* me, const Opponent::Sit& mySit )
{
    // double start = GfTimeClock();

    switch( _stuckState )
    {
        case RACING:
            executeRacing( track, s, me, mySit );
            break;

        case REORIENT_FORWARDS:
        case REORIENT_BACKWARDS:
            executeReorient( track, s, me, mySit );
            break;

        case REINIT:
            executeInit( track, s, me );
            break;

        case SOLVING:
            executeSolving( track, s, me );
            break;

        case EXEC_PLAN:
            executePlan( track, s, me );
            break;
    }

    // double elapsed = GfTimeClock() - start;
//	LogSHADOW.debug( "[%d] stuck CPU time: %0.6f seconds.\n", me->index, elapsed );

    return _stuckState == EXEC_PLAN;
}

void Stuck::executeRacing( const MyTrack& track, const tSituation* s, const tCarElt* me, const Opponent::Sit& mySit )
{
    updateStuckTime( me, s );

    double dirAng = mySit.pi.oang - me->_yaw;
    NORM_PI_PI(dirAng);

    if( fabs(dirAng) > 30 * PI / 180 )
        reorient( me, dirAng );

    if( _stuckTime < 1 )
        return;

    init( track, s, me );
}

void Stuck::executeReorient( const MyTrack& track, const tSituation* s, tCarElt* me, const Opponent::Sit& mySit )
{
    LogSHADOW.debug( "[%d] reorient.  rev count %d\n", me->index, _stuckCount );

    updateStuckTime( me, s );

    double	dirAng = mySit.pi.oang - me->_yaw;
    NORM_PI_PI(dirAng);

    if( fabs(dirAng) < 30 * PI / 180 )
    {
        _stuckState = RACING;
        LogSHADOW.debug( "[%d] reorient.  finished.\n", me->index );

        return;
    }

    if( _stuckCount > 10 )
    {
        // give up... use the solver instead.
        LogSHADOW.debug( "[%d] reorient.  start solvers.\n", me->index );
        _stuckState = REINIT;
        _stuckCount = 0;
        _stuckTime  = 0.0;

        return;
    }

    double sideAhead  = dirAng > 0 ? mySit.pi.extR + me->pub.trkPos.toMiddle : mySit.pi.extL - me->pub.trkPos.toMiddle;
    double sideBehind = dirAng > 0 ? mySit.pi.extL - me->pub.trkPos.toMiddle : mySit.pi.extR + me->pub.trkPos.toMiddle;

    double	distAhead  = 25;
    double	distBehind = 25;
    CarBounds2d	bounds(me);

    for( int i = 0; i < s->raceInfo.ncars; i++ )
    {
        const tCarElt* oCar = s->cars[i];

        if( oCar == me || (oCar->pub.state & RM_CAR_STATE_NO_SIMU & ~RM_CAR_STATE_PIT) != 0 )
            continue;

        const CarBounds2d	oBounds(oCar);

        distAhead  = bounds.distToSide(CarBounds2d::SIDE_FRONT, distAhead,  oBounds);
        distBehind = bounds.distToSide(CarBounds2d::SIDE_REAR,  distBehind, oBounds);
    }

    int		gear = 1;
    double	brk  = 0;

    switch( _stuckState )
    {
        case REORIENT_BACKWARDS:
            gear = -1;
            brk = me->pub.DynGC.vel.x > 0 ? 0.5 : 0;

            if( distBehind < 0.2 || sideBehind < 2.5 )
            {
                LogSHADOW.debug( "[%d] reorient go forwards\n", me->index );
                _stuckCount++;
                _stuckState = REORIENT_FORWARDS;
                _stuckTime = 0;
            }
            break;

        case REORIENT_FORWARDS:
            brk = me->pub.DynGC.vel.x < 0 ? 0.5 : 0;

            if( distAhead < 0.2 || sideAhead < 2.5 )
            {
                LogSHADOW.debug( "[%d] reorient go backwards\n", me->index );
                _stuckCount++;
                _stuckState = REORIENT_BACKWARDS;
                _stuckTime = 0;
            }

            break;

        default:
            break;
    }

    double steer = 0;

    if( me->_speed_x < 0 )
        steer = -SGN(dirAng);
    else
        steer = SGN(dirAng);

    double acc = fabs(me->_speed_x) < 3 ? 1.0 : 0.2;

    double w1_speed = me->priv.wheel[0].spinVel * me->info.wheel[0].wheelRadius;
    double w2_speed = me->priv.wheel[1].spinVel * me->info.wheel[1].wheelRadius;
    double w3_speed = me->priv.wheel[2].spinVel * me->info.wheel[2].wheelRadius;
    double w4_speed = me->priv.wheel[3].spinVel * me->info.wheel[3].wheelRadius;

    double front_speed = (w1_speed + w2_speed) * 0.5;

    if ((gear > 0 && (w3_speed > front_speed + 2 || w4_speed > front_speed + 2)) ||
        (gear < 0 && (w3_speed < front_speed - 2 || w4_speed < front_speed - 2)))
        acc = 0.1;

    me->ctrl.steer		= steer;
    me->ctrl.gear		= gear;
    me->ctrl.accelCmd	= acc;
    me->ctrl.brakeCmd	= brk;
}

void Stuck::executeInit( const MyTrack& track, const tSituation* s, tCarElt* me )
{
    if( _stuckTime > 0 )
        _stuckTime -= s->deltaTime;
    else
        init( track, s, me );

    me->ctrl.steer		= 0;
    me->ctrl.accelCmd	= 0;
    me->ctrl.brakeCmd	= 1;
}

void Stuck::executeSolving( const MyTrack& track, const tSituation* s, tCarElt* me )
{
    if( clearAhead(track, s, me) )
    {
        _stuckState = RACING;
        return;
    }

    me->ctrl.accelCmd = 0;
    me->ctrl.brakeCmd = 1;

    if( !solveR(me) )
    {
        // no solution possible with initial state... try again in a short time.
        _stuckCount++;
        LogSHADOW.debug( "stuck: [%d] No solution: re-initting.\n", _stuckCount );
        _stuckState = _stuckCount < 10 ? REINIT : RACING;
        _stuckTime  = 0.09;
    }
}

void Stuck::executePlan( const MyTrack& track, const tSituation* s, tCarElt* me )
{
    if( clearAhead(track, s, me) )
    {
        _stuckState = RACING;

        return;
    }

    getUnstuck( track, me, s );
}

void Stuck::updateStuckTime( const tCarElt* me, const tSituation* s )
{
    if( fabs(me->_speed_x) > 2 || s->currentTime < 0 )
        _stuckTime = 0;
    else
        _stuckTime += s->deltaTime;
}

void Stuck::sort( vector<Edge>& row, int y )
{
    for( int i = 0; i < (int)row.size(); i++ )
        row[i].x = row[i].calcX(y);

    std::sort( row.begin(), row.end() );
}

void	Stuck::reorient( const tCarElt* me, double dirAng )
{
    _stuckCount = 0;
    _stuckState = dirAng * me->_trkPos.toMiddle < 0 ?
                    REORIENT_BACKWARDS : REORIENT_FORWARDS;
    _stuckTime = 0;
}

void	Stuck::makeOpponentsList( const tSituation* s, const tCarElt* me, vector<OppInfo>* opponents )
{
    opponents->clear();

    for( int i = 0; i < s->raceInfo.ncars; i++ )
    {
        const tCarElt* other = s->cars[i];
        if( other->index == me->index || (other->pub.state & RM_CAR_STATE_NO_SIMU & ~RM_CAR_STATE_PIT) )
            continue;

        double	carX = other->pub.DynGCg.pos.x - _gridOrigin.x;
        double	carY = other->pub.DynGCg.pos.y - _gridOrigin.y;

        // int intCarX = (int)floor(carX + 0.5);
        // int intCarY = (int)floor(carY + 0.5);

        if( other->pub.speed > 2 ||
            carX < 0 || carX >= GRID_SIZE ||
            carY < 0 || carY >= GRID_SIZE )
            continue;	// ignore cars that aren't stopped, or are outside of our grid area.

        opponents->push_back( OppInfo(carX, carY, other) );
    }
}

bool	Stuck::clearAhead( const MyTrack& track, const tSituation* s, const tCarElt* me ) const
{
//	const MyTrack::Seg& seg(track.GetAt(track.IndexFromPos(me->race.distFromStartLine)));
    double	width = track.GetWidth();
    double	offs = -me->pub.trkPos.toMiddle;
    LogSHADOW.debug( "offs=%.2f width=%.2f\n", offs, width );

    if( offs < -width / 2 || offs > width / 2 )
        return false;	// still not back onto main raceway.

    for( int i = 0; i < s->raceInfo.ncars; i++ )
    {
        const tCarElt* other = s->cars[i];
        if( other->index == me->index || (other->pub.state & RM_CAR_STATE_NO_SIMU) )
            continue;

        int carX = (int)floor(other->pub.DynGCg.pos.x - _gridOrigin.x + 0.5);
        int carY = (int)floor(other->pub.DynGCg.pos.y - _gridOrigin.y + 0.5);

        if( other->pub.speed > 2 ||
            carX < 0 || carX >= GRID_SIZE ||
            carY < 0 || carY >= GRID_SIZE )
            continue;	// ignore cars that aren't stopped, or are outside of our grid area.

        double relPos = other->race.distFromStartLine - me->race.distFromStartLine;

        if( relPos > track.GetLength() / 2 )
            relPos -= track.GetLength();
        else if( relPos < -track.GetLength() / 2 )
            relPos += track.GetLength();

        if( relPos > 0 )
            return false;
    }

    return true;
}

bool	Stuck::opponentsChanged( const tSituation* s, const tCarElt* me )
{
    vector<OppInfo>	opponents;
    makeOpponentsList( s, me, &opponents );

    return _opponents != opponents;
}

void	Stuck::init( const MyTrack& track, const tSituation* s, const tCarElt* me )
{
    LogSHADOW.debug( "[%d] stuck::init\n", me->index );
    LogSHADOW.debug( "[%d] len %g  steer lock %g\n",
            me->index, me->priv.wheel[2].relPos.x - me->priv.wheel[0].relPos.x,
            me->info.steerLock );

    _me = me;
    _gridOrigin = Vec2d(me->pub.DynGCg.pos.x - GRID_RAD, me->pub.DynGCg.pos.y - GRID_RAD);
    _stuckState = RACING;
    _plan.clear();

    // clear all cells.
    for( int x = 0; x < GRID_SIZE; x++ )
        for( int y = 0; y < GRID_SIZE; y++ )
            _grid[x][y].clear();

    fillTrackCells( track );

    const tCarElt* aheadCar  = NULL;
    const tCarElt* behindCar = NULL;

    // make opponent car cells unavailable.
    makeOpponentsList( s, me, &_opponents );

    for( size_t i = 0; i < _opponents.size(); i++ )
    {
        const OppInfo& oppInfo = _opponents[i];
        const tCarElt* other   = oppInfo.car;

        if( aheadCar == NULL ||
            aheadCar->race.distFromStartLine < other->race.distFromStartLine )
            aheadCar = other;

        if( behindCar == NULL ||
            behindCar->race.distFromStartLine > other->race.distFromStartLine )
            behindCar = other;

        fillCarCells( i, oppInfo.x, oppInfo.y, other->pub.DynGC.pos.az, 2.25, 1.0, 1.0, true );
    }

    fillCarCells( -1, 0, 0, me->pub.DynGC.pos.az, 1.5, 0.5, 0.0, false );

    double	width = track.GetWidth();
    double	offs = -me->pub.trkPos.toMiddle;
    LogSHADOW.debug( "offs=%.2f width=%.2f\n", offs, width );

    bool onMainRaceway = offs > -width / 2 && offs < width / 2;

    if( onMainRaceway &&
        (aheadCar == NULL ||
         aheadCar->race.distFromStartLine + 2 < me->race.distFromStartLine) )
    {
        // no cars ahead are in the way so go back to racing.
        _stuckState = RACING;
        _stuckTime  = 0;

        return;
    }

    double aheadCutOff = 7;

    if( aheadCar == NULL ||
        aheadCar->race.distFromStartLine < me->race.distFromStartLine )
    {
        aheadCar = me;
        aheadCutOff = 15;	// stuck even though there's no other car ahead, so probably
                            // stuck in the scenery.  add some more space to make sure to
                            // navigate around it.
    }

    if( behindCar == NULL ||
        behindCar->race.distFromStartLine > me->race.distFromStartLine )
        behindCar = me;

    // figure out position ahead for destination (and cut-off).
    LogSHADOW.debug( "my car: %g\n", me->race.distFromStartLine );
    LogSHADOW.debug( "car ahead: %g\n", aheadCar->race.distFromStartLine );
    double aheadPos = aheadCar->race.distFromStartLine + aheadCutOff;

    if( aheadPos > me->race.distFromStartLine + GRID_RAD * 9 / 10 )
        aheadPos = me->race.distFromStartLine + GRID_RAD * 9 / 10;

    LogSHADOW.debug( "ahead pos: %g\n", aheadPos );

    // figure out position behind for cut-off.
    LogSHADOW.debug( "car behind: %g\n", behindCar->race.distFromStartLine );
    double behindPos = behindCar->race.distFromStartLine - 10;
    if( behindPos < me->race.distFromStartLine - GRID_RAD * 9 / 10 )
        behindPos = me->race.distFromStartLine - GRID_RAD * 9 / 10;
    LogSHADOW.debug( "behind pos: %g\n", behindPos );

    // draw lines ahead and behind to help limit the search.
    int aheadSegIndex = (track.IndexFromPos(aheadPos) + 1) % track.GetSize();
    const Seg& aheadCutSeg = track.GetAt(aheadSegIndex);
    int lastX = -1;

    for( double offs = -aheadCutSeg.el - 1.5; offs < aheadCutSeg.er + 1.5; offs += 0.5 )
    {
        Vec3d pt = aheadCutSeg.pt + offs * aheadCutSeg.norm;

        int x = (int)floor(pt.x - _gridOrigin.x);
        int y = (int)floor(pt.y - _gridOrigin.y);
        if( !isValid(x, y) )
            continue;

        Cell& cell = at(x, y);
        cell.addEdgeMask();

        if( lastX >= 0 )
        {
            Cell& cell2 = at(lastX, y);
            cell2.addEdgeMask();
        }

        lastX = x;
    }

    const Seg& behindCutSeg = track.GetAt(track.IndexFromPos(behindPos));
    lastX = -1;

    for( double offs = -behindCutSeg.el - 1.5; offs < behindCutSeg.er + 1.5; offs += 0.5 )
    {
        Vec3d pt = behindCutSeg.pt + offs * behindCutSeg.norm;

        int x = (int)floor(pt.x - _gridOrigin.x);
        int y = (int)floor(pt.y - _gridOrigin.y);
        if( !isValid(x, y) )
            continue;

        Cell& cell = at(x, y);
        cell.addEdgeMask();

        if( lastX >= 0 )
        {
            Cell& cell2 = at(lastX, y);
            cell2.addEdgeMask();
        }

        lastX = x;
    }

    GridPoint car_pt1(*this, me, true, 0);
    GridPoint car_pt2(car_pt1.x(), car_pt1.y(), car_pt1.iang(), false, 0, 0);
    _origCarPt = car_pt1;

    // invalidate bad directions in cells.
    const int carx	  = car_pt1.x();
    const int cary	  = car_pt1.y();
    const int cariang = car_pt1.iang();

    for( int y = 0; y < GRID_SIZE; y++ )
    {
        for( int x = 0; x < GRID_SIZE; x++ )
        {
            Cell& cell = at(x, y);
            if( !cell.isAvailable() )
                continue;

            for( int octang = 0; octang < 8; octang++ )
            {
                int dx   = delta8_x[octang];
                int dy   = delta8_y[octang];
                int iang = 8 * octang;

                if( !isValid(x + dx, y + dy) || !at(x + dx, y + dy).isAvailable() ||
                    !isValid(x - dx, y - dy) || !at(x - dx, y - dy).isAvailable())
                {
                    for( int ang = iang - 4; ang < iang + 4; ang++ )
                    {
                        if( x == carx && y == cary && (ang & 0x3F) == cariang )
                        {
                            // don't invalidate where the car actually is.
                            continue;
                        }

                        cell.times[fwang(ang & 0x3F, true)]  = -1;
                        cell.times[fwang(ang & 0x3F, false)] = -1;
                    }
                }
            }
        }
    }

    // fill in distances from cars and walls.
    deque<GridPoint> dq;
    for( int x = 0; x < GRID_SIZE; x++ )
        for( int y = 0; y < GRID_SIZE; y++ )
            if( !at(x, y).isAvailable() )
                dq.push_back(GridPoint(x, y, 0, false, 0, 0));

    while( !dq.empty() )
    {
        GridPoint pt = dq.front();
        dq.pop_front();

        int x = pt.x();
        int y = pt.y();
        Cell& from = at(x, y);

        for( int a = 0; a < 8; a += 2 )	// += 2 to only do orthogonal directions.
        {
            int		dx = delta8_x[a];
            int		dy = delta8_y[a];
            // float	dt = delta8_t[a];

            int x2 = x + dx;
            int y2 = y + dy;
            if( !isValid(x2, y2) )
                continue;

            Cell& cell = at(x2, y2);
            if( !cell.isAvailable() )
                continue;

            if( cell.dist_from_walls == 0 )
            {
                cell.dist_from_walls = from.dist_from_walls + 1;
                dq.push_back( GridPoint(x2, y2, 0, false, 0, 0) );
            }
        }
    }

    // fill in estimates to car.
    at(car_pt1).est_time_to_car = 0;
    at(car_pt2).est_time_to_car = 0;
    dq.push_back( car_pt1 );

    while( !dq.empty() )
    {
        GridPoint pt = dq.front();
        dq.pop_front();

        int x = pt.x();
        int y = pt.y();

        for( int a = 0; a < 8; a++ )
        {
            int		dx = delta8_x[a];
            int		dy = delta8_y[a];
            float	dt = delta8_t[a];

            int x2 = x + dx;
            int y2 = y + dy;

            Cell& cell = at(x2, y2);
            if( !cell.isAvailable() )
                continue;

            if( cell.est_time_to_car < 0 )
            {
                float est = pt.est_time + dt;
                cell.est_time_to_car = est;
                dq.push_back( GridPoint(x2, y2, a, true, 0, est) );
            }
        }
    }

    // fill in estimates to dest.
    const Seg& destSeg = track.GetAt(track.IndexFromPos(aheadPos));
    int mid_ang = to_iang(destSeg.norm.GetXY().GetAngle() + PI / 2);
    _destinations.clear();

    for( double offs = -destSeg.wl; offs < destSeg.wr; offs += 0.5 )
    {
        Vec3d pt = destSeg.pt + offs * destSeg.norm;
        int x = (int)floor(pt.x - _gridOrigin.x);
        int y = (int)floor(pt.y - _gridOrigin.y);

        if( !isValid(x, y) )
            continue;

        Cell& cell = at(x, y);

        if( !cell.isAvailable() )
            continue;

        cell.est_time_to_dest = 0;
        GridPoint dest = GridPoint(x, y, mid_ang, true, 0, 0);
        cell.setSolution( dest.fwang() );
        _destinations.push_back( dest );
        dq.push_back( dest );
    }
    while( !dq.empty() )
    {
        GridPoint pt = dq.front();
        dq.pop_front();

        int x = pt.x();
        int y = pt.y();

        for( int a = 0; a < 8; a++ )
        {
            int		dx = delta8_x[a];
            int		dy = delta8_y[a];
            float	dt = delta8_t[a];

            int x2 = x + dx;
            int y2 = y + dy;

            Cell& cell = at(x2, y2);

            if( !cell.isAvailable() )
                continue;

            if( cell.est_time_to_dest < 0 )
            {
                float est = pt.est_time + dt;
                cell.est_time_to_dest = est;
                dq.push_back( GridPoint(x2, y2, a, true, 0, est) );
            }
        }
    }

    // if the car position doesn't have an estimate to the destination
    // then the car is trapped.  no need to search, just re-init in a
    // short time in the hope something has changed.
    if( at(car_pt1).est_time_to_dest < 0 && at(car_pt2).est_time_to_dest < 0 )
    {
        LogSHADOW.debug( "[%d] stuck::init -- no solution -- reinit.\n", me->index );
        _stuckState = RACING;
        _stuckTime  = 0.0;
        return;
    }

    // initialise priority queue with destination positions.
    _pqN = priority_queue<GridPoint>();

    for( double offs = -destSeg.wl; offs < destSeg.wr; offs += 0.5 )
    {
        // mark all distinations as 0 time (which flags them as having been visited.)
        Vec3d pt = destSeg.pt + offs * destSeg.norm;
        int x = (int)floor(pt.x - _gridOrigin.x);
        int y = (int)floor(pt.y - _gridOrigin.y);

        if( !isValid(x, y) )
            continue;

        Cell& cell = at(x, y);

        if( !cell.isAvailable() )
            continue;

        int mid_ang = to_iang(destSeg.norm.GetXY().GetAngle() + PI / 2);

        if( cell.times[fwang(mid_ang, true)] > 0 )
        {
//			for( int a = -OCTANT; a <= OCTANT; a++ )
            for( int a = -1; a <= 1; a++ )
            {
                int iang = (mid_ang + N_ANGLES + a) % N_ANGLES;
                _pqN.push( GridPoint(x, y, iang, true, 0, cell.est_time_to_car) );
            }
        }
    }

    // initialise priority queue with car start positions.
    _pqR = priority_queue<GridPoint>();

    for( int i =  0; i <= 0; i++ )
    {
        int iang = (car_pt1.iang() + i) & 63;
        _pqR.push( GridPoint(car_pt1.x(), car_pt1.y(), iang, car_pt1.fw(), 0, at(car_pt1).est_time_to_dest + abs(i) * 2.0) );
        _pqR.push( GridPoint(car_pt2.x(), car_pt2.y(), iang, car_pt2.fw(), 0, at(car_pt2).est_time_to_dest + abs(i) * 2.0) );
    }

    // initialise other fields.
    _expansionsN = 0;
    _expansionsR = 0;
    _bestTime = 9e9f;
    _stuckState = SOLVING;

    dumpGrid();

    LogSHADOW.debug( "[%d] stuck::init -- done\n", me->index );
}

void	Stuck::fillCarCells( int carI, double carX, double carY, double carAng, double len, double wid, double rad, bool addMask )
{
    int minX = MX(0, MN((int)floor(carX - CAR_RAD), GRID_SIZE - 1));
    int minY = MX(0, MN((int)floor(carY - CAR_RAD), GRID_SIZE - 1));
    int maxX = MX(0, MN((int)ceil( carX + CAR_RAD), GRID_SIZE - 1));
    int maxY = MX(0, MN((int)ceil( carY + CAR_RAD), GRID_SIZE - 1));

    double	vx = cos(carAng);
    double	vy = sin(carAng);

    // work out enclosing rectangle car coordinates.
    double	rx = len + rad;
    double	ry = wid + rad;

    for( int x = minX; x <= maxX; x++ )
    {
        for( int y = minY; y <= maxY; y++ )
        {
            if( x == GRID_RAD && y == GRID_RAD )
            {
                // ignore cell containing my car
                continue;
            }

            double dx =  vx * (x - carX) + vy * (y - carY);
            double dy = -vy * (x - carX) + vx * (y - carY);
            if( fabs(dx) > rx || fabs(dy) > ry )
            {
                // pt not inside rectangle around car
                continue;
            }

            // now check the corners?
            if( rad )
            {
                double cx = fabs(dx) - len;
                double cy = fabs(dy) - wid;
                if( cx > 0 && cy > 0 )
                {
                    if( cx * cx + cy * cy > rad * rad )
                    {
                        // point is outside of the rounded corners.
                        continue;
                    }
                }
            }

            if( addMask )
                _grid[x][y].addCarMask(carI);
            else
                _grid[x][y].clearAllCarMasks();
        }
    }
}

void	Stuck::fillTrackCells( const MyTrack& track )
{
    // make track cells available.
    vector<vector<Edge> > edges(GRID_SIZE);

    _leftPoints.clear();
    _rightPoints.clear();

    const double margin = 0.5;
    const int NSEG = track.GetSize();
    const Seg& lastSeg = track.GetAt(NSEG - 1);
    Vec3d l1 = lastSeg.pt - (lastSeg.el - margin) * lastSeg.norm;
    Vec3d r1 = lastSeg.pt + (lastSeg.er - margin) * lastSeg.norm;

    _leftPoints.push_back(  lastSeg.pt.GetXY() - lastSeg.el * lastSeg.norm.GetXY() );
    _rightPoints.push_back( lastSeg.pt.GetXY() + lastSeg.er * lastSeg.norm.GetXY() );

    for( int i = 0; i < NSEG; i++ )
    {
        const Seg& currSeg = track.GetAt(i);
        Vec3d l2 = currSeg.pt - (currSeg.el - margin) * currSeg.norm;
        Vec3d r2 = currSeg.pt + (currSeg.er - margin) * currSeg.norm;

        Edge el(l1.x - _gridOrigin.x, l1.y - _gridOrigin.y, l2.x - _gridOrigin.x, l2.y - _gridOrigin.y);
        if( el.sy <= el.ey && el.ey >= 0 && el.sy < GRID_SIZE )
            edges[max(0, el.sy)].push_back(el);

        Edge er(r1.x - _gridOrigin.x, r1.y - _gridOrigin.y, r2.x - _gridOrigin.x, r2.y - _gridOrigin.y);
        if( er.sy <= er.ey && er.ey >= 0 && er.sy < GRID_SIZE )
            edges[max(0, er.sy)].push_back(er);

        l1 = l2;
        r1 = r2;

        _leftPoints.push_back(  lastSeg.pt.GetXY() - lastSeg.el * lastSeg.norm.GetXY() );
        _rightPoints.push_back( lastSeg.pt.GetXY() + lastSeg.er * lastSeg.norm.GetXY() );
    }

    // now have an edge list in bottom to top order.
    for( int scan_y = 0; scan_y < GRID_SIZE; scan_y++ )
    {
        vector<Edge>&    row = edges[scan_y];
        sort( row, scan_y );

        for( int i = 0; i + 1 < (int)row.size(); i += 2 )
        {
            int x1 = MX(0, row[i].x);
            int x2 = MN(GRID_SIZE - 1, row[i + 1].x );

            for( int x = x1; x <= x2; x++ )
                _grid[x][scan_y].removeEdgeMask();
        }

        if( scan_y + 1 < GRID_SIZE )
        {
            vector<Edge>&    nextRow = edges[scan_y + 1];

            for( int i = 0; i < (int)row.size(); i++ )
            {
                if( scan_y + 1 <= row[i].ey )
                    nextRow.push_back(row[i]);
            }

            vector<Edge>().swap(row);
        }
    }

    // make border cells unavailable.
    for( int x = 0; x < GRID_SIZE; x++ )
        for( int y = 0; y < GRID_SIZE; y++ )
            if( x == 0 || y == 0 || x == GRID_SIZE - 1 || y == GRID_SIZE -1 )
                _grid[x][y].addEdgeMask();
}

bool	Stuck::solve( const tCarElt* me )
{
#if defined(_MSC_VER) && _MSC_VER < 1800
    LogSHADOW.debug( "[%d] stuck::solve (exp=%d, qlen=%Iu, best time=%g)\n", me->index, _expansionsN, _pqN.size(), _bestTime );
#else
    LogSHADOW.debug( "[%d] stuck::solve (exp=%d, qlen=%zu, best time=%g)\n", me->index, _expansionsN, _pqN.size(), _bestTime );
#endif

    vector<GridPoint> succs;
    GridPoint car_pt1(*this, me, true, 0);
    GridPoint car_pt2(car_pt1.x(), car_pt1.y(), car_pt1.iang(), false, 0, 0);

    for( int count = 0; count < 500 && !_pqN.empty(); )
    {
        // pop position from edge priority list.
        GridPoint	pt = _pqN.top();
        _pqN.pop();

        if( pt.est_time > _bestTime )
            continue;

        count++;
        _expansionsN++;

        // generate list of successors.
        generateSuccessorsN( pt, succs );

        // for each successor.
        for( vector<GridPoint>::iterator it = succs.begin(); it != succs.end(); ++it )
        {
            // if already visited with a better time --> continue.
            Cell& succCell = at(*it);
            if( succCell.times[it->fwang()] <= it->time )
                continue;

            // calc distance, and use to update time.
            // fill in in visited.
            succCell.times[it->fwang()] = it->time;
            succCell.from[it->fwang()]  = pt.pt;

            // got a new cell to search.
            _pqN.push( *it );

            // if car cell filled in, done.
            if( car_pt1.pt == it->pt || car_pt2.pt == it->pt )
            {
                _bestTime = it->time;
                _bestPt = *it;
            }
        }
    }

    if( !_pqN.empty() )
    {
        return true;
    }

    LogSHADOW.debug( "%d expansions\n", _expansionsN );
    LogSHADOW.debug( "best time: %g\n", _bestTime );
    LogSHADOW.debug( "best x: %d, y: %d, a: %d, fw %d\n", _bestPt.x(), _bestPt.y(), _bestPt.iang(), _bestPt.fw() );

    if( fabs(_bestTime - 9e9f) < 1e8f )
    {
        // failed to find a solution.
        LogSHADOW.debug( "no solution!\n" );

        return false;
    }

    _planIndex = 0;
    _plan.clear();
    _plan.push_back( _bestPt );

    int from = at(_bestPt).from[_bestPt.fwang()];
    float lastTime = 9e9f;
    float time = at(_bestPt).times[_bestPt.fwang()];

    while( from >= 0 && time < lastTime )
    {
        GridPoint	pt(from);
        LogSHADOW.debug( "from x: %d, y: %d, a: %d, fw %d, time %f\n", pt.x(), pt.y(), pt.iang(), pt.fw(), time );
        _plan.push_back( pt );

        lastTime = time;
        from = at(pt).from[pt.fwang()];
        time = at(pt).times[pt.fwang()];
    }

//	dumpGrid();

    LogSHADOW.debug( "stuck::solve -- done\n" );

    _stuckState = EXEC_PLAN;
    _stuckTime = 0;

    return true;
}

// search from car to destination.
bool	Stuck::solveR( const tCarElt* me )
{
#if defined(_MSC_VER) && _MSC_VER < 1800
    LogSHADOW.debug( "[%d] stuck::solveR (exp=%d, qlen=%Iu, best time=%g)\n", me->index, _expansionsR, _pqR.size(), _bestTime );
#else
    LogSHADOW.debug( "[%d] stuck::solveR (exp=%d, qlen=%zu, best time=%g)\n", me->index, _expansionsR, _pqR.size(), _bestTime );
#endif

    vector<GridPoint> succs;

    for( int count = 0; count < 500 && !_pqR.empty(); )
    {
        // pop position from edge priority list.
        GridPoint	pt = _pqR.top();
        _pqR.pop();

        if( pt.est_time > _bestTime )
            continue;

        count++;
        _expansionsR++;

        // generate list of successors.
        generateSuccessorsR( pt, succs );

        // for each successor.
        // Cell& fromCell = at(pt);

        for( vector<GridPoint>::iterator it = succs.begin(); it != succs.end(); ++it )
        {
            // if already visited with a better time --> continue.
            Cell& succCell = at(*it);
            if( succCell.times[it->fwang()] <= it->time )
                continue;

            // calc distance, and use to update time.
            // fill in in visited.
            succCell.times[it->fwang()] = it->time;
            succCell.from[it->fwang()]  = pt.pt;

            // got a new cell to search.
            _pqR.push( *it );

            // if car cell filled in, done.
            if( succCell.isSolution(it->fwang()) )
            {
                _bestTime = it->time;
                _bestPt = *it;
            }
        }
    }

    if( !_pqR.empty() )
    {
        return true;
    }

    LogSHADOW.debug( "%d expansions\n", _expansionsR );
    LogSHADOW.debug( "best time: %g\n", _bestTime );
    LogSHADOW.debug( "best x: %d, y: %d, a: %d, fw %d\n", _bestPt.x(), _bestPt.y(), _bestPt.iang(), _bestPt.fw() );

    if( fabs(_bestTime - 9e9f) < 1e8f )
    {
        // failed to find a solution.
        LogSHADOW.debug( "no solution!\n" );

        return false;
    }

//	dumpGrid();

    _planIndex = 0;
    _plan.clear();
    _plan.push_back( _bestPt );

    int from = at(_bestPt).from[_bestPt.fwang()];
    float lastTime = 9e9f;
    float time = at(_bestPt).times[_bestPt.fwang()];

    while( from >= 0 && time < lastTime )
    {
        GridPoint	pt(from);
        LogSHADOW.debug( "from x: %d, y: %d, a: %d, fw %d, time %f\n", pt.x(), pt.y(), pt.iang(), pt.fw(), time );
        _plan.push_back( pt );

        lastTime = time;
        from = at(pt).from[pt.fwang()];
        time = at(pt).times[pt.fwang()];
    }

    std::reverse(_plan.begin(), _plan.end());

    for( int i = 0; i < (int)_plan.size() - 1; i++ )
    {
        if( _plan[i].fw() != _plan[i + 1].fw() )
            _plan[i].set_fw( _plan[i + 1].fw() );
    }

    dumpGrid();

    LogSHADOW.debug( "stuck::solveR -- done\n" );

    _stuckState = EXEC_PLAN;
    _stuckTime = 0;

    return true;
}

void Stuck::dumpGrid() const
{
    set<unsigned int> pts;

    for( size_t i = 0; i < _plan.size(); i++ )
    {
        const GridPoint& pt = _plan[i];
        pts.insert( pt.x() * 256 + pt.y() );
    }

    char line[GRID_SIZE + 1] = {0};
    for( int y = GRID_SIZE - 1; y >= 0; y-- )
    {
        for( int x = 0; x < GRID_SIZE; x++ )
        {
            const Cell& cell = at(x, y);
            if( cell.isAvailable() )
            {
                if( cell.est_time_to_car == 0 )
                    line[x] = '@';
                else
                {
                    if( pts.find(x * 256 + y) != pts.end() )
                        line[x] = '*';
                    else
                    {
                        int count  = 0;
                        int ctimes = 0;
                        for( int i = 0; i < N_ANGLES * 2; i++ )
                        {
                            if( cell.from[i] != -1 )
                                count += 1;
                            if( cell.times[i] < 0 )
                                ctimes += 1;
                        }
                        line[x] = count == N_ANGLES * 2 ? '~' :
                                  count ? (count < 10 ? count + '0' : count - 10 + 'A') :
                                  cell.est_time_to_dest <  0 ? '-' :
                                  cell.est_time_to_dest == 0 ? '=' :
                                  ctimes != 0 ? ':' :
                                  cell.dist_from_walls < 4 ? '0' + cell.dist_from_walls : '.';
                    }
                }
            }
            else
                line[x] = '#';
        }
        LogSHADOW.debug( "%s\n", line );
    }

    GridPoint	pt(*this, _me, true, 0);
    int iang = pt.iang();
    int octang = ((iang + 4) / 8) & 7;
    int x = pt.x();
    int y = pt.y();
    int dx = delta8_x[octang];
    int dy = delta8_y[octang];
    float ft = at(x, y).times[fwang(iang, true)];
    float bt = at(x, y).times[fwang(iang, false)];
    LogSHADOW.debug( "[%2d,%2d]  CAR  iang %d  ft %g  bt %g\n", x, y, iang, ft, bt );

    for( int i = 0; i < (int)_destinations.size(); i++ )
    {
        const GridPoint& dest = _destinations[i];
        LogSHADOW.debug( "[%2d,%2d]  DEST  iang %d  t %g\n", dest.x(), dest.y(), dest.iang(), at(dest).times[dest.fwang()] );
    }

    int xx = x - dx;
    int yy = y - dy;
    for( int da = -1; da <= 1; da++ )
    {
        int fa = (iang + da) & 0x3F;
        ft = at(xx, yy).times[fwang(fa, true)];
        bt = at(xx, yy).times[fwang(fa, false)];
        LogSHADOW.debug( "[%2d,%2d]  iang %d  ft %g  bt %g\n", xx, yy, fa, ft, bt );
    }
}

void	Stuck::getUnstuck( const MyTrack& track, tCarElt* me, const tSituation* s )
{
    LogSHADOW.debug( "[%d] stuck::getUnstuck\n", me->index );

    if ( static_cast<size_t>(_planIndex) >= _plan.size() - 1 )
    {
        _stuckState = RACING;
        return;
    }

    //	look forward in the plan until either:
    //	1. a change of forwards/backwards movement.
    //	2. a substancial change in heading.
    GridPoint car_pt1(*this, me, true, 0);
    int best = -1;
    double bestDist = 9e9;
    LogSHADOW.debug( "[%d] (%d,%d) nearest pt: ",  me->index, car_pt1.x(), car_pt1.y() );
    for( size_t i = _planIndex; i < _plan.size(); i++ )
    {
        const GridPoint& pt = _plan[i];
        double dist = pt.dist(car_pt1);
#if defined(_MSC_VER) && _MSC_VER < 1800
        LogSHADOW.debug( "[%Iu]=%g, ", i, dist );
#else
        LogSHADOW.debug( "[%zu]=%g, ", i, dist );
#endif
        if( bestDist > dist )
        {
            bestDist = dist;
            best = i;
        }
    }
    LogSHADOW.debug(" best=%d\n", best );

    if( best < 0 )
    {
        // failed to find a best point to follow.
        _stuckState = REINIT;
        _stuckTime  = 0.0;
        return;
    }

    int ahead = best + 1 < static_cast<int>(_plan.size()) ? best + 1 : best;
    double heading = _plan[ahead].iang() * 2 * PI / 64;
    double deltaAng = heading - me->pub.DynGC.pos.az;
    NORM_PI_PI(deltaAng);

    if( bestDist > 1.0 || deltaAng > 20 * PI / 180 )
    {
        // too far from path.  start again.
        _stuckState = REINIT;
        _stuckTime  = 0.0;
        return;
    }

    _planIndex = best;
    bool  fw  = _plan[best].fw();
    float spd = me->pub.DynGC.vel.x;
    int   gear = fw ? 1 : -1;
//	me->ctrl.accelCmd	= MN(0.25f, (8 - fabs(spd)) * 0.05);
    me->ctrl.accelCmd	= MN(0.25f, (10 - fabs(spd)) * 0.25);
    me->ctrl.brakeCmd	= (fw && spd < -0.1) || (!fw && spd > 0.1 ? 0.5 : 0);
    me->ctrl.clutchCmd	= 0;//gear == me->ctrl.gear ? 0.0f : 0.5f;
    me->ctrl.gear		= gear;
    me->ctrl.steer		= (spd > 0 ? deltaAng : -deltaAng) * 2 / me->info.steerLock;

    double dist = calcCarDist(fw, 10, me, s);
    LogSHADOW.debug( "[%d] dir=%d  dist=%g\n", me->index, fw, dist );
    if( dist < 0.2 )
    {
        me->ctrl.accelCmd = 0;
        //me->ctrl.brakeCmd = 1;
        me->ctrl.gear = -me->ctrl.gear;
        _stuckTime += s->deltaTime;

        if( _stuckTime > 1 )
        {
            _stuckState = REINIT;
            _stuckTime  = 0.0;
            return;
        }
    }

#if defined(_MSC_VER) && _MSC_VER < 1800
    LogSHADOW.debug( "[%d] plan index: %d/%Iu  acc=%.3f, gear=%d, da=%.3f, steer=%.3f, dist-ahead=%.3f\n",
#else
    LogSHADOW.debug( "[%d] plan index: %d/%zu  acc=%.3f, gear=%d, da=%.3f, steer=%.3f, dist-ahead=%.3f\n",
#endif
            me->index, _planIndex, _plan.size(), me->ctrl.accelCmd, me->ctrl.gear, deltaAng * 180 / PI,
            me->ctrl.steer * me->info.steerLock * 180 / PI, dist );
}

double	Stuck::calcCarDist( bool fw, double maxDist, const tCarElt* me, const tSituation* s ) const
{
    double	dist  = maxDist;
    CarBounds2d	bounds(me);
    for( int i = 0; i < s->raceInfo.ncars; i++ )
    {
        const tCarElt* oCar = s->cars[i];
        if( oCar == me || (oCar->pub.state & RM_CAR_STATE_NO_SIMU & ~RM_CAR_STATE_PIT) != 0 )
            continue;

        const CarBounds2d	oBounds(oCar);

        dist = bounds.distToSide(fw ? CarBounds2d::SIDE_FRONT : CarBounds2d::SIDE_REAR, dist,  oBounds);
    }

    dist = bounds.distToSide(fw ? CarBounds2d::SIDE_FRONT : CarBounds2d::SIDE_REAR, dist, _leftPoints);
    dist = bounds.distToSide(fw ? CarBounds2d::SIDE_FRONT : CarBounds2d::SIDE_REAR, dist, _rightPoints);

    return dist;
}

void	Stuck::generateSuccessorsN( const GridPoint& from, vector<GridPoint>& succs ) const
{
    succs.clear();

    int x = from.x();
    int y = from.y();
    int a = from.iang();

    for( int da = -1; da <= 1; da++ )
    {
        int		af = (a + da) & ANGLE_MASK;
        int		oct_ang = ((af + HALF_OCTANT) / OCTANT) & 7;

        int		dx = delta8_x[oct_ang];
        int		dy = delta8_y[oct_ang];
//		float	dt = delta8_t[oct_ang];
        float	dt = delta64_t[af];

        // forwards direction.
        const Cell&	fSuccCell1 = at(x - dx, y - dy);
/*		if( fSuccCell1.isAvailable() )
        {
            const Cell&	fSuccCell2 = at(x - 2 * dx, y - 2 * dy);
            if( fSuccCell2.isAvailable() )
            {
                float	fSuccTime = from.time + dt + from.bw() * 1.5f;
                float	fSuccEst  = fSuccTime + fSuccCell1.est_time_to_car;
                succs.push_back( GridPoint(x - dx, y - dy, af, true, fSuccTime, fSuccEst) );
            }
        }*/
        if( fSuccCell1.isAvailable(fwang(af, true)) )
        {
            float	fSuccTime = from.time + dt + from.bw() * 1.5f;
            float	fSuccEst  = fSuccTime + fSuccCell1.est_time_to_car;
            succs.push_back( GridPoint(x - dx, y - dy, af, true, fSuccTime, fSuccEst) );
        }

        // backwards direction.
        const Cell&	bSuccCell1 = at(x + dx, y + dy);
/*		if( bSuccCell1.isAvailable() )
        {
            const Cell&	bSuccCell2 = at(x + 2 * dx, y + 2 * dy);
            if( bSuccCell2.isAvailable() )
            {
                float	bSuccTime = from.time + dt + from.fw() * 1.5f;
                float	bSuccEst  = bSuccTime + bSuccCell1.est_time_to_car;
                succs.push_back( GridPoint(x + dx, y + dy, af, false, bSuccTime, bSuccEst) );
            }
        }*/
        if( bSuccCell1.isAvailable(fwang(af, false)) )
        {
            float	bSuccTime = from.time + dt + from.fw() * 1.5f;
            float	bSuccEst  = bSuccTime + bSuccCell1.est_time_to_car;
            succs.push_back( GridPoint(x + dx, y + dy, af, false, bSuccTime, bSuccEst) );
        }
    }
}

void	Stuck::generateSuccessorsR( const GridPoint& from, vector<GridPoint>& succs ) const
{
    succs.clear();

    int x = from.x();
    int y = from.y();
    int a = from.iang();

    for( int da = -1; da <= 1; da++ )
    {
        int		af = (a + da) & ANGLE_MASK;
//		int		oct_ang = ((af + HALF_OCTANT) / OCTANT) & 7;
        int		oct_ang = ((a + HALF_OCTANT) / OCTANT) & 7;

        int		dx = delta8_x[oct_ang];
        int		dy = delta8_y[oct_ang];
//		float	dt = delta8_t[oct_ang];
        float	dt = delta64_t[af];

        // forwards direction.
        const Cell&	fSuccCell1 = at(x + dx, y + dy);
        if( fSuccCell1.isAvailable() )
        {
            const Cell&	fSuccCell2 = at(x + 2 * dx, y + 2 * dy);
            if( fSuccCell2.isAvailable() )
            {
                float	fSuccTime = from.time + dt + from.bw() * 1.5f + (fSuccCell2.dist_from_walls != 1 ? 0 : 1);
                float	fSuccEst  = fSuccTime + fSuccCell1.est_time_to_dest;
                succs.push_back( GridPoint(x + dx, y + dy, af, true, fSuccTime, fSuccEst) );
            }
        }

        // backwards direction.
        const Cell&	bSuccCell1 = at(x - dx, y - dy);
        if( bSuccCell1.isAvailable() )
        {
            const Cell&	bSuccCell2 = at(x - 2 * dx, y - 2 * dy);
            if( bSuccCell2.isAvailable() )
            {
                float	bSuccTime = from.time + dt + from.fw() * 1.5f + (bSuccCell2.dist_from_walls != 1 ? 0 : 1);;
                float	bSuccEst  = bSuccTime + bSuccCell1.est_time_to_dest;
                succs.push_back( GridPoint(x - dx, y - dy, af, false, bSuccTime, bSuccEst) );
            }
        }
    }
}
