/***************************************************************************

    file        : CarModel.cpp
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

// CarModel.cpp: implementation of the CarModel class.
//
//////////////////////////////////////////////////////////////////////

#include "CarModel.h"
#include "Quadratic.h"

#include <portability.h>
#include <math.h>
#include "Utils.h"

// The "SHADOW" logger instance.
extern GfLogger* PLogSHADOW;
#define LogSHADOW (*PLogSHADOW)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CarModel::CarModel()
://	FLAGS(0),
//	FLAGS(F_SEPARATE_FRONT_REAR),
    FLAGS(F_SEPARATE_FRONT_REAR | F_USE_KV),
    MASS(0),
    FUEL(0),
    DAMAGE(0),
    WIDTH(2),
    TYRE_MU(0),
    TYRE_MU_F(0),
    TYRE_MU_R(0),
    EFFECTIVEMU(0),
    MU_SCALE(1),
    SKILL(1.0),
    KZ_SCALE(0),
    KV_SCALE(1),
    BRAKE_MU_SCALE(0.95),
    GRIP_SCALE_F(1),
    GRIP_SCALE_R(1),
    WEARTREAD(1.0),
    WING_ANGLE_F(0),
    WING_ANGLE_R(0),
    CA(0),
    CA_FW(0),
    CA_RW(0),
    CA_GE(0),
    CD_BODY(0),
    CD_WING(0),
    OP_LOAD(0),
    LF_MIN(0.8),
    LF_MAX(1.6),
    LF_K(log((1.0f - LF_MIN) / (LF_MAX - LF_MIN))),
    POS_AZ(0),
    VEL_AZ(0),
    F_AXLE_X(1.5),
    R_AXLE_X(-1.5),
    F_AXLE_WB(0.5),
    R_AXLE_WB(0.5),
    F_AXLE_CG(0),
    R_AXLE_CG(0),
    F_WING_X(1.5),
    R_WING_X(-1.5),
    TARGET_SLIP(0.175),	// for defaults of 30, 0.8, 0.7
    MAX_SLIP(0.27),
    GEAR_CHANGE_REVS(8200 * 2 * PI / 60),
    DIFF_RATIO(1),
    DIFF_EFF(1),
    ENGINE_REV_LIMIT(8500 * 2 * PI / 60),
    ENGINE_MAX_REVS(10000 * 2 * PI / 60),
    HASTYC(false),
    HASABS(false),
    HASESP(false),
    HASTCL(false)
{
    for( int w = 0; w < 4; w++ )
        _wheel[w].setWheel( w );

    setupDefaultGearbox();
    setupDefaultEngine();
}

//===========================================================================

CarModel::~CarModel()
{
}

//===========================================================================

void    CarModel::config( const tCarElt* car )
{
    configWheels( car );
    configCar( car->_carHandle );
}

void    CarModel::config( void* hCar )
{
    configWheels( hCar );
    configCar( hCar );
}

void	CarModel::setupDefaultGearbox()
{
    GEAR_RATIOS.clear();
    GEAR_EFFS.clear();

    GEAR_RATIOS.push_back( 2.66 );
    GEAR_EFFS.push_back( 0.955 );
    GEAR_RATIOS.push_back( 1.78 );
    GEAR_EFFS.push_back( 0.957 );
    GEAR_RATIOS.push_back( 1.3 );
    GEAR_EFFS.push_back( 0.95 );
    GEAR_RATIOS.push_back( 1.0 );
    GEAR_EFFS.push_back( 0.983 );
    GEAR_RATIOS.push_back( 0.84 );
    GEAR_EFFS.push_back( 0.948 );
    GEAR_RATIOS.push_back( 0.74 );
    GEAR_EFFS.push_back( 0.94 );
}

void	CarModel::setupDefaultEngine()
{
    ENGINE_REVS.clear();
    ENGINE_TORQUES.clear();

    ENGINE_REVS.push_back(     0 );
    ENGINE_REVS.push_back(  1000 * 2 * PI / 60 );
    ENGINE_REVS.push_back(  2000 * 2 * PI / 60 );
    ENGINE_REVS.push_back(  3000 * 2 * PI / 60 );
    ENGINE_REVS.push_back(  4000 * 2 * PI / 60 );
    ENGINE_REVS.push_back(  5000 * 2 * PI / 60 );
    ENGINE_REVS.push_back(  6000 * 2 * PI / 60 );
    ENGINE_REVS.push_back(  7000 * 2 * PI / 60 );
    ENGINE_REVS.push_back(  8000 * 2 * PI / 60 );
    ENGINE_REVS.push_back(  9000 * 2 * PI / 60 );
    ENGINE_REVS.push_back( 10000 * 2 * PI / 60 );

    ENGINE_TORQUES.push_back( 97 );
    ENGINE_TORQUES.push_back( 222 );
    ENGINE_TORQUES.push_back( 325 );
    ENGINE_TORQUES.push_back( 470 );
    ENGINE_TORQUES.push_back( 560 );
    ENGINE_TORQUES.push_back( 555 );
    ENGINE_TORQUES.push_back( 545 );
    ENGINE_TORQUES.push_back( 511 );
    ENGINE_TORQUES.push_back( 471 );
    ENGINE_TORQUES.push_back( 410 );
    ENGINE_TORQUES.push_back( 320 );
}

void    CarModel::configCar( void* hCar )
{
    const char *enabling;
    enabling = GfParmGetStr(hCar, SECT_FEATURES, PRM_TIRETEMPDEG, VAL_NO);

    if (strcmp(enabling, VAL_YES) == 0)
    {
      HASTYC = true;
      LogSHADOW.info("#Car has TYC yes\n");
    }
    else
      LogSHADOW.info("#Car has TYC no\n");

    enabling = GfParmGetStr(hCar, SECT_FEATURES, PRM_ABSINSIMU, VAL_NO);

    if (strcmp(enabling, VAL_YES) == 0)
    {
      HASABS = true;
      LogSHADOW.info("#Car has ABS yes\n");
    }
    else
      LogSHADOW.info("#Car has ABS no\n");

    enabling = GfParmGetStr(hCar, SECT_FEATURES, PRM_ESPINSIMU, VAL_NO);

    if (strcmp(enabling, VAL_YES) == 0)
    {
      HASESP = true;
      LogSHADOW.info("#Car has ESP yes\n");
    }
    else
      LogSHADOW.info("#Car has ESP no\n");

    enabling = GfParmGetStr(hCar, SECT_FEATURES, PRM_TCLINSIMU, VAL_NO);

    if (strcmp(enabling, VAL_YES) == 0)
    {
      HASTCL = true;
      LogSHADOW.info("#Car has TCL yes\n");
    }
    else
      LogSHADOW.info("#Car has TCL no\n");

    MASS = GfParmGetNum(hCar, SECT_CAR, PRM_MASS, NULL, 1000.0);
    FUEL = GfParmGetNum(hCar, SECT_CAR, PRM_FUEL, NULL, 95.0);

    float fwingarea	= GfParmGetNum(hCar, SECT_FRNTWING, PRM_WINGAREA,  NULL, 0.0);
    WING_ANGLE_F	= GfParmGetNum(hCar, SECT_FRNTWING, PRM_WINGANGLE, NULL, 0.0);
    float rwingarea	= GfParmGetNum(hCar, SECT_REARWING, PRM_WINGAREA,  NULL, 0.0);
    WING_ANGLE_R	= GfParmGetNum(hCar, SECT_REARWING, PRM_WINGANGLE, NULL, 0.0);
    float fwingArea = fwingarea * sinf((float)WING_ANGLE_F);
    float rwingArea = rwingarea * sinf((float)WING_ANGLE_R);
    float wingca	= 1.23f * (fwingArea + rwingArea);

    float cl =  GfParmGetNum(hCar, SECT_AERODYNAMICS, PRM_FCL, NULL, 0.0) +
                GfParmGetNum(hCar, SECT_AERODYNAMICS, PRM_RCL, NULL, 0.0);
    float h  =  GfParmGetNum(hCar, SECT_FRNTRGTWHEEL, PRM_RIDEHEIGHT, NULL, 0.20f) +
                GfParmGetNum(hCar, SECT_FRNTLFTWHEEL, PRM_RIDEHEIGHT, NULL, 0.20f) +
                GfParmGetNum(hCar, SECT_REARRGTWHEEL, PRM_RIDEHEIGHT, NULL, 0.20f) +
                GfParmGetNum(hCar, SECT_REARLFTWHEEL, PRM_RIDEHEIGHT, NULL, 0.20f);
    h *= 1.5f; h = h*h; h = h*h; h = 2.0f * (float)exp(-3.0*h);
    CA = h*cl + 4.0f*wingca;
    CA_FW = 4 * 1.23f * fwingArea;
    CA_RW = 4 * 1.23f * rwingArea;
    CA_GE = h * cl;

    LogSHADOW.info( "CA %g   CA_FW %g   CA_RW %g   CA_GE %g\n", CA, CA_FW, CA_RW, CA_GE );

    double	cx = GfParmGetNum(hCar, SECT_AERODYNAMICS, PRM_CX, NULL, 0.0);
    double	frontArea = GfParmGetNum(hCar, SECT_AERODYNAMICS, PRM_FRNTAREA, NULL, 0.0);

    CD_BODY = 0.645 * cx * frontArea;
    CD_WING = wingca;

    // assumes load characteristics are the same for all tyres.
    OP_LOAD = GfParmGetNum(hCar, SECT_REARRGTWHEEL, PRM_OPLOAD, (char*)NULL, MASS * G * 1.2f);
    LF_MIN	= GfParmGetNum(hCar, SECT_REARRGTWHEEL, PRM_LOADFMIN, (char*)NULL, 0.8f);
    LF_MAX	= GfParmGetNum(hCar, SECT_REARRGTWHEEL, PRM_LOADFMAX, (char*)NULL, 1.6f);
    LF_K	= log((1.0f - LF_MIN) / (LF_MAX - LF_MIN));

    WIDTH = GfParmGetNum(hCar, SECT_CAR, PRM_WIDTH, NULL, 1.9f);

    TYRE_MU_F = MN(GfParmGetNum(hCar, SECT_FRNTRGTWHEEL, PRM_MU, NULL, 1.0),
                   GfParmGetNum(hCar, SECT_FRNTLFTWHEEL, PRM_MU, NULL, 1.0));
    TYRE_MU_R = MN(GfParmGetNum(hCar, SECT_REARRGTWHEEL, PRM_MU, NULL, 1.0),
                   GfParmGetNum(hCar, SECT_REARLFTWHEEL, PRM_MU, NULL, 1.0));
    TYRE_MU   = MN(TYRE_MU_R, TYRE_MU_R);

    F_AXLE_X  = GfParmGetNum(hCar, SECT_FRNTAXLE, PRM_XPOS, NULL, 0);
    R_AXLE_X  = GfParmGetNum(hCar, SECT_REARAXLE, PRM_XPOS, NULL, 0);
    F_AXLE_WB = GfParmGetNum(hCar, SECT_FRNTAXLE, PRM_FRWEIGHTREP, NULL, 0.5f);
    R_AXLE_WB = 1 - F_AXLE_WB;
    F_AXLE_CG = h * GfParmGetNum(hCar, SECT_AERODYNAMICS, PRM_FCL, NULL, 0);
    R_AXLE_CG = h * GfParmGetNum(hCar, SECT_AERODYNAMICS, PRM_RCL, NULL, 0);

    F_WING_X  = GfParmGetNum(hCar, SECT_FRNTWING, PRM_XPOS, NULL, 0);
    R_WING_X  = GfParmGetNum(hCar, SECT_REARWING, PRM_XPOS, NULL, 0);

    // assume all 4 tyres have the same coefficients...
    const double	Ca      = GfParmGetNum(hCar, SECT_FRNTLFTWHEEL, PRM_CA, (char*)NULL, 30.0f);
    const double	RFactor = GfParmGetNum(hCar, SECT_FRNTLFTWHEEL, PRM_RFACTOR, (char*)NULL, 0.8f);
    const double	EFactor	= GfParmGetNum(hCar, SECT_FRNTLFTWHEEL, PRM_EFACTOR, (char*)NULL, 0.7f);

    const double	mfC = 2 - asin(RFactor) * 2 / PI;
    const double	mfB = Ca / mfC;
    const double	mfE = EFactor;

    double	targetSlip = 0.175;
    double	targetF = 0.0;
    double	lastF = 0.0;
    double	maxSlip = 0.27;
    for( double slip = 0.0; slip < 0.5; slip += 0.001 )
    {
        double	Bx = mfB * slip;
        double	F = sin(mfC * atan(Bx * (1.0f - mfE) + mfE * atan(Bx)));	// assumes PRO skill level.

//		PRINTF( "slip=%g  F=%g\n", slip, F );

        if( F > targetF )
        {
            targetSlip = slip;
            targetF = F;
        }

        if( lastF > 0.99 && F < 0.99 )
            maxSlip = slip;

        lastF = F;
    }

    TARGET_SLIP = targetSlip;
    MAX_SLIP	= maxSlip;

    LogSHADOW.info( "TARGET_SLIP=%g  MAX_SLIP=%g\n", TARGET_SLIP, MAX_SLIP );

    char buf[64];
    snprintf( buf, sizeof (buf), "%s/%s", SECT_ENGINE, ARR_DATAPTS );
    int nPts = GfParmGetEltNb(hCar, buf);

    if( nPts == 0 )
    {
        setupDefaultEngine();
    }
    else
    {
        ENGINE_REVS.clear();
        ENGINE_TORQUES.clear();
        for( int i = 0; i < nPts; i++ )
        {
            snprintf( buf, sizeof(buf), "%s/%s/%d", SECT_ENGINE, ARR_DATAPTS, i + 1 );
            double revs    = GfParmGetNum(hCar, buf, PRM_RPM, (char*)NULL, 0);
            double torque  = GfParmGetNum(hCar, buf, PRM_TQ, (char*)NULL, 0);
            ENGINE_REVS.push_back( revs );
            ENGINE_TORQUES.push_back( torque );
        }
    }

    ENGINE_REV_LIMIT = GfParmGetNum(hCar, SECT_ENGINE, PRM_REVSLIM, (char*)NULL, 800);
    GEAR_CHANGE_REVS = ENGINE_REV_LIMIT - 200 * 2 * PI / 60;  // change gear a little below rev limit.
    ENGINE_MAX_REVS  = ENGINE_REVS.back();

    GEAR_RATIOS.clear();
    GEAR_EFFS.clear();
    for( int i = 1; ; i++ )
    {
        snprintf( buf, sizeof(buf), "%s/%s/%d", SECT_GEARBOX, ARR_GEARS, i );
        double ratio = GfParmGetNum(hCar, buf, PRM_RATIO, (char*)NULL, 0.0f);

        if( ratio == 0.0 )
            break;

        double eff   = GfParmGetNum(hCar, buf, PRM_EFFICIENCY, (char*)NULL, 1.0f);
        GEAR_RATIOS.push_back( ratio );
        GEAR_EFFS.push_back( eff );
    }

    DIFF_RATIO = GfParmGetNum(hCar, SECT_REARDIFFERENTIAL, PRM_RATIO, (char*)NULL, 1.0f);
    DIFF_EFF   = GfParmGetNum(hCar, SECT_REARDIFFERENTIAL, PRM_EFFICIENCY, (char*)NULL, 1.0f);

    // cache values of acceleration from speed.
    ACCF_FROM_SPEED.clear();
    double  w_radius = rearWheelsAverageRadius();
    double  max_revs = GfParmGetNum(hCar, SECT_ENGINE, PRM_REVSMAX, (char*)NULL, 1000);
    double  max_speed = max_revs * w_radius / (GEAR_RATIOS.back() * DIFF_RATIO);

    for( int spd = 0; spd < max_speed; spd++ )
    {
        ACCF_FROM_SPEED.push_back( CalcAccForceFromSpeed(spd) );
    }
}

//===========================================================================

void	CarModel::update( const tCarElt* car, const tSituation* sit )
{
    Vec3d	new_pos_g = Vec3d(car->pub.DynGCg.pos.x, car->pub.DynGCg.pos.y, car->pub.DynGCg.pos.z);
    Vec3d	new_vel_g = (new_pos_g - POS_G) / sit->deltaTime;
    Vec3d	new_acc_g = (new_vel_g - VEL_G) / sit->deltaTime;

    POS_G = new_pos_g;
    VEL_G = new_vel_g;
    ACC_G = new_acc_g;

    const sgMat4& m = car->pub.posMat;

    VEL_L.x = VEL_G * Vec3d(m[0][0], m[0][1], m[0][2]);
    VEL_L.y = VEL_G * Vec3d(m[1][0], m[1][1], m[1][2]);
    VEL_L.z = VEL_G * Vec3d(m[2][0], m[2][1], m[2][2]);

    ACC_L.x = ACC_G * Vec3d(m[0][0], m[0][1], m[0][2]);
    ACC_L.y = ACC_G * Vec3d(m[1][0], m[1][1], m[1][2]);
    ACC_L.z = ACC_G * Vec3d(m[2][0], m[2][1], m[2][2]);

    double	new_pos_az = car->pub.DynGCg.pos.az;
    double	new_vel_az = Utils::NormPiPi(new_pos_az - POS_AZ) / sit->deltaTime;

    POS_AZ = new_pos_az;
    VEL_AZ = new_vel_az;

    if (HASTYC)
    {
        GRIP_SCALE_F = MN(car->priv.wheel[0].condition, car->priv.wheel[1].condition);
        GRIP_SCALE_R = MN(car->priv.wheel[2].condition, car->priv.wheel[3].condition);
        EFFECTIVEMU = MN(MN(car->priv.wheel[0].effectiveMu, car->priv.wheel[1].effectiveMu),
                MN(car->priv.wheel[2].effectiveMu, car->priv.wheel[3].effectiveMu));
        WEARTREAD = MN(MN(car->priv.wheel[0].treadDepth, car->priv.wheel[1].treadDepth),
                MN(car->priv.wheel[2].treadDepth, car->priv.wheel[3].treadDepth));

        LogSHADOW.debug("GRIP F = %.3f - GRIP R = %.3f - EFFECTIVEMU = %.3f - WEARTREAD = %.5f - Temperature = %.3f\n", GRIP_SCALE_F, GRIP_SCALE_R, EFFECTIVEMU, WEARTREAD, MN(car->priv.wheel[0].temp_mid, car->priv.wheel[1].temp_mid));
    }

    updateWheels( car, sit );
}

//===========================================================================

const WheelModel& CarModel::wheel( int wheel ) const
{
    return _wheel[wheel];
}

//===========================================================================

void	CarModel::configWheels( const tCarElt* car )
{
    for( int w = 0; w < 4; w++ )
        _wheel[w].config( car );
}

//===========================================================================

void	CarModel::configWheels( void* hCar )
{
    for( int w = 0; w < 4; w++ )
        _wheel[w].config( hCar );
}

//===========================================================================

void	CarModel::updateWheels( const tCarElt* car, const tSituation* s )
{
    for( int w = 0; w < 4; w++ )
        _wheel[w].update( car, s, *this );
}

//===========================================================================

double  CarModel::rearWheelsAverageRadius() const
{
    return (_wheel[2].radius() + _wheel[3].radius()) * 0.5;
}

//===========================================================================

double	CarModel::frontAxleSlipTangential() const
{
    double xfslip = (wheel(0).slipX() + wheel(1).slipX()) * 0.5;
    return xfslip;
}

//===========================================================================

double	CarModel::CalcMaxSpeed(
    double k,
    double kz,
    double kv,
    double trackMu,
    double rollAngle,
    double pitchAngle ) const
{
    if( FLAGS & F_SEPARATE_FRONT_REAR )
        return CalcMaxSpeedAeroNew(k, kz, kv, trackMu, rollAngle, pitchAngle);
    else
        return CalcMaxSpeedAeroOld(k, kz, kv, trackMu, rollAngle, pitchAngle);
}

//===========================================================================

double	CarModel::CalcMaxSpeedAeroOld(
    double k,
    double kz,
    double kv,
    double trackMu,
    double trackRollAngle,
    double trackPitchAngle ) const
{
    //
    //	Here we calculate the theoretical maximum speed at a point on the
    //	path.  This takes into account the curvature of the path (k), the
    //	grip on the road (mu), the downforce from the wings and the ground
    //	effect (CA), the tilt of the road (left to right slope) (sn)
    //	and the curvature of the road in z (kz).
    //
    //	There are still a few silly fudge factors to make the theory match
    //	with the reality (the car goes too slowly otherwise, aarrgh!).
    //

    double	M  = MASS + FUEL;

    double	mua, muf, mur;

    if( FLAGS & F_OLD_AERO_1 )
    {
        double	MU_F = trackMu * TYRE_MU_F;
        double	MU_R = trackMu * TYRE_MU_R;

        muf = MU_F * MU_SCALE;
        mur = MU_R * MU_SCALE;
        mua = (MU_F + MU_R) * 0.5;
    }
    else
    {
        double	MU = trackMu * TYRE_MU;

        mua   = MU * MU_SCALE;// * 0.975;
    }

 //	mua *= (GRIP_SCALE_F + GRIP_SCALE_R) * 0.5;
    mua *= MN(GRIP_SCALE_F, GRIP_SCALE_R);

    double	cs = cos(trackRollAngle) * cos(trackPitchAngle);
    double	sn = sin(trackRollAngle);

    double	absK = MX(0.001, fabs(k));
    double	sgnK = SGN(k);

    double	num, den;

    if( FLAGS & F_OLD_AERO_1 )
    {
        num = M * (cs * G * mua + sn * G * sgnK);
//		den = M * (absK - 0.1 * kz) -
        if( FLAGS & F_USE_KV )
            den = M * (absK - KV_SCALE * kv) -
                        (CA_FW * muf + CA_RW * mur + CA_GE * mua);
        else
            den = M * (absK - KZ_SCALE * kz) -
                        (CA_FW * muf + CA_RW * mur + CA_GE * mua);
    }
    else
    {
//		num = M * (G * mu + sn * G * sgnK);
        num = M * (cs * G * mua + sn * G * sgnK);
        if( FLAGS & F_USE_KV )
            den = M * (absK - KV_SCALE * kv) - CA * mua; //mu_df;
        else
            den = M * (absK - KZ_SCALE * kz) - CA * mua; //mu_df;
    }

    if( den < 0.00001 )
        den = 0.00001;

    double	spd = sqrt(num / den);

    if( spd > 200 )
        spd = 200;

    return spd * SKILL;
}

//===========================================================================

double	CarModel::CalcMaxSpeedAeroNew(
    double k,
    double kz,
    double kv,
    double trackMu,
    double rollAngle,
    double pitchAngle ) const
{
    double maxSpeedFrontAxle = AxleCalcMaxSpeed(k, kz, kv, trackMu,
                                                rollAngle, pitchAngle,
                                                GRIP_SCALE_F,
//	                                            (GRIP_SCALE_F + GRIP_SCALE_R) * 0.5,
                                                TYRE_MU_F, F_AXLE_X, F_WING_X,
                                                F_AXLE_WB, CA_FW, F_AXLE_CG);
    double maxSpeedRearAxle  = AxleCalcMaxSpeed(k, kz, kv, trackMu,
                                                rollAngle, pitchAngle,
                                                GRIP_SCALE_R,
//	                                            (GRIP_SCALE_F + GRIP_SCALE_R) * 0.5,
                                                TYRE_MU_R, R_AXLE_X, R_WING_X,
                                                R_AXLE_WB, CA_RW, R_AXLE_CG);
    return MN(maxSpeedFrontAxle, maxSpeedRearAxle) * SKILL;
}

//===========================================================================

double	CarModel::calcPredictedLoad(
    double speed,
    double weight_fraction,
    double downforce_constant,
    double k,
    double kz,
    double kv,
    double sin_roll,
    double cos_roll,
    double cos_pitch ) const
{
    double	load_g = (MASS + FUEL) * weight_fraction * G * cos_roll * cos_pitch;
    double	load_a = downforce_constant * speed * speed;
//	double	load_h = (MASS + FUEL) * weight_fraction * sin_roll * k  * speed * speed;
//	double	load_v = (MASS + FUEL) * weight_fraction * cos_roll * kz/6 * speed * speed;	// /6 as kz is already *6
    double	load_v;
    if( FLAGS & F_USE_KV )
        load_v = (MASS + FUEL) * weight_fraction * kv * KV_SCALE * speed * speed;
    else
        load_v = (MASS + FUEL) * weight_fraction * cos_roll * kz * KZ_SCALE * speed * speed;
//	return	load_g + load_a;// + load_h + load_v;
    return	load_g + load_a + /*load_h*/ + load_v;
}

