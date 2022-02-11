/***************************************************************************

    file        : grMoonpos.cpp
    copyright   : (C) 2012 by Xavier Bertaux (based on simgear code)
    web         : http://www.speed-dreams.org
    version     : $Id: grSphere.h 3162 2012-07-05 13:11:22Z torcs-ng $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <ctime>
#include "grMoonpos.h"

// Position of the Moon first month 2012
static const double MoonPositionDay[31] = { 232.25, 243.86, 255.47, 267.08, 278.69, 290.3, 301.91, 313.53, 325.13, 336.75, 348.36, 0,
                                        11.61, 23.22, 34.83, 46.44, 58.05, 69.66, 81.27, 92.88, 104.49, 116.10, 127.71, 139.32, 150.93,
                                        162.54, 174.15, 185.76, 197.37, 208.98, 220.59 };

double grUpdateMoonPos(double timeOfDay)
{
    double moonpos;  //, actual = 0;
    time_t Time;
    struct tm *Date;

    time(&Time);
    Date = localtime(&Time);

    int day = Date->tm_mday;

#if 0   // dead code
    actual = timeOfDay;
#endif
    moonpos = (MoonPositionDay[day-1] - day);
    moonpos = moonpos + (15 * ((double)timeOfDay / 3600 - 90.0));
    if (moonpos > 360.0)
        moonpos = 0.0;
    if (moonpos > 0.0)
        moonpos = 0.0;
    /*if (actual > moonpos)
        moonpos = actual - moonpos;
    else
        moonpos = moonpos - actual;*/


    return moonpos;

}



