/***************************************************************************

    file        : MuFactors.h
    created     : 29 Apr 2017
    copyright   : (C) 2017 D.Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _MUFACTORS_H_
#define _MUFACTORS_H_

#include "MuFactorsSect.h"
#include "Path.h"

#include <vector>

class MuFactors
{
public:
    MuFactors();
    void read(const std::string &datadir, const std::string &trackname);
    double muFactor(double fromstart) const;
    double minMuFactor() const { return mMinMuFactor; }

private:
    int sectIdx(double fromstart) const ;
    double calcMinMuFactor() const;
    void printMuFactors() const;

    std::vector<MuFactorsSect> mSect;
    double mMinMuFactor;
};

#endif // _MUFACTORS_H_
