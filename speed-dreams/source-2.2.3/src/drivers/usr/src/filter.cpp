/***************************************************************************

    file                 : filter.cpp
    created              : Jul 07 07:37:49 UTC 2019
    copyright            : (C) 2019 Daniel Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "filter.h"

Filter::Filter() :
    mNext(3000),
    mFiltered(0.0)
{
}

void Filter::sample(unsigned n, double value)
{
    if (mValues.size() < n)
    {
        mValues.push_back(value);
    }
    else
    {
        mValues[mNext] = value;
    }

    mNext = (mNext + 1) % n;
    double sum = 0.0;

    for (unsigned i = 0; i < mValues.size(); ++i)
    {
        sum += mValues[i];
    }

    mFiltered = sum / mValues.size();
}
