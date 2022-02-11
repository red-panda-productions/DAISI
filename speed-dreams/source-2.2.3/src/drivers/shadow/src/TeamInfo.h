/***************************************************************************

    file        : TeamInfo.h
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

// TeamInfo.h: interface for the TeamInfo class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _TEAMINFO_H_
#define _TEAMINFO_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include <car.h>

class TeamInfo
{
public:
    struct	Item
    {
        int			index;			// index of car in race.
        const char*	teamName;		// name of team.
        int			damage;			// damage of this team member.
        bool		usingPit;		// true if entering pits or in pit.
        double		lapsUntilPit;	// how many more laps until need to pit due to fuel.
        Item*		pOther;			// the other team member.
        CarElt*		pCar;			// the car of this team member.
    };

public:
    TeamInfo();
    ~TeamInfo();

    void		Empty();
    void		Add( int index, Item* pItem );
    const Item*	GetAt( int index ) const;
    Item*		GetAt( int index );

    const Item*	GetTeamMate( const CarElt* pCar ) const;

    bool		IsTeamMate( const CarElt* pCar0, const CarElt* pCar1 ) const;

private:
    std::vector<Item*>	m_items;
};

#endif // !defined(AFX_TEAMINFO_H__7EA9649D_1527_4B70_BA9A_63E73AEFC9FF__INCLUDED_)
