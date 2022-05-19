/***************************************************************************

    file        : Opponent.cpp
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

// Opponent.cpp: implementation of the Opponent class.
//
//////////////////////////////////////////////////////////////////////

#include "Opponent.h"

#include "Quadratic.h"
#include "Utils.h"
#include "Driver.h"

#include <robottools.h>

// The "SHADOW" logger instance.
extern GfLogger* PLogSHADOW;
#define LogSHADOW (*PLogSHADOW)

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Opponent::Opponent()
{
}

Opponent::~Opponent()
{
}

void	Opponent::Initialise( MyTrack* pTrack, CarElt* pCar )
{
    m_path.Initialise( pTrack, pCar );
}

CarElt*	Opponent::GetCar()
{
    return m_path.GetCar();
}

const CarElt*	Opponent::GetCar() const
{
    return m_path.GetCar();
}

const Opponent::Info&	Opponent::GetInfo() const
{
    return m_info;
}

Opponent::Info&	Opponent::GetInfo()
{
    return m_info;
}

void	Opponent::UpdatePath()
{
    m_path.Update();
}

void	Opponent::UpdateSit(
    const CarElt*		myCar,
    const Situation*	s,
    const TeamInfo*		pTeamInfo,
    double				myDirX,
    double				myDirY,
    const PtInfo&		oppPi )
{
    CarElt*	oCar = m_path.GetCar();
//	if( (oCar->pub.state & (RM_CAR_STATE_NO_SIMU | RM_CAR_STATE_OUT)) )
    if( (oCar->pub.state & RM_CAR_STATE_NO_SIMU & ~RM_CAR_STATE_PIT) )
        return;

    // save point info
    m_info.sit.pi = oppPi;

    // work out speed of car.
    m_info.sit.spd = hypot(oCar->_speed_X, oCar->_speed_Y);

    // decide if car is stuck.
    if( m_info.sit.spd < 1 && s->currentTime > 1 )
        m_info.sit.stuckTime += s->deltaTime;
    else
        m_info.sit.stuckTime = MX(0, MN(m_info.sit.stuckTime - 0.25, 4));

    // work out track relative speed of other car.
    Vec2d norm = m_path.GetTrack()->CalcNormal(oCar->_distFromStartLine);
    m_info.sit.tVX = norm.x * oCar->_speed_Y - norm.y * oCar->_speed_X;
    m_info.sit.tVY = norm.x * oCar->_speed_X + norm.y * oCar->_speed_Y;

    // work out track relative yaw of other car.
    m_info.sit.tYaw = oCar->_yaw - Utils::VecAngle(norm) - PI * 0.5;
    NORM_PI_PI(m_info.sit.tYaw);

    // work out avg velocity of other car.
    m_info.sit.agVX = m_info.sit.agVX * 0.75 + oCar->pub.DynGCg.vel.x * 0.25;
    m_info.sit.agVY = m_info.sit.agVY * 0.75 + oCar->pub.DynGCg.vel.y * 0.25;
    m_info.sit.ragVX = myDirX * m_info.sit.agVX + myDirY * m_info.sit.agVY;
    m_info.sit.ragVY = myDirY * m_info.sit.agVX - myDirX * m_info.sit.agVY;

    // work out avg acceleration of other car.
    m_info.sit.agAX = m_info.sit.agAX * 0.75 + oCar->pub.DynGCg.acc.x * 0.25;
    m_info.sit.agAY = m_info.sit.agAY * 0.75 + oCar->pub.DynGCg.acc.y * 0.25;
    m_info.sit.ragAX = myDirX * m_info.sit.agAX + myDirY * m_info.sit.agAY;
    m_info.sit.ragAY = myDirY * m_info.sit.agAX - myDirX * m_info.sit.agAY;

    // work out lateral accelerations.
    m_info.sit.rAX = myDirX * oCar->pub.DynGCg.acc.x + myDirY * oCar->pub.DynGCg.acc.y;
    m_info.sit.rAY = myDirY * oCar->pub.DynGCg.acc.x - myDirX * oCar->pub.DynGCg.acc.y;
    m_info.sit.arAX = m_info.sit.arAX * 0.75 + m_info.sit.rAX * 0.25;
    m_info.sit.arAY = m_info.sit.arAY * 0.75 + m_info.sit.rAY * 0.25;

    // offset from track centre line.
    m_info.sit.offs = -oCar->_trkPos.toMiddle;

    // the rest of the calcs are car-car relative, and make no sense if both
    //	cars are the same.
    if( oCar == myCar )
        return;

    // calc other cars position, velocity relative to my car (global coords).
    double	dPX = oCar->pub.DynGCg.pos.x - myCar->pub.DynGCg.pos.x;
    double	dPY = oCar->pub.DynGCg.pos.y - myCar->pub.DynGCg.pos.y;
    double	dVX  = oCar->_speed_X - myCar->_speed_X;
    double	dVY  = oCar->_speed_Y - myCar->_speed_Y;

    // work out relative position, velocity in local coords (coords of my car).
    double	rdPX = myDirX * dPX + myDirY * dPY;
    double	rdPY = myDirY * dPX - myDirX * dPY;
    double	rdVX = myDirX * dVX + myDirY * dVY;
    double	rdVY = myDirY * dVX - myDirX * dVY;

    m_info.sit.rdPX = rdPX;
    m_info.sit.rdPY = rdPY;
    m_info.sit.rdVX = rdVX;
    m_info.sit.rdVY = rdVY;

    m_info.sit.minDXa = (myCar->_dimension_x + oCar->_dimension_x) / 2;
    m_info.sit.minDXb = m_info.sit.minDXa;
    m_info.sit.minDY = (myCar->_dimension_y + oCar->_dimension_y) / 2;

    double	myVelAng = fabs(myCar->pub.speed) < 0.1 ? myCar->_yaw : atan2(myCar->_speed_Y, myCar->_speed_X);
    double	myYaw = myCar->_yaw - myVelAng;
    NORM_PI_PI(myYaw);
    double	oYaw = oCar->_yaw - myVelAng;
    NORM_PI_PI(oYaw);
    double	extraSide = (m_info.sit.minDXa - m_info.sit.minDY) *
                (fabs(sin(myYaw)) + fabs(sin(oYaw)));
//	double	extraTan rdPX > 0 && m_info.sit.agAX < -2 ? 0.25 : 0;
    double	extraTan = 0;
//	m_info.sit.minDY += MX(0, MN((rdPX - m_info.sit.minDY) * 0.1, 4));
    if( (oCar->pub.state & RM_CAR_STATE_PIT) == 0 )
    {
        // opponent on track.
//  	m_info.sit.minDX  += 1.0;//pTeamInfo->IsTeamMate(myCar, oCar) ? 0.5 : 1.0;
//  	m_info.sit.minDXa += extraTan + 2.0;//pTeamInfo->IsTeamMate(myCar, oCar) ? 0.5 : 1.0;
//  	m_info.sit.minDXb += extraTan + 0.5;//pTeamInfo->IsTeamMate(myCar, oCar) ? 0.5 : 1.0;
        m_info.sit.minDY  += extraSide + 1;//0.5;
        m_info.sit.minDXa += extraTan + 2.0;//pTeamInfo->IsTeamMate(myCar, oCar) ? 0.5 : 1.0;
        m_info.sit.minDXb += extraTan + 0.5;//pTeamInfo->IsTeamMate(myCar, oCar) ? 0.5 : 1.0;
    }
    else
    {
        // opponent stationary in pit box.
        m_info.sit.minDY  += extraSide;
        m_info.sit.minDXa += extraTan + 0.5;
        m_info.sit.minDXb += extraTan + 0.5;
    }
    m_info.sit.decDX = m_info.sit.minDXb + 2;
    m_info.sit.accDX = m_info.sit.minDXb;

//	if( fabs(extraSide) > 0.2 )
//		DEBUGF( "****** angDiff %.3f extraSide %.2f ******\n", angDiff, extraSide );

    // work out positions of car from start of track.
    double	myPos = RtGetDistFromStart((tCarElt*)myCar);
    double	hisPos = RtGetDistFromStart((tCarElt*)oCar);
    double	relPos = hisPos - myPos;
    double	trackLen = m_path.GetTrack()->GetLength();
    if( relPos > trackLen / 2 )
        relPos -= trackLen;
    else if( relPos < -trackLen / 2 )
        relPos += trackLen;

    m_info.sit.relPos = relPos;
}

void	Opponent::ProcessMyCar(
    const Situation*	s,
    const TeamInfo*		pTeamInfo,
    const CarElt*		myCar,
    const Sit&			mySit,
    const Driver&		me,
    double				myMaxAccX,
    int					idx )
{
    CarElt*	oCar = m_path.GetCar();

    m_info.flags = 0;

    if( oCar == myCar ||
//		(oCar->pub.state & (RM_CAR_STATE_NO_SIMU | RM_CAR_STATE_OUT)) )
        (oCar->pub.state & RM_CAR_STATE_NO_SIMU & ~RM_CAR_STATE_PIT) )
    {
        return;
    }

    const Sit&	oSit = m_info.sit;

/*
    if( oSit.relPos < 30 && oSit.relPos > -5 )
    {
        DEBUGF( "%2d (%6.2f, %5.1f)   rdv(%6.2f, %6.2f)  rtv(%6.2f)\n",
                idx, oSit.rdPX - oSit.minDX, oSit.rdPY,
                oSit.rdVX, oSit.rdVY,
                oSit.tVX - mySit.tVX );
    }
*/
    m_info.flags |= oSit.rdPY < 0 ? F_LEFT : F_RIGHT;
    m_info.flags |= oSit.offs < 0 ? F_TRK_LEFT : F_TRK_RIGHT;

    if( fabs(oSit.tYaw) > 45 * PI / 180 || oSit.spd < 15 )
    {
        m_info.flags |= F_DANGEROUS;
        m_info.dangerousLatchTime = 2.0;
        LogSHADOW.debug( "danger (%s) angle=%6.1f  relx=%6.1f  relvx=%6.1f  roppavga=%6.1f  roppa=%6.1f\n",
                oCar->_name, oSit.tYaw * 180 / PI, oSit.rdPX, oSit.rdVX, oSit.ragAX, oSit.rAX );
    }
    else
    {
        m_info.dangerousLatchTime -= s->deltaTime;
        if( m_info.dangerousLatchTime <= 0 )
        {
            m_info.flags &= ~F_DANGEROUS;
            m_info.dangerousLatchTime = 0;
        }
    }