// version with load sensitivity
double	CarModel::AxleCalcMaxSpeed(
    double k,
    double kz,
    double kv,
    double trackMu,
    double trackRollAngle,
    double trackPitchAngle,
    double gripScale,
    double tyreMu,
    double ax,	// axle x position
    double wx,	// wing x position
    double wf,	// axle weight balance factor (e.g. 0.47 for car6)
    double Cw,	// wing downforce constant
    double Cg ) const	// axle ground effect downforce constant -- assumes nominal ride height.
{
    //	This function calculates the theoretical maximum speed for a single axle of
    //	the car at a point on the path.	 This takes into account the curvature of
    //	the path (k), the grip on the road (mu), the downforce from the nearest
    //	wing and the axle's ground effect (CA), the tilt of the road (angle left to
    //	right), the pitch of the road (angle rear to front), and the curvature of
    //	the road in z (kz).
    //
    //	At this point the car is assumed to be travelling at a constant speed, and
    //	thus any additional weight transfer (apart from the static weight balance)
    //	due to accleration or braking is not taken into consideration.
    //
    //	Also makes a simplifying assumption that the downforce from the other wing
    //	on this axle is small enough to be ignored.
    //
    //  ax      -- axle x position (relative to COG)
    //  wx      -- wing x position (relative to COG)
    //  wf      -- weight factor for this axle (0.47 for car6)
    //  Cw      -- wing downforce constant
    //  Cg      -- axle ground effect downforce constant -- but is dependent on the ride height.
    //  ra      -- roll angle
    //	pa		-- pitch angle
    //
    //  af = wx / ax				-- wing downforce factor for this axle
    //  Ma = wf M					-- mass supported by axle
    //  Ca = Cg + Cw af				-- total downforce constant for axle
    //
    //	csp = cos(pa)
    //	csr = cos(ra)
    //	snr = sin(ra)
    //
    //  Gz = G * cos(ra) * cos(pa)	-- downwards acceleration due to gravity
    //  Gy = G * sin(ra)			-- sideways acceleration due to gravity
    //
    //  Az  = v^2 Kz csr			-- intent is that Kz is +ve if resultant acceleration is down
    //
    //	-- downwards forces --
    //  Fdg = Ma Gz					-- gravity
    //  Fda = v^2 Ca				-- aero on axle
    //  Fdv = v^2 Ma Kz csr			-- vertical curvature along track (Kz is +ve --> acceleration is down)
    //	Fdh = v^2 Ma Kh snr			-- horizontal curvature along path.
    //
    //	-- lateral forces --
    //  Fyg = Ma Gy					-- sideways force due to gravity
    //	Fyh = v^2 Ma Kh csr			-- horizontal curvature along path.
    //
    //  Fd = Fdg + Fda + Fdv + Fdh	-- total downwards force on axle
    //  Fd = Ma Gz + v^2 Ca + v^2 Ma Kz csr + v^2 Ma Kh snr
    //
    //	FL = M wf G					-- operational load on axle (stationary unfuelled weight.)
    //	LS = 0.8 + 0.8 exp(-1.386 Fd / FL)	-- load sensitivity factor
    //
    //  Fh = MU Fd LS + Fyg + Fyh	-- maximum horizontal (lateral) force
    //  Fh = MU Fd LS + Ma Gy + v^2 Ma Kh csr
    //  a  = Fh / Ma				-- maximum horizontal (lateral) acceleration
    //
    //  a = v^2 K
    //  Fh / Ma = v^2 K
    //  v^2 Ma K = Fh
    //  v^2 Ma K = MU LS Fd + Ma Gy
    //  v^2 Ma K = MU LS (Ma Gz + v^2       Ma Kz + v^2       Ca) + Ma Gy
    //  v^2 Ma K = MU LS  Ma Gz + v^2 MU LS Ma Kz + v^2 MU LS Ca  + Ma Gy

    //  v^2  Ma K - v^2 MU LS Ma Kz - v^2 MU LS Ca  = MU LS Ma Gz + Ma Gy
    //  v^2 (Ma K -     MU LS Ma Kz -     MU LS Ca) = MU LS Ma Gz + Ma Gy

    //  v^2 = (MU LS Ma Gz + Ma Gy) / (Ma K - MU LS Ma Kz - MU LS Ca)
    //  v^2 = Ma (MU LS Gz + Gy)    / (Ma K - MU LS Ma Kz - MU LS Ca)

    double	absK = MX(0.001, fabs(k));
    double	sgnK = SGN(k);

    double	af = wx / ax;
    double	Ma = wf * (MASS + FUEL);
    double	Ca = Cg + Cw * af;

    double	opLoad = OP_LOAD * wf;
    double	loadFactor = LF_MIN;

//	trackRollAngle *= 1.1;
    double	cs_roll	 = cos(trackRollAngle);
    double	sn_roll	 = sin(trackRollAngle);
    double	cs_pitch = cos(trackPitchAngle);

    double	Gz  = cs_roll * cs_pitch * G;
    double	Gy	= sn_roll * G * sgnK;

    double	spd = 0;
    int i = 0;
    for( ; i < 100; i++ )
    {
        double	mu	= trackMu * tyreMu * MU_SCALE * gripScale * loadFactor;
        double  num = Ma * (mu * Gz + Gy);
//		double  den = MX(0.000001, Ma * absK - /*mu * */ Ma * kz * KZ_SCALE - mu * Ca);
        double  den;
        if( FLAGS & F_USE_KV )
            den = MX(0.000001, Ma * absK -   mu *    Ma * kv * KV_SCALE
                                               /*-   mu *    Ma * k             * sn_roll*/ - mu * Ca);
        else
            den = MX(0.000001, Ma * absK -   mu *    Ma * kz * KZ_SCALE * cs_roll
                                               /*-   mu *    Ma * k             * sn_roll*/ - mu * Ca);

        spd = MN(200, sqrt(num / den));

        double	load = calcPredictedLoad(spd, wf, Ca, k, kz, kv, sn_roll, cs_roll, cs_pitch);
        double	newLoadFactor = LF_MIN + (LF_MAX - LF_MIN) * exp(LF_K * load / opLoad);

        if( fabs(newLoadFactor - loadFactor) < 0.001 )
            break;

        loadFactor = (loadFactor + newLoadFactor) * 0.5;
    }

    if( i == 100 )
        LogSHADOW.debug( "failed to find load factor!!!!! spd %g, lf %g\n", spd, loadFactor );

    return spd;
}

