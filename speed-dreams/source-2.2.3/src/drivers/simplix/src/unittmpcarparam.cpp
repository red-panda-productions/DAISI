//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
// unittmpcarparam.cpp
//--------------------------------------------------------------------------*
// A robot for Speed Dreams-Version 2.X simuV4
//--------------------------------------------------------------------------*
// Car parameters changing in time
// Zeitlich variable Parameter des Fahrzeugs
// 
// File         : unittmpcarparam.cpp
// Created      : 2007.11.25
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
#include <math.h>

#include "unitglobal.h"
#include "unitcommon.h"

#include "unitcarparam.h"
#include "unitparam.h"

//==========================================================================*
// Default constructor
//--------------------------------------------------------------------------*
TTmpCarParam::TTmpCarParam():
  oDamage(0),
  oEmptyMass(1000.0),
  oFuel(0),
  oMass(1000.0),
  oSkill(1.0)
{
}
//==========================================================================*

//==========================================================================*
// Destructor
//--------------------------------------------------------------------------*
TTmpCarParam::~TTmpCarParam()
{
}
//===========================================================================

//==========================================================================*
// Initialize
//--------------------------------------------------------------------------*
void TTmpCarParam::Initialize(PtCarElt Car)
{
  oCar = Car;
}
//==========================================================================*

//==========================================================================*
// Recalculation needed?
//--------------------------------------------------------------------------*
bool TTmpCarParam::Needed()
{
  if ((fabs(oFuel - CarFuel) > 5)
    || (fabs(oDamage - CarDamage) > 500))
	return true;
  else
	return false;
}
//==========================================================================*

//==========================================================================*
// Update
//--------------------------------------------------------------------------*
void TTmpCarParam::Update()
{
//  oFuel = 5 * floor(CarFuel/5);
  oFuel = CarFuel;
  oMass = oEmptyMass + oFuel;
  oDamage = CarDamage;
}
//==========================================================================*

//--------------------------------------------------------------------------*
// end of file unittmpcarparam.cpp
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
