/***************************************************************************

    file        : Shared.h
    created     : 118 Apr 2017
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

// Shared.h: interface for the Shared class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SHARED_H_
#define _SHARED_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TeamInfo.h"
#include "ClothoidPath.h"

#include <track.h>

class Shared
{
public:
    Shared();
    ~Shared();

public:
    TeamInfo		m_teamInfo;
    tTrack*			m_pTrack;
    ClothoidPath	m_path[3];
};

#endif // _SHARED_H_