//===========================================================================

double	CarModel::CalcBraking(
    double k0, double kz0, double kv0, double k1, double kz1, double kv1,
    double spd1, double dist, double trackMu,
    double trackRollAngle, double trackPitchAngle ) const
{
    double	M  = MASS + FUEL;

    double	MU = trackMu * TYRE_MU;
    double	MU_F = MU;
    double	MU_R = MU;
    if( FLAGS & F_OLD_AERO_1 )
    {
        MU_F = trackMu * TYRE_MU_F;
        MU_R = trackMu * TYRE_MU_R;
        MU   = (MU_F + MU_R) * 0.5;
    }

    double	CD = CD_BODY * (1.0 + DAMAGE / 10000.0) + CD_WING;

    MU *= BRAKE_MU_SCALE * SKILL;

//	MU *= (GRIP_SCALE_F + GRIP_SCALE_R) * 0.5;
    MU *= MN(GRIP_SCALE_F, GRIP_SCALE_R);

    double	K  = (k0  + k1)  * 0.5;
    double  kz = (kz0 + kz1) * 0.5;// * KZ_SCALE;
    double	kv = (kv0 + kv1) * 0.5;// * KV_SCALE;

    double	Kz = FLAGS & F_USE_KV ? kv : kz;
//	double	Kz = (kz0 + kz1) * 0.5 * KZ_SCALE;
    if( Kz > 0 )
        Kz = 0;

//	trackRollAngle *= 1.1;
    double	sn_roll  = sin(trackRollAngle);
    double	cs_roll  = cos(trackRollAngle);
    double	sn_pitch = sin(trackPitchAngle);
    double	cs_pitch = cos(trackPitchAngle);

    double	Gdown = G * cs_roll * cs_pitch;
    double	Glat  = G * sn_roll;
    double	Gtan  = G * -sn_pitch;

    double	v = spd1;
    double	u = v;

//	double	dist = Utils::VecLenXY(m_pPath[i].CalcPt() -
//								   m_pPath[j].CalcPt());

    double	axle_r = (fabs(F_AXLE_X) + fabs(R_AXLE_X)) * 0.5;

    for( int count = 0; count < 100; count++ )
    {
        double	avgV = (u + v) * 0.5;
        double	avgVV = avgV * avgV;

        double	Froad;
        if( FLAGS & F_OLD_AERO_1 )
        {
            double	Fdown = M * Gdown + M * Kz * avgVV + CA_GE * avgVV;
            double	Ffrnt = CA_FW * avgVV;
            double	Frear = CA_RW * avgVV;

            Froad = Fdown * MU + Ffrnt * MU_F + Frear * MU_R;
        }
        else
        {
            double	Fdown = M * Gdown + M * Kz * avgVV + CA * avgVV;

            Froad = Fdown * MU;
        }

        double	load = calcPredictedLoad(avgV, 1, CA, K, kz, kv, sn_roll, cs_roll, cs_pitch);
        double	loadFactor = LF_MIN + (LF_MAX - LF_MIN) * exp(LF_K * load / OP_LOAD);
        Froad *= loadFactor;

        double	Flat  = M * Glat;
        double	Ftan  = M * Gtan - CD * avgVV;

        // Frot = Iz (w1 - w0) / (r t)
        // t  = dist / avgV
        // w1 = v * k1
        // w0 = u * k0
        // r  = average distance of axles from COG.
        // Iz = inertia around z axis.
        double t = dist / avgV;
        double INERTIA_Z = 1000;
        double Frot = INERTIA_Z * fabs(v * k1 - u * k0) / (axle_r * t);
        Froad -= Frot;	// change in rotation speed uses some grip.

        double	Flatroad = fabs(M * avgVV * K - Flat);
        if( Flatroad > Froad )
            Flatroad = Froad;
        double	Ftanroad = -sqrt(Froad * Froad - Flatroad * Flatroad) + Ftan;

        double	acc = Ftanroad / M;// * 0.95;

//		DEBUGF( "%4d K %7.4f  Glat %.3f\n",
//				i, K, Glat );
//		DEBUGF( "%4d K %7.4f  Fr %.3f  Fl %.3f  Ft %.3f  u %.1f  acc %.1f\n",
//				i, K, Froad, Flat, Ftan, u, acc );
//		DEBUGF( "%4d K %7.4f  Flr %.3f  Ftr %.3f  u %.1f  acc %.1f\n",
//				i, K, Flatroad, Ftanroad, u, acc );

        double	inner = MX(0, v * v - 2 * acc * dist );
        double	oldU = u;
        u = sqrt(inner);
        if( fabs(u - oldU) < 0.001 )
            break;
    }

    return u;
}

