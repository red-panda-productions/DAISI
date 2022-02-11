/***************************************************************************

    file        : PathMargins.cpp
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

#include "PathMargins.h"

#include <fstream>
#include <tgf.h>

extern GfLogger* PLogUSR;
#define LogUSR (*PLogUSR)

PathMargins::PathMargins()
{
}

void PathMargins::read(const std::string &datafile)
{
    PathMarginsSect sect;
    std::ifstream myfile(datafile);

    if (myfile.is_open())
    {
        std::string s;
        myfile >> s >> s >> s; // ignore column heads

        while (myfile >> sect.fromstart >> sect.left >> sect.right)
        {
            mPathMargin.push_back(sect);
        }
    }
    else
    {
        sect.fromstart = 0.0;
        sect.left = 1.2;
        sect.right = 1.2;
        mPathMargin.push_back(sect);
        LogUSR.info("Couldn't load : %s, using default margins\n", datafile.c_str());
    }
}

PathMarginsSect PathMargins::margins(double fromstart) const
{
    return mPathMargin[sectIdx(fromstart)];
}

int PathMargins::sectIdx(double fromstart) const
{
    int idx = -1;

    for (int i = 0; i < (int)mPathMargin.size() - 1; i++)
    {
        if (fromstart >= mPathMargin[i].fromstart && fromstart < mPathMargin[i + 1].fromstart)
        {
            idx = i;
        }
    }

    if (idx == -1)
    {
        idx = (int)mPathMargin.size() - 1;
    }

    return idx;
}