//	double	distAhead = MX(20, mySit.spd * mySit.spd / 30);
//	double	distAhead = MX(20, mySit.spd * mySit.spd / 25);
    double	distAhead = MX(20, mySit.spd * mySit.spd / 20);

    if( (m_info.flags & F_DANGEROUS) == 0 )
        distAhead = MN(MX(40, distAhead), 80);

    if( pTeamInfo->IsTeamMate(myCar, oCar) )
    {
        m_info.flags |= F_TEAMMATE;
    }

    if( oSit.relPos > 0 && oSit.relPos < 10 )
        m_info.closeAheadTime += s->deltaTime;
    else
        m_info.closeAheadTime = MX(0, m_info.closeAheadTime - s->deltaTime * 0.1);

    if( oSit.relPos < 0 && oSit.relPos > -10 )
        m_info.closeBehindTime += s->deltaTime;
    else
        m_info.closeBehindTime = MX(0, m_info.closeBehindTime - s->deltaTime * 0.1);

    LogSHADOW.debug( "(%s) dist ahead %6.1f   relPos %6.1f\n", oCar->_name, distAhead, oSit.relPos );

    if( oSit.relPos < distAhead && oSit.relPos > -25 )
//	if( oSit.relPos < 40 && oSit.relPos > -25 )
    {
//		double		oVX = oSit.ragVX;
        double		oVX = mySit.spd + oSit.rdVX;

        m_info.flags |= F_TRAFFIC;

        if( oSit.rdPX > oSit.minDXa )
        {
            m_info.flags |= F_AHEAD | F_FRONT;

//			Quadratic	myPar(0, 0, 0, mySit.arAY);
//			Quadratic	oPar(0, oSit.rdPY, oSit.rdVY, oSit.arAY);
            Quadratic	myPar(0, 0, 0, mySit.ragAY);
            Quadratic	oPar(0, oSit.rdPY, oSit.rdVY, oSit.ragAY);
            Quadratic	relPar = oPar - myPar;

//			if( relVX < 0 || oAXg < 0 )
//			if( relVX < 0 || relAX < 0 )
            {
                // time to catch up at present speeds...
                double		acc = oSit.ragAX;// - (myCar->_accel_x + 3);
//				Quadratic	q(acc / 2, oSit.rdVX, oSit.rdPX - oSit.minDX - 0.2);
                Quadratic	q(acc / 2, oSit.rdVX, oSit.rdPX - oSit.minDXa);
//				Quadratic	q(acc / 2, oSit.ragVX - mySit.ragVX, oSit.rdPX - oSit.minDX - 0.2);
                double		t;
                if( q.SmallestNonNegativeRoot(t) )
                {
                    double	catchY = relPar.CalcY(t);
/*
                    if( t < 10 && idx == 16 )
                        DEBUGF( "%2d (%6.2f, %5.1f) t %.2f  rdv(%6.2f, %6.2f)  oAg(%6.2f, %6.2f)  cY %.2f\n",
                                idx, oSit.rdPX - oSit.minDX, oSit.rdPY, t,
                                oSit.rdVX, oSit.rdVY,
                                oSit.ragAX, oSit.ragAY, catchY );
*/
                    m_info.flags |= F_CATCHING;
                    m_info.catchTime = t;
                    m_info.catchY = catchY;
//					m_info.catchSpd = oVX;
//					m_info.catchSpd = oSit.tVX;
//					m_info.catchSpd = oCar->_speed_x;
                    m_info.catchSpd = oSit.rdPX < 15 ? oVX : oSit.tVX;

//					double	hisSpd = oSit.ragVX + oSit.ragAX * t;
//					double	decel = (mySit.ragVX - hisSpd) / t;
                    double	decel = (oSit.rdVX * oSit.rdVX) / (2 * (oSit.rdPX - 5)) - oSit.ragAX;

                    m_info.catchDecel = MX(0, decel);

//					if( fabs(catchY) < oSit.minDY ||
//						fabs(oSit.rdPY) < oSit.minDY ||
//						catchY * oSit.rdPY < 0 )
                    double raceLineY = oSit.pi.offs + oCar->pub.trkPos.toMiddle;
                    LogSHADOW.debug( "(%s) racelineoffs %0.3f  pioffs %0.3f  tomid %0.3f  catch-t %0.3f  catch-decel %0.3f\n",
                            oCar->info.name, raceLineY, oSit.pi.offs, oCar->pub.trkPos.toMiddle, t, m_info.catchDecel );
                    if( fabs(catchY) < oSit.minDY || fabs(raceLineY) < oSit.minDY )
                    {
                        m_info.flags |= F_COLLIDE;

                        if( //m_info.catchDecel < 2 &&
                            oSit.rdPX < oSit.minDXa + 0.15 )
                            m_info.catchDecel = 999;
                    }
                    else
                    {
                        // see if we hit on the side while passing.
                        q.Setup( acc / 2, oSit.rdVX, oSit.rdPX + oSit.minDXa );// + m_info.minDX );
//						q.Setup( acc / 2, oSit.ragVX - mySit.ragVX, oSit.rdPX );// + m_info.minDX );
                        if( q.SmallestNonNegativeRoot(t) )
                        {
                            catchY = relPar.CalcY(t);

//							if( fabs(catchY) < oSit.minDY ||
                            if( fabs(catchY) < oSit.minDY ||
//								catchY * m_info.catchY < 0 )
                                catchY * oSit.rdPY < 0 )
                            {
                                m_info.flags |= F_COLLIDE;
                                m_info.catchY = SGN(m_info.catchY) * (oSit.minDY - 0.1);
                            }
                        }
                    }
                }

                q.Setup( oSit.ragAX - myMaxAccX, oSit.ragVX - mySit.ragVX,// - 3,
                            oSit.rdPX - oSit.minDXa - 0.2 );
                if( q.SmallestNonNegativeRoot(t) )
                {
                    double	catchY = relPar.CalcY(t);

                    m_info.flags |= F_CATCHING_ACC;
                    m_info.catchAccTime = t;
                    m_info.catchAccY = catchY;
                    m_info.catchAccSpd = oVX;
                }
            }

            if( myCar->race.distRaced > oCar->race.distRaced + 50 )
            {
                m_info.flags |= F_BEING_LAPPED;
            }
        }
        else
        {
            if( oSit.rdPX < -oSit.minDXb )	// behind
            {
                m_info.flags |= F_BEHIND | F_REAR;

                if( oSit.rdVX < 0 )
                {
                    m_info.flags |= F_CATCHING;
                    m_info.catchTime = (oSit.rdPX + oSit.minDXb) / oSit.rdVX;
                    m_info.catchY = oSit.rdPY;
                    m_info.catchSpd = oVX;
                }
            }
            else // to side
            {
                m_info.flags |= F_TO_SIDE;
                m_info.flags |= oSit.rdPX > 0 ? F_FRONT : F_REAR;

                double	aheadDist = oSit.minDXa * 0.5;//0.33;
                if( fabs(oSit.rdPY) < oSit.minDY )
                {
                    // colliding now.
                    m_info.flags |= F_COLLIDE;
                    m_info.catchTime = 0;
                    m_info.catchY = oSit.rdPY;
                    m_info.catchSpd = oSit.rdPX > aheadDist ? oVX - 3 : 200;
                    m_info.catchDecel = 999;
                }
                else if( oSit.rdPX > 0 && oSit.rdPY * oSit.rdVY < 0 )
                {
                    // side collision in t seconds?
                    double	t = (fabs(oSit.rdPY) - oSit.minDY) / fabs(oSit.rdVY);
                    double	collX = oSit.rdPX + oSit.rdVX * t;

                    if( collX > aheadDist && collX < oSit.minDXa )
                    {
                        double	relSpd = (oSit.minDXa - oSit.rdPX) / t;
                        m_info.flags |= F_COLLIDE;
                        m_info.catchTime = t;
                        m_info.catchY = SGN(oSit.rdPY) * (oSit.minDY - 0.1);
                        m_info.catchSpd = oVX - 3;
                        m_info.catchDecel = (mySit.spd - (oVX - relSpd)) / t;
                    }
                }
            }

            if( (m_info.flags & (F_REAR | F_TO_SIDE)) &&
                myCar->race.distRaced + 50 < oCar->race.distRaced &&
                m_info.closeAheadTime <= m_info.closeBehindTime )
            {
                m_info.flags |= F_LAPPER;
            }
        }

        if( 0 < oSit.rdPX && oSit.rdPX < oSit.minDXa + 2 &&
            fabs(oSit.rdPY) < oSit.minDY + 2 )
        {
            m_info.flags |= F_CLOSE;
        }
    }
    else if( oSit.relPos < 0 )
    {
        m_info.flags |= F_BEHIND | F_REAR;
    }

    const double timeLimit = 4;
    const double closeDist = 10;
    m_info.newCatchSpd = oSit.tVX - mySit.tVX;
    m_info.newCatching = false;

    if( oSit.relPos > oSit.minDXa )
    {
        bool	oDangerous = (m_info.flags & F_DANGEROUS) != 0;

        if( m_info.newCatchSpd < 0 )
        {
            double	t1 = -(oSit.relPos - oSit.minDXa) / m_info.newCatchSpd;
            double	t2 = -(oSit.relPos + oSit.minDXa) / m_info.newCatchSpd;
            m_info.newCatching = t1 <= timeLimit || oDangerous ||
                                 oSit.relPos - oSit.minDXa < closeDist;
            m_info.newCatchTime = t1;
            m_info.newAheadTime = t2;
        }
    }
    else if( oSit.relPos >= -oSit.minDXb )
    {
        m_info.newCatching = true;
        m_info.newCatchTime = 0;
        m_info.newAheadTime = 0;
    }

    if( m_info.newCatching )
    {
        double	pos = oCar->_distFromStartLine;
        double	myPos = myCar->_distFromStartLine;
        double	offs = -oCar->_trkPos.toMiddle;

        double	w = m_path.GetTrack()->GetWidth() * 0.5 - 1;

        double	catPos  = pos + oSit.tVX * m_info.newCatchTime;
        double	catOffs = offs + oSit.tVY * m_info.newCatchTime;
        catOffs = MX(-w, MN(catOffs, w));

        double	ahdPos  = pos + oSit.tVX * m_info.newAheadTime;
        double	ahdOffs = offs + oSit.tVY * m_info.newAheadTime;
        ahdOffs = MX(-w, MN(ahdOffs, w));

        double	midPos = (catPos + ahdPos) * 0.5;

        double	trackLen = m_path.GetTrack()->GetLength();
        ahdPos = fmod(ahdPos, trackLen);
        catPos = fmod(catPos, trackLen);
        midPos = fmod(midPos, trackLen);

        m_info.newMidPos = midPos;

        PtInfo	pi;
        me.GetPtInfo( Driver::PATH_NORMAL, midPos * 0.5, pi );
        m_info.newBestOffset = pi.offs;

        double	L = catOffs - oSit.minDY - 1.0;
        double	R = catOffs + oSit.minDY + 1.0;

        // TODO: change this to be the predicted position...
        double	toL, toR;
        me.GetPathToLeftAndRight( oCar, toL, toR );

        m_info.newPiL.isSpace = L > offs - toL;
        m_info.newPiL.goodPath = false;
        m_info.newPiL.myOffset = 0;

        if( m_info.newPiL.isSpace )
        {
            m_info.newPiL.offset = L;
            m_info.newPiL.mySpeed = me.CalcBestSpeed(midPos, MN(L, pi.offs));
            m_info.newPiL.goodPath = m_info.newPiL.mySpeed > oSit.spd;

            {
                double	u, v;
                me.CalcBestPathUV(midPos, L, u, v);
                m_info.newPiL.bestU = u;
                m_info.newPiL.bestV = v;
                m_info.newPiL.myOffset = me.CalcPathOffset(myPos, u, v);
            }
        }

        m_info.newPiR.isSpace = R < offs + toR;
        m_info.newPiR.goodPath = false;
        m_info.newPiR.myOffset = 0;
        if( m_info.newPiR.isSpace )
        {
            m_info.newPiR.offset = R;
            m_info.newPiR.mySpeed = me.CalcBestSpeed(midPos, MX(R, pi.offs));
            m_info.newPiR.goodPath = m_info.newPiR.mySpeed > oSit.spd;

            {
                double	u, v;
                me.CalcBestPathUV(midPos, R, u, v);
                m_info.newPiR.bestU = u;
                m_info.newPiR.bestV = v;
                m_info.newPiR.myOffset = me.CalcPathOffset(myPos, u, v);
            }
        }
    }
}
