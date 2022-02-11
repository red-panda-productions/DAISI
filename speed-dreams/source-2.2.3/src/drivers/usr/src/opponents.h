/***************************************************************************

    file                 : opponents.h
    created              : Thu Aug 31 01:21:49 UTC 2006
    copyright            : (C) 2006 Daniel Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _OPPONENTS_H_
#define _OPPONENTS_H_

#include "opponent.h"
#include "MyCar.h"
#include "Path.h"

#include <track.h>
#include <car.h>
#include <robot.h>

#include <vector>

class Opponents
{
public:
  Opponents();
  void init( tTrack* track, const tSituation* situation,  MyCar* mycar,  Path* mypath);
  void update();
  Opponent* opp(int idx);
  Opponent* oppNear() const { return mOppNear; };
  Opponent* oppLetPass() const { return mOppLetPass; };
  Opponent* oppBack() const { return mOppBack; };
  int nrOpponents() const { return mOpp.size(); };
  bool oppComingFastBehind() const { return mOppComingFastBehind; };
  bool mateFrontAside() const { return mMateFrontAside; };

private:
  std::vector<Opponent> mOpp;
  Opponent* mOppNear;
  Opponent* mOppLetPass;
  Opponent* mOppBack;
  bool mOppComingFastBehind;
  bool mMateFrontAside;
};


#endif // _OPPONENTS_H_
