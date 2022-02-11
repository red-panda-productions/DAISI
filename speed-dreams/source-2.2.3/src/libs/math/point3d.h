/**
 * \file point3d.hxx
 * A 3d point class (depricated).  This class is depricated and we are
 * in the process of removing all usage of it in favor of plib's "sg"
 * library of point, vector, and math routines.  Plib's sg lib is less
 * object oriented, but integrates more seamlessly with opengl.
 *
 * Adapted from algebra3 by Jean-Francois Doue, started October 1998.
 */

// Copyright (C) 1998  Curtis L. Olson  - http://www.flightgear.org/~curt
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// $Id: point3d.hxx,v 1.6 2006-03-08 18:16:08 mfranz Exp $


#ifndef _POINT3D_H
#define _POINT3D_H

# include <istream>
# include <cassert>
# include <cmath>

#include "SGMath.h"

using namespace std;

const double fgPoint3_Epsilon = 0.0000001;

enum {PX, PY, PZ};		    // axes

class Point3D;
istream& operator>> ( istream&, Point3D& );
ostream& operator<< ( ostream&, const Point3D& );
Point3D operator- (const Point3D& p);      
bool operator== (const Point3D& a, const Point3D& b);

/**
 * 3D Point class.
 */

class Point3D 
{
protected:
    double n[3];

public:

    /** Default constructor */
    Point3D();
    Point3D(const double x, const double y, const double z);
    explicit Point3D(const double d);
    Point3D(const Point3D &p);

    static Point3D fromSGGeod(const SGGeod& geod);
    static Point3D fromSGGeoc(const SGGeoc& geoc);
    static Point3D fromSGVec3(const SGVec3<double>& cart);

    // Assignment operators

    Point3D& operator = ( const Point3D& p );	 // assignment of a Point3D
    Point3D& operator += ( const Point3D& p );	 // incrementation by a Point3D
    Point3D& operator -= ( const Point3D& p );	 // decrementation by a Point3D
    Point3D& operator *= ( const double d );     // multiplication by a constant
    Point3D& operator /= ( const double d );	 // division by a constant

    void setx(const double x);
    void sety(const double y);
    void setz(const double z);
    void setlon(const double x);
    void setlat(const double y);
    void setradius(const double z);
    void setelev(const double z);

    // Queries 

    double& operator [] ( int i);		 // indexing
    double operator[] (int i) const;		 // read-only indexing

    inline const double *get_n() const { return n; };
    double x() const;      // cartesian x
    double y() const;      // cartesian y
    double z() const;      // cartesian z

    double lon() const;    // polar longitude
    double lat() const;    // polar latitude
    double radius() const; // polar radius
    double elev() const;   // geodetic elevation (if specifying a surface point)

    SGGeod toSGGeod(void) const;
    SGGeoc toSGGeoc(void) const;

    // friends
    friend Point3D operator - (const Point3D& p);	            // -p1
    friend bool operator == (const Point3D& a, const Point3D& b);  // p1 == p2?
    friend istream& operator>> ( istream&, Point3D& );
    friend ostream& operator<< ( ostream&, const Point3D& );

    // Special functions
    double distance3D(const Point3D& a) const;        // distance between
    double distance3Dsquared(const Point3D& a) const; // distance between ^ 2
};


// input from stream
inline istream&
operator >> ( istream& in, Point3D& p)
{
    char c;

    in >> p.n[PX];

    // read past optional comma
    while ( in.get(c) ) {
	if ( (c != ' ') && (c != ',') ) 
	{
	    in.putback(c);
	    break;
	}
    }
	
    in >> p.n[PY];
    
    while ( in.get(c) ) 
    {
	if ( (c != ' ') && (c != ',') ) 
	{
	    in.putback(c);
	    break;
	}
    }
	
    in >> p.n[PZ];

    return in;
}

inline ostream&
operator<< ( ostream& out, const Point3D& p )
{
    return out << p.n[PX] << ", " << p.n[PY] << ", " << p.n[PZ];
}

///////////////////////////
//
// Point3D Member functions
//
///////////////////////////

// CONSTRUCTORS

inline Point3D::Point3D()
{
   n[PX] = n[PY] = 0.0;
   n[PZ] = -9999.0;
}

inline Point3D::Point3D(const double x, const double y, const double z)
{
    n[PX] = x; n[PY] = y; n[PZ] = z;
}

inline Point3D::Point3D(const double d)
{
    n[PX] = n[PY] = n[PZ] = d;
}

inline Point3D::Point3D(const Point3D& p)
{
    n[PX] = p.n[PX]; n[PY] = p.n[PY]; n[PZ] = p.n[PZ];
}

inline Point3D Point3D::fromSGGeod(const SGGeod& geod)
{
  Point3D pt;
  pt.setlon(geod.getLongitudeRad());
  pt.setlat(geod.getLatitudeRad());
  pt.setelev(geod.getElevationM());
  return pt;
}

