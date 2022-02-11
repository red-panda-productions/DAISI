//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
// unitcollision.h
//--------------------------------------------------------------------------*
// A robot for Speed Dreams-Version 2.X simuV4
//--------------------------------------------------------------------------*
// Collisions and avoiding
// Kollisionen ausweichen
//
// File         : unitcollision.h
// Created      : 2007.11.17
// Last changed : 2014.11.29
// Copyright    : � 2007-2014 Wolf-Dieter Beelitz
// eMail        : wdbee@users.sourceforge.net
// Version      : 4.05.000
//--------------------------------------------------------------------------*
// Teile diese Unit basieren auf diversen Header-Dateien von TORCS
//
//    Copyright: (C) 2000 by Eric Espie
//    eMail    : torcs@free.fr
//
// dem erweiterten Robot-Tutorial bt
//
//    Copyright: (C) 2002-2004 Bernhard Wymann
//    eMail    : berniw@bluewin.ch
//
// dem Roboter delphin
//
//    Copyright: (C) 2006-2007 Wolf-Dieter Beelitz
//    eMail    : wdbee@users.sourceforge.net
//
// dem Roboter wdbee_2007
//
//    Copyright: (C) 2006-2007 Wolf-Dieter Beelitz
//    eMail    : wdbee@users.sourceforge.net
//
// und dem Roboter mouse_2006
//
//    Copyright: (C) 2006-2007 Tim Foden
//
//--------------------------------------------------------------------------*
// This program was developed and tested on windows XP
// There are no known Bugs, but:
// Who uses the files accepts, that no responsibility is adopted
// for bugs, dammages, aftereffects or consequential losses.
//
// Das Programm wurde unter Windows XP entwickelt und getestet.
// Fehler sind nicht bekannt, dennoch gilt:
// Wer die Dateien verwendet erkennt an, dass f�r Fehler, Sch�den,
// Folgefehler oder Folgesch�den keine Haftung �bernommen wird.
//--------------------------------------------------------------------------*
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Im �brigen gilt f�r die Nutzung und/oder Weitergabe die
// GNU GPL (General Public License)
// Version 2 oder nach eigener Wahl eine sp�tere Version.
//--------------------------------------------------------------------------*
#ifndef _UNITCOLLISION_H_
#define _UNITCOLLISION_H_

#include <car.h>

#include "unitglobal.h"
#include "unitcommon.h"

#include "unitopponent.h"

//==========================================================================*
// Deklaration der Klasse TCollision
//--------------------------------------------------------------------------*
class TCollision  
{
  public:
	struct TCollInfo                             // Infos to possible Collision
	{
	  int Flags;                                 // Flags
	  int LappersBehind;                         // Lappers behind?
	  double MinLSideDist;                        
	  double MinRSideDist;
	  double CarDistLong;
      int NextSide;                              // Side of next curve
	  int OppsAhead;                             // Opponents ahead?
	  int OppsAtSide;                            // Opponents at side?
	  int OppsBehindFaster;                      // Opponents behind faster?
	  double TargetSpeed;                        // Adjusted target speed
	  double MinOppDistance;                     // Min distance to opponent
	  double AvoidSide;                          // Avoid to side
	  double ToL;								 // Distance to left raciongline
	  double ToR;                                // Distance to right raciongline
	  bool Blocked[MAXBLOCKED];

	  TCollInfo():                                // Default constructor
		Flags(0),
		LappersBehind(0),
		MinLSideDist(INT_MAX),
		MinRSideDist(INT_MAX),
		CarDistLong(INT_MAX),
		NextSide(0),
		OppsAhead(0),
		OppsAtSide(0),
	    OppsBehindFaster(0),
		TargetSpeed(500),
  	    MinOppDistance(1000),
  	    AvoidSide(0.0),
	    ToL(100),	
	    ToR(100)        
	  {
        for (int I = 0; I < MAXBLOCKED; I++)
  	      Blocked[I] = false;
	  }
	};

  public:
	TCollision();                                // Default constructor
	~TCollision();                               // Destructor

	double AvoidTo                               // Direction to go
	  (const TCollInfo& CollInfo,                //   to avoid collision
	  const PCarElt Car, 
	  TDriver& Me,
	  bool& AvoidAhead, 
	  double& TempOffset);
};
//==========================================================================*
#endif // _UNITCOLLISION_H_
//--------------------------------------------------------------------------*
// end of file unitcollision.h
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