//===========================================================================

double  CarModel::CalcEngineTorque( double revs ) const
{
    if( revs < ENGINE_REVS[0] )
        revs = ENGINE_REVS[0];

    if( revs > ENGINE_REVS.back() )
        revs = ENGINE_REVS.back();

    int index = 0;
    while( index + 1 < (int)ENGINE_REVS.size() )
    {
        if( revs <= ENGINE_REVS[index + 1] )
            break;

        index += 1;
    }

    double t = (revs - ENGINE_REVS[index]) / (ENGINE_REVS[index + 1] - ENGINE_REVS[index]);
    double torque = ENGINE_TORQUES[index] + (ENGINE_TORQUES[index + 1] - ENGINE_TORQUES[index]) * t;
    return torque;
}

//===========================================================================

// acceleration available from engine in m/s/s from speed in m/s
double  CarModel::CalcAccForceFromSpeed( double speed ) const
{
    // NOTE: assumes drive is via rear wheels only.
    const double wheel_radius = rearWheelsAverageRadius();

    double  bestAccF = 0;
    int lastGear = (int)GEAR_RATIOS.size() - 1;
    for( int i = 0; i <= lastGear ; i++ )
    {
        double engine_revs  = speed * GEAR_RATIOS[i] * DIFF_RATIO / wheel_radius;
        if( i <  lastGear && engine_revs > GEAR_CHANGE_REVS )
            continue;
        double tq_engine    = CalcEngineTorque(engine_revs);
        double tq_axle      = tq_engine * GEAR_EFFS[i] * DIFF_EFF * GEAR_RATIOS[i] * DIFF_RATIO;
        double f_road		= tq_axle / wheel_radius;
        if( bestAccF < f_road )
            bestAccF = f_road;
    }

    return bestAccF;
}

