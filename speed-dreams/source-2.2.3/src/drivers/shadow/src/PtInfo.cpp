/***************************************************************************

    file        : PitInfo.cpp
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

// PtInfo.cpp: implementation of the PtInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "PtInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PtInfo::PtInfo()
:   idx(0),
    t(0.0),
    offs(0.0),
    oang(0.0),
    toL(0.0),
    toR(0.0),
    extL(0.0),
    extR(0.0),
    k(0.0),
    spd(0.0),
    acc(0.0)
{

}

PtInfo::~PtInfo()
{

}
