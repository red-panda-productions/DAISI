/***************************************************************************

    file        : Utils.cpp
    created     : 18 Apr 2017
    copyright   : (C) 2017 Tim Foden

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// Utils.cpp: implementation of the Utils class.
//
//////////////////////////////////////////////////////////////////////

#include "Utils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Utils::Utils()
{

}

Utils::~Utils()
{

}

 double	Utils::NormPiPi( double angle )
{
	if( angle < -PI )
		angle += 2 * PI;
	else if( angle > PI )
		angle -= 2 * PI;
	return angle;
}

double	Utils::ClosestPtOnLine(
	double ptx,
	double pty,
	double px,
	double py,
	double vx,
	double vy )
{
	// P from AB
	// Q is closest pt on AB
	// (P-Q).(B-A) == 0 then Q is closest pt.
	// Q = A + t.(B-A)
	// (P-(A+t.(B-A)).(B-A)
	// use AB for const B-A, and AP for P-A.
	// (AP + tAB).AB == AP.AB + tAB.AB
	// t = -AP.AB / AB.AB == PA.AB / AB.AB

	double	pax = px - ptx;
	double	pay = py - pty;
	double	den = vx * vx + vy * vy;
	if( den == 0 )
		return 0;

	double	num = pax * vx + pay * vy;
	double	t = num / den;
	return t;
}

double	Utils::DistPtFromLine(
	double ptx,
	double pty,
	double px,
	double py,
	double vx,
	double vy )
{
	double	t = ClosestPtOnLine(ptx, pty, px, py, vx, vy);
	double	qx = px + vx * t;
	double	qy = py + vy * t;
	double	dist = hypot(ptx - qx, pty - qy);
	return dist;
}

bool	Utils::LineCrossesLine(
	double	p0x,
	double	p0y,
	double	v0x,
	double	v0y,
	double	p1x,
	double	p1y,
	double	v1x,
	double	v1y,

	double&	t )
{
//	double	denom = lv0 % lv1;
	double	denom = v0x * v1y - v0y * v1x;
	if( denom == 0 )
		return false;

//	double	numer = lv1 % (lp0 - lp1);
	double	numer = v1x * (p0y - p1y) - v1y * (p0x - p1x);

	t = numer / denom;

	return true;
}

bool	Utils::LineCrossesLine(
	const Vec2d&	p0,
	const Vec2d&	v0,
	const Vec2d&	p1,
	const Vec2d&	v1,

	double&			t )
{
	return LineCrossesLine(p0.x, p0.y, v0.x, v0.y, p1.x, p1.y, v1.x, v1.y, t);
}

bool	Utils::LineCrossesLineXY(
	const Vec3d&	p0,
	const Vec3d&	v0,
	const Vec3d&	p1,
	const Vec3d&	v1,

	double&			t )
{
	return LineCrossesLine(p0.x, p0.y, v0.x, v0.y, p1.x, p1.y, v1.x, v1.y, t);
}

bool	Utils::LineCrossesLine(
	const Vec2d&	p0,
	const Vec2d&	v0,
	const Vec2d&	p1,
	const Vec2d&	v1,

	double&			t0,
	double&			t1 )
{
	double	denom = v0.x * v1.y - v0.y * v1.x;
	if( denom == 0 )
		return false;

	double	numer0 = v1.x * (p0.y - p1.y) - v1.y * (p0.x - p1.x);
	double	numer1 = v0.x * (p1.y - p0.y) - v0.y * (p1.x - p0.x);

	t0 =  numer0 / denom;
	t1 = -numer1 / denom;

	return true;
}

bool	Utils::LineCrossesCircle(
	const Vec2d&	lp,		// start point of line
	const Vec2d&	lv,		// vector for line
	const Vec2d&	cp,		// centre point of circle
	double			cr,		// radius of circle
	double&			t0,		// returned solution, smallest
	double&			t1 )	// returned solution, largest
{
	Vec2d	dp = lp - cp;

	double	a = lv * lv;
	if( a == 0 )
		return false;

	double	b = 2 * (lv * dp);
	double	c = dp * dp - cr * cr;

	double	inner = b * b - 4 * a * c;

	if( inner < 0 )
		return false;

	inner = sqrt(inner);

	t0 = (-b - inner) / (2 * a);
	t1 = (-b + inner) / (2 * a);

	return true;
}

double	Utils::CalcCurvature(
	double p1x, double p1y,
	double p2x, double p2y,
	double p3x, double p3y )
{
	double	px = p1x - p2x;
	double	py = p1y - p2y;
	double	qx = p2x - p3x;
	double	qy = p2y - p3y;
	double	sx = p3x - p1x;
	double	sy = p3y - p1y;

	double	den = sqrt((px * px + py * py) *
					   (qx * qx + qy * qy) *
					   (sx * sx + sy * sy));
	if( den == 0 )
		return 0;

	double	num = 2 * (px * qy - py * qx);
	double	K = num / den;
	return K;
}

double	Utils::CalcCurvature(
	const Vec2d& p1,
	const Vec2d& p2,
	const Vec2d& p3 )
{
	return CalcCurvature(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
}

double	Utils::CalcCurvatureTan(
	const Vec2d& p1,
	const Vec2d& tangent,
	const Vec2d& p2 )
{
	Vec2d	v = VecUnit(VecNorm(tangent));
	Vec2d	u = VecNorm(p2 - p1);
	Vec2d	q = (p1 + p2) * 0.5;
	double	radius;
	if( !LineCrossesLine(p1, v, q, u, radius) )
		return 0;
	else
		return 1.0 / radius;
}

double	Utils::CalcCurvatureXY(
	const Vec3d& p1,
	const Vec3d& p2,
	const Vec3d& p3 )
{
	return CalcCurvature(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
}

double	Utils::CalcCurvatureZ(
	const Vec3d& p1,
	const Vec3d& p2,
	const Vec3d& p3 )
{
	double	x1 = 0;
	double	x2 = (p1 - p2).len();
	double	x3 = x2 + (p2 - p3).len();
	return CalcCurvature(x1, p1.z, x2, p2.z, x3, p3.z);
}

bool	Utils::CalcTangent(
	const Vec2d&	p1,
	const Vec2d&	p2,
	const Vec2d&	p3,

	Vec2d&			tangent )
{
	Vec2d	mid1  = (p1 + p2) * 0.5;
	Vec2d	norm1 = VecNorm(p2 - p1);
	Vec2d	mid2  = (p2 + p3) * 0.5;
	Vec2d	norm2 = VecNorm(p3 - p2);

	double	t;
	if( !LineCrossesLine(mid1, norm1, mid2, norm2, t) )
	{
		if( p1 != p3 )
		{
			tangent = VecUnit(p3 - p1);
			return true;
		}

		return false;
	}

	Vec2d	centre = mid1 + norm1 * t;
//	tangent = p2 - centre;
//	tangent = VecNorm(p2 - centre);
	tangent = VecUnit(VecNorm(p2 - centre));
	if( norm1 * (p3 - p1) < 0 )
		tangent = -tangent;
	return true;
}
/*
bool	Utils::CalcCircleCentre(
	const Vec2d&	p1,
	const Vec2d&	p2,
	const Vec2d&	p3,

	Vec2d&			centre )
{
}
*/
double	Utils::InterpCurvatureLin( double k0, double k1, double t )
{
	return k0 + (k1 - k0) * t;
}

