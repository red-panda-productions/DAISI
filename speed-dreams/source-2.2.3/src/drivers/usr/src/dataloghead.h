/***************************************************************************

    file                 : dataloghead.h
    created              : 2017-07-20 06:35:00 UTC
    copyright            : (C) Daniel Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _DATALOGHEAD_H_
#define _DATALOGHEAD_H_

#include <string>

class DataLogHead
{
public:
    std::string name;
    double* dataptr;
    double scale;
};

#endif // _DATALOGHEAD_H_
