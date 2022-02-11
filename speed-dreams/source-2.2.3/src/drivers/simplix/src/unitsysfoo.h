//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
// unitsysfoo.h
//--------------------------------------------------------------------------*
// A robot for Speed Dreams-Version 2.X simuV4
//--------------------------------------------------------------------------*
// (C++-Portierung der Unit UnitSysFoo.pas)
//
// File         : unitsysfoo.h
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
#ifndef _UNITSYSFOO_H_
#define _UNITSYSFOO_H_

#include <math.h>

//==========================================================================*
// Forewarding
//--------------------------------------------------------------------------*
class TSysFoo;
typedef TSysFoo* PSysFoo;
//==========================================================================*

//==========================================================================*
// Definition unserer Klasse TSysFoo
//--------------------------------------------------------------------------*
// Der Ringpuffer hat eine L�nge von 256 Werten (255 nutzbar, einer wird f�r
// das automatische L�schen ben�tigt!)
// Die Faltung nutzt den Byte-Inkrement-Overflow aus um eine explizite
// Abfrage am Pufferende einzusparen, also Inc(255) -> 0!
// Eine �berwachung von Overflow-Events durch den Compiler darf deshalb nicht
// aktiviert sein!
//--------------------------------------------------------------------------*
// Der Aufruf ... (N,K)  erzeugt eine Systemfunktion ...
// TSysFoo.Create 1.0 0.0 0.0 0.0 ...
// ...Create(1,1) 0.0 1.0 0.0 0.0 ...
// ...Create(2)   0.5 0.5 0.0 0.0 ...
// ...Create(2,1) 0.0 0.5 0.5 0.0 ...
// ...Create(4,2) 0.0 0.0 0.25 0.25 0.25 0.25 0.0 ...
// usw. wobei N + K < 255 sein muss!
//
// D.h. mit diesen Aufrufen lassen sich sehr bequem Systemfunkionen z.B. f�r
// die Berechnung von gleitenden Mittelwerten (�ber N Werte) mit einer
// zus�tzlichen Verz�gerung (K) erzeugen.
//
// Andere Systemfunktionen k�nnen �ber die Eigenschaft SysFoo[I] gesetzt
// werden. Die Summe der Werte der Systemfunktion muss 1.0 ergeben.
// Es k�nnen auch beliebige Werte definiert und dann mit dem
// Aufruf Normalize auf die Summe 1.0 skaliert werden.
//
// Die �berwachung der Integrit�t ist auskommentiert, da sie hier nicht
// ben�tigt wird (Funktion Faltung).
//--------------------------------------------------------------------------*
class TSysFoo {
  private:
    bool oAutoNorm;                              // Automatisch normalisieren
    bool oDirty;                                 // Normalisierung n�tig
    int oNSysFoo;                                // L�nge der Systemfunktion
    float oSignal[256];                          // Ringpuffer f�r Signal
    float oSysFoo[256];                          // Systemfunktion
    unsigned char oSigIndex;                     // Ringpufferstartindex

    float Get(int Index);                        //
    void Put(int Index, float Value);

  public:
	  TSysFoo                                    // Standardkonstruktor
	    (unsigned int N = 1, unsigned int K = 0);// L�nge und Verz�gerung

    float Faltung                                // Faltung d. Signalimpulses
      (float Impuls);                            // mit der Systemfunktion

    void Normalize();                            // SysFoo normieren

    int Length();                                // L�nge der Systemfunktion

    bool AutoNorm();                             // Autoamtisch normalisieren
    void Reset();                                // Clear buffers
    float SysFoo(int Index);                     // Systemfunktion

};
//==========================================================================*
#endif // _UNITLINALG_H_
//--------------------------------------------------------------------------*
// end of file unitsysfoo.h
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