double	Utils::InterpCurvatureRad( double k0, double k1, double t )
{
	// r = r0 + (r1 - r0) * t;
	//
	// 1/k = 1/k0 + (1/k1 - 1/k0) * t
	// 1/k = (k1 + (k0 - k1) * t) / (k0 * k1);
	// k = (k0 * k1) / (k1 + (k0 - k1) * t)
	//
	double	den = k1 + (k0 - k1) * t;
	if( fabs(den) < 0.000001 )
		den = 0.000001;
	return k0 * k1 / den;
}

double	Utils::InterpCurvature( double k0, double k1, double t )
{
//	return InterpCurvatureRad(k0, k1, t);
	return InterpCurvatureLin(k0, k1, t);
}

double	Utils::VecAngXY( const Vec3d& v )
{
	return atan2(v.y, v.x);
}

double	Utils::VecLenXY( const Vec3d& v )
{
	return hypot(v.y, v.x);
}

Vec3d	Utils::VecNormXY( const Vec3d& v )
{
	return Vec3d(-v.y, v.x, v.z);
}

double	Utils::VecAngle( const Vec2d& v )
{
	return atan2(v.y, v.x);
}

Vec2d	Utils::VecNorm( const Vec2d& v )
{
	return Vec2d(-v.y, v.x);
}

