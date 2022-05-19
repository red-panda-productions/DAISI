/***************************************************************************

    file        : Strategy.cpp
    created     : 3 Jan 2020
    copyright   : (C) 2020 Xavier BERTAUX

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// Strategy.cpp: implementation of the Strategy class.
//
//////////////////////////////////////////////////////////////////////

#include "Strategy.h"
#include "Utils.h"

#include <robottools.h>

// The "SHADOW" logger instance.
extern GfLogger* PLogSHADOW;
#define LogSHADOW (*PLogSHADOW)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Strategy::Strategy( const MyTrack& track, const PitPath& pitPath ):
    m_track(track),
    m_pitPath(pitPath),
      m_warnTyreLimit(0.3),
      m_dangerTyreLimit(0.03),
      m_HasTYC(false),
      m_state(PIT_NONE),
      m_lastTyreWear(1.0),
      m_totalLaps(0),
      m_pitType(PT_NORMAL)
{
}

Strategy::~Strategy()
{
}

void	Strategy::SetTyreLimits( double warnTireLimit, double dangerTireLimit )
{
    m_warnTyreLimit	= warnTireLimit;
    m_dangerTyreLimit = dangerTireLimit;
    LogSHADOW.info(" # Tyre warn %.2f - Tyre Danger = %.2f\n", m_warnTyreLimit, m_dangerTyreLimit);
}

void	Strategy::Process( CarElt* pCar, TeamInfo::Item* pTeamInfo )
{
    //
    //	update statistics.
    //
    double tyreWear = 1.0;

    if (m_HasTYC)
    {
        for( int w = 0; w < 4; w++ )
            tyreWear = MN(tyreWear, pCar->priv.wheel[w].treadDepth);

        LogSHADOW.debug(" # Tyre wear = %.8f\n", tyreWear);

        if( tyreWear < m_lastTyreWear )
        {
            m_totalTyreWear += (tyreWear - m_lastTyreWear);
            m_lastTyreWear = tyreWear;
            LogSHADOW.debug(" # TyreWear = %.8f - Last TyreWear = %.8f\n", tyreWear, m_lastTyreWear);
        }        
    }
    double	tyreWearPerM = 0;

    if( pCar->_distRaced > 0 )
    {
        if(m_HasTYC)
        {
            tyreWearPerM = (1.0 - m_lastTyreWear) / pCar->_distRaced;
            LogSHADOW.debug(" Wear per meter = %.8f\n", tyreWearPerM);
        }
    }

    //
    //	decide if we want to pit.
    //

    int		raceLaps = pCar->_laps + pCar->_remainingLaps;
    double	trackLen = m_track.GetLength();
    double	distToRace = trackLen * raceLaps - pCar->_distRaced;
    int		nPitsIfPitNow = 1 + int(ceil(pCar->_tank));

    bool	delayRepair = nPitsIfPitNow;
    double	repairLimit = delayRepair;

    // if(m_HasTYC)
    //      double tyreWearPerLap = tyreWearPerM * trackLen;

    bool	pitAvailable = true;
    double	minPitLaps = 1;

    //#ifndef DEV
    if( pTeamInfo->pOther &&
            (pTeamInfo->pOther->pCar->pub.state & RM_CAR_STATE_NO_SIMU) == 0 )
    {
        // if same number of laps left until pit required, raise min pit laps
        // for the car with most urgent need.
        minPitLaps = 2;
        pitAvailable = !pTeamInfo->pOther->usingPit;
    }
    //#endif

    //	bool	likeToPit = pCar->_dammage >=  500 || pCar->_fuel < 90;
    //	bool	likeToPit = pCar->_dammage + damagePerLap >= repairLimit ||
    m_pitType = PT_NORMAL;
    bool likeToPit = pitAvailable && (raceLaps > 20 &&
                                      tyreWear < m_warnTyreLimit);

#if defined(DEV) && 0  // don't want to leave this in the code by mistake for TRB races.
    likeToPit = true;
    tyreWear = 1.0;
#endif

    // pitting on the last lap is a silly thing to do (think about it!).
    //	DEBUGF( "******* remaining laps %d\n", pCar->race.remainingLaps );
    int	remainingLaps = pCar->race.remainingLaps + 1;
    if( remainingLaps <= 1 )
        likeToPit = false;

    // need to pit due to a penalty -- normal pitting has priority here.
    const CarPenalty* pPenalty = likeToPit ? 0 : GF_TAILQ_FIRST(&pCar->race.penaltyList);
    if( pPenalty &&
            pPenalty->lapToClear < raceLaps &&
            (pPenalty->penalty == RM_PENALTY_DRIVETHROUGH ||
             pPenalty->penalty == RM_PENALTY_STOPANDGO) )
    {
        if( pitAvailable || pPenalty->penalty == RM_PENALTY_DRIVETHROUGH )
        {
            // got a pit related penalty to serve before the end of the race.
            likeToPit = true;

            // if got a drive through penalty then change type so the correct
            // raceline can be used.
            if( pPenalty->penalty == RM_PENALTY_DRIVETHROUGH )
                m_pitType = PT_DRIVE_THROUGH;
        }
    }

    //
    //	manage the pit state.
    //

    double	trackPos = RtGetDistFromStart(pCar);

    switch( m_state )
    {
    case PIT_NONE:
        // only enable pitting while not in the pitting section of the track.
        if( !m_pitPath.InPitSection(trackPos) && likeToPit )
        {
            // let's stop in the pits.
            m_state = PIT_ENABLED;
            LogSHADOW.debug( "***** PIT_ENTER\n " );
        }
        break;

    case PIT_ENABLED:
        // pitting enabled: change to pitting if now in the pitting section of the track.
        if( !likeToPit )
        {
            // changed it's mind!
            m_state = PIT_NONE;
        }
        else if( m_pitPath.InPitSection(trackPos) )
        {
            m_state = PIT_ENTER;
        }
        break;

    case PIT_ENTER:
        //			if( pCar->ctrl.brakeCmd > 0.1 && pCar->ctrl.brakeCmd < 0.5 )
        //				pCar->ctrl.brakeCmd = 0.5;

        if( !m_pitPath.CanStop(trackPos) )
            break;

        pCar->ctrl.raceCmd = RM_CMD_PIT_ASKED;

        if(m_HasTYC)
            pCar->pitcmd.tireChange	= tyreWear > 0.5 ? tCarPitCmd::ALL : tCarPitCmd::NONE;
        m_state = PIT_ASKED;
        LogSHADOW.debug( "****** PIT_ASKED\n" );

        // falls through...

    case PIT_ASKED:
        if(m_HasTYC)
            pCar->pitcmd.tireChange	= tyreWear < m_warnTyreLimit ? tCarPitCmd::ALL : tCarPitCmd::NONE;

        if( m_pitPath.CanStop(trackPos) &&
                (pCar->ctrl.raceCmd & RM_CMD_PIT_ASKED) )
        {
            // stop.
            pCar->ctrl.accelCmd = 0;
            pCar->ctrl.brakeCmd = 0.7f;

            PtInfo	pi;
            m_pitPath.GetPtInfo( trackPos, pi );

            if( //(pi.offs - pCar->_trkPos.toMiddle) > 0.5 &&
                    pCar->_speed_x < 0.5 )
            {
                pCar->ctrl.accelCmd = 0.1f;
                pCar->ctrl.brakeCmd = 0;
            }

            if( pPenalty )
            {
                pCar->pitcmd.stopType = pPenalty->penalty == RM_PENALTY_STOPANDGO ?
                            RM_PIT_STOPANDGO : RM_PIT_REPAIR;
                pCar->pitcmd.repair		= 0;
                pCar->pitcmd.tireChange	= tCarPitCmd::NONE;
            }
            else
            {
                //					double	fuel = distToRace * fuelPerM * 1.02 + fuelPerLap - pCar->_fuel;
                //					double	fuel = distToRace * fuelPerM * 1.02 + fuelPerLap;
                pCar->pitcmd.stopType	= RM_PIT_REPAIR;

                if(m_HasTYC)
                    pCar->pitcmd.tireChange	= tyreWear < 0.5 ? tCarPitCmd::ALL : tCarPitCmd::NONE;
            }

            LogSHADOW.debug( "****** repair %d  twear %0.4f  tchg 0x%x\n",
                             pCar->pitcmd.repair, tyreWear, pCar->pitcmd.tireChange );
        }
        else
        {
            // pit stop finished -- need to exit pits now.
            m_state = PIT_EXIT;
            LogSHADOW.debug( "***** PIT_EXIT\n " );
        }
        break;

    case PIT_EXIT:
        if( !m_pitPath.InPitSection(trackPos) )
        {
            m_state = PIT_NONE;
            LogSHADOW.debug( "***** PIT_NONE\n " );
        }
        else if( m_pitPath.CanStop(trackPos) )
        {
            // go.
            pCar->ctrl.accelCmd = 0.5;
            pCar->ctrl.brakeCmd = 0;
        }

        break;
    }

    //	if( m_pitPath.CanStop(trackPos) )
    //		pTeamInfo->lapsUntilPit = pCar->_fuel / fuelPerLap - 1;
    pTeamInfo->usingPit = m_state == PIT_ENTER || m_state == PIT_ASKED;
}

bool	Strategy::WantToPit() const
{
    return m_state >= PIT_ENTER && m_state <= PIT_EXIT;
    //	return true;
}

int		Strategy::PitType() const
{
    return m_pitType;
    //	return PT_DRIVE_THROUGH;
    //	return PT_NORMAL;
}