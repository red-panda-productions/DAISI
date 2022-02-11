/***************************************************************************

    file                 : pidcontroller.h
    created              : Apr 09 01:21:49 UTC 2006
    copyright            : (C) 2006 Tim Foden, 2018 Daniel Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _PIDCONTROLLER_H_
#define _PIDCONTROLLER_H_

class PidController
{
public:
  PidController();
  double sample(double error, double dt);

public:
  double mPrevError;
  double mAccum;
  double mMaxAccum;
  double mP;
  double mD;
  double mI;
};

#endif // _PIDCONTROLLER_H_