Vec2d	Utils::VecUnit( const Vec2d& v )
{
	double	h = hypot(v.x, v.y);
	if( h == 0 )
		return Vec2d(0, 0);
	else
		return Vec2d(v.x / h, v.y / h);
}

int		Utils::SolveQuadratic( double A, double B, double C, double* r1, double* r2 )
{
    if( A == 0 )
    {
        // trivial solution: B x + C == 0, x == -C / B
        if( B == 0 )
			return 0;    // no solution!

		double root = -C / B;
		if( r1 ) *r1 = root;
		if( r2 ) *r2 = root;
        return 1;
    }
    else if( B == 0 )
    {
        // another fairly trivial solution: A x^2 + C == 0, x^2 == -C / A
        double  rhs = -C / A;
        if( rhs < 0 )
			return 0;   // no real roots

		double root = sqrt(rhs);
		if( r1 ) *r1 = -root;
		if( r2 ) *r2 = root;
        return 2;
    }
    else
    {
        double  insideSqrt = B * B - 4 * A * C;
        if( insideSqrt < 0 )
        {
            // no real roots!
            return 0;
        }

        double  afterSqrt = sqrt(insideSqrt);

		// smallest root 1st.
		if( r1 ) *r1 = (-B - afterSqrt) / (2 * A);
		if( r2 ) *r2 = (-B + afterSqrt) / (2 * A);
        return 2;
    }
}

int		Utils::NewtonRaphson( double A, double B, double C, double D, double* root )
{
    // first, find min/max via differenciation
    double  dA = 3 * A;
    double  dB = 2 * B;
    double  dC = C;

    double  x, y;

    double  x1;
    double  x2;

    if( SolveQuadratic(dA, dB, dC, &x1, &x2) )
    {
        // find y values for the x values
        double  y1 = x1 * (x1 * (x1 * A + B) + C) + D;
        double  y2 = x2 * (x2 * (x2 * A + B) + C) + D;

        if( y1 == 0 )
        {
			if( root ) *root = x1;
            return 1;
        }

        if( y2 == 0 )
        {
			if( root ) *root = x2;
            return 1;
        }

        if( x1 > x2 )
        {
            // swap them round
            double temp = x1; x1 = x2; x2 = temp;
        }

        // decide where to start from
        if( y1 > 0 && y2 > 0 )
        {
            // test whether A is posititive or negative
            if( A > 0 )
            {
                x = x1 - 1;
            }
            else
            {
                x = x2 + 1;
            }
        }
        else if( y1 < 0 && y2 < 0 )
        {
            // test whether A is posititive or negative
            if( A > 0 )
            {
                x = x2 + 1;
            }
            else
            {
                x = x1 - 1;
            }
        }
        else
        {
            // y's are on opposite sides - start between them
            x = (x1 + x2) * 0.5;
        }
    }
    else
    {
        // no min or max - got to start somewhere - try 0
        x = 0;
    }

	// finally, perform a NewtonRaphson search
    int count = 100;
    while( count-- )  
    {
        y = x * (x * (x * A + B) + C) + D;

		if( fabs(y) < 1e-6 )
		    break;   // found a root

        // work out the equation of the tangent
        double m = x * (x * dA + dB) + dC;
        double c = y - m * x;

        // work out new x
        x = -c / m;
    }
    if( count <= 0 )
		return 0;      // failed!

    // got a root at x
	if( root ) *root = x;

    return 1;
}

int Utils::SolveCubic(	double A, double B, double C, double D,
						double* r1, double* r2, double* r3 )
{
    if( A == 0.0 )
    {
        // trivial solution - have a quadratic!
        int nRoots = SolveQuadratic(B, C, D, r1, r2);
        return nRoots;
    }

    if( D == 0.0 )
    {
        // trivial solution - x==0 is a root, and also have a quadratic!
        if( r1 )
			*r1 = 0.0;
        int nRoots = SolveQuadratic(A, B, C, r2, r3);
        return 1 + nRoots;
    }

	// find a root via the NewtonRaphson method
	if( NewtonRaphson(A, B, C, D, r1) == 0 )
    {
        // failed to find a root
        return 0;
    }

    // make up quadratic to solve for other roots
    double  qA = A * (*r1);			//  ARx
    double  qB = (qA + B) * (*r1);	//  (ARR + BR)x
    double  qC = -D;				//  -D

    // solve the quadraic for the other roots
    int nRoots = SolveQuadratic(qA, qB, qC, r2, r3);
    return 1 + nRoots;
}