inline Point3D Point3D::fromSGGeoc(const SGGeoc& geoc)
{
  Point3D pt;
  pt.setlon(geoc.getLongitudeRad());
  pt.setlat(geoc.getLatitudeRad());
  pt.setradius(geoc.getRadiusM());
  return pt;
}

inline Point3D Point3D::fromSGVec3(const SGVec3<double>& cart)
{
  Point3D pt;
  pt.setx(cart.x());
  pt.sety(cart.y());
  pt.setz(cart.z());
  return pt;
}

// ASSIGNMENT OPERATORS

inline Point3D& Point3D::operator = (const Point3D& p)
{
    n[PX] = p.n[PX]; n[PY] = p.n[PY]; n[PZ] = p.n[PZ]; return *this;
}

inline Point3D& Point3D::operator += ( const Point3D& p )
{
    n[PX] += p.n[PX]; n[PY] += p.n[PY]; n[PZ] += p.n[PZ]; return *this;
}

inline Point3D& Point3D::operator -= ( const Point3D& p )
{
    n[PX] -= p.n[PX]; n[PY] -= p.n[PY]; n[PZ] -= p.n[PZ]; return *this;
}

inline Point3D& Point3D::operator *= ( const double d )
{
    n[PX] *= d; n[PY] *= d; n[PZ] *= d; return *this;
}

inline Point3D& Point3D::operator /= ( const double d )
{
    double d_inv = 1./d; n[PX] *= d_inv; n[PY] *= d_inv; n[PZ] *= d_inv;
    return *this;
}

inline void Point3D::setx(const double x) 
{
    n[PX] = x;
}

inline void Point3D::sety(const double y) 
{
    n[PY] = y;
}

inline void Point3D::setz(const double z) 
{
    n[PZ] = z;
}

inline void Point3D::setlon(const double x) 
{
    n[PX] = x;
}

inline void Point3D::setlat(const double y) 
{
    n[PY] = y;
}

inline void Point3D::setradius(const double z) 
{
    n[PZ] = z;
}

inline void Point3D::setelev(const double z) 
{
    n[PZ] = z;
}

// QUERIES

inline double& Point3D::operator [] ( int i)
{
    assert(! (i < PX || i > PZ));
    return n[i];
}

inline double Point3D::operator [] ( int i) const 
{
    assert(! (i < PX || i > PZ));
    return n[i];
}


inline double Point3D::x() const { return n[PX]; }

inline double Point3D::y() const { return n[PY]; }

inline double Point3D::z() const { return n[PZ]; }

inline double Point3D::lon() const { return n[PX]; }

inline double Point3D::lat() const { return n[PY]; }

inline double Point3D::radius() const { return n[PZ]; }

inline double Point3D::elev() const { return n[PZ]; }

inline SGGeod Point3D::toSGGeod(void) const
{
  SGGeod geod;
  geod.setLongitudeRad(lon());
  geod.setLatitudeRad(lat());
  geod.setElevationM(elev());
  return geod;
}

inline SGGeoc Point3D::toSGGeoc(void) const
{
  SGGeoc geoc;
  geoc.setLongitudeRad(lon());
  geoc.setLatitudeRad(lat());
  geoc.setRadiusM(radius());
  return geoc;
}

// FRIENDS

inline Point3D operator - (const Point3D& a)
{
    return Point3D(-a.n[PX],-a.n[PY],-a.n[PZ]);
}

inline Point3D operator + (const Point3D& a, const Point3D& b)
{
    return Point3D(a) += b;
}

inline Point3D operator - (const Point3D& a, const Point3D& b)
{
    return Point3D(a) -= b;
}

inline Point3D operator * (const Point3D& a, const double d)
{
    return Point3D(a) *= d;
}

inline Point3D operator * (const double d, const Point3D& a)
{
    return a*d;
}

inline Point3D operator / (const Point3D& a, const double d)
{
    return Point3D(a) *= (1.0 / d );
}

inline bool operator == (const Point3D& a, const Point3D& b)
{
    return
	fabs(a.n[PX] - b.n[PX]) < fgPoint3_Epsilon &&
	fabs(a.n[PY] - b.n[PY]) < fgPoint3_Epsilon &&
	fabs(a.n[PZ] - b.n[PZ]) < fgPoint3_Epsilon;
}

inline bool operator != (const Point3D& a, const Point3D& b)
{
    return !(a == b);
}

// Special functions

inline double
Point3D::distance3D(const Point3D& a ) const
{
    double x, y, z;

    x = n[PX] - a.n[PX];
    y = n[PY] - a.n[PY];
    z = n[PZ] - a.n[PZ];

    return sqrt(x*x + y*y + z*z);
}


inline double
Point3D::distance3Dsquared(const Point3D& a ) const
{
    double x, y, z;

    x = n[PX] - a.n[PX];
    y = n[PY] - a.n[PY];
    z = n[PZ] - a.n[PZ];

    return(x*x + y*y + z*z);
}

#endif // _POINT3D_H
