/***************************************************************************

    file                 : torcs_or_sd.h
    created              : 2012-06-19 01:21:49 UTC
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

#ifndef _TORCS_OR_SD_H_
#define _TORCS_OR_SD_H_

// Set here TORCS or SPEEDDREAMS manually
//#define DANDROID_TORCS
#define DANDROID_SPEEDDREAMS

// The following globals will be set automatically
#ifdef DANDROID_TORCS
const bool IS_DANDROID_TORCS = true;
const bool IS_DANDROID_SPEEDDREAMS = false;
#else
const bool IS_DANDROID_TORCS = false;
const bool IS_DANDROID_SPEEDDREAMS = true;
#endif

#endif // _TORCS_OR_SD_H_

