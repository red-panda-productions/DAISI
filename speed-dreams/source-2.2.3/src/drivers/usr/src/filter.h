/***************************************************************************

    file                 : filter.h
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

#ifndef _FILTER_H_
#define _FILTER_H_

#include <vector>

class Filter
{
public:
    Filter();
    void sample(unsigned n, double value);
    double get() const { return mFiltered; };

private:
    unsigned mNext;
    std::vector<double> mValues;
    double mFiltered;
};

#endif // _FILTER_H_
