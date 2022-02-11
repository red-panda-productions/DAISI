/****************************************************************************

    file                 : celestialbody.h
    created              : Sun Aug 02 22:54:56 CET 2012
    copyright            : (C) 2000  Curtis L. Olson - (C)2012 Xavier Bertaux
    email                : bertauxx@yahoo.fr
    version              : $Id: celestialbody.h 4811 2012-07-29 17:16:37Z torcs-ng $

 ****************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef _CELESTIALBODY_H_
#define _CELESTIALBODY_H_

#include <constants.h>

class ePhStar;

class ePhCelestialBody
{
protected:              // make the data protected, in order to give the
                        //  inherited classes direct access to the data
  double NFirst;       	/* longitude of the ascending node first part */
  double NSec;		/* longitude of the ascending node second part */
  double iFirst;       	/* inclination to the ecliptic first part */
  double iSec;		/* inclination to the ecliptic second part */
  double wFirst;       	/* first part of argument of perihelion */
  double wSec;		/* second part of argument of perihelion */
  double aFirst;       	/* semimayor axis first part*/
  double aSec;		/* semimayor axis second part */
  double eFirst;       	/* eccentricity first part */
  double eSec;		/* eccentricity second part */
  double MFirst;       	/* Mean anomaly first part */
  double MSec;		/* Mean anomaly second part */

  double N, i, w, a, e, M; /* the resulting orbital elements, obtained from the former */

  double rightAscension, declination;
  double r, R, s, FV;
  double magnitude;
  double lonEcl, latEcl;

  double sdCalcEccAnom(double M, double e);
  double sdCalcActTime(double mjd);
  void updateOrbElements(double mjd);

public:
  ePhCelestialBody(double Nf, double Ns,
		double If, double Is,
		double wf, double ws,
		double af, double as,
		double ef, double es,
		double Mf, double Ms, double mjd);
  ePhCelestialBody(double Nf, double Ns,
		double If, double Is,
		double wf, double ws,
		double af, double as,
		double ef, double es,
		double Mf, double Ms);
		
  void getPos(double *ra, double *dec);
  void getPos(double *ra, double *dec, double *magnitude);
  double getRightAscension();
  double getDeclination();
  double getMagnitude();
  double getLon() const;
  double getLat() const; 
  void updatePosition(double mjd, ePhStar *ourSun);
};

inline double ePhCelestialBody::getRightAscension() { return rightAscension; }
inline double ePhCelestialBody::getDeclination() { return declination; }
inline double ePhCelestialBody::getMagnitude() { return magnitude; }

inline double ePhCelestialBody::getLon() const
{
  return lonEcl;
}

inline double ePhCelestialBody::getLat() const
{
  return latEcl;
}

#endif // _CELESTIALBODY_H_