//===========================================================================

double  CarModel::AccForceFromSpeed( double speed ) const
{
    if( speed < 0 )
        speed = 0;
    else if( speed > ACCF_FROM_SPEED.size() - 2 )
        speed = ACCF_FROM_SPEED.size() - 2;
    int ispeed = (int)speed;
    if( ispeed < 0 || ispeed + 1 > (int)ACCF_FROM_SPEED.size() )
        return 0;
    double fspeed = speed - ispeed; // 0 <= fspeed < 1
    return ACCF_FROM_SPEED[ispeed] + (ACCF_FROM_SPEED[ispeed + 1] - ACCF_FROM_SPEED[ispeed]) * fspeed;
}

//===========================================================================

double	CarModel::CalcAcceleration(
    double k0, double kz0, double kv0, double k1, double kz1, double kv1,
    double spd0, double dist, double trackMu,
    double trackRollAngle, double trackPitchAngle ) const
{
    double	M  = MASS + FUEL;
    double	MU = trackMu * TYRE_MU;
    double	CD = CD_BODY * (1.0 + DAMAGE / 10000.0) + CD_WING;

    // when under braking we keep some grip in reserve.
    //MU *= BRAKE_MU_SCALE;

//	MU *= (GRIP_SCALE_F + GRIP_SCALE_R) * 0.5;
    MU *= MN(GRIP_SCALE_F, GRIP_SCALE_R);

    double	K  = (k0  + k1)  * 0.5;
    double Kz;
    if( FLAGS & F_USE_KV )
        Kz = (kv0 + kv1) * 0.5;// * KV_SCALE;
    else
        Kz = (kz0 + kz1) * 0.5;// * KZ_SCALE;
//	double	Kz = (kz0 + kz1) * 0.5 * KZ_SCALE;
    if( Kz > 0 )
        Kz = 0;

//	trackRollAngle *= 1.1;
    double	Gdown = G * cos(trackRollAngle) * cos(trackPitchAngle);
    double	Glat  = G * sin(trackRollAngle);
    double	Gtan  = G * -sin(trackPitchAngle);

    double	u = spd0;
    double	v = u;

    // 30m/ss @ 0m/s
    //  3m/ss @ 60m/s
    //	1m/ss @ 75m/s
    //	0m/ss @ 85m/s
//	Quadratic	accFromSpd(21.0/5400, -43.0/60, 30);	// approx. clkdtm

    // Power (kW) = Torque (Nm) x Speed (RPM) / 9.5488

    double	axle_r = (fabs(F_AXLE_X) + fabs(R_AXLE_X)) * 0.5;

    for( int count = 0; count < 100; count++ )
    {
        double	avgV = (u + v) * 0.5;
        double	vv = avgV * avgV;

        double	Fdown = M * Gdown + M * Kz * vv + CA * vv;
        double	Froad = Fdown * MU;
        double	Flat  = M * Glat;
        double	Ftan  = M * Gtan - CD * vv;

        // Frot = Iz (w1 - w0) / (r t)
        // t  = dist / avgV
        // w1 = v * k1
        // w0 = u * k0
        // r  = average distance of axles from COG.
        // Iz = inertia around z axis.
        double t = dist / avgV;
        double INERTIA_Z = 1000;
        double Frot = INERTIA_Z * fabs(v * k1 - u * k0) / (axle_r * t);
        Froad -= Frot;	// change in rotation speed uses some grip.

        // work out tangential force available after accounting for the lateral force
        // needed for turning.
        double	Flatroad = fabs(M * vv * K - Flat);
        if( Flatroad > Froad )
            Flatroad = Froad;
        double	Ftanroad = sqrt(Froad * Froad - Flatroad * Flatroad);

        // account for acceleration available from engine.
        double	Facc = AccForceFromSpeed(avgV);

        if( Ftanroad > Facc )
            Ftanroad = Facc;

        // account for track grade and drag.
        Ftanroad += Ftan;

        double	acc = Ftanroad / M;
        double	inner = MX(0, u * u + 2 * acc * dist );
        double	oldV = v;
        v = sqrt(inner);

        if( fabs(v - oldV) < 0.001 )
            break;
    }

    /*if( v < u )
        int f = 1;*/

    return MX(u, v);
}

