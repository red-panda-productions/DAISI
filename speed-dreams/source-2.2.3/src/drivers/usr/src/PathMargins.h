/***************************************************************************

    file        : PathMargins.h
    created     : 17 Dec 2018
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

#ifndef _PATHMARGINS_H_
#define _PATHMARGINS_H_

#include "PathMarginsSect.h"

#include <string>
#include <vector>

class PathMargins
{
public:
    PathMargins();
    void read(const std::string &datafile);
    PathMarginsSect margins(double fromstart) const;

private:
    int sectIdx(double fromstart) const;

    std::vector<PathMarginsSect> mPathMargin;
};

#endif // _PATHMARGINS_H_
