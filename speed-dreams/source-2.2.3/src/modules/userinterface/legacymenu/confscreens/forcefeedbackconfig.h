/***************************************************************************

    file        : controlconfig.h
    created     : Wed Mar 12 22:09:01 CET 2003
    copyright   : (C) 2003 by Eric Espie
    email       : eric.espie@torcs.org   
    version     : $Id$

***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
/** @file    
    		
    @author	<a href=mailto:torcs@free.fr>Eric Espie</a>
    @version	$Id$
*/
#ifndef _FORCEFEEDBACKCONFIG_H_
#define _FORCEFEEDBACKCONFIG_H_

#include "confscreens.h"

extern void *ForceFeedbackMenuInit(void *prevMenu, void *prefHdle, int curPlayerIdx, const std::string &carName);

#endif 