//===========================================================================

double	CarModel::CalcMaxSpdK() const
{
    const double	MAX_SPD = 110;	// ~400 kph
    double	maxSpdK = G * TYRE_MU / (MAX_SPD * MAX_SPD);
    return maxSpdK;
}

//===========================================================================

double	CarModel::CalcMaxLateralF( double spd, double trackMu ) const
{
    double	M  = MASS + FUEL;
    double	MU = trackMu * TYRE_MU;	// * GRIP_SCALE; ???

    double	vv = spd * spd;

    double	Fdown = M * G + /*M * Kz * vv*/ + CA * vv;
    double	Flat  = Fdown * MU;

    return Flat;
}

//===========================================================================

void	CarModel::CalcSimuSpeeds(
    double	spd0,
    double	dy,
    double	dist,
    double	trackMu,
    double&	minSpd,
    double&	maxSpd ) const
{
    // simple speed calc for use in simulation for path optimisation... the
    //	overriding pre-requisite of which is speed of calculation.
    //
    // a = v*v/r
    // max_a = M * G * MU;
    // max_spd = sqrt(max_a r) = sqrt(M * G * MU / k)

    //double	M  = MASS + FUEL;
//	double	MU = trackMu * TYRE_MU * (GRIP_SCALE_F + GRIP_SCALE_R) * 0.5;
    double	MU = trackMu * TYRE_MU * MN(GRIP_SCALE_F, GRIP_SCALE_R);

    double	max_acc = G * MU;
//	double	max_spd = k == 0 ? 200 : MN(200, sqrt(max_acc / k));

    //	s = ut + 0.5 att = dy
    //	a = 2(dy - ut) / tt      ... but lateral u = 0
    double	estT = dist / spd0;

    double	lat_acc = 2 * dy / (estT * estT);

    if( lat_acc > max_acc )
        lat_acc = max_acc;

    double	lin_acc = sqrt(max_acc * max_acc - lat_acc * lat_acc);

    //
    // accelerate
    //

    // acceleration is limited by engine power... and this quadratic
    //	is an estimation (poor, but hopefully good enough for our purposes).
    static const Quadratic	accFromSpd(21.0/5400, -43.0/60, 30);
    double	eng_acc = accFromSpd.CalcY(spd0) * trackMu;

    if( eng_acc > lin_acc )
        eng_acc = lin_acc;

    maxSpd = sqrt(spd0 * spd0 + 2 * eng_acc * dist);
//	if( maxSpd > max_spd )
//		maxSpd = max_spd;

    //
    // brake
    //

    minSpd = sqrt(spd0 * spd0 - 2 * lin_acc * dist);
}

