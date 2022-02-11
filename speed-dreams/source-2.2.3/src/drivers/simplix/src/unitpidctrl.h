//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
// unitpidctrl.h
//--------------------------------------------------------------------------*
// A robot for Speed Dreams-Version 2.X simuV4
//--------------------------------------------------------------------------*
// PID Controller
//
// File         : unitpidctrl.h
// Created      : 2007.11.25
// Last changed : 2014.11.29
// Copyright    : � 2007-2014 Wolf-Dieter Beelitz
// eMail        : wdbee@users.sourceforge.net
// Version      : 4.05.000
//--------------------------------------------------------------------------*
// Diese Unit basiert auf dem Roboter mouse_2006
//
//    Copyright: (C) 2006-2007 Tim Foden
//
//--------------------------------------------------------------------------*
// Das Programm wurde unter Windows XP entwickelt und getestet.
// Fehler sind nicht bekannt, dennoch gilt:
// Wer die Dateien verwendet erkennt an, dass f�r Fehler, Sch�den,
// Folgefehler oder Folgesch�den keine Haftung �bernommen wird.
//
// Im �brigen gilt f�r die Nutzung und/oder Weitergabe die
// GNU GPL (General Public License)
// Version 2 oder nach eigener Wahl eine sp�tere Version.
//--------------------------------------------------------------------------*
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//--------------------------------------------------------------------------*
#ifndef _UNITPIDCTRL_H_
#define _UNITPIDCTRL_H_

//==========================================================================*
// Deklaration der Klasse TPidController
//--------------------------------------------------------------------------*
class TPidController  
{
public:
	TPidController();
	virtual ~TPidController();

	double Sample(double PropValue);
	double Sample(double PropValue, double DiffValue);

public:
	double oLastPropValue;	// for calculating differential (if not supplied)
	double oTotal;			// for integral.
	double oMaxTotal;		// for integral.
	double oMinTotal;		// for integral.
	double oTotalRate;		// for integral.

	double oP;
	double oI;
	double oD; 
};
//==========================================================================*
#endif // _UNITPIDCTRL_H_
//--------------------------------------------------------------------------*
// end of file unitcubic.h
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
