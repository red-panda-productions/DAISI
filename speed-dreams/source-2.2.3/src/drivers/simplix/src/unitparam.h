//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
// unitparam.h
//--------------------------------------------------------------------------*
// A robot for Speed Dreams-Version 2.X simuV4
//--------------------------------------------------------------------------*
// Container for parameters of car, lane, pit ...
// Container f�r Parameter des Fahrzeugs, der Fahrspuren, der Box usw.
//
// File         : unitparam.h
// Created      : 2007.04.11
// Last changed : 2014.11.29
// Copyright    : � 2007-2014 Wolf-Dieter Beelitz
// eMail        : wdbee@users.sourceforge.net
// Version      : 4.05.000
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
#ifndef _UNITPARAM_H_
#define _UNITPARAM_H_

#include "unitglobal.h"
#include "unitcommon.h"

#include "unitcarparam.h"
#include "unitfixcarparam.h"
#include "unitpitparam.h"
#include "unittmpcarparam.h"

//==========================================================================*
// Deklaration der Klasse TParam
//--------------------------------------------------------------------------*
class TParam  
{
  public:
	TParam();                                    // Default constructor 
    ~TParam();                                   // Destructor

	void Initialize
	  (PDriver Driver, PtCarElt Car);
	void SetEmptyMass(float EmptyMass);
    void Update();

    PtCarElt oCar;                               // Pointer to TORCS data of car

	TCarParam oCarParam;                         // Main parameter set
	TCarParam oCarParam2;                        // Avoiding parameter set
	TCarParam oCarParam3;                        // Pitting parameter set

	TPitParam Pit;                               // Parameters of the pit
	TTmpCarParam Tmp;                            // State of the car
	TFixCarParam Fix;                            // Data of the car
};
//==========================================================================*
#endif // _UNITPARAM_H_
//--------------------------------------------------------------------------*
// end of file unitparam.h
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
