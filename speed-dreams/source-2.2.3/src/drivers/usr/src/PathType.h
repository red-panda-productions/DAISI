/***************************************************************************

    file        : PathType.h
    created     : 11 Jan 2018
    copyright   : (C) 2018 D.Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _PATHTYPE_H_
#define _PATHTYPE_H_

#include <string>
#include <vector>

#include <portability.h>

enum PathType{ PATH_O, PATH_L, PATH_R };

static std::string PathNames[] = { "PATH_O", "PATH_L", "PATH_R" };

#endif // _PATHTYPE_H_
