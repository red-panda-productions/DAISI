//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
// unitsysfoo.cpp
//--------------------------------------------------------------------------*
// A robot for Speed Dreams-Version 2.X simuV4
//--------------------------------------------------------------------------*
// Systemfunktion
// (C++-Portierung der Unit UnitSysFoo.pas)
// 
// File         : unitsysfoo.cpp
// Created      : 2007.02.20
// Last changed : 2014.11.29
// Copyright    : � 2007-2014 Wolf-Dieter Beelitz
// eMail        : wdbee@users.sourceforge.net
// Version      : 4.05.000
//--------------------------------------------------------------------------*
// Realisierung einer speziellen "Systemfunktion" zur einfachen und schnellen
// Berechnung des Faltungsintegrals eines lineraren Systems.
//
// Mit dieser Systemfunktion k�nnen u.a. auch gleitende Mittelwerte sehr
// schnell berechnet oder Ringpuffer f�r die verz�gerte Auswertung
// von Signalen bereitgestellt werden.
//
// Hier wird z.B. die Bewegungserkennung damit realisiert. Wenn ein Fahrzeug
// durch ein Hindernis blockiert ist (Mauer, andere Wagen usw.), dann �ndern
// sich die Koordinaten der Position nicht bzw. nur sehr gering.
// Durch den Vergleich von alter Position mit aktueller Position bei frei
// w�hlbarer L�nge der Verz�grung im Ringpuffer kann diese Situation
// zuverl�ssig erkannt werden.
// Die in anderen Quellen ver�ffentlichten Ans�tze zur Erkennung von
// Blockaden beruhen auf einer Verkn�pfung von verschiedenen aktuellen
// Zustandswerten wie der Richtung, was in ausgefallenen F�llen nicht
// immer funktioniert.
//
// Die erforderliche Rechenzeit ist bei diesem Ansatz von der L�nge der
// Verz�gerung unabh�ngig und es werden keine Winkelfunktionen ben�tigt!
// Da die Bewegungserkennung st�ndig mitlaufen muss, ist das eine
// entscheidende Verbesserung.
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
#include "unitsysfoo.h"

//==========================================================================*
// Konstruktor
//--------------------------------------------------------------------------*
TSysFoo::TSysFoo(unsigned int N, unsigned int K) :
  oAutoNorm(false),
  oDirty(false),
  oNSysFoo(0),
  oSigIndex(0)
{
  unsigned int I;

  if (N < 1)                                     // Keine Division durch 0!
    N = 1;
  if (N + K > 255)                               // Gesamtl�nge begrenzen,
    N = 255 - K;                                 //   Verz�gerung erhalten!

  for (I = 0; I < 256; I++)
  {
    oSignal[I] = 0.0;
    oSysFoo[I] = 0.0;
  };

  for (I = K; I < K + N; I++)                    // Systemfunktion generieren
    oSysFoo[I] = 1.0f / N;

  oNSysFoo = N + K;                              // L�nge der Systemfunktion
};
//==========================================================================*

//==========================================================================*
// SysFoo auslesen
//--------------------------------------------------------------------------*
float TSysFoo::Get(int Index)
{
  return oSysFoo[Index];
};
//==========================================================================*

//==========================================================================*
// SysFoo setzen
//--------------------------------------------------------------------------*
void TSysFoo::Put(int Index, float Value)
{
  oDirty = true;                                 // �nderungen erfordern
  oSysFoo[Index] = Value;                        // ggf. eine Normalisierung
  if (oAutoNorm)                                 // Wenn aktiviert,
    Normalize();                                   //   automatisch normieren
};
//==========================================================================*

//==========================================================================*
// SysFoo normieren
//--------------------------------------------------------------------------*
void TSysFoo::Normalize()
{

  int I;
  float Sum;

  if (oDirty)                                    // Falls eforderlich
  {                                              // auf Summe = 1.0
    Sum = 0.0;                                   // normieren
    for (I = 0; I < 256; I++)
      Sum += oSysFoo[I];
    for (I = 0; I < 256; I++)
      oSysFoo[I] /= Sum;

    oDirty = false;                              // Normierung Erledigt
  }
};
//==========================================================================*

//==========================================================================*
// Faltung der Systemfunktion mit dem Eingangsimpuls
//--------------------------------------------------------------------------*
float TSysFoo::Faltung(float Impuls)
{
  int I;
  unsigned char J; 

  oSignal[oSigIndex] = 0.0;                      // Alte Werte l�schen
  oSigIndex++;                                   // Start im Ringpuffer
  J = oSigIndex;                                 // Faltungsindex
  for (I = 0; I < oNSysFoo; I++)                 // �ber die L�nge der
  {                                              //   Faltung
    oSignal[J] += oSysFoo[I] * Impuls;
    J++;
//	if (J > 255)
//	  J = 0;
  };
  return oSignal[oSigIndex];                     // Aktuelles Ausgangssignal
};
//==========================================================================*

//==========================================================================*
// Reset buffer
//--------------------------------------------------------------------------*
void TSysFoo::Reset()
{
  for (int I = 0; I < oNSysFoo; I++) 
    oSignal[I] = 0.0;
};
//==========================================================================*

//--------------------------------------------------------------------------*
// end of file unitsysfoo.cpp
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
