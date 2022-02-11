/**
 * \file vector.hxx
 * Additional vector routines.
 */

// Written by Curtis Olson, started December 1997.
//
// Copyright (C) 1997  Curtis L. Olson  - http://www.flightgear.org/~curt
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
// $Id: vector.hxx,v 1.4 2006-03-08 18:16:08 mfranz Exp $


#ifndef _VECTOR_HXX
#define _VECTOR_HXX


#ifndef __cplusplus
# error This library requires C++
#endif

#include <simgear/compiler.h>

#include <plib/sg.h>


/**
 * Map a vector onto a plane.
 * @param normal (in) normal vector for the plane
 * @param v0 (in) a point on the plane
 * @param vec (in) the vector to map onto the plane
 * @param result (out) the result vector
 */
inline void sgmap_vec_onto_cur_surface_plane( sgVec3 normal, 
					      sgVec3 v0, 
					      sgVec3 vec,
					      sgVec3 result )
{
    sgVec3 u1, v, tmp;

    // calculate a vector "u1" representing the shortest distance from
    // the plane specified by normal and v0 to a point specified by
    // "vec".  "u1" represents both the direction and magnitude of
    // this desired distance.

    // u1 = ( (normal <dot> vec) / (normal <dot> normal) ) * normal

    sgScaleVec3( u1,
		 normal,
		 ( sgScalarProductVec3(normal, vec) /
		   sgScalarProductVec3(normal, normal)
		   )
		 );

    // printf("  vec = %.2f, %.2f, %.2f\n", vec[0], vec[1], vec[2]);
    // printf("  v0 = %.2f, %.2f, %.2f\n", v0[0], v0[1], v0[2]);
    // printf("  u1 = %.2f, %.2f, %.2f\n", u1[0], u1[1], u1[2]);
   
    // calculate the vector "v" which is the vector "vec" mapped onto
    // the plane specified by "normal" and "v0".

    // v = v0 + vec - u1

    sgAddVec3(tmp, v0, vec);
    sgSubVec3(v, tmp, u1);
    // printf("  v = %.2f, %.2f, %.2f\n", v[0], v[1], v[2]);

    // Calculate the vector "result" which is "v" - "v0" which is a
    // directional vector pointing from v0 towards v

    // result = v - v0

    sgSubVec3(result, v, v0);
    // printf("  result = %.2f, %.2f, %.2f\n", 
    // result[0], result[1], result[2]);
}


/**
 * Copy and negate a vector.
 * @param dst (out) result vector
 * @param src (in) input vector
 */
inline void sgCopyNegateVec4( sgVec4 dst, sgVec4 src )
{
	dst [ 0 ] = -src [ 0 ] ;
	dst [ 1 ] = -src [ 1 ] ;
	dst [ 2 ] = -src [ 2 ] ;
	dst [ 3 ] = -src [ 3 ] ;
}

/**
 * Given a point p, and a line through p0 with direction vector d,
 * find the closest point (p1) on the line (float version).
 * @param p1 (out) closest point to p on the line
 * @param p (in) original point
 * @param p0 (in) point on the line
 * @param d (in) vector defining line direction
 */
void sgClosestPointToLine( sgVec3 p1, const sgVec3 p, const sgVec3 p0,
			   const sgVec3 d );

/**
 * Given a point p, and a line through p0 with direction vector d,
 * find the closest point (p1) on the line (double version).
 * @param p1 (out) closest point to p on the line
 * @param p (in) original point
 * @param p0 (in) point on the line
 * @param d (in) vector defining line direction
 */
void sgdClosestPointToLine( sgdVec3 p1, const sgdVec3 p, const sgdVec3 p0,
			    const sgdVec3 d );

/**
 * Given a point p, and a line through p0 with direction vector d,
 * find the shortest distance (squared) from the point to the line (float
 * version.)
 * @param p (in) original point
 * @param p0 (in) point on the line
 * @param d (in) vector defining line direction
 * @return shortest distance (squared) from p to line
 */
double sgClosestPointToLineDistSquared( const sgVec3 p, const sgVec3 p0,
					const sgVec3 d );

/**
 * Given a point p, and a line through p0 with direction vector d,
 * find the shortest distance (squared) from the point to the line (double
 * version.)
 * @param p (in) original point
 * @param p0 (in) point on the line
 * @param d (in) vector defining line direction
 * @return shortest distance (squared) from p to line
 */
double sgdClosestPointToLineDistSquared( const sgdVec3 p, const sgdVec3 p0,
					 const sgdVec3 d );

/**
 * This is same as:
 *
 * <li> sgMakeMatTrans4( sgMat4 sgTrans, sgVec3 trans )
 * <li> sgPostMultMat4( sgMat4 src, sgTRANS );
 *
 * @param src starting sgMat4 matrix
 * @param trans translation vector
 */
void sgPostMultMat4ByTransMat4( sgMat4 src, const sgVec3 trans );


#endif // _VECTOR_HXX


