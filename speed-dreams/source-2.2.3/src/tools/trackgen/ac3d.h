/***************************************************************************

    file                 : ac3d.h
    created              : Wed May 29 22:15:37 CEST 2002
    copyright            : (C) 2001 by Eric Espi�
    email                : Eric.Espie@torcs.org
    version              : $Id: ac3d.h 2917 2010-10-17 19:03:40Z pouillot $

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
    @version	$Id: ac3d.h 2917 2010-10-17 19:03:40Z pouillot $
*/

#ifndef _AC3D_H_
#define _AC3D_H_

extern FILE *Ac3dOpen(char *filename, int nbObjects);
extern int Ac3dGroup(FILE *save_fd, const char *name, int nbObjects);
extern void Ac3dClose(FILE *save_fd);

#endif /* _AC3D_H_ */ 