//===========================================================================

void	CarModel::CalcSimuSpeedRanges(
    double	spd0,
    double	dist,
    double	trackMu,
    double&	minSpd,
    double&	maxSpd,
    double&	maxDY ) const
{
    // simple speed calc for use in simulation for path optimisation... the
    //	overriding pre-requisite of which is speed of calculation.
    //
    // a = v*v/r
    // max_a = M * G * MU;
    // max_spd = sqrt(max_a r) = sqrt(M * G * MU / k)

    //double	M  = MASS + FUEL;
//	double	MU = trackMu * TYRE_MU * (GRIP_SCALE_F + GRIP_SCALE_R) * 0.5;
    double	MU = trackMu * TYRE_MU * MN(GRIP_SCALE_F, GRIP_SCALE_R);

    double	max_acc = G * MU;

    //
    // accelerate
    //

    // acceleration is limited by engine power... and this quadratic
    //	is an estimation (poor, but hopefully good enough for our purposes).
    static const Quadratic	accFromSpd(21.0/5400, -43.0/60, 30);
    double	eng_acc = accFromSpd.CalcY(spd0) * trackMu;
    if( eng_acc > max_acc )
        eng_acc = max_acc;

    maxSpd = sqrt(spd0 * spd0 + 2 * eng_acc * dist);

    //
    // brake
    //

    minSpd = sqrt(spd0 * spd0 - 2 * max_acc * dist);

    //
    // turn (turning is symmetrical)
    //

    // s = ut + 1/2 att    u = 0, as we're looking along vel vector.
    // t = dist / spd0;
    double	turnT = dist / spd0;
    maxDY = 0.5 * max_acc * turnT * turnT;
}

//===========================================================================
