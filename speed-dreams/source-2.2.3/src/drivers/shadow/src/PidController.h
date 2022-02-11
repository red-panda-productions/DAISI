/***************************************************************************

    file        : PidController.h
    created     : 18 Apr 2017
    copyright   : (C) 2017 Tim Foden

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// PidController.h: interface for the PidController class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _PIDCONTROLLER_H_
#define _PIDCONTROLLER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PidController
{
public:
    PidController();
    virtual ~PidController();

    double	Sample( double propValue );
    double	Sample( double propValue, double diffValue );

public:
    double	m_lastPropValue;	// for calculating differential (if not supplied)
    double	m_total;			// for integral.
    double	m_maxTotal;			// for integral.
    double	m_totalRate;		// for integral.

    double	m_p;
    double	m_i;
    double	m_d;
};

#endif // _PIDCONTROLLER_H_
