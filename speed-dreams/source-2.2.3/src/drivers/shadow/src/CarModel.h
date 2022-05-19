/***************************************************************************

    file        : CarModel.h
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

// CarModel.h: interface for the CarModel class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CARMODEL_H_
#define _CARMODEL_H_

#include "WheelModel.h"
#include "Vec3d.h"

#include <vector>

class CarModel
{
public:
    enum
    {
        F_NONE					= 0x00,
        F_OLD_AERO_1			= 0x01,
        F_SEPARATE_FRONT_REAR	= 0x02,
        F_USE_PATH_PITCH		= 0x04,
        F_USE_KV				= 0x08,
    };

public:
    CarModel();
    ~CarModel();

    void	setupDefaultGearbox();
    void	setupDefaultEngine();

    void    config( const tCarElt* car );
    void    config( void* hCar );
    void	update( const tCarElt* car, const tSituation* sit );

    double	CalcMaxSpeed( double k, double kz, double kv, double kFriction,
                          double rollAngle, double pitchAngle ) const;

    double	CalcBraking( double k0, double kz0, double kv0, double k1, double kz1, double kv1,
                          double spd1, double dist, double kFriction,
                          double trackRollAngle, double pitchAngle ) const;

    double	CalcAcceleration( double k0, double kz0, double kv0, double k1, double kz1, double kv1,
                              double spd0, double dist, double kFriction,
                              double trackRollAngle, double pitchAngle ) const;
    double	CalcMaxSpdK() const;
    double	CalcMaxLateralF( double spd, double kFriction ) const;

    void	CalcSimuSpeeds( double spd0, double dy, double dist, double kFriction,
                            double& minSpd, double& maxSpd ) const;
    void	CalcSimuSpeedRanges( double spd0, double dist, double kFriction,
                                 double& minSpd, double& maxSpd, double& maxDY ) const;

    const WheelModel&	wheel( int wheel ) const;
    void				configWheels( const tCarElt* car );
    void				configWheels( void* hCar );
    void				updateWheels( const tCarElt* car, const tSituation* s );

    double              rearWheelsAverageRadius() const;

    double				frontAxleSlipTangential() const;

    double				calcPredictedLoad( double speed, double weight_fraction, double downforce_constant,
                                           double k, double kz, double kv, double sin_roll, double cos_roll, double cos_pitch ) const;

    double				AccForceFromSpeed( double speed ) const;

    double				CalcEngineTorque( double rpm ) const;

private:
    void    configCar( void* hCar );
    double	CalcMaxSpeedAeroOld(double k, double kz, double kv, double kFriction,
                                double rollAngle, double pitchAngle ) const;

    double	CalcMaxSpeedAeroNew(double k, double kz, double kv, double kFriction,
                                double rollAngle, double pitchAngle ) const;

    double	AxleCalcMaxSpeed(   double k, double kz, double kv, double kFriction,
                                double trackRollAngle, double trackPitchAngle,
                                double gripScale, double tyreMu, double axleX, double wingX,
                                double weightBalanceFactor,
                                double wingDownforceConst, double axleGroundEffectConst ) const;

    double  CalcAccForceFromSpeed( double speed ) const;

public:
    int		FLAGS;			// options that modify calculations
    double	MASS;			// fixed mass of car.
    double	WIDTH;			// width of car (m).
    double	TYRE_MU;		// mu value of tyres (min of those avail).
    double	TYRE_MU_F;		// mu value of front tyres.
    double	TYRE_MU_R;		// mu value of rear  tyres.
    double  EFFECTIVEMU;    // effective MU.
    double	MU_SCALE;		// scaling of MU to use for this car.
    double  SKILL;          // Skill Driver.
    double	KZ_SCALE;		// bump sensitivity.
    double	KV_SCALE;		// bump sensitivity.
    double	BRAKE_MU_SCALE;	// extra scaling of MU to apply when braking.
    double	GRIP_SCALE_F;	// scaling of grip due to condition of front tyres.
    double	GRIP_SCALE_R;	// scaling of grip due to condition of rear  tyres.
    double  WEARTREAD;      // wear tread.
    double	WING_ANGLE_F;	// front wing angle.
    double	WING_ANGLE_R;	// rear wing angle.

    // calculated aerodynamic constants.
    double	CA;				// aerodynamic downforce constant -- total.
    double	CA_FW;			// aerodynamic downforce constant -- front wing.
    double	CA_RW;			// aerodynamic downforce constant -- rear wing.
    double	CA_GE;			// aerodynamic downforce constant -- ground effect.
    double	CD_BODY;		// aerodynamic drag constant -- car body.
    double	CD_WING;		// aerodynamic drag constant -- wings.

    // load factor related.
    double	OP_LOAD;
    double	LF_MIN;
    double	LF_MAX;
    double	LF_K;

    // real-time tracking.
    Vec3d	POS_G;			// position in global coords.
    Vec3d	VEL_G;			// velocity in global coords.
    Vec3d	ACC_G;			// acceleration in global coords.
    Vec3d	VEL_L;			// velocity in local  coords.
    Vec3d	ACC_L;			// acceleration in local  coords.
    double	POS_AZ;			// angle around z axis.
    double	VEL_AZ;			// speed around z axis.

    double	F_AXLE_X;		// front axle x position
    double	R_AXLE_X;		// rear  axle x position
    double	F_AXLE_WB;		// front axle weight balance (fraction of mass on axle)
    double	R_AXLE_WB;		// rear  axle weight balance (fraction of mass on axle)
    double	F_AXLE_CG;		// front axle ground effect constant
    double	R_AXLE_CG;		// rear  axle ground effect constant

    double	F_WING_X;		// front wing x position
    double	R_WING_X;		// rear  wing x position

    double	TARGET_SLIP;	// amount of slip to give maximum grip.
    double	MAX_SLIP;		// amount of slip where grip level drops below 99% of maximum grip.

    double  GEAR_CHANGE_REVS;   // revs at which to change gear.

    double  DIFF_RATIO;     // rear differential ratio.
    double  DIFF_EFF;       // rear differential efficiency.
    double	ENGINE_REV_LIMIT;	// revs where limit kicks in.
    double  ENGINE_MAX_REVS;    // max revs for this engine.

    std::vector<double> ENGINE_REVS;    // anglular rate (in radians per second.)
    std::vector<double> ENGINE_TORQUES;
    std::vector<double> GEAR_RATIOS;    // gear ratios
    std::vector<double> GEAR_EFFS;      // gear efficiencies.

    std::vector<double> ACCF_FROM_SPEED; // cached table for fast acceleration force calculation from speed.

    bool    HASTYC;
    bool    HASABS;
    bool    HASESP;
    bool    HASTCL;

    WheelModel	_wheel[4];
};

#endif // _CARMODEL_H_
