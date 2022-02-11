/***************************************************************************

    file        : TeamInfo.cpp
    created     : 21 Apr 2017
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

// TeamInfo.cpp: implementation of the TeamInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "TeamInfo.h"
#include "Utils.h"

using namespace std;

// The "SHADOW" logger instance.
extern GfLogger* PLogSHADOW;
#define LogSHADOW (*PLogSHADOW)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TeamInfo::TeamInfo()
{
}

TeamInfo::~TeamInfo()
{
    Empty();
}

void	TeamInfo::Empty()
{
//	PRINTF( "TeamInfo::Empty()\n" );
    for( int i = 0; i < (int)m_items.size(); i++ )
        delete m_items[i];
    m_items.clear();
}

void	TeamInfo::Add( int index, Item* pItem )
{
    LogSHADOW.debug( "TeamInfo::Add [%d] %s\n", index, pItem->pCar->info.carName );

    if( static_cast<size_t>(index) >= m_items.size() )
    {
        // expand array.
        m_items.resize( index + 1 );
    }

    if( m_items[index] )
        delete m_items[index];
    m_items[index] = pItem;

    // see if we can find a team mate.
//	if( pItem->team >= 0 )
    {
        for( int i = 0; i < (int)m_items.size(); i++ )
        {
            if( i != index && m_items[i] &&
                strcmp(m_items[i]->teamName, pItem->teamName) == 0 &&
                m_items[i]->pOther == 0 )
            {
                // found a team-mate.
                pItem->pOther = m_items[i];
                m_items[i]->pOther = pItem;
                break;
            }
        }
    }
}

const TeamInfo::Item*	TeamInfo::GetAt( int index ) const
{
    return m_items[index];
}

TeamInfo::Item*	TeamInfo::GetAt( int index )
{
    return m_items[index];
}

const TeamInfo::Item*	TeamInfo::GetTeamMate( const CarElt* pCar ) const
{
    for( int i = 0; i < (int)m_items.size(); i++ )
    {
        if( m_items[i] && IsTeamMate(m_items[i]->pCar, pCar) )
            return m_items[i];
    }

    return NULL;
}

bool	TeamInfo::IsTeamMate( const CarElt* pCar0, const CarElt* pCar1 ) const
{
//	return strcmp(pCar0->_teamname, pCar1->_teamname) == 0;
    return pCar0->race.pit == pCar1->race.pit;
//	const Item*	pItem0 = GetAt(pCar0->index);
//	const Item*	pItem1 = pItem0 ? pItem0->pOther : 0;
//	return pItem1 ? (pItem1->pCar == pCar1) : false;
//	return false;
//	return true;
}

