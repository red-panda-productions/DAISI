/***************************************************************************

    file        : Driver.cpp
    created     : 3 Jan 2020
    copyright   : (C) 2020 Xavier Bertaux

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// Driver.cpp: implementation of the Driver class.
//
//////////////////////////////////////////////////////////////////////


#include <stdlib.h>
#include <algorithm>
#include <string>
#include <time.h>

#include "Driver.h"
#include "Quadratic.h"
#include "Utils.h"
#include "Avoidance.h"
#include "CarBounds2d.h"

#include <portability.h>
#include <robottools.h>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define SECT_PRIV					"private"
#define SECT_PRIV_LEFT				"private/left"
#define SECT_PRIV_RIGHT				"private/right"
#define PRV_GEAR_UP_RPM				"gear up rpm"
#define PRV_MU_SCALE				"mu scale"
#define PRV_BRAKE_MU_SCALE			"brake mu scale"
#define PRV_FLY_HEIGHT				"fly height"
#define PRV_APEX_FACTOR				"apex factor"
#define PRV_FACTOR					"factor"
#define PRV_INNER_MOD				"inner mod"
#define PRV_CARMODEL_FLAGS			"carmodel flags"
#define PRV_SAVE_PATHS				"save paths"
#define PRV_BUMP_MOD				"bump mod"
#define PRV_SIDE_MOD				"side mod"
#define PRV_QUAD_SMOOTH_ITERS		"quad smooth iters"
#define PRV_KZ_SCALE				"kz scale"
#define PRV_KV_SCALE				"kv scale"
#define PRV_ACC_MAX_SPIN_SPEED		"acc max spin speed"
#define PRV_DEC_MAX_SPIN_SPEED		"dec max spin speed"
#define PRV_STEER_K_ACC				"steer k acc"
#define PRV_STEER_K_DEC				"steer k dec"
#define PRV_AVOID_WIDTH				"avoid width"
#define PRV_SPDC_NORMAL				"spd ctrl normal"
#define PRV_SPDC_TRAFFIC			"spd ctrl traffic"
#define PRV_STAY_TOGETHER			"stay together"
#define PRV_PIT_ENTRY_OFFS			"pit entry offset"
#define PRV_PIT_EXIT_OFFS			"pit exit offset"
#define PRV_PIT_DAMAGE_WARN			"pit damage warn limit"
#define PRV_PIT_DAMAGE_DANGER		"pit damage danger limit"
#define PRV_PIT_TIRE_WARN			"pit tire warn limit"
#define PRV_PIT_TIRE_DANGER		    "pit tire danger limit"
#define PRV_PRACTICE_INIT_FUEL      "practice init fuel"
#define PRV_PIT_TEST_STOP           "pit test stop"
#define PRV_SKID_FACTOR				"skid factor"
#define PRV_SKID_FACTOR_TRAFFIC		"skid factor traffic"
#define PRV_REAR_LAT_SLIP_FACTOR	"rear lat slip factor"
#define PRV_REAR_LAT_SLIP_LIMIT		"rear lat slip limit"
#define PRV_REAR_LAT_SLIP_DSCALE	"rear lat slip dscale"
#define PRV_STEER_0_LINE_SCALE		"steer 0 line scale"
#define PRV_TCL_TARGET_SPEED		"tcl target speed"
#define PRV_SAFETY_LIMIT			"safety limit"
#define PRV_SAFETY_MULTIPLIER		"safety multiplier"
#define PRV_BRAKE_LIMIT				"brake limit"
#define PRV_LIMIT_SIDE_USE          "limit side use"

#define SECT_SKILL                  "skill"
#define PRV_SKILL_LEVEL             "level"
#define PRV_SKILL_AGGRO             "aggression"

#define FLY_COUNT		20

#define	STEER_SPD_IDX(x)	(int(floor((x) / 5)))
#define	STEER_K_IDX(k)		(MX(0, MN(int(20 + floor((k) * 500 + 0.5)), 40)))

#define RANDOM_SEED 0xfded
#define RANDOM_A    1664525
#define RANDOM_C    1013904223

//==========================================================================*

//==========================================================================*
// Skilling: Initialize Randomness
//--------------------------------------------------------------------------*
void Driver::SetRandomSeed(unsigned int Seed)
{
    //m_RandomSeed = Seed ? Seed : RANDOM_SEED;
    srand(time(NULL));
    m_RandomSeed = Seed ? Seed : RANDOM_SEED;

    return;
}
//==========================================================================*

//==========================================================================*
// Skilling: Get Randomness
//--------------------------------------------------------------------------*
unsigned int Driver::getRandom()
{
    m_RandomSeed = RANDOM_A * m_RandomSeed + RANDOM_C;
    LogSHADOW.info(" # Random Seed = %d\n", m_RandomSeed);

    return (m_RandomSeed >> 16);
}

static const double	s_sgMin[] = { 0.00,  10 };
static const double	s_sgMax[] = { 0.03, 100 };
static const int	s_sgSteps[] = { 10,  18 };

void	Driver::PathRange::AddGreater(
        double pos,
        double offs,
        bool incRL,
        const Driver& me )
{
}

void	Driver::PathRange::AddLesser(
        double pos,
        double offs,
        bool incRL,
        const Driver& me )
{
}

Driver::Driver(int index) :	INDEX(index),
      m_Strategy(m_track, m_pitPath[PATH_NORMAL][0]),
      _acc(0),
      m_driveType(cDT_RWD),
      m_gearUpRpm(8000),
      rain(false),
      rainintensity(0.0),
      weathercode(0),
      driver_aggression(0.0),
      globalskill(0.0),
      driverskill(0.0),
      pitsharing(false),
      m_prevYawError(0),
      m_prevLineError(0),
      m_steerLimit(0),
      m_prevSteer(0),
      m_flying(0),
      m_avgAY(0),
      m_raceStart(false),
      m_avoidS(1),
      m_avoidT(0),
      m_followPath(PATH_NORMAL),
      m_stuck(NOT_STUCK),
      m_stuckTime(0),
      m_RandomSeed(0),
      m_maxAccel(0, 150, 30, 1),
      m_steerGraph(2, s_sgMin, s_sgMax, s_sgSteps, 0),
      m_lastB(0),
      m_lastBrk(0),
      m_lastTargV(0),
      _tctrlAcc(0),
      _deltaCounter(0),
      _prevDelta(0),
      _lastSpd0(0),
      m_garage(false)
{
    for( int i = 0; i < 50; i++ )
    {
        m_brkCoeff[i] = 0.5;//0.12;
    }

    for( int i = 0; i < STEER_SPD_MAX; i++ )
    {
        for( int j = 0; j < STEER_K_MAX; j++ )
        {
            m_steerCoeff[i][j] = 0;
        }
    }

    memset( m_angle, 0, sizeof(m_angle) );
}

Driver::~Driver()
{
}

static void*	MergeParamFile( void* hParams, const char* fileName, bool relSrc = true )
{
    void*	hNewParams = GfParmReadFile(fileName, GFPARM_RMODE_STD);

    if( hNewParams == NULL )
    {
        LogSHADOW.debug("loaded: 'hParams' \n");
        return hParams;
    }

    if( hParams == NULL )
    {
        LogSHADOW.debug( "loaded: '%s'\n", fileName );
        return hNewParams;
    }

    //const char* path = SECT_GROBJECTS "/" LST_RANGES "/1";
    //const char* ac3d_car = GfParmGetStr(hNewParams, path, "car", "");

#if 0   // dead code
    float fw_ang = GfParmGetNum(hNewParams, "Front Wing", "angle", NULL, 0);
#endif

    GfParmCheckHandle( hParams, hNewParams );

    LogSHADOW.debug( "merging: '%s'\n", fileName );
    hParams = GfParmMergeHandles(hParams, hNewParams,
                                 GFPARM_MMODE_SRC | GFPARM_MMODE_DST |
                                 (relSrc ? GFPARM_MMODE_RELSRC : 0) | GFPARM_MMODE_RELDST);

    //const char* ac3d_car2 = GfParmGetStr(hParams, path, "car", "");
    return hParams;
}

static double	SafeParmGetNum( void *handle, const char *path, const char *key, const char *unit, tdble deflt )
{
    if( handle == NULL )
        return deflt;

    return GfParmGetNum(handle, path, key, unit, deflt);
}

void	Driver::SetShared( Shared* pShared )
{
    m_pShared = pShared;
}

// Called for every track change or new race.
void	Driver::InitTrack(
        int			index,
        tTrack*		pTrack,
        void*		pCarHandle,
        void**		ppCarParmHandle,
        tSituation*	pS )
{
    LogSHADOW.debug( "Shadow : initTrack()\n" );

    //
    //	get the name of the car (e.g. "clkdtm").
    //
    //	this is a sucky way to get this, but it's the only way I've managed
    //	to come up with so far.  it basically gets the name of the car .acc
    //	file, and strips off the ".acc" part, to get the name of the car.  yuk.
    //
    const char*	path = SECT_GROBJECTS "/" LST_RANGES "/" "1";
    const char*	key  = PRM_CAR;
    strncpy(m_carName, GfParmGetStr(pCarHandle, path, key, ""), 99);
    m_carName[99] = '\0';
    char*	p = strrchr(m_carName, '.');

    if( p )
        *p = '\0';

    LogSHADOW.info( " #Shadow carName: '%s'\n", m_carName );

    //
    //	get the name of the track (e.g. "e-track-1")
    //

    strncpy(m_trackName, strrchr(pTrack->filename, '/') + 1, 99);
    m_trackName[99] = '\0';
    *strrchr(m_trackName, '.') = '\0';
    LogSHADOW.info( " # Shadow trackName: '%s'\n", m_trackName );

    if (strcmp(m_trackName, "garage") == 0)
        m_garage = true;

    //
    //	set up race type array.
    //

    const char*	raceTypeStr[] = { "practice", "qualify", "race" };
    unsigned int raceType = pS->raceInfo.type;

    LogSHADOW.debug(" # Shadow Race type = %s \n", raceTypeStr[raceType]);

    //
    //	set up the base param path.
    //
    char	buf[1024];
    snprintf(buf, sizeof(buf), "drivers/%s", MyBotName);
    //const char*	baseParamPath = buf;

    weathercode = GetWeather(pTrack);
    LogSHADOW.info(" # Shadow weather code = %d\n\n", weathercode);

    // Get skill level
    SetRandomSeed(index);
    globalskill = driverskill = driver_aggression = 0.0;
    //SetRandomSeed(10);

    // load the global skill level, range 0 - 10
    snprintf(buf, sizeof(buf), "%sconfig/raceman/extra/skill.xml", GetLocalDir());
    void *skillHandle = GfParmReadFile(buf, GFPARM_RMODE_REREAD);

    if(!skillHandle)
    {
        snprintf(buf, sizeof(buf), "%sconfig/raceman/extra/skill.xml", GetDataDir());
        skillHandle = GfParmReadFile(buf, GFPARM_RMODE_REREAD);
    }//if !skillHandle

    if (skillHandle)
    {
        globalskill = GfParmGetNum(skillHandle, SECT_SKILL, PRV_SKILL_LEVEL, (char *) NULL, 30.0f);
    }

    globalskill = MAX(0.7, 1.0 - 0.5 * globalskill / 10.0);

    LogSHADOW.info(" # Global Skill: %.3f\n", globalskill);

    //load the driver skill level, range 0 - 1
    // float driver_skill = 0.0f;
    snprintf(buf, sizeof(buf), "drivers/%s/%d/skill.xml", MyBotName, INDEX);
    LogSHADOW.debug("Path skill driver: %s\n", buf);
    skillHandle = GfParmReadFile(buf, GFPARM_RMODE_STD);

    if (skillHandle)
    {
        driverskill = GfParmGetNum(skillHandle, SECT_SKILL, PRV_SKILL_LEVEL, (char *) NULL, 0.0);
        driver_aggression = GfParmGetNum(skillHandle, SECT_SKILL, PRV_SKILL_AGGRO, (char *)NULL, 0.0);
        double Rand2 = (double) (getRandom() / 65536.0) / 100;
        driverskill = MAX(0.95, 1.0 - 0.05 * (driverskill - Rand2));
        LogSHADOW.info(" # Global skill = %.2f - driver skill: %.8f - driver agression: %.2f\n", globalskill, driverskill, driver_aggression);
    }

    //
    //	ok, lets read/merge the car parms.
    //
    void*	hCarParm = pCarHandle;

    if (weathercode > 0)
    {
        // override params for default car type.
        snprintf( buf, sizeof(buf), "%sdrivers/%s/%s/default.xml",
                  GfDataDir(), MyBotName, m_carName);
        hCarParm = MergeParamFile(hCarParm, buf, hCarParm != pCarHandle);
        LogSHADOW.info("PATH = %s \n", buf);

        // override params for car type on track of specific race type and driver.
        snprintf( buf, sizeof(buf), "%sdrivers/%s/%s/%s-%d.xml",
                  GfDataDir(), MyBotName, m_carName, m_trackName, weathercode);
        hCarParm = MergeParamFile(hCarParm, buf, hCarParm != pCarHandle);
        LogSHADOW.info("PATH = %s \n", buf);
    }
    else
    {
        // override params for default car type.
        snprintf( buf, sizeof(buf), "%sdrivers/%s/%s/default.xml",
                  GfDataDir(), MyBotName, m_carName);
        hCarParm = MergeParamFile(hCarParm, buf, hCarParm != pCarHandle);
        LogSHADOW.info(" # Loaded default = %s \n", buf);

        // override params for car type on track of specific race type and driver.
        snprintf( buf, sizeof(buf), "%sdrivers/%s/%s/%s.xml",
                  GfDataDir(), MyBotName, m_carName, m_trackName);
        hCarParm = MergeParamFile(hCarParm, buf, hCarParm != pCarHandle);
        LogSHADOW.info(" # Loaded = %s \n", buf);
    }

    // setup the car param handle to be returned.

    *ppCarParmHandle = hCarParm;

    // get the private parameters now.
    //const char* ac3d_car = GfParmGetStr(hCarParm, SECT_GROBJECTS "/" LST_RANGES "/1", PRM_CAR, "");

    double revs_limiter = GfParmGetNum(hCarParm, SECT_ENGINE, PRM_REVSLIM, "rpm", 8000);
    double rpm = GfParmGetNum(hCarParm, SECT_PRIV, PRV_GEAR_UP_RPM, "rpm", revs_limiter * 0.974);
    // shift point must be less than rev limiter or car will not shift
    if (rpm > (revs_limiter - 10.0))
    {
        LogSHADOW.info( "*** gear up rpm changed from : %g to : %g\n", rpm, revs_limiter * 0.974 );
        rpm = revs_limiter * 0.974;
    }
    m_gearUpRpm = rpm * 2 * PI / 60;
    LogSHADOW.debug( "*** gear up rpm: %g (%g)\n", rpm, m_gearUpRpm );

    START_HOLD_LINE_TIME = SafeParmGetNum(hCarParm, SECT_PRIV, "start hold line time", 0, 5.0f);
    LogSHADOW.debug( "*** Start hold line time: %g \n\n", START_HOLD_LINE_TIME );

    MyTrack::SideMod	sideMod[N_PATHS];
    string sect;
    for( int p = 0; p < N_PATHS; p++ )
    {
        switch( p )
        {
        case PATH_NORMAL:	sect = SECT_PRIV;		break;
        case PATH_LEFT:		sect = SECT_PRIV_LEFT;	break;
        case PATH_RIGHT:	sect = SECT_PRIV_RIGHT;	break;
        }

        if( p == PATH_NORMAL )
        {
            m_priv[p].FACTORS.clear();
            m_cm[p].config( hCarParm );
        }
        else
        {
            m_priv[p] = m_priv[PATH_NORMAL];
            m_cm[p]   = m_cm[PATH_NORMAL];
        }

        m_cm[p].FLAGS = (int)SafeParmGetNum(hCarParm, sect.c_str(), PRV_CARMODEL_FLAGS, NULL, (tdble)m_cm[PATH_NORMAL].FLAGS);
        m_cm[p].MU_SCALE = SafeParmGetNum(hCarParm, sect.c_str(), PRV_MU_SCALE, NULL, m_cm[PATH_NORMAL].MU_SCALE);
        m_cm[p].SKILL = driverskill;
        m_cm[p].BRAKE_MU_SCALE = SafeParmGetNum(hCarParm, sect.c_str(), PRV_BRAKE_MU_SCALE, NULL, m_cm[PATH_NORMAL].BRAKE_MU_SCALE);
        m_cm[p].KZ_SCALE = SafeParmGetNum(hCarParm, sect.c_str(), PRV_KZ_SCALE, NULL, m_cm[PATH_NORMAL].KZ_SCALE);
        m_cm[p].KV_SCALE = SafeParmGetNum(hCarParm, sect.c_str(), PRV_KV_SCALE, NULL, m_cm[PATH_NORMAL].KV_SCALE);

        LogSHADOW.info( "*** FLAGS             : 0x%02X\n", m_cm[p].FLAGS );
        LogSHADOW.info( "*** BRAKE_MU_SCALE[%d]: %.3f\n", p, m_cm[p].BRAKE_MU_SCALE );
        LogSHADOW.info( "*** MU_SCALE[%d]      : %.3f\n", p, m_cm[p].MU_SCALE );
        LogSHADOW.info( "*** KZ_SCALE[%d]      : %.3f\n", p, m_cm[p].KZ_SCALE );
        LogSHADOW.info( "*** KV_SCALE[%d]      : %.3f\n\n", p, m_cm[p].KV_SCALE );

        for( int i = 0; ; i++ )
        {
            snprintf( buf, sizeof(buf), "%s %d", PRV_FACTOR, i );
            double	factor = SafeParmGetNum(hCarParm, sect.c_str(), buf, 0, -1);
            LogSHADOW.debug( "%s: %g\n", buf, factor );

            if( factor == -1 )
                break;

            if( i == 0 )
                m_priv[p].FACTORS.clear();

            m_priv[p].FACTORS.push_back( factor );
        }

        if( m_priv[p].FACTORS.size() == 0 )
            m_priv[p].FACTORS.push_back( 1.005 );

        for( int bend = 0; bend < 100; bend++ )
        {
            snprintf( buf, sizeof(buf), "%s %d", PRV_INNER_MOD, bend );
            double	innerModA = SafeParmGetNum(hCarParm, sect.c_str(), buf, 0, 999);
            double	innerModB = innerModA;
            innerModA = SafeParmGetNum(hCarParm, sect.c_str(), (string(buf) + "a").c_str(), 0, innerModA);
            innerModB = SafeParmGetNum(hCarParm, sect.c_str(), (string(buf) + "b").c_str(), 0, innerModA);

            if( innerModA == 999 && innerModB == 999 )
                continue;

            LogSHADOW.debug( "%s: %g, %g\n", buf, innerModA, innerModB );

            while (m_priv[p].INNER_MOD.size() < static_cast<size_t>(bend * 2 + 2))
                m_priv[p].INNER_MOD.push_back( 0 );

            if( innerModA != 999 )
                m_priv[p].INNER_MOD[bend * 2] = innerModA;
            if( innerModB != 999 )
                m_priv[p].INNER_MOD[bend * 2 + 1] = innerModB;
        }

        if( m_priv[p].INNER_MOD.size() == 0 )
            m_priv[p].INNER_MOD.push_back( 0.0 );

        m_priv[p].FLY_HEIGHT = SafeParmGetNum(hCarParm, sect.c_str(), PRV_FLY_HEIGHT, "m", m_priv[p].FLY_HEIGHT);
        m_priv[p].BUMP_MOD = int(SafeParmGetNum(hCarParm, sect.c_str(), PRV_BUMP_MOD, 0, (float)m_priv[p].BUMP_MOD));
        m_priv[p].APEX_FACTOR = SafeParmGetNum(hCarParm, sect.c_str(), PRV_APEX_FACTOR, 0, (float)m_priv[p].APEX_FACTOR);
        m_priv[p].SAVE_PATHS = SafeParmGetNum(hCarParm, sect.c_str(), PRV_SAVE_PATHS, 0, (float)m_priv[p].SAVE_PATHS) != 0;
        m_priv[p].QUAD_SMOOTH_ITERS = int(SafeParmGetNum(hCarParm, sect.c_str(), PRV_QUAD_SMOOTH_ITERS, 0, (float)m_priv[p].QUAD_SMOOTH_ITERS) + 0.5f);
        m_priv[p].SPDC_NORMAL = int(SafeParmGetNum(hCarParm, sect.c_str(), PRV_SPDC_NORMAL, 0, (float)m_priv[p].SPDC_NORMAL));
        m_priv[p].SPDC_TRAFFIC = int(SafeParmGetNum(hCarParm, sect.c_str(), PRV_SPDC_TRAFFIC, 0, (float)m_priv[p].SPDC_TRAFFIC));
        m_priv[p].ACC_MAX_SPIN_SPEED = SafeParmGetNum(hCarParm, sect.c_str(), PRV_ACC_MAX_SPIN_SPEED, 0, m_priv[p].ACC_MAX_SPIN_SPEED);
        m_priv[p].DEC_MAX_SPIN_SPEED = SafeParmGetNum(hCarParm, sect.c_str(), PRV_DEC_MAX_SPIN_SPEED, 0, m_priv[p].DEC_MAX_SPIN_SPEED);
        m_priv[p].AVOID_WIDTH = SafeParmGetNum(hCarParm, sect.c_str(), PRV_AVOID_WIDTH, 0, m_priv[p].AVOID_WIDTH);
        m_priv[p].STAY_TOGETHER = SafeParmGetNum(hCarParm, sect.c_str(), PRV_STAY_TOGETHER, 0, m_priv[p].STAY_TOGETHER);
        m_priv[p].STEER_K_ACC = SafeParmGetNum(hCarParm, sect.c_str(), PRV_STEER_K_ACC, 0, m_priv[p].STEER_K_ACC);
        m_priv[p].STEER_K_DEC = SafeParmGetNum(hCarParm, sect.c_str(), PRV_STEER_K_DEC, 0, m_priv[p].STEER_K_DEC);
        m_priv[p].PIT_ENTRY_OFFSET = SafeParmGetNum(hCarParm, sect.c_str(), PRV_PIT_ENTRY_OFFS, 0, m_priv[p].PIT_ENTRY_OFFSET);
        m_priv[p].PIT_EXIT_OFFSET = SafeParmGetNum(hCarParm, sect.c_str(), PRV_PIT_EXIT_OFFS, 0, m_priv[p].PIT_EXIT_OFFSET);
        m_priv[p].PIT_DAMAGE_WARN = (int)SafeParmGetNum(hCarParm, sect.c_str(), PRV_PIT_DAMAGE_WARN, 0, (float)m_priv[p].PIT_DAMAGE_WARN);
        m_priv[p].PIT_TIRE_WARN = SafeParmGetNum(hCarParm, sect.c_str(), PRV_PIT_TIRE_WARN, 0, 0.3);
        m_priv[p].PIT_DAMAGE_DANGER = (int)SafeParmGetNum(hCarParm, sect.c_str(), PRV_PIT_DAMAGE_DANGER, 0, (float)m_priv[p].PIT_DAMAGE_DANGER);
        m_priv[p].PIT_TIRE_DANGER = SafeParmGetNum(hCarParm, sect.c_str(), PRV_PIT_TIRE_DANGER, 0, 0.2);
        m_priv[p].SKID_FACTOR = SafeParmGetNum(hCarParm, sect.c_str(), PRV_SKID_FACTOR, 0, m_priv[p].SKID_FACTOR);
        m_priv[p].SKID_FACTOR_TRAFFIC = SafeParmGetNum(hCarParm, sect.c_str(), PRV_SKID_FACTOR_TRAFFIC, 0, m_priv[p].SKID_FACTOR_TRAFFIC);
        m_priv[p].REAR_LAT_SLIP_FACTOR = SafeParmGetNum(hCarParm, sect.c_str(), PRV_REAR_LAT_SLIP_FACTOR, 0, m_priv[p].REAR_LAT_SLIP_FACTOR);
        m_priv[p].REAR_LAT_SLIP_LIMIT = SafeParmGetNum(hCarParm, sect.c_str(), PRV_REAR_LAT_SLIP_LIMIT, 0, m_priv[p].REAR_LAT_SLIP_LIMIT);
        m_priv[p].REAR_LAT_SLIP_DSCALE = SafeParmGetNum(hCarParm, sect.c_str(), PRV_REAR_LAT_SLIP_DSCALE, 0, m_priv[p].REAR_LAT_SLIP_DSCALE);
        m_priv[p].STEER_0_LINE_SCALE = SafeParmGetNum(hCarParm, sect.c_str(), PRV_STEER_0_LINE_SCALE, 0, m_priv[p].STEER_0_LINE_SCALE);
        m_priv[p].TCL_TARGET_SPEED = SafeParmGetNum(hCarParm, sect.c_str(), PRV_TCL_TARGET_SPEED, 0, m_priv[p].TCL_TARGET_SPEED);
        m_priv[p].SAFETY_LIMIT = SafeParmGetNum(hCarParm, sect.c_str(), PRV_SAFETY_LIMIT, 0, m_priv[p].SAFETY_LIMIT);
        m_priv[p].SAFETY_MULTIPLIER = SafeParmGetNum(hCarParm, sect.c_str(), PRV_SAFETY_MULTIPLIER, 0, m_priv[p].SAFETY_MULTIPLIER);
        m_priv[p].BRAKE_LIMIT = SafeParmGetNum(hCarParm, sect.c_str(), PRV_BRAKE_LIMIT, 0, m_priv[p].BRAKE_LIMIT);
        m_priv[p].USE_SIDE_LIMIT = SafeParmGetNum(hCarParm, sect.c_str(), PRV_LIMIT_SIDE_USE, 0, m_priv[p].USE_SIDE_LIMIT);

        LogSHADOW.debug( "FLY_HEIGHT %g\n", m_priv[p].FLY_HEIGHT );
        LogSHADOW.debug( "BUMP_MOD %d\n\n", m_priv[p].BUMP_MOD );
        LogSHADOW.debug("USE SIDE LIMIT = %.2f\n", m_priv[p].USE_SIDE_LIMIT);

        sideMod[p].side = -1;

        if( hCarParm != NULL )
        {
            const char*	pStr = GfParmGetStr(hCarParm, sect.c_str(), PRV_SIDE_MOD, "");

            if( pStr == 0 || sscanf(pStr, "%d , %d , %d", &sideMod[p].side, &sideMod[p].start, &sideMod[p].end) != 3 )
            {
                sideMod[p].side = -1;
            }
        }

        LogSHADOW.debug( "SIDE MOD %d %d %d\n", sideMod[p].side, sideMod[p].start, sideMod[p].end );
        LogSHADOW.debug( "STAY_TOGETHER %g\n", m_priv[p].STAY_TOGETHER );
    }

    LogSHADOW.debug( "*** FLAGS: 0x%02X\n\n", m_cm[PATH_NORMAL].FLAGS );

    m_track.setWidth(m_priv[PATH_NORMAL].USE_SIDE_LIMIT);
    m_track.NewTrack( pTrack, &m_priv[PATH_NORMAL].INNER_MOD, false, &sideMod[PATH_NORMAL],
                      m_priv[PATH_NORMAL].PIT_START_BUF_SEGS);

    GfParmSetNum( hCarParm, SECT_CAR, NULL, (char*) NULL, NULL );

    m_Strategy.SetTyreLimits( m_priv[PATH_NORMAL].PIT_TIRE_WARN, m_priv[PATH_NORMAL].PIT_TIRE_DANGER);

    // override params for car type on track of specific race type.
    snprintf( buf, sizeof(buf), "%sdrivers/%s/%s/track-%s",
              GfDataDir(), MyBotName, m_carName, m_trackName );
    m_pathOffsets.setBaseFilename( buf );

    Meteorology(pTrack);
}

// Start a new race.
void	Driver::NewRace( int index, tCarElt* pCar, tSituation* pS )
{
    LogSHADOW.debug( "Shadow : newRace()\n" );

    m_nCars = pS->_ncars;
    m_myOppIdx = -1;

    for( int i = 0; i < m_nCars; i++ )
    {
        m_opp[i].Initialise( &m_track, pS->cars[i] );

        if( pS->cars[i] == pCar )
            m_myOppIdx = i;
    }

    pitsharing = CheckPitSharing(pCar);

    m_cm[PATH_NORMAL].config( pCar );
    m_cm[PATH_LEFT]  = m_cm[PATH_NORMAL];
    m_cm[PATH_RIGHT] = m_cm[PATH_NORMAL];

    LogSHADOW.info( "CA %g   CA_FW %g   CA_RW %g   CA_GE %g\n",
                    m_cm[PATH_NORMAL].CA, m_cm[PATH_NORMAL].CA_FW,
                    m_cm[PATH_NORMAL].CA_RW, m_cm[PATH_NORMAL].CA_GE );

    double gripScaleF = GripFactor(pCar, true);
    double gripScaleR = GripFactor(pCar, false);

    m_cm[PATH_NORMAL].GRIP_SCALE_F	= gripScaleF;
    m_cm[PATH_NORMAL].GRIP_SCALE_R	= gripScaleR;
    m_cm[PATH_LEFT].GRIP_SCALE_F	= gripScaleF;
    m_cm[PATH_LEFT].GRIP_SCALE_R	= gripScaleR;
    m_cm[PATH_RIGHT].GRIP_SCALE_F	= gripScaleF;
    m_cm[PATH_RIGHT].GRIP_SCALE_R	= gripScaleR;

    LogSHADOW.info( "CARMASS %g   TYRE_MU %g   TYRE_MU_F %g   TYRE_MU_R %g \n",
                    m_cm[PATH_NORMAL].MASS, m_cm[PATH_NORMAL].TYRE_MU,
                    m_cm[PATH_NORMAL].TYRE_MU_F, m_cm[PATH_NORMAL].TYRE_MU_R );
    LogSHADOW.info( "NORMAL: MU_SC %g   KZ_SCALE %g   FLY_HEIGHT %g\n",
                    m_cm[PATH_NORMAL].MU_SCALE, m_cm[PATH_NORMAL].KZ_SCALE, m_priv[PATH_NORMAL].FLY_HEIGHT );
    LogSHADOW.info( "LEFT:   MU_SC %g   KZ_SCALE %g   FLY_HEIGHT %g\n",
                    m_cm[PATH_LEFT].MU_SCALE, m_cm[PATH_LEFT].KZ_SCALE, m_priv[PATH_LEFT].FLY_HEIGHT );
    LogSHADOW.info( "RIGHT:  MU_SC %g   KZ_SCALE %g   FLY_HEIGHT %g\n",
                    m_cm[PATH_RIGHT].MU_SCALE, m_cm[PATH_RIGHT].KZ_SCALE, m_priv[PATH_RIGHT].FLY_HEIGHT );

    for( int p = PATH_LEFT; p <= PATH_RIGHT; p++ )
    {
        m_cm[p].CD_BODY	= m_cm[PATH_NORMAL].CD_BODY;
        m_cm[p].CD_WING	= m_cm[PATH_NORMAL].CD_WING;
        m_cm[p].WIDTH	= m_cm[PATH_NORMAL].WIDTH;

        m_cm[p].MASS	= m_cm[PATH_NORMAL].MASS;

        m_cm[p].CA		= m_cm[PATH_NORMAL].CA;
        m_cm[p].CA_FW	= m_cm[PATH_NORMAL].CA_FW;
        m_cm[p].CA_RW	= m_cm[PATH_NORMAL].CA_RW;
        m_cm[p].CA_GE	= m_cm[PATH_NORMAL].CA_GE;

        m_cm[p].TYRE_MU		= m_cm[PATH_NORMAL].TYRE_MU;
        m_cm[p].TYRE_MU_F	= m_cm[PATH_NORMAL].TYRE_MU_F;
        m_cm[p].TYRE_MU_R	= m_cm[PATH_NORMAL].TYRE_MU_R;
    }

    if( m_pShared->m_path[PATH_NORMAL].GetOptions().factors != m_priv[PATH_NORMAL].FACTORS ||
            m_pShared->m_path[PATH_LEFT].GetOptions().factors   != m_priv[PATH_LEFT].FACTORS ||
            m_pShared->m_path[PATH_RIGHT].GetOptions().factors  != m_priv[PATH_RIGHT].FACTORS ||
            m_pShared->m_pTrack != m_track.GetTrack() )
    {
        if( m_pShared->m_pTrack != m_track.GetTrack() )
        {
            m_pShared->m_pTrack = m_track.GetTrack();
            m_pShared->m_teamInfo.Empty();
        }

        LogSHADOW.debug( "Generating smooth paths...\n" );

        PathOptions options(m_priv[PATH_NORMAL].BUMP_MOD,
                            m_priv[PATH_NORMAL].SAFETY_LIMIT,
                            m_priv[PATH_NORMAL].SAFETY_MULTIPLIER);

        options.apexFactor = m_priv[PATH_NORMAL].APEX_FACTOR,
                options.factors = m_priv[PATH_NORMAL].FACTORS;

        options.quadSmoothIters = m_priv[PATH_NORMAL].QUAD_SMOOTH_ITERS;

        m_pShared->m_path[PATH_NORMAL].MakeSmoothPath( &m_track, m_cm[PATH_NORMAL], options);
        LogSHADOW.debug("m_pShared->m_path passed !\n");

        const double car_margin = m_track.GetWidth() / 2 - 4;

        options = PathOptions(m_priv[PATH_LEFT].BUMP_MOD,
                              m_priv[PATH_LEFT].SAFETY_LIMIT,
                              m_priv[PATH_LEFT].SAFETY_MULTIPLIER, 999, car_margin);

        options.apexFactor = m_priv[PATH_LEFT].APEX_FACTOR,
                options.factors = m_priv[PATH_LEFT].FACTORS;

        options.quadSmoothIters = m_priv[PATH_LEFT].QUAD_SMOOTH_ITERS;

        m_pShared->m_path[PATH_LEFT].MakeSmoothPath( &m_track, m_cm[PATH_LEFT], options);

        options = PathOptions(m_priv[PATH_RIGHT].BUMP_MOD,
                              m_priv[PATH_RIGHT].SAFETY_LIMIT,
                              m_priv[PATH_RIGHT].SAFETY_MULTIPLIER, car_margin, 999);

        options.apexFactor = m_priv[PATH_RIGHT].APEX_FACTOR,
                options.factors = m_priv[PATH_RIGHT].FACTORS;

        options.quadSmoothIters = m_priv[PATH_RIGHT].QUAD_SMOOTH_ITERS;

        m_pShared->m_path[PATH_RIGHT].MakeSmoothPath( &m_track, m_cm[PATH_RIGHT], options);
    }

    m_path[PATH_NORMAL] = m_pShared->m_path[PATH_NORMAL];

    {
        char	buf[1024];
        snprintf( buf, sizeof(buf),"%sdrivers/%s/%s/track-%s.spr", GfDataDir(), MyBotName, m_carName, m_trackName );

        bool loadedOk = m_path[PATH_NORMAL].LoadPath(buf);
        LogSHADOW.debug( "loaded springs data: %s\n", loadedOk ? "true" : "false" );
    }
    m_path[PATH_NORMAL].CalcMaxSpeeds( m_cm[PATH_NORMAL] );
    m_path[PATH_NORMAL].PropagateBraking( m_cm[PATH_NORMAL] );

    //#define BRAKING_TEST
#if defined(DEV) && defined(BRAKING_TEST)
    // code for track Tim Test 1
    m_path[PATH_NORMAL].GenMiddle();
    m_path[PATH_NORMAL].CalcMaxSpeeds( m_cm[PATH_NORMAL] );
    m_path[PATH_NORMAL].GetAt(700).maxSpd = m_path[PATH_NORMAL].GetAt(700).spd = 0;
    m_path[PATH_NORMAL].PropagateBraking(m_cm[PATH_NORMAL] );
#endif

    m_path[PATH_LEFT] = m_pShared->m_path[PATH_LEFT];
    m_path[PATH_LEFT].CalcMaxSpeeds( m_cm[PATH_LEFT] );
    m_path[PATH_LEFT].PropagateBraking( m_cm[PATH_LEFT] );

    m_path[PATH_RIGHT] = m_pShared->m_path[PATH_RIGHT];
    m_path[PATH_RIGHT].CalcMaxSpeeds( m_cm[PATH_RIGHT] );
    m_path[PATH_RIGHT].PropagateBraking( m_cm[PATH_RIGHT] );

    for( int p = PATH_NORMAL; p <= PATH_RIGHT; p++ )
    {
        for( int i = 0; i < 2; i++ )
        {
            CarModel	pitCm(m_cm[p]);
            pitCm.BRAKE_MU_SCALE = 0.8;
            m_pitPath[p][i].MakePath( pCar->race.pit, &m_path[p], pitCm, i == 1,
                                      m_priv[p].PIT_ENTRY_OFFSET, m_priv[p].PIT_EXIT_OFFSET );
        }
    }

    {
        // get name of car (e.g. 'car5-trb1')
        const char*	path = SECT_GROBJECTS "/" LST_RANGES "/" "1";
        const char*	key  = PRM_CAR;
        char	carName[256];
        strncpy(carName, GfParmGetStr(pCar->_carHandle, path, key, ""), 255);
        carName[255] = '\0';
        char* p = strrchr(carName, '.');
        if( p )
            *p = '\0';

        //	get the name of the track (e.g. "e-track-1")
        char	trackName[256];
        strncpy(trackName, strrchr(m_track.GetTrack()->filename, '/') + 1, 255);
        trackName[255] = '\0';
        *strrchr(trackName, '.') = '\0';

#if 0   // dead code
        //	set up the base param path.
        char	baseParamPath[] = "drivers/shadow";
#endif
        //snprintf( buf, sizeof(buf), "%s/%s/track-%s-generated.spr", MyBotName, carName, trackName );

        //	save the generated racing line.
        if( m_priv[PATH_NORMAL].SAVE_PATHS )
        {
            char	buf[1024];
            snprintf( buf, sizeof(buf), "%s/%s/track-%s-generated.spr",
                      MyBotName, carName, trackName );
            m_path[PATH_NORMAL].SavePath( buf );
        }
    }

    const char* traintype = GfParmGetStr(pCar->_carHandle,
                                         SECT_DRIVETRAIN, PRM_TYPE, VAL_TRANS_RWD);
    m_driveType = cDT_RWD;

    if( strcmp(traintype, VAL_TRANS_RWD) == 0 )
        m_driveType = cDT_RWD;
    else if( strcmp(traintype, VAL_TRANS_FWD) == 0 )
        m_driveType = cDT_FWD;
    else if( strcmp(traintype, VAL_TRANS_4WD) == 0 )
        m_driveType = cDT_4WD;

    m_flying = 0;
    m_raceStart = true;

    m_avoidS = 0;
    m_avoidT = 0;

    m_accBrkCoeff.Clear();
    m_accBrkCoeff.Sample( 0, 0 );
    m_accBrkCoeff.Sample( 1, 0.85 );

    TeamInfo::Item*	pItem = new TeamInfo::Item();
    pItem->index = pCar->index;
    pItem->usingPit = false;
    pItem->pOther = 0;
    pItem->pCar = pCar;

    m_pShared->m_teamInfo.Add( pCar->index, pItem );
}

bool	Driver::Pitting( int path, double pos ) const
{
    return	m_Strategy.WantToPit() &&
            m_pitPath[path][m_Strategy.PitType()].ContainsPos(pos);
}

bool	Driver::Pitting( tCarElt* car ) const
{
    double	pos = m_track.CalcPos(car);
    return Pitting(PATH_NORMAL, pos);
}

void	Driver::GetPtInfo( int path, double pos, PtInfo& pi ) const
{

    if( Pitting(path, pos) )
        m_pitPath[path][m_Strategy.PitType()].GetPtInfo( pos, pi );
    else
        m_path[path].GetPtInfo( pos, pi );
}

void	InterpPtInfo( PtInfo& pi0, const PtInfo& pi1, double t )
{
    pi0.k	= Utils::InterpCurvature(pi0.k, pi1.k, t);
    double	deltaOAng = pi1.oang - pi0.oang;
    NORM_PI_PI(deltaOAng);
    pi0.oang = pi0.oang + deltaOAng * t;
    pi0.offs = pi0.offs * (1 - t) + pi1.offs * t;
    pi0.spd	 = pi0.spd  * (1 - t) + pi1.spd  * t;
    pi0.acc  = pi0.acc  * (1 - t) + pi1.acc  * t;
}

void	Driver::GetPosInfo(
        double		pos,
        PtInfo&		pi,
        double		u,
        double		v ) const
{
    GetPtInfo( PATH_NORMAL, pos, pi );

    PtInfo	piL, piR;

    if( u != 1 )
    {
        GetPtInfo( PATH_LEFT,  pos, piL );
        GetPtInfo( PATH_RIGHT, pos, piR );

        double	s = u;
        double	t = (v + 1) * 0.5;

        InterpPtInfo( piL, pi, s );
        InterpPtInfo( piR, pi, s );

        pi = piL;

        InterpPtInfo( pi, piR, t );
    }
}

void	Driver::GetPosInfo(
        double		pos,
        PtInfo&		pi ) const
{
    GetPosInfo(pos, pi, m_avoidS, m_avoidT);
}

double	Driver::CalcPathTarget( double pos, double offs, double s ) const
{
    PtInfo	pi, piL, piR;
    GetPtInfo( PATH_NORMAL, pos, pi );
    GetPtInfo( PATH_LEFT, pos, piL );
    GetPtInfo( PATH_RIGHT, pos, piR );

    InterpPtInfo( piL, pi, s );
    InterpPtInfo( piR, pi, s );

    double	t = (offs - piL.offs) / (piR.offs - piL.offs);

    return MX(-1, MN(t, 1)) * 2 - 1;
}

double	Driver::CalcPathTarget( double pos, double offs ) const
{
    return CalcPathTarget(pos, offs, m_avoidS);
}

Vec2d	Driver::CalcPathTarget2( double pos, double offs ) const
{
    PtInfo	pi, piL, piR;
    GetPtInfo( PATH_NORMAL, pos, pi );
    GetPtInfo( PATH_LEFT, pos, piL );
    GetPtInfo( PATH_RIGHT, pos, piR );

    double	s = m_avoidS;

    InterpPtInfo( piL, pi, s );
    InterpPtInfo( piR, pi, s );

    double	t = (offs - piL.offs) / (piR.offs - piL.offs);

    return Vec2d(MX(-1, MN(t, 1)) * 2 - 1, 1);
}

double	Driver::CalcPathOffset( double pos, double s, double t ) const
{
    PtInfo	pi, piL, piR;
    GetPtInfo( PATH_NORMAL, pos, pi );
    GetPtInfo( PATH_LEFT, pos, piL );
    GetPtInfo( PATH_RIGHT, pos, piR );

    InterpPtInfo( piL, pi, s );
    InterpPtInfo( piR, pi, s );

    InterpPtInfo( piL, piR, (t + 1) * 0.5 );

    return piL.offs;
}

void	Driver::CalcBestPathUV( double pos, double offs, double& u, double& v ) const
{
    PtInfo	pi, piL, piR;
    GetPtInfo( PATH_NORMAL, pos, pi );

    if( fabs(offs - pi.offs) < 0.01 )
    {
        u = 1;
        v = 0;
        return;
    }

    GetPtInfo( PATH_LEFT,  pos, piL );
    GetPtInfo( PATH_RIGHT, pos, piR );

    double	doffs = offs - pi.offs;
    if( doffs < 0 )
    {
        double	den = piL.offs - pi.offs;
        if( fabs(den) > 0.001 )
            u = 1 - MN(1, doffs / den);
        else
            u = 0;
        v = -1;
    }
    else
    {
        double	den = piR.offs - pi.offs;
        if( fabs(den) > 0.001 )
            u = 1 - MN(1, doffs / den);
        else
            u = 0;
        v = 1;
    }
}

double	Driver::CalcBestSpeed( double pos, double offs ) const
{
    double	u, v;
    CalcBestPathUV( pos, offs, u, v );

    PtInfo	pi;
    GetPosInfo( pos, pi, u, v );

    return pi.spd;
}

void	Driver::GetPathToLeftAndRight( const CarElt* pCar, double& toL, double& toR ) const
{
    double	pos = pCar->_distFromStartLine;
    double	offs = -pCar->_trkPos.toMiddle;

    PtInfo	pi;
    GetPtInfo( PATH_LEFT, pos, pi );
    toL = -(pi.offs - offs);
    GetPtInfo( PATH_RIGHT, pos, pi );
    toR = pi.offs - offs;
}

double	Driver::GripFactor( const CarElt* pCar, bool front ) const
{
    //const double initialTemperature	= 273.15 + 20.0;

    double	gripFactor = 1.0;

    if(m_cm[PATH_NORMAL].HASTYC)
    {
        if(front)
            gripFactor = m_cm[PATH_NORMAL].GRIP_SCALE_F;
        else
        {
            gripFactor = m_cm[PATH_NORMAL].GRIP_SCALE_R;
        }
    }

    return gripFactor;
}

double	CalcMaxSlip( double fRatio )
{
    return 0;
}

double	Driver::SteerAngle0( tCarElt* car, PtInfo& pi, PtInfo& aheadPi, const Private& priv )
{
    // work out current car speed.
    double	spd0 = hypot(car->_speed_x, car->_speed_y);

    // get current pos on track.
    double	pos = m_track.CalcPos(car);

    GetPosInfo( pos, pi );

    double	delta = pi.offs + car->_trkPos.toMiddle;
    double	aheadDist = car->_dimension_x * 0.5 + spd0 * 0.02;
    double	aheadPos = m_track.CalcPos(car, aheadDist);

    // get info about pts on track.
    GetPosInfo( aheadPos, aheadPi );

    PtInfo	piOmega;
    double	aheadOmega = car->_dimension_x * 0.5 + spd0 * 0.02;// * 10;
    double	aheadOmegaPos = m_track.CalcPos(car, aheadOmega);
    GetPosInfo( aheadOmegaPos, piOmega );

    // work out basic steering angle (that aligns the front wheels parallel to the racing line).
    // double	velAng = atan2(car->_speed_Y, car->_speed_X);
    double	angle = aheadPi.oang - car->_yaw;
    NORM_PI_PI(angle);
    double	basicAngle = angle;

    // control rotational velocity.
    double	avgK = (pi.k + piOmega.k) * 0.5;
    double	targetOmega = car->_speed_x * avgK;//aheadPi.k;
    double	o2 = (aheadPi.k - pi.k) * spd0 / aheadDist;
    static PidController	yawCtrl;
    yawCtrl.m_p = 0.12;
    yawCtrl.m_d = 0.012;

    double omegaAngle = 0.08 * (targetOmega - car->_yaw_rate);
    angle += omegaAngle;

    double omegaAngle2 = o2 * 0.08;
    angle += omegaAngle2;

    double	accDecAngle = 0;
    if( car->_accel_x > 0 )
        accDecAngle = avgK * m_priv[PATH_NORMAL].STEER_K_ACC;
    else
        accDecAngle = avgK * m_priv[PATH_NORMAL].STEER_K_DEC;
    angle += accDecAngle;

    // control offset from path.
    m_lineControl.m_p = 1.0;
    m_lineControl.m_d = 10;

    const double SC = priv.STEER_0_LINE_SCALE;
    double lineAngle = SC * -atan(m_lineControl.Sample(delta));

    angle += lineAngle;

    double	frontSlipSide = (car->priv.wheel[0].slipSide  + car->priv.wheel[1].slipSide)  / 2;
    if( fabs(frontSlipSide) > 8 )
        LogSHADOW.debug( "slip: front(tan=%7.3f side=%7.3f) rear(tan=%7.3f side=%7.3f) acc(tan=%7.3f side=%7.3f)  steer=%g\n",
                         (car->priv.wheel[0].slipAccel + car->priv.wheel[1].slipAccel) / 2,
                frontSlipSide,
                (car->priv.wheel[2].slipAccel + car->priv.wheel[3].slipAccel) / 2,
                (car->priv.wheel[2].slipSide  + car->priv.wheel[3].slipSide)  / 2,
                car->pub.DynGC.acc.x, car->pub.DynGC.acc.y, angle * 180 / PI);

    const double steerHardFactor = 0.5;
    bool steeringHard = fabs(angle) > car->info.steerLock * steerHardFactor;
    bool limitSteering =	fabs(frontSlipSide) > 6 &&
            steeringHard &&
            frontSlipSide * angle < 0;
    if( limitSteering )
    {
        angle = (angle < 0 ? -car->info.steerLock : car->info.steerLock) * steerHardFactor;
    }

    LogSHADOW.debug( "%4d o=%6.2f a=%6.3f ba=%6.3f oa=%6.3f oa2=%6.3f ada=%6.3f la=%6.3f %g,%g\n",
                     pi.idx, delta, angle, basicAngle, omegaAngle, omegaAngle2, accDecAngle, lineAngle,
                     m_avoidS, m_avoidT );

    return angle;
}

double	Driver::SteerAngle1( tCarElt* car, PtInfo& carPi, PtInfo& aheadPi )
{
    tTrkLocPos	trkPos;
    PtInfo		axlePi, ahead2Pi;

    // calc position of car along track.
    double	carPos			= m_track.CalcPos(car);
    // double	carToMiddle		= car->pub.trkPos.toMiddle;

    // calc position of centre of fron axle along track.
    double	midPt			= m_cm[PATH_NORMAL].F_AXLE_X;
    double	axleX			= car->pub.DynGCg.pos.x + midPt * cos(car->_yaw);
    double	axleY			= car->pub.DynGCg.pos.y + midPt * sin(car->_yaw);
    RtTrackGlobal2Local(car->_trkPos.seg, axleX, axleY, &trkPos, 0);
    double	axleToMiddle	= trkPos.toMiddle;
    double	axlePos			= m_track.CalcPos(trkPos);

    // work out current car speed.
    double	spd0 = hypot(car->_speed_X, car->_speed_Y);

    // calc axle position four timesteps ahead.
    double	DT = 0.02 * 4;	// simulation time step.
    double	aheadX			= axleX + car->_speed_X * DT;
    double	aheadY			= axleY + car->_speed_Y * DT;
    RtTrackGlobal2Local(car->_trkPos.seg, aheadX, aheadY, &trkPos, 0);
    double	aheadPos		= m_track.CalcPos(trkPos);

    // calc axle position eight timesteps ahead.
    double	DT2 = 0.02 * 8;	// simulation time step.
    double	ahead2X			= axleX + car->_speed_X * DT2;
    double	ahead2Y			= axleY + car->_speed_Y * DT2;
    RtTrackGlobal2Local(car->_trkPos.seg, ahead2X, ahead2Y, &trkPos, 0);
    double	ahead2Pos		= m_track.CalcPos(trkPos);

    // get info about the pts on track.
    GetPosInfo( carPos, carPi );
    GetPosInfo( axlePos, axlePi );
    GetPosInfo( aheadPos, aheadPi );
    GetPosInfo( ahead2Pos, ahead2Pi );

    // basic steering angle to put front wheels parallel to the racing line.
    double	angle = axlePi.oang - car->_yaw;
    NORM_PI_PI(angle);

    double	avgK = (axlePi.k + aheadPi.k + ahead2Pi.k) / 3;
    double	accDecAngle = 0;

    if( car->_accel_x > 0 )
        accDecAngle = avgK * m_priv[PATH_NORMAL].STEER_K_ACC;
    else
        accDecAngle = avgK * m_priv[PATH_NORMAL].STEER_K_DEC;

    angle += accDecAngle;

    double omega = spd0 * avgK;//aheadPi.k;

    angle += 0.15 * (omega - car->_yaw_rate);
    angle += omega * 0.15;

    double	velAng = atan2(car->_speed_Y, car->_speed_X);
    double	velAngCtrl = carPi.oang - velAng;
    NORM_PI_PI(velAngCtrl);
    m_velAngControl.m_p = 0.5;
    m_velAngControl.m_d = 5;
    velAngCtrl = m_velAngControl.Sample(velAngCtrl);
    angle += tanh(velAngCtrl);

    // control offset from path.
    m_lineControl.m_p = 0.6;
    m_lineControl.m_d = 10;
    m_lineControl.m_i = 0;//0.02;
    m_lineControl.m_totalRate = 0;
    m_lineControl.m_maxTotal = 2;
    static double avgLineAngle = 0;
    const double SC = 0.15;

    double	delta = axlePi.offs + axleToMiddle;
    double lineAngle = SC * tanh(m_lineControl.Sample(delta));
    const double avgLineAngleFactor = 1;
    avgLineAngle = avgLineAngle * (1 - avgLineAngleFactor) + lineAngle * avgLineAngleFactor;
    angle -= avgLineAngle;

    return angle;
}

double	Driver::SteerAngle2( tCarElt* car, PtInfo& pi, PtInfo& aheadPi )
{
    // work out current car speed.
    double	spd0 = hypot(car->_speed_x, car->_speed_y);

    // calc x,y coords of mid point on frt axle.
    double	midPt = 1.37;

    double	x = car->pub.DynGCg.pos.x + midPt * cos(car->_yaw);
    double	y = car->pub.DynGCg.pos.y + midPt * sin(car->_yaw);

    // static double	oldX = x;
    // static double	oldY = y;
    // double	velX = (x - oldX) / 0.02;
    // double	velY = (y - oldY) / 0.02;
    // oldX = x;
    // oldY = y;

    tTrkLocPos	trkPos;
    RtTrackGlobal2Local(car->_trkPos.seg, x, y, &trkPos, 0);
    double	toMiddle = trkPos.toMiddle;

    // get current pos on track.
    double	pos = m_track.CalcPos(trkPos);

    // look this far ahead.
    double	aheadDist = spd0 * 0.02;
    double	aheadPos = m_track.CalcPos(trkPos, aheadDist);

    // get info about pts on track.
    GetPosInfo( pos, pi );
    GetPosInfo( aheadPos, aheadPi );

    double	angle = aheadPi.oang - car->_yaw;
    NORM_PI_PI(angle);

    double	velAng = atan2(car->_speed_Y, car->_speed_X);
    double	velAngCtrl = aheadPi.oang - velAng;
    NORM_PI_PI(velAngCtrl);
    m_velAngControl.m_p = 1;//0.5;//1;
    m_velAngControl.m_d = 10;//25;
    velAngCtrl = m_velAngControl.Sample(velAngCtrl);

    angle += tanh(velAngCtrl);

    double	avgK = aheadPi.k;
    double omega = car->_speed_x * avgK;
    angle += 0.02 * (omega - car->_yaw_rate);

    // control offset from path.
    m_lineControl.m_p = 1.0;
    m_lineControl.m_d = 10;
    const double SC = 0.15;
    double	delta = pi.offs + toMiddle;
    angle -= SC * tanh(m_lineControl.Sample(delta));

    return angle;
}

double	Driver::SteerAngle3( tCarElt* car, PtInfo& pi, PtInfo& aheadPi )
{
    // work out current car speed.
    double	spd0 = hypot(car->_speed_x, car->_speed_y);

    // get curret pos on track.
    double	pos = m_track.CalcPos(car);

    // look this far ahead.
    double	aheadTime = 0.2;//5;
    double	aheadDist = spd0 * aheadTime;

    double	aheadPos = m_track.CalcPos(car, aheadDist);

    // get info about pts on track.
    GetPosInfo( pos, pi );
    GetPosInfo( aheadPos, aheadPi );

    // we are trying to control 4 things with the steering...
    //	1. the distance of the car from the driving line it is to follow.
    //	2. the gradient of the distance of car from driving line.
    //	3. the angle of the car (yaw).
    //	4. the rotation speed (omega) of the car.

    // we want the angle of the car to be aheadPi.oang in aheadTime seconds.
    //	our current rotation speed is car->_yaw_rate.  our current rotation

    // current yaw rate.
    double	yawU = car->_yaw_rate;

    // future yaw rate required ahead.
    // double	yawV = aheadPi.k * spd0;

    // future yaw required ahead (assuming current yaw to be 0).
    double	yawS = aheadPi.oang - car->_yaw;
    NORM_PI_PI(yawS);

    // acceleration to apply.
    double	yawA = 2 * (yawS - yawU * aheadTime) / (aheadTime * aheadTime);
    double	yaw1s = yawU + 0.5 * yawA;

    // angle to steer to get desired yaw angle after timestep.
    double	dist1s = spd0;
    double	len = 2.63;	// dist between front/back wheel centres.
    double	radiusRear = dist1s / yaw1s;
    double	angle = atan(len / radiusRear);

    if( spd0 < 1 )
        angle = 0;

    // control offset from path.
    const double SC1 = 1;
    m_lineControl.m_p = SC1 * 0.25;	// 1.0 == oscillates
    m_lineControl.m_d = SC1 * 2.5;	// 9.5 == oscillates
    double	delta = pi.offs + car->_trkPos.toMiddle;
    const double SC2 = 1.0 / SC1;
    angle -= SC2 * atan(m_lineControl.Sample(delta));

    double	frontSlipSide = (car->priv.wheel[0].slipSide  + car->priv.wheel[1].slipSide)  / 2;
    if( fabs(frontSlipSide) > 8 )
        LogSHADOW.debug( "slip: front(tan=%7.3f side=%7.3f) rear(tan=%7.3f side=%7.3f) acc(tan=%7.3f side=%7.3f)  steer=%g\n",
                         (car->priv.wheel[0].slipAccel + car->priv.wheel[1].slipAccel) / 2,
                frontSlipSide,
                (car->priv.wheel[2].slipAccel + car->priv.wheel[3].slipAccel) / 2,
                (car->priv.wheel[2].slipSide  + car->priv.wheel[3].slipSide)  / 2,
                car->pub.DynGC.acc.x, car->pub.DynGC.acc.y, angle * 180 / PI);

    {
        double  zforce  = car->_reaction[0];
        double  s       = car->_skid[0] / (zforce * 0.0002f);
        // double  stmp	= MIN(s, 1.5f);
        double  sv      = hypot(car->_wheelSlipSide(0), car->_wheelSlipAccel(0));
        double  v       = sv / s;
        double  sx      = car->_wheelSlipAccel(0) / v;
        double  sy      = car->_wheelSlipSide(0) / v;
        // double  sa      = asin(sy);

        // double  wrl     = car->_wheelSpinVel(0) * car->_wheelRadius(0);

        if( s > m_cm[PATH_NORMAL].TARGET_SLIP + 0.0004 )
            LogSHADOW.debug( "acc %6.2f  zf %6.1f  s %.6f  v %6.2f  sx %.6f  sy %.6f\n",
                             car->pub.DynGC.acc.x, zforce, s, v, sx, sy );
    }

    const double steerHardFactor = 0.3;
    bool steeringHard = fabs(angle) > car->info.steerLock * steerHardFactor;
    bool limitSteering =	fabs(frontSlipSide) > 6 &&
            steeringHard &&
            frontSlipSide * angle < 0;
    if( limitSteering )
    {
        angle = (angle < 0 ? -car->info.steerLock : car->info.steerLock) * steerHardFactor;
    }

    return angle;
}

double	Driver::SteerAngle4( tCarElt* car, PtInfo& pi, PtInfo& aheadPi )
{
    // work out current car speed.
    double	spd0 = hypot(car->_speed_x, car->_speed_y);

    // get curret pos on track.
    double	pos = m_track.CalcPos(car);

    // look this far ahead.
    double	aheadDist = car->_dimension_x * 0.5 + spd0 * 0.02;
    double	aheadPos = m_track.CalcPos(car, aheadDist);

    // get info about pts on track.
    GetPosInfo( pos, pi );
    GetPosInfo( aheadPos, aheadPi );

    //
    //	deal with yaw.
    //
    double	yawError = aheadPi.oang - car->_yaw;
    NORM_PI_PI(yawError);
    double	yawERate = car->pub.DynGC.vel.az;

    // PID with no integration term.
    const double Y_PROP = 0.1;
    const double Y_DIFF = 2.5;
    const double Y_SC = 1;
    double angle = Y_SC * atan((Y_PROP * yawError + Y_DIFF * yawERate) / Y_SC);

    //
    //	deal with line.
    //
    double	lineError = -(pi.offs + car->_trkPos.toMiddle);
    double	lineERate = lineError - m_prevLineError;
    m_prevLineError = lineError;

    // PID with no integration term.
    const double L_PROP = 0;
    const double L_DIFF = 0;
    const double L_SC = 0.15;
    angle += L_SC * atan((L_PROP * lineError + L_DIFF * lineERate) / L_SC);

    return angle;
}

void	Driver::SpeedControl0(
        double	targetSpd,
        double	spd0,
        double&	acc,
        double&	brk )
{
    if( m_lastBrk && m_lastTargV )
    {
        if( m_lastBrk > 0 )
        {
            double	err = m_lastTargV - spd0;
            m_accBrkCoeff.Sample( err, m_lastBrk );
        }

        m_lastBrk = 0;
        m_lastTargV = 0;
    }

    if( spd0 - 0.25 > targetSpd )
    {
        if( spd0 > targetSpd )
        {

            acc = 0.0;
            brk = spd0 < 50 ? 0.5 : 0.6;
        }
        else
        {
            if( targetSpd > 1 )
            {
                // slow naturally.
                acc = MN(acc, 0.1);
            }
            else
            {
                acc = 0;
                brk = 0.1;
            }
        }
    }
    else
    {
        double x = (10 + spd0) * (targetSpd - spd0) / 20;
        if (x > 0)
            acc = x;
    }

    m_lastBrk = brk;
    m_lastTargV = 0;
}

void	Driver::SpeedControl5(
        double	targetSpd,
        double	spd0,
        double	targetAcc,
        double	acc0,
        double	slip0,
        double&	acc,
        double&	brk,
        bool	traffic )
{
    double ta = MN(m_brk.adjAccForSpdError(targetAcc, spd0, targetSpd), 0);
    if( ta < 0 )
    {
        if( spd0 > targetSpd )
        {
            m_brk.execute5( acc0, ta, slip0, m_cm[PATH_NORMAL].TARGET_SLIP, m_cm[PATH_NORMAL].MAX_SLIP,
                            m_priv[PATH_NORMAL].BRAKE_LIMIT, traffic );
            acc = 0.0;
            brk = m_brk.targetBrk;
        }
        else
        {
            m_brk.clear();
            if( targetSpd > 1 )
            {
                // slow naturally.
                acc = MN(acc, traffic ? 0.1 : 0.11 * (targetSpd - spd0));
            }
            else
            {
                acc = 0;
                brk = 0.1;
            }
        }
    }
    else
    {
        m_brk.clear();
        double x = (10 + spd0) * (targetSpd - spd0) / 20;
        if (x > 0)
            acc = MN(x, 1);
    }
}

void	Driver::SpeedControl6(
        double	targetSpd,
        double	spd0,
        double	targetAcc,
        double	acc0,
        double	slip0,
        double&	acc,
        double&	brk,
        bool	traffic )
{
    if (spd0 > targetSpd || (spd0 + 0.5 > targetSpd && targetAcc < -5))
    {
        double ta = MN(m_brk.adjAccForSpdError(targetAcc, spd0, targetSpd), 0);

        m_brk.execute6( acc0, ta, slip0, m_cm[PATH_NORMAL].TARGET_SLIP, m_cm[PATH_NORMAL].MAX_SLIP,
                        m_priv[PATH_NORMAL].BRAKE_LIMIT, traffic );

        acc = 0.0;
        brk = m_brk.targetBrk;
    }
    else
    {
        m_brk.clear();
        double x = (10 + spd0) * (targetSpd - spd0) / 20;
        if (x > 0)
            acc = MN(x, 1);
    }

    m_lastBrk = brk;
    m_lastTargV = 0;
}

void	Driver::SpeedControl7(
        double	targetSpd,
        double	spd0,
        double	targetAcc,
        double	acc0,
        double	xslip0,
        double	slip0,
        double&	acc,
        double&	brk,
        bool	traffic )
{
    double ta = MN(m_brk.adjAccForSpdError(targetAcc, spd0, targetSpd), 0);
    if( ta < 0 )
    {
        if( spd0 > targetSpd )
        {
            m_brk.execute7( acc0, ta, xslip0, slip0, m_cm[PATH_NORMAL].TARGET_SLIP, m_cm[PATH_NORMAL].MAX_SLIP,
                            m_priv[PATH_NORMAL].BRAKE_LIMIT, traffic );
            acc = 0.0;
            brk = m_brk.targetBrk;
        }
        else
        {
            m_brk.clear();
            if( targetSpd > 1 )
            {
                // slow naturally.
                acc = MN(acc, traffic ? 0.1 : 0.11 * (targetSpd - spd0));
            }
            else
            {
                acc = 0;
                brk = 0.1;
            }
        }
    }
    else
    {
        m_brk.clear();
        double x = (10 + spd0) * (targetSpd - spd0) / 20;
        if (x > 0)
            acc = MN(x, 1);
    }
}

void	Driver::SpeedControl8(
        double	targetSpd,
        double	spd0,
        double	targetAcc,
        double	acc0,
        double	fslip0,
        double	rslip0,
        double&	acc,
        double&	brk,
        bool	traffic )
{
    double ta = MN(m_brk.adjAccForSpdError(targetAcc, spd0, targetSpd), 0);

    if( ta < 0 )
    {
        if( spd0 > targetSpd )
        {
            m_brk.execute8( acc0, ta, fslip0, rslip0, m_cm[PATH_NORMAL].TARGET_SLIP, m_cm[PATH_NORMAL].MAX_SLIP,
                            m_priv[PATH_NORMAL].BRAKE_LIMIT, traffic );
            acc = m_brk.acc;
            brk = m_brk.targetBrk;
        }
        else
        {
            m_brk.clear();
            if( targetSpd > 1 )
            {
                // slow naturally.
                acc = MN(acc, traffic ? 0.1 : 0.11 * (targetSpd - spd0));
            }
            else
            {
                acc = 0;
                brk = 0.1;
            }
        }
    }
    else
    {
        m_brk.clear();
        double x = (10 + spd0) * (targetSpd - spd0) / 20;
        if (x > 0)
            acc = x;
    }
}

void	Driver::SpeedControl9(
        double	targetSpd,
        double	spd0,
        double	targetAcc,
        double	acc0,
        double	fslip0,
        double	rslip0,
        double&	acc,
        double&	brk,
        bool	traffic )
{
    static double s_brk = 0;
    static double s_acc = 0;

    if( spd0 + 0.1 < targetSpd )
    {
        s_acc = MN(1, s_acc + 0.1);
        s_brk = MX(0, s_brk - 0.1);
    }
    else if( spd0 - 0.1 > targetSpd )
    {
        s_acc = MN(1, s_acc - 0.1);
        if( s_brk == 0 )
            s_brk = 0.5;
        else if( fslip0 < m_cm[PATH_NORMAL].TARGET_SLIP )
            s_brk = MN(1, s_brk + 0.1);
        else
            s_brk = MX(0, s_brk - 0.1);
    }

    acc = s_acc;
    brk = s_brk;
}

void	Driver::SpeedControl1(
        double	targetSpd,
        double	spd0,
        double&	acc,
        double&	brk )
{
    if( spd0 > targetSpd )
    {
        if( spd0 - 1 > targetSpd )
        {
            if( spd0 - 2 < targetSpd )
                brk = 0.07;
            else if( spd0 - 3 < targetSpd )
                brk = 0.14;
            else if( spd0 - 4 < targetSpd )
                brk = 0.20;
            else if( spd0 - 5 < targetSpd )
                brk = 0.25;
            else if( spd0 - 5 < targetSpd )
                brk = 0.5;
            else
                brk = 0.5;

            acc = 0;
        }
        else
        {
            if( targetSpd > 1 )
                // slow naturally.
                acc = MN(acc, 0.25);
            else
            {
                acc = 0;
                brk = 0.1;
            }
        }
    }

    m_lastTargV = 0;
}

void	Driver::SpeedControl2(
        double	targetSpd,
        double	spd0,
        double&	acc,
        double&	brk )
{
    if( m_lastBrk && m_lastTargV )
    {
        if( m_lastBrk > 0 )
        {
            double	err = m_lastTargV - spd0;
            m_accBrkCoeff.Sample( err, m_lastBrk );
        }
        m_lastBrk = 0;
        m_lastTargV = 0;
    }

    if( spd0 > targetSpd )
    {
        {
            double	MAX_BRK = 0.5;
            double	err = spd0 - targetSpd;
            brk = MX(0, MN(m_accBrkCoeff.CalcY(err), MAX_BRK));
            acc = 0;

            m_lastBrk = brk;
            m_lastTargV = 0;

            if( brk > 0 )
            {
                if( targetSpd > 0 )
                    m_lastTargV = spd0;
            }
        }
    }
}

void	Driver::SpeedControl3(
        double	targetSpd,
        double	spd0,
        double&	acc,
        double&	brk )
{
    if( m_lastBrk && m_lastTargV )
    {
        double	err = spd0 - m_lastTargV;
        m_brkCoeff[m_lastB] += err * 0.001;
        m_lastBrk = 0;
        m_lastTargV = 0;
    }

    if( spd0 > targetSpd )
    {
        {
            int		b = int(floor(spd0 / 2));
            double	MAX_BRK = 0.5;
            brk = MX(0, MN(m_brkCoeff[b] * (spd0 - targetSpd), MAX_BRK));
            acc = 0;
            m_lastB = b;
            m_lastBrk = brk;
            m_lastTargV = 0;

            if( brk > 0 && brk < MAX_BRK)
            {
                if( targetSpd > 0 )
                    m_lastTargV = targetSpd;
            }
        }
    }
}

void	Driver::SpeedControl4(
        double	targetSpd,
        double	spd0,
        double	k,
        CarElt*	car,
        double&	acc,
        double&	brk )
{
    if( m_lastBrk && m_lastTargV )
    {
#if 0   // dead code
        if( m_lastBrk > 0 || (car->ctrl.accelCmd == -m_lastBrk) )
        {
            double	err = m_lastTargV - spd0;
        }
#endif

        m_lastBrk = 0;
        m_lastTargV = 0;
    }

    {
        {
            // this is a crude way to limit the slip... better to control the
            // slip more directly.
            double	MAX_BRK = fabs(k) < 0.0015 ? 0.95 :
                                                 fabs(k) < 0.0035 ? 0.75 : 0.6;
            double	err = spd0 - targetSpd;
            double	t = m_accBrkCoeff.CalcY(err);

            if( t > 0 )
            {
                brk = MN(t, 0.9);

                if( m_cm[PATH_NORMAL].frontAxleSlipTangential() > m_cm[PATH_NORMAL].TARGET_SLIP )
                    brk *= 0.7;

                acc = 0;
            }
            else
            {
                brk = 0;
                acc = MN(-t, 1);
            }

            m_lastBrk = t;
            m_lastTargV = 0;

            if( t > -1 && t < MAX_BRK )
            {
                if( targetSpd > 0 )
                    m_lastTargV = spd0;
            }
        }
    }

    double w1_speed = car->priv.wheel[0].spinVel * car->info.wheel[0].wheelRadius;
    double w2_speed = car->priv.wheel[1].spinVel * car->info.wheel[1].wheelRadius;
    double w3_speed = car->priv.wheel[2].spinVel * car->info.wheel[2].wheelRadius;
    double w4_speed = car->priv.wheel[3].spinVel * car->info.wheel[3].wheelRadius;

    double front_speed = (w1_speed + w2_speed) * 0.5;
    double left_spin_speed  = w3_speed - front_speed;
    double right_spin_speed = w4_speed - front_speed;
    double max_spin_speed = MX(left_spin_speed, right_spin_speed);

    if( max_spin_speed > 2 )
    {
        _acc -= 0.01 * (max_spin_speed - 2);
    }
    else if( targetSpd > spd0 )
    {
        _acc += 0.01 * (targetSpd - spd0);
    }
    else if( targetSpd < spd0 )
    {
        _acc -= 0.01 * (targetSpd - spd0);
    }

    _acc = MX(0, MN(_acc, 1));
}

void	Driver::SpeedControl(
        int		which,
        double	targetSpd,
        double	spd0,
        double	targetAcc,
        double	acc0,
        double	fslip0,
        double	rxslip0,
        double	rslip0,
        double	k,
        CarElt*	car,
        double&	acc,
        double&	brk,
        bool	traffic )
{
    switch( which )
    {
    case 0:		SpeedControl0(targetSpd, spd0, acc, brk);			break;
    case 1:		SpeedControl1(targetSpd, spd0, acc, brk);			break;
    case 2:		SpeedControl2(targetSpd, spd0, acc, brk);			break;
    case 3:		SpeedControl3(targetSpd, spd0, acc, brk);			break;
    case 4:		SpeedControl4(targetSpd, spd0, k, car, acc, brk);	break;
    case 5:		SpeedControl5(targetSpd, spd0, targetAcc, acc0, rxslip0, acc, brk, traffic);			break;
    case 6:		SpeedControl6(targetSpd, spd0, targetAcc, acc0, rxslip0, acc, brk, traffic);			break;
    case 7:		SpeedControl7(targetSpd, spd0, targetAcc, acc0, rxslip0, MX(fslip0, rslip0), acc, brk, traffic);	break;
    case 8:		SpeedControl8(targetSpd, spd0, targetAcc, acc0, fslip0, rslip0, acc, brk, traffic);		break;
    case 9:		SpeedControl9(targetSpd, spd0, targetAcc, acc0, fslip0, rslip0, acc, brk, traffic);		break;
    default:	SpeedControl3(targetSpd, spd0, acc, brk);			break;
    }
}

void	Driver::launchControlClutch( tCarElt* car, tSituation* s )
{
    static bool accel = true;
    static int gearChangeCounter = 0;

    if( car->pub.speed > 250.0 / 3.6 )
    {
        // hit 200 kph...
        accel = false;
    }

    if( !accel )
    {
        car->ctrl.accelCmd = 0.0;
        car->ctrl.brakeCmd = 1.0;
    }

    car->ctrl.steer = -car->pub.DynGC.pos.az;

    if( s->currentTime < 0 )
    {
        accel = true;
        car->ctrl.accelCmd = 1.0f;
        car->ctrl.brakeCmd = 0.0;
        car->ctrl.clutchCmd = 1.0;
        gearChangeCounter = 10;
        return;
    }

    double	wv = 0;
    int		count = 0;

    if( m_driveType == cDT_FWD || m_driveType == cDT_4WD )
    {
        wv +=	car->_wheelRadius(FRNT_LFT) * car->priv.wheel[FRNT_LFT].spinVel +
                car->_wheelRadius(FRNT_RGT) * car->priv.wheel[FRNT_RGT].spinVel;
        count += 2;
    }

    if( m_driveType == cDT_RWD || m_driveType == cDT_4WD )
    {
        wv +=	car->_wheelRadius(REAR_LFT) * car->priv.wheel[REAR_LFT].spinVel +
                car->_wheelRadius(REAR_RGT) * car->priv.wheel[REAR_RGT].spinVel;
        count += 2;
    }
    wv /= count;

    double delta = wv - car->pub.speed;
    double ddiff = delta - _prevDelta;
    _prevDelta = delta;

    if( car->ctrl.clutchCmd > 0 || gearChangeCounter > 0 || delta > 4 )
    {
        if( s->currentTime < 0.2 )
        {
            car->ctrl.clutchCmd = 0.75f;
            _deltaCounter = 5;
        }
        else if( (delta > 0.1 || _deltaCounter > 0) && car->priv.gear < 3 )
        {
            if( delta > 3 )
                _deltaCounter = 5;
            else
                _deltaCounter = MX(0, _deltaCounter - 1);

            //if( _deltaCounter > 0 )
            car->ctrl.clutchCmd = MX(0, MN(car->ctrl.clutchCmd + (delta - 4) * 0.02 + ddiff * 0.04, 1));
        }
        else if( gearChangeCounter > 0 )
        {
            car->ctrl.clutchCmd = MX(0, gearChangeCounter * 0.02 - 0.05);
        }
        else
        {
            car->ctrl.clutchCmd = 0;
        }
    }

    gearChangeCounter = MX(0, gearChangeCounter - 1);

    double acc = 0;
    int newGear = CalcGear(car, acc);

    if( newGear > car->ctrl.gear )
        gearChangeCounter = 10;

    car->ctrl.gear = newGear;

    static double lastSpd = 0;
    LogSHADOW.debug( "%1.3f,%d,%6.3f,%4.0f,%5.3f,%5.3f,%6.3f,%6.3f,%6.3f,%6.3f,%6.3f\n",
                     s->currentTime,
                     car->priv.gear,
                     car->pub.speed,
                     car->priv.enginerpm * 60 / (2 * PI),
                     car->ctrl.accelCmd,
                     car->ctrl.clutchCmd,
                     (car->pub.DynGC.vel.x - lastSpd) / s->deltaTime,//car->pub.DynGC.acc.x,
                     delta,
                     car->priv.wheel[2].slipAccel,
            (m_cm[PATH_NORMAL].wheel(2).slipX() + m_cm[PATH_NORMAL].wheel(3).slipX()) * 0.5,
            (wv - car->pub.DynGC.vel.x) / car->pub.DynGC.vel.x );
    lastSpd = car->pub.DynGC.vel.x;
}

void	Driver::launchControlSimple( tCarElt* car, tSituation* s )
{
    static int ctrlCount = 0;

    car->ctrl.accelCmd = 1.0;
    car->ctrl.brakeCmd = 0.0;

    if( s->currentTime < 0 )
    {
        car->ctrl.clutchCmd = 0.75;
    }
    else
    {
        if( ctrlCount == 0 )
        {
            car->ctrl.gear = 1;
            car->ctrl.clutchCmd = 1.0;
        }
        else if( ctrlCount < 10 )
        {
            car->ctrl.clutchCmd = 1 - ctrlCount * 0.1;
        }

        ctrlCount++;
    }

    double	wv = 0;
    int		count = 0;

    if( m_driveType == cDT_FWD || m_driveType == cDT_4WD )
    {
        wv +=	car->_wheelRadius(FRNT_LFT) * car->priv.wheel[FRNT_LFT].spinVel +
                car->_wheelRadius(FRNT_RGT) * car->priv.wheel[FRNT_RGT].spinVel;
        count += 2;
    }

    if( m_driveType == cDT_RWD || m_driveType == cDT_4WD )
    {
        wv +=	car->_wheelRadius(REAR_LFT) * car->priv.wheel[REAR_LFT].spinVel +
                car->_wheelRadius(REAR_RGT) * car->priv.wheel[REAR_RGT].spinVel;
        count += 2;
    }
    wv /= count;

    double delta = wv - car->pub.speed;
    // double ddiff = delta - _prevDelta;
    _prevDelta = delta;

    LogSHADOW.debug( "%1.3f,%d,%5.2f,%3.0f,%5.3f,%5.3f,%6.3f,%6.3f\n",
                     s->currentTime,
                     car->priv.gear,
                     car->pub.speed,
                     car->priv.enginerpm * 60 / (2 * PI),
                     car->ctrl.accelCmd,
                     car->ctrl.clutchCmd,
                     car->pub.DynGC.acc.x,
                     delta );
}

void	Driver::launchControlAcclerator( tCarElt* car, tSituation* s )
{
    static bool accel = true;
    static int gearChangeCounter = 0;

    if( car->pub.speed > 250.0 / 3.6 )
    {
        // hit 200 kph...
        accel = false;
    }

    if( !accel )
    {
        car->ctrl.accelCmd = 0.0;
        car->ctrl.brakeCmd = 0.5;
    }

    car->ctrl.steer = -car->pub.DynGC.pos.az;

    if( s->currentTime < 0 )
    {
        accel = true;
        car->ctrl.accelCmd = 1.0;
        car->ctrl.brakeCmd = 0.0;
        car->ctrl.clutchCmd = 1.0;
        gearChangeCounter = 10;
        return;
    }

    double	wv = 0;
    int		count = 0;

    if( m_driveType == cDT_FWD || m_driveType == cDT_4WD )
    {
        wv +=	car->_wheelRadius(FRNT_LFT) * car->priv.wheel[FRNT_LFT].spinVel +
                car->_wheelRadius(FRNT_RGT) * car->priv.wheel[FRNT_RGT].spinVel;
        count += 2;
    }

    if( m_driveType == cDT_RWD || m_driveType == cDT_4WD )
    {
        wv +=	car->_wheelRadius(REAR_LFT) * car->priv.wheel[REAR_LFT].spinVel +
                car->_wheelRadius(REAR_RGT) * car->priv.wheel[REAR_RGT].spinVel;
        count += 2;
    }
    wv /= count;

    if( car->ctrl.clutchCmd > 0 || gearChangeCounter > 0 )
    {
        if( gearChangeCounter > 0 )
        {
            car->ctrl.clutchCmd = MX(0, gearChangeCounter * 0.02 - 0.05);
        }
        else
        {
            car->ctrl.clutchCmd = 0;
        }
    }

    double delta = wv - car->pub.speed;
    double ddiff = delta - _prevDelta;
    _prevDelta = delta;

    if( accel )
    {
        if( s->currentTime >= 0.2 && car->priv.gear < 3 )
        {
            double slipErr = 4.0 - delta;
            car->ctrl.accelCmd = MX(0, MN(car->ctrl.accelCmd + 0.013 * slipErr - 0.05 * ddiff, 1.0));
        }
        else
        {
            car->ctrl.accelCmd = 1.0;
        }
    }

    gearChangeCounter = MX(0, gearChangeCounter - 1);

    double acc = 0;
    int newGear = CalcGear(car, acc);
    if( newGear > car->ctrl.gear )
        gearChangeCounter = 10;
    car->ctrl.gear = newGear;

    static double lastSpd = 0;
    LogSHADOW.debug( "%1.3f,%d,%6.3f,%4.0f,%5.3f,%5.3f,%6.3f,%6.3f,%6.3f,%6.3f,%6.3f\n",
                     s->currentTime,
                     car->priv.gear,
                     car->pub.speed,
                     car->priv.enginerpm * 60 / (2 * PI),
                     car->ctrl.accelCmd,
                     car->ctrl.clutchCmd,
                     (car->pub.DynGC.vel.x - lastSpd) / s->deltaTime,//car->pub.DynGC.acc.x,
                     delta,
                     car->priv.wheel[2].slipAccel,
            (m_cm[PATH_NORMAL].wheel(2).slipX() + m_cm[PATH_NORMAL].wheel(3).slipX()) * 0.5,
            (wv - car->pub.DynGC.vel.x) / car->pub.DynGC.vel.x );
    lastSpd = car->pub.DynGC.vel.x;
}

void	Driver::launchControlAccSlip( tCarElt* car, tSituation* s )
{
    static bool accel = true;
    static int gearChangeCounter = 0;

    if( car->pub.speed > 250.0 / 3.6 )
    {
        // hit 200 kph...
        accel = false;
    }

    if( !accel )
    {
        car->ctrl.accelCmd = 0.0;
        car->ctrl.brakeCmd = 0.5;
    }

    car->ctrl.steer = -car->pub.DynGC.pos.az;

    if( s->currentTime < 0 )
    {
        accel = true;
        car->ctrl.accelCmd = 1.0;
        car->ctrl.brakeCmd = 0.0;
        car->ctrl.clutchCmd = 1.0;
        gearChangeCounter = 10;
        return;
    }

    double	wv = 0;
    int		count = 0;

    if( m_driveType == cDT_FWD || m_driveType == cDT_4WD )
    {
        wv +=	car->_wheelRadius(FRNT_LFT) * car->priv.wheel[FRNT_LFT].spinVel +
                car->_wheelRadius(FRNT_RGT) * car->priv.wheel[FRNT_RGT].spinVel;
        count += 2;
    }

    if( m_driveType == cDT_RWD || m_driveType == cDT_4WD )
    {
        wv +=	car->_wheelRadius(REAR_LFT) * car->priv.wheel[REAR_LFT].spinVel +
                car->_wheelRadius(REAR_RGT) * car->priv.wheel[REAR_RGT].spinVel;
        count += 2;
    }

    wv /= count;

    if( car->ctrl.clutchCmd > 0 || gearChangeCounter > 0 )
    {
        if( gearChangeCounter > 0 )
        {
            car->ctrl.clutchCmd = MX(0, gearChangeCounter * 0.02 - 0.05);
        }
        else
        {
            car->ctrl.clutchCmd = 0;
        }
    }

    double delta = wv - car->pub.speed;
    double ddiff = delta - _prevDelta;
    _prevDelta = delta;

    if( accel )
    {
        if( s->currentTime >= 0.0 && car->priv.gear < 3 )
        {
            double targSpd = 0.195 * MX(5, car->pub.DynGC.vel.x);
            double slipErr = targSpd - delta;
            car->ctrl.accelCmd = MX(0, MN(car->ctrl.accelCmd + 0.013 * slipErr - 0.05 * ddiff, 1.0));
        }
        else
        {
            car->ctrl.accelCmd = 1.0;
        }
    }

    gearChangeCounter = MX(0, gearChangeCounter - 1);

    double acc = 0;
    int newGear = CalcGear(car, acc);

    if( newGear > car->ctrl.gear )
        gearChangeCounter = 10;

    car->ctrl.gear = newGear;

    static double lastSpd = 0;
    LogSHADOW.debug( "%1.3f,%d,%6.3f,%4.0f,%5.3f,%5.3f,%6.3f,%6.3f,%6.3f,%6.3f,%6.3f\n",
                     s->currentTime,
                     car->priv.gear,
                     car->pub.speed,
                     car->priv.enginerpm * 60 / (2 * PI),
                     car->ctrl.accelCmd,
                     car->ctrl.clutchCmd,
                     (car->pub.DynGC.vel.x - lastSpd) / s->deltaTime,//car->pub.DynGC.acc.x,
                     delta,
                     car->priv.wheel[2].slipAccel,
            (m_cm[PATH_NORMAL].wheel(2).slipX() + m_cm[PATH_NORMAL].wheel(3).slipX()) * 0.5,
            (wv - car->pub.DynGC.vel.x) / car->pub.DynGC.vel.x );
    lastSpd = car->pub.DynGC.vel.x;
}

void	Driver::launchControlAccSlip2( tCarElt* car, tSituation* s )
{
    static bool accel = true;
    static int gearChangeCounter = 0;

    if( car->pub.speed > 250.0 / 3.6 )
    {
        // hit 200 kph...
        accel = false;
    }

    if( !accel )
    {
        car->ctrl.accelCmd = 0.0;
        car->ctrl.brakeCmd = 0.5;
    }

    car->ctrl.steer = -car->pub.DynGC.pos.az;

    if( s->currentTime < 0 )
    {
        accel = true;
        car->ctrl.accelCmd = 1.0;
        car->ctrl.brakeCmd = 0.0;
        car->ctrl.clutchCmd = 1.0;
        gearChangeCounter = 10;

        return;
    }

    double	wv = 0;
    int		count = 0;

    if( m_driveType == cDT_FWD || m_driveType == cDT_4WD )
    {
        wv +=	car->_wheelRadius(FRNT_LFT) * car->priv.wheel[FRNT_LFT].spinVel +
                car->_wheelRadius(FRNT_RGT) * car->priv.wheel[FRNT_RGT].spinVel;
        count += 2;
    }

    if( m_driveType == cDT_RWD || m_driveType == cDT_4WD )
    {
        wv +=	car->_wheelRadius(REAR_LFT) * car->priv.wheel[REAR_LFT].spinVel +
                car->_wheelRadius(REAR_RGT) * car->priv.wheel[REAR_RGT].spinVel;
        count += 2;
    }
    wv /= count;

    if( car->ctrl.clutchCmd > 0 || gearChangeCounter > 0 )
    {
        if( gearChangeCounter > 0 )
        {
            car->ctrl.clutchCmd = MX(0, gearChangeCounter * 0.02 - 0.05);
        }
        else
        {
            car->ctrl.clutchCmd = 0;
        }
    }

    double delta = wv - car->pub.speed;
    double ddiff = delta - _prevDelta;
    _prevDelta = delta;

    if( accel )
    {
        if( s->currentTime >= 0.0 && car->priv.gear < 3 )
        {
            double targSpd = (car->priv.gear <= 1 ? 0.300 : 0.195) * MX(5, car->pub.DynGC.vel.x);
            double slipErr = targSpd - delta;
            car->ctrl.accelCmd = MX(0, MN(car->ctrl.accelCmd + 0.013 * slipErr - 0.05 * ddiff, 1.0));
        }
        else
        {
            car->ctrl.accelCmd = 1.0;
        }
    }

    gearChangeCounter = MX(0, gearChangeCounter - 1);

    double acc = 0;
    int newGear = CalcGear(car, acc);
    if( newGear > car->ctrl.gear )
        gearChangeCounter = 10;
    car->ctrl.gear = newGear;

    //PRINTF( "time: %g  speed: %g  rpm: %g\n",
    //		s->currentTime, car->pub.speed, car->priv.enginerpm );
    if( s->currentTime < 1 )
    {
        static double cl = 0.9;
        car->ctrl.accelCmd = 1.0;
        car->ctrl.clutchCmd = cl;
        car->ctrl.gear = 1;
    }

    static double lastSpd = 0;
    LogSHADOW.debug( "%1.3f,%d,%6.3f,%4.0f,%5.3f,%5.3f,%6.3f,%6.3f,%6.3f,%6.3f,%6.3f\n",
                     s->currentTime,
                     car->priv.gear,
                     car->pub.speed,
                     car->priv.enginerpm * 60 / (2 * PI),
                     car->ctrl.accelCmd,
                     car->ctrl.clutchCmd,
                     (car->pub.DynGC.vel.x - lastSpd) / s->deltaTime,//car->pub.DynGC.acc.x,
                     delta,
                     car->priv.wheel[2].slipAccel,
            (m_cm[PATH_NORMAL].wheel(2).slipX() + m_cm[PATH_NORMAL].wheel(3).slipX()) * 0.5,
            (wv - car->pub.DynGC.vel.x) / car->pub.DynGC.vel.x );
    lastSpd = car->pub.DynGC.vel.x;

}

void	Driver::launchControlFullThrottle( tCarElt* car, tSituation* s )
{
    static bool accel = true;
    static int gearChangeCounter = 0;
    static int bcount = 0;

    if( car->pub.speed > 300.0 / 3.6 )
    {
        // hit 200 kph...
        accel = false;
    }

    if( !accel )
    {
        static double brk = 1.0;
        static double lastSlip = 0;
        double fslip = (m_cm[PATH_NORMAL].wheel(0).slipX() + m_cm[PATH_NORMAL].wheel(1).slipX()) * 0.5;
        double rslip = (m_cm[PATH_NORMAL].wheel(2).slipX() + m_cm[PATH_NORMAL].wheel(3).slipX()) * 0.5;
        double err = m_cm[PATH_NORMAL].TARGET_SLIP - fslip;
        double dslip = fslip - lastSlip;
        brk += err * 0.2 - dslip * 1.5;
        brk = MX(0, MN(brk, 1));
        lastSlip = fslip;

        car->ctrl.accelCmd = rslip < m_cm[PATH_NORMAL].TARGET_SLIP ? 0.0 : 0.1;
        car->ctrl.brakeCmd = MX(0, MN(brk, 1));
        bcount += 1;
    }

    car->ctrl.steer = -car->pub.DynGC.pos.az;

    if( s->currentTime < 0 )
    {
        accel = true;
        car->ctrl.accelCmd = 1.0;
        car->ctrl.brakeCmd = 0.0;
        car->ctrl.clutchCmd = 1.0;
        gearChangeCounter = 10;
        return;
    }

    double	wv = 0;
    int		count = 0;

    if( m_driveType == cDT_FWD || m_driveType == cDT_4WD )
    {
        wv +=	car->_wheelRadius(FRNT_LFT) * car->priv.wheel[FRNT_LFT].spinVel +
                car->_wheelRadius(FRNT_RGT) * car->priv.wheel[FRNT_RGT].spinVel;
        count += 2;
    }

    if( m_driveType == cDT_RWD || m_driveType == cDT_4WD )
    {
        wv +=	car->_wheelRadius(REAR_LFT) * car->priv.wheel[REAR_LFT].spinVel +
                car->_wheelRadius(REAR_RGT) * car->priv.wheel[REAR_RGT].spinVel;
        count += 2;
    }
    wv /= count;

    if( car->ctrl.clutchCmd > 0 || gearChangeCounter > 0 )
    {
        if( gearChangeCounter > 0 )
        {
            car->ctrl.clutchCmd = MX(0, gearChangeCounter * 0.02 - 0.05);
        }
        else
        {
            car->ctrl.clutchCmd = 0;
        }
    }

    if( accel )
    {
        car->ctrl.accelCmd = 1.0;
    }

    gearChangeCounter = MX(0, gearChangeCounter - 1);

    double acc = 0;
    int newGear = CalcGear(car, acc);
    if( newGear != car->ctrl.gear )
        gearChangeCounter = 10;
    car->ctrl.gear = newGear;

    static double lastPos = 0;
    static double lastSpd = 0;
    //		  t    del-t  acc   brk   clut gr  rpm   pos    spd   cspd  acc   cacc rslpv  fslp  rslp freac rreac
    LogSHADOW.debug( "%1.3f,%7.5f,%5.3f,%5.3f,%5.3f,%d,%4.0f,%12.7f,%5.3f,%6.3f,%6.3f,%6.3f,%6.3f,%6.3f,%6.3f,%5.0f,%5.0f\n",
                     s->currentTime,
                     s->deltaTime,
                     car->ctrl.accelCmd,
                     car->ctrl.brakeCmd,
                     car->ctrl.clutchCmd,
                     car->priv.gear,
                     car->priv.enginerpm * 60 / (2 * PI),
                     car->pub.DynGCg.pos.x,
                     car->pub.DynGCg.vel.x,
                     (car->pub.DynGCg.pos.x - lastPos) / s->deltaTime,
                     car->pub.DynGC.acc.x,
                     (car->pub.DynGC.vel.x - lastSpd) / s->deltaTime,
                     car->priv.wheel[2].slipAccel,
            (m_cm[PATH_NORMAL].wheel(0).slipX() + m_cm[PATH_NORMAL].wheel(1).slipX()) * 0.5,
            (m_cm[PATH_NORMAL].wheel(2).slipX() + m_cm[PATH_NORMAL].wheel(3).slipX()) * 0.5,
            car->priv.reaction[0] + car->priv.reaction[1],
            car->priv.reaction[2] + car->priv.reaction[3] );
    lastSpd = car->pub.DynGC.vel.x;
    lastPos = car->pub.DynGC.pos.x;
}

void	Driver::Drive( int index, tCarElt* car, tSituation* s )
{
    if (strcmp(m_trackName, "garage") == 0)
    {
        m_garage = true;
        return;
    }

    m_cm[PATH_NORMAL].update( car, s );

/*#if EXPERIMENTAL
    //#if 1
    launchControlFullThrottle(car, s);
    //	launchControlClutch(car, s);
    //	launchControlAcclerator(car, s);
    //	launchControlAccSlip(car, s);
    //	launchControlAccSlip2(car, s);
    //	launchControlSimple(car, s);
    return;
#endif*/

    double	h[4];
    for( int i = 0; i < 4; i++ )
    {
        tTrkLocPos	wp;
        //		double		wx = car->_pos_X;
        //		double		wy = car->_pos_Y;
        double		wx = car->pub.DynGCg.pos.x;
        double		wy = car->pub.DynGCg.pos.y;
        RtTrackGlobal2Local(car->_trkPos.seg, wx, wy, &wp, TR_LPOS_SEGMENT);
        h[i] = car->_pos_Z - RtTrackHeightL(&wp) - car->_wheelRadius(i);
    }

    if( m_raceStart || s->currentTime <= 0.5 )
    {
        if( m_raceStart )
        {
            Vec2d	thisPt(car->_pos_X, car->_pos_Y);
            for( int i = 0; i + 1 < HIST; i++ )
                m_lastPts[i] = m_lastPts[i + 1];
            m_lastSpd = 0;
            m_lastAng = 0;
            m_lastLap = car->race.laps;
            m_steerGraph.SetBeta( 0.1 );
        }

        m_raceStart = false;
        LogSHADOW.debug( "m_avoidS = %g (race start 2)\n", 0.0 );
        m_avoidS = 0;
        m_avoidSVel = 0;
        m_avoidT = CalcPathTarget(m_track.CalcPos(car), -car->_trkPos.toMiddle);
        m_avoidTVel = 0;
    }

    if( car->race.laps != m_lastLap )
    {
        m_lastLap = car->race.laps;
        double a, b;
        m_accBrkCoeff.CalcCoeffs(&a, &b);
        LogSHADOW.debug( "[%d] accbrk: a=%g, b=%g\n", car->index, a, b );
    }

    if( m_priv[PATH_NORMAL].SAVE_PATHS )
        m_pathOffsets.update(m_track, car);

    double	gripScaleF = GripFactor(car, true);
    double	gripScaleR = GripFactor(car, false);

#if !(defined(DEV) && defined(BRAKING_TEST))
        for( int p = PATH_NORMAL; p <= PATH_RIGHT; p++ )
        {
            m_path[p].CalcMaxSpeeds( m_cm[p] );
            m_path[p].PropagateBraking( m_cm[p] );
            m_path[p].PropagateAcceleration( m_cm[p] );
        }
#endif
 

    //	DEBUGF( "**** wheel fz %g %g %g %g\n", h[0], h[1], h[2], h[3] );

    if( h[0] > m_priv[PATH_NORMAL].FLY_HEIGHT )
    {
        m_flying = MN(FLY_COUNT, m_flying + (FLY_COUNT / 2));
    }
    else if( m_flying > 0 )
    {
        m_flying--;
    }

    //calcSkill(s);

    // get curret pos on track.

    PtInfo	pi, aheadPi;
    double	angle = SteerAngle1(car, pi, aheadPi);
    double	steer = angle / car->_steerLock;
    const double steer_damping = 0.75;
    steer = m_prevSteer * steer_damping + steer * (1 - steer_damping);
    m_prevSteer = steer;

    // double	offset = -car->pub.trkPos.toMiddle - pi.offs;
    double	oangle = car->pub.DynGC.pos.az     - pi.oang;
    // double	ospeed = car->pub.DynGC.vel.x      - pi.spd;
    double	xfslip = (m_cm[PATH_NORMAL].wheel(0).slipX() +
                      m_cm[PATH_NORMAL].wheel(1).slipX()) * 0.5;
    double	xrslip = (m_cm[PATH_NORMAL].wheel(2).slipX() +
                      m_cm[PATH_NORMAL].wheel(3).slipX()) * 0.5;
    double	yfslip = (m_cm[PATH_NORMAL].wheel(0).slipY() +
                      m_cm[PATH_NORMAL].wheel(1).slipY()) * 0.5;
    double	yrslip = (m_cm[PATH_NORMAL].wheel(2).slipY() +
                      m_cm[PATH_NORMAL].wheel(3).slipY()) * 0.5;
    double	fslip = hypot(xfslip, yfslip);
    double	rslip = hypot(xrslip, yrslip);
    NORM_PI_PI(oangle);

    // work out current car speed.
    double	spd0 = hypot(car->_speed_x, car->_speed_y);
    double	acc0 = (spd0 - _lastSpd0) / s->deltaTime;
    _lastSpd0 = spd0;

    {
        static Vec2d prevPos;
        static Vec2d lastPos;
        Vec2d currPos = Vec2d(car->pub.DynGCg.pos);
        Vec2d lastVel = (lastPos - prevPos) / s->deltaTime;
        Vec2d currVel = (currPos - lastPos) / s->deltaTime;
        Vec2d currAcc = (currVel - lastVel) / s->deltaTime;
        Vec2d carDir  = Vec2d(currVel).GetUnit();
        double carTanAcc = currAcc * carDir;
        double carLatAcc = currAcc * carDir.GetNormal();
        double pathLatAcc = spd0 * spd0 * pi.k;
        double pathFullAcc = sqrt(pathLatAcc * pathLatAcc + pi.acc * pi.acc);
        LogSHADOW.debug( "[%d, %4d] p.ac %6.2f,%6.2f=%6.2f  c.ac %6.2f,%6.2f=%6.2f fsl %6.3f,%6.3f=%6.3f  rsl %6.3f,%6.3f=%6.3f\n",
                         car->index, pi.idx,
                         pi.acc, pathLatAcc, pathFullAcc,
                         carTanAcc, carLatAcc, currAcc.len(),
                         xfslip, yfslip, fslip, xrslip, yrslip, rslip );
        prevPos = lastPos;
        lastPos = currPos;
    }

    {
        const CarModel& cm = m_cm[PATH_NORMAL];
        double	opLoad = cm.MASS * G;
        const Path::PathPt& pp = m_path[PATH_NORMAL].GetAt(pi.idx);
        double	load = cm.calcPredictedLoad(spd0, 1.0, cm.CA, pi.k, pp.kz, pp.kv, sin(pp.ar), cos(pp.ar), cos(pp.ap));
        double	sysLoad = car->priv.reaction[0] + car->priv.reaction[1] + car->priv.reaction[2] + car->priv.reaction[3];
        LogSHADOW.debug( "[%d] load ratio %g %g path speed %g  overspeed %g  offset to path %g  engine acc %g\n",
                         pi.idx, load / opLoad, sysLoad / opLoad, pi.spd, spd0 - pi.spd, pi.offs + car->pub.trkPos.toMiddle,
                         cm.AccForceFromSpeed(pp.accSpd) / (cm.MASS) );
    }

    double	targetSpd = pi.spd;
    double	targetAcc = pi.acc;
    double	avoidTargetSpd = pi.spd;
    double	avoidTargetAcc = pi.acc;

    bool	close = false;
    bool	lapper = false;
    AvoidOtherCars( index, car, s, pi.k, &avoidTargetSpd, &avoidTargetAcc, &close, &lapper );
#if 0   // dead code
    bool	slowing = false;
#endif

    if( car->pub.speed > avoidTargetSpd && targetSpd > avoidTargetSpd )
    {
        LogSHADOW.debug( "[%d] slowing for avoidance.  curr %g  targ %g  avoid spd %g, avoid acc %g, curr acc %g\n",
                         car->index, car->pub.speed, targetSpd, avoidTargetSpd, avoidTargetAcc, car->pub.DynGC.acc.x );
#if 0   // dead code
        slowing = true;
#endif
    }


    targetSpd = avoidTargetSpd;
    targetAcc = avoidTargetAcc;

    targetSpd *= globalskill;
    targetAcc *= driverskill;

    //
    // steer into the skid
    if( !m_Strategy.WantToPit())
    {
        double skidfactor = (m_avoidS != 1 ? m_priv[PATH_NORMAL].SKID_FACTOR_TRAFFIC :
                                             m_priv[PATH_NORMAL].SKID_FACTOR);
        if (skidfactor > 0.0)
        {
            double vx = car->_speed_X;
            double vy = car->_speed_Y;
            double dirx = cos(car->_yaw);
            double diry = sin(car->_yaw);
            double Skid = (dirx * vy - vx * diry) / (spd0 == 0.0 ? 0.1 : spd0);
            Skid = MN(0.9, MX(-0.9, Skid));
            steer += (asin(Skid) / car->_steerLock) * skidfactor;
        }
        else
        {
            static double last_srslip[MAX_MOD_ITF] = {0};

            const double slip_limit  = m_priv[PATH_NORMAL].REAR_LAT_SLIP_LIMIT;
            const double slip_factor = m_priv[PATH_NORMAL].REAR_LAT_SLIP_FACTOR;
            const double slip_dscale = m_priv[PATH_NORMAL].REAR_LAT_SLIP_DSCALE;

            double srslip = SGN(yrslip) * rslip;
            double delta_srslip = srslip - last_srslip[index];
            last_srslip[index] = srslip;
            steer += atan(delta_srslip) * slip_dscale;// 0.1;

            if( rslip > slip_limit && fabs(yrslip) > slip_limit * 0.5 )
            {
                double slip_delta = yrslip > 0 ? rslip - slip_limit : slip_limit - rslip;
                steer += atan(slip_delta) * slip_factor;
            }
        }
    }

    {
        Vec2d	thisPt(car->_pos_X, car->_pos_Y);
        if( (thisPt - m_lastPts[0]).len() > 0.1 &&
                car->ctrl.accelCmd == 1.0 )
        {
            double	x[2];
            x[0] = Utils::CalcCurvature(m_lastPts[0], m_lastPts[HIST / 2], thisPt);
            x[0] = fabs(x[0]);
            x[1] = m_lastSpd;
            m_steerGraph.Learn( x, fabs(m_lastAng) );
        }

        for( int i = 0; i + 1 < HIST; i++ )
        {
            m_lastPts[i] = m_lastPts[i + 1];
        }

        m_lastPts[HIST - 1] = thisPt;
        m_lastSpd = spd0;
        m_lastAng = m_lastAng * 0.75 + angle * 0.25;
    }

    // const double G = 9.81;

    double	acc = 1.0;
    string	accWho = "none";
    double	brk = 0;

    int spdCtrl = m_priv[PATH_NORMAL].SPDC_NORMAL;
    bool traffic = close;// || m_Strategy.WantToPit();
    if( traffic )
        spdCtrl = m_priv[PATH_NORMAL].SPDC_TRAFFIC;

    SpeedControl( spdCtrl, targetSpd, spd0, targetAcc, acc0, fslip, xfslip, rslip, pi.k, car, acc, brk, traffic );
    accWho = "speed control";

    if( lapper )
    {
        acc = MN(acc, 0.6);
        accWho = "being lapped";
    }

    {
        double w1_speed = car->priv.wheel[0].spinVel * car->info.wheel[0].wheelRadius;
        double w2_speed = car->priv.wheel[1].spinVel * car->info.wheel[1].wheelRadius;
        double w3_speed = car->priv.wheel[2].spinVel * car->info.wheel[2].wheelRadius;
        double w4_speed = car->priv.wheel[3].spinVel * car->info.wheel[3].wheelRadius;

        double front_speed = (w1_speed + w2_speed) * 0.5;
        double left_spin_speed  = w3_speed - front_speed;
        double right_spin_speed = w4_speed - front_speed;
        double max_spin_speed = MX(left_spin_speed, right_spin_speed);
        // double avg_spin_speed = (left_spin_speed + right_spin_speed) * 0.5;
        double delta = max_spin_speed;

        double ddiff = delta - _prevDelta;
        _prevDelta = delta;


        double slipErr = m_priv[PATH_NORMAL].TCL_TARGET_SPEED - delta;
        _tctrlAcc = RG(_tctrlAcc + 0.013 * slipErr - 0.05 * ddiff, 0, 1.2);

        if( spdCtrl != 8 )
        {
            if( brk == 0 && acc > _tctrlAcc )//&& _tctrlAcc < 0.75 )
            {
                acc = _tctrlAcc;
                accWho = "traction control";
            }
            if( brk != 0 && max_spin_speed > m_priv[PATH_NORMAL].DEC_MAX_SPIN_SPEED )
            {
                acc = 0.0;
                accWho = "braking";
            }
        }
        if( s->currentTime < 0 )
        {
            acc = 1.0;
            brk = 1.0;
            car->ctrl.clutchCmd = 1.0;
            accWho = "before start";
        }
    }

    double	delta = pi.offs + car->_trkPos.toMiddle;

    // out of control??
    double	skidAng = atan2(car->_speed_Y, car->_speed_X) - car->_yaw;
    NORM_PI_PI(skidAng);
    if( spdCtrl != 8 )
    {
        if( car->_speed_x > 5 && fabs(skidAng) > 0.2 )
        {
            acc = MN(acc, 0.25 + 0.75 * cos(skidAng));
            accWho = "skidding";
        }

        if( car->_speed_x > 5 )
        {
            skidAng = MN(skidAng * 2, PI);
            brk *= MX(0, fabs(cos(skidAng)));
        }
    }
    // too far off line?
    double	fDelta = fabs(delta);
    const double	offDist = 2;

    if( fDelta > offDist && car->_distRaced > 50 )
    {
        double	mx = MX(1.0 - (fDelta - offDist) * 0.2, 0.3);
        acc = MN(mx, acc);
        accWho = "too far off line";
    }

    if( car->ctrl.clutchCmd > 0 )
    {
        double	wr = 0;
        int		count = 0;

        if( m_driveType == cDT_FWD || m_driveType == cDT_4WD )
        {
            wr += car->_wheelRadius(FRNT_LFT) + car->_wheelRadius(FRNT_RGT);
            count += 2;
        }

        if( m_driveType == cDT_RWD || m_driveType == cDT_4WD )
        {
            wr += car->_wheelRadius(REAR_LFT) + car->_wheelRadius(REAR_RGT);
            count += 2;
        }
        wr /= count;
        // double	gr = car->_gearRatio[car->_gear + car->_gearOffset];
        // double rpmForSpd = gr * car->_speed_x / wr;
        // double rpm = car->_enginerpm;

        const float dec = 0.02f;
        if( car->ctrl.clutchCmd > dec * 7.5f )
        {
            car->ctrl.clutchCmd = dec * 7.5f;
        }
        else
        {
            car->ctrl.clutchCmd -= dec;
            if( car->ctrl.clutchCmd < 0 )
                car->ctrl.clutchCmd = 0;
        }
    }

    int gear = CalcGear(car, acc);

    // facing the wrong way on the track??
    double	dirAng = pi.oang - car->_yaw;
    NORM_PI_PI(dirAng);

    if( fabs(car->_speed_x) > 2 || s->currentTime < 0 )
        m_stuckTime = 0;
    else
        m_stuckTime += s->deltaTime;

    const Opponent::Sit& mySit = m_opp[car->index].GetInfo().sit;

    if( spdCtrl != 8 )
    {
        if( gear > 0 && car->_speed_x < -0.01 )
        {
            gear = 1;
            brk = car->_speed_x < -0.5 ? 0.25 : 0;
            acc = 0.25;
            accWho = "going backwards";
        }

        if( gear == 1 && car->_speed_x >= -0.01 && car->_speed_x < 10 &&
                acc == 1.0 && brk == 0 )
        {
            if( fabs(dirAng) > 10 )
            {
                acc = 0.5;
                accWho = "drifting";
            }
            else
            {
                double	rpm = car->_enginerpm;
                double	clutch = (850 - rpm) / 400;

                if( car->_speed_x < 0.05 )
                    clutch = 0.5;

                car->ctrl.clutchCmd = MX(0, MN(clutch, 0.9));
            }
        }
    }

    if( fabs(pi.offs) > 5 )
    {
    }

    double	toL = pi.toL - car->_trkPos.toMiddle;
    double	toR = pi.toR + car->_trkPos.toMiddle;

    if( toL < -1 || toR < -1 )
    {
    }

    if( car->ctrl.accelCmd == 1 && car->ctrl.brakeCmd == 0 )
    {
        m_maxAccel.Learn( car->_speed_x, car->_accel_x );
    }

    if( fabs(pi.k * spd0 - car->_yaw_rate) < 0.02 )
    {
        double	velAng = atan2(car->_speed_Y, car->_speed_X);
        double	ang = car->_yaw - velAng;
        NORM_PI_PI(ang);

        int	k = int(floor((pi.k - K_MIN) / K_STEP));
        int	s = int(floor((spd0 - SPD_MIN) / SPD_STEP));
        double	ae = 0;
        if( k >= 0 && k < K_N && s >= 0 && s < SPD_N )
        {
            ae = ang - m_angle[s][k];
            m_angle[s][k] += ae * 0.1;
            ae = m_angle[s][k] - ang;
        }
    }

    if( spdCtrl != 5 && spdCtrl != 6 && spdCtrl != 7 && spdCtrl != 8 )
        brk = ApplyAbs(car, brk);

    // set up the values to return
    car->ctrl.steer = steer;
    car->ctrl.gear = gear;
    car->ctrl.accelCmd = acc;
    car->ctrl.brakeCmd = brk;

    ////if( car->race.laps > 3 )
    bool doStuckThing = true;
    if( Pitting(car) )
    {
        doStuckThing = false;
    }
    if( doStuckThing )
        m_stuckThing.execute( m_track, s, car, mySit );

    m_Strategy.Process( car, m_pShared->m_teamInfo.GetAt(car->index) );
}

// Pitstop callback.
int		Driver::PitCmd( int index, tCarElt* car, tSituation* s )
{
    m_Strategy.Process( car, m_pShared->m_teamInfo.GetAt(car->index) );

    return false;
}


// End of the current race.
void	Driver::EndRace( int index, tCarElt* car, tSituation* s )
{
}


// Called before the module is unloaded.
void	Driver::Shutdown( int index )
{
    // dump out some interesting info....
    //	const int	NSEG = m_track.GetSize();
    //	{for( int i = 0; i < NSEG; i++ )
    //	{
    //		DEBUGF( "dist %7.1f  spd %7.1f  rec %7.1f\n",
    //				m_track[i].segDist,
    //				m_path[PATH_NORMAL].GetAt(i).spd,
    //				m_oppPath[m_myOppIdx].GetAt(i).avgV );
    //	}}
}

void	Driver::AvoidOtherCars(
        int					index,
        const tCarElt*		car,
        const tSituation*	s,
        double				k,
        double*				carTargetSpd,
        double*				carTargetAcc,
        bool*				close,
        bool*				lapper )
{
    // double	trackLen = m_track.GetLength();
    // double	myPos = RtGetDistFromStart(const_cast<tCarElt*>(car));
    double	mySpd = hypot(car->_speed_X, car->_speed_Y);

    double	myDirX, myDirY;
    if( fabs(mySpd) < 0.01 )
    {
        myDirX = cos(car->_yaw);
        myDirY = sin(car->_yaw);
    }
    else
    {
        myDirX = car->_speed_X / mySpd;
        myDirY = car->_speed_Y / mySpd;
    }

    for( int i = 0; i < m_nCars; i++ )
    {
        m_opp[i].UpdatePath();

        PtInfo	oppPi;
        const tCarElt* oCar = m_opp[i].GetCar();
        GetPosInfo( oCar->race.distFromStartLine, oppPi );
        m_opp[i].UpdateSit( car, s, &m_pShared->m_teamInfo, myDirX, myDirY,	oppPi );
    }

    const Opponent::Sit&	mySit = m_opp[m_myOppIdx].GetInfo().sit;

    for( int i = 0; i < m_nCars; i++ )
    {
        m_opp[i].ProcessMyCar( s, &m_pShared->m_teamInfo, car, mySit, *this,
                               m_maxAccel.CalcY(car->_speed_x), i );
    }

    // accumulate all the collision the flags...
    Avoidance::Info	ai;
    double	minCatchTime = 99;
    double	minCatchAccTime = 99;
    double	minVCatTime = 99;
    *lapper = false;

    // double width = m_track.GetWidth();

    PtInfo	pi;
    GetPtInfo( PATH_NORMAL, car->_distFromStartLine, pi );
    ai.bestPathOffs = pi.offs;

    for( int i = 0; i < m_nCars; i++ )
    {
        Opponent::Info&	oi = m_opp[i].GetInfo();
        CarElt*			oCar = m_opp[i].GetCar();

        if( oCar != car && fabs(oi.sit.rdPX) < 25 )
            ai.nearbyCars++;
    }

    for( int i = 0; i < m_nCars; i++ )
    {
        Opponent::Info&	oi = m_opp[i].GetInfo();
        CarElt*			oCar = m_opp[i].GetCar();

        ai.flags |= oi.flags;

        if( oi.GotFlags(Opponent::F_FRONT) )
        {
            if( oi.flags & (Opponent::F_COLLIDE | Opponent::F_CATCHING |
                            Opponent::F_CATCHING_ACC) )
            {
            }

            bool dangerous = oi.GotFlags(Opponent::F_DANGEROUS);
            if( oi.GotFlags(Opponent::F_COLLIDE) && true )
            {
                LogSHADOW.debug( "[%d] catching: (%s) cdec %g (dgr %d), cspd %g (in %g s)\n",
                                 car->index, oCar->info.name, oi.catchDecel, dangerous, oi.catchSpd, oi.catchTime );

                // this is a really shitty estimate of the speed we need to be going
                // to slow down for the being caught ahead... would really like to work out a better
                // way.
                bool followingClosely = oi.sit.rdPX > 0 &&
                        oi.sit.rdPX < oi.sit.minDXa + 3 &&
                        fabs(oi.sit.rdPY) < oi.sit.minDY;

                bool dangerous = oi.GotFlags(Opponent::F_DANGEROUS);
                const double MIN_DECEL = 15.0;
                double decel =  (car->pub.speed - oi.catchSpd) / MX(oi.catchTime, 0.1);
                LogSHADOW.debug( "[%d] catching: (%s) decel %g (dgr %d)\n", car->index, oCar->info.name, oi.catchDecel, dangerous );
                LogSHADOW.debug( "[%d] catching: (%s) path spd: %g (%g) %g  path acc: %g (%g) %g\n",
                                 car->index, oCar->info.name, oi.sit.pi.spd, oi.sit.pi.spd - oCar->pub.DynGC.vel.x, oCar->pub.DynGC.vel.x,
                                 oi.sit.pi.acc, oi.sit.pi.acc - oCar->pub.DynGC.acc.x, oCar->pub.DynGC.acc.x );
                if( decel > MIN_DECEL || dangerous || followingClosely )
                {
                    double decelSpeed = MX(oi.catchSpd, car->pub.speed - 2 * s->deltaTime * decel);
                    if( dangerous )
                        decelSpeed = oi.catchSpd;
                    LogSHADOW.debug( "[%d] catching: (%s) myspd %g, decel spd %g, hisspd %g, catch (time %g  spd %g  dec %g)\n",
                                     car->index, oCar->info.name, car->pub.speed, decelSpeed, oCar->pub.speed, oi.catchTime, oi.catchSpd, oi.catchDecel );
                    if( car->pub.speed > decelSpeed )
                        LogSHADOW.debug( "[%d] slowing to avoid car: (%s) spd=%g\n",
                                         car->index, oCar->info.name, oCar->pub.speed );

                    ai.spdF = MN(ai.spdF, oi.catchSpd);
                    ai.accF = MN(ai.accF, -oi.catchDecel);
                }
            }

            if( oi.flags & (Opponent::F_COLLIDE | Opponent::F_CATCHING) )
                minCatchTime = MN(minCatchTime, oi.catchTime);

            if( oi.flags & Opponent::F_CATCHING_ACC )
                minCatchAccTime = MN(minCatchAccTime, oi.catchAccTime);

            if( oi.sit.rdVX < 0 )
            {
                double	vCatTime = -(oi.sit.rdPX - oi.sit.minDXa) / oi.sit.rdVX;

                if( vCatTime > 0 )
                    minVCatTime = MN(minVCatTime, vCatTime);
            }

            bool	ignoreTeamMate = oi.GotFlags(Opponent::F_TEAMMATE) &&
                    (car->_laps < oCar->_laps);

            oi.avoidLatchTime = MX(0, oi.avoidLatchTime - s->deltaTime);

            double	maxSpdK = 15.0 / (110 * 110);
            double	colTime = fabs(k) > maxSpdK ? 0.5 : 0.7;
            double	catTime = fabs(k) > maxSpdK ? 0.5 :	2.5;
            double	cacTime = fabs(k) > maxSpdK ? 0.5 : 2.5;
            bool	catching =
                    (oi.catchTime    < colTime && fabs(oi.catchY)    < 5 && oi.GotFlags(Opponent::F_COLLIDE))  ||
                    (oi.catchTime    < catTime && fabs(oi.catchY)    < 5 && oi.GotFlags(Opponent::F_CATCHING)) ||
                    (oi.catchAccTime < cacTime && fabs(oi.catchAccY) < 5 && oi.GotFlags(Opponent::F_CATCHING_ACC));
            if(( !ignoreTeamMate && oi.avoidLatchTime > 0 )|| catching || oi.GotFlags(Opponent::F_DANGEROUS))
            {
                LogSHADOW.debug( "%.3f catch %d (dgr %d)  catch[%d%d t %.3f y %.4g]  acc[%d t %.3f y %.4g]\n",
                                 oi.avoidLatchTime, catching, oi.GotFlags(Opponent::F_DANGEROUS),
                                 oi.GotFlags(Opponent::F_CATCHING), oi.GotFlags(Opponent::F_COLLIDE), oi.catchTime, oi.catchY,
                                 oi.GotFlags(Opponent::F_CATCHING_ACC), oi.catchAccTime, oi.catchAccY );
                double	toL, toR;
                GetPathToLeftAndRight( oCar, toL, toR );
                toL += oi.sit.tVY * oi.catchTime;
                toR -= oi.sit.tVY * oi.catchTime;
                bool	spaceL = toL > oi.sit.minDY;
                bool	spaceR = toR > oi.sit.minDY;
                bool	avoidL = oi.sit.rdPY < 0 && spaceR;
                bool	avoidR = oi.sit.rdPY > 0 && spaceL;

                if( catching )
                    oi.avoidLatchTime = fabs(k) < maxSpdK ? 0.5 : 0.1;

                if( fabs(k) < maxSpdK )
                {
                    if( !avoidL && !avoidR )
                    {
                        avoidL = !spaceL && spaceR;
                        avoidR = !spaceR && spaceL;
                    }
                }

                if( avoidL )
                {
                    ai.avoidAhead |= Opponent::F_LEFT;
                }

                if( avoidR )
                {
                    ai.avoidAhead |= Opponent::F_RIGHT;
                }

                if( avoidL )
                    ai.minLDist = MN(oi.sit.ragVX, ai.minLDist);

                if( avoidR )
                    ai.minRDist = MN(oi.sit.ragVX, ai.minRDist);
            }
        }

        if( oi.GotFlags(Opponent::F_TO_SIDE) )
        {
            int	av = oi.sit.rdPY < 0 ? Opponent::F_LEFT : Opponent::F_RIGHT;

            ai.avoidToSide |= av;

            if( oi.sit.rdPY < 0 )
            {
                ai.minLSideDist = MN(ai.minLSideDist, -oi.sit.rdPY - oi.sit.minDY);
            }
            else
            {
                ai.minRSideDist = MN(ai.minRSideDist,  oi.sit.rdPY - oi.sit.minDY);
            }

        }

        if( oi.GotFlags(Opponent::F_AHEAD) )
        {
            if( ai.pClosestAhead == 0 ||
                    ai.pClosestAhead->sit.rdPX > oi.sit.rdPX )
            {
                ai.pClosestAhead = &oi;
            }
        }


        bool	treatTeamMateAsLapper =
                oi.GotFlags(Opponent::F_TEAMMATE | Opponent::F_REAR) &&
                oi.sit.relPos > -25 &&
                car->_laps == oCar->_laps &&
                ai.nearbyCars <= 1;

        if( oi.GotFlags(Opponent::F_LAPPER) || treatTeamMateAsLapper )
        {
            int	av = oi.sit.rdPY < 0 ? Opponent::F_LEFT : Opponent::F_RIGHT;
            ai.avoidLapping |= av;
            *lapper = true;
        }
    }

    ai.k = k;
    ai.nextK = k;

    double	pos = car->_distFromStartLine;
    int		carIdx = m_track.IndexFromPos(m_track.CalcPos(car));
    ai.k = 	m_path[PATH_NORMAL].GetAt(carIdx).k;
    int		NSEG = m_track.GetSize();

    for( int i = 1; i < NSEG; i++ )
    {
        int	idx = (carIdx + i) % NSEG;
        double	thisK = m_path[PATH_NORMAL].GetAt(idx).k;

        if( fabs(thisK) > 0.01 )
        {
            ai.nextK = thisK;

            break;
        }
    }

    Avoidance avoidance;
    Vec2d	target = avoidance.calcTarget(ai, car, *this);

    *carTargetSpd = MN(*carTargetSpd, ai.spdF);
    *carTargetAcc = MN(*carTargetAcc, ai.accF);
    *close = (ai.flags & Opponent::F_CLOSE) != 0;

    if( m_flying || s->currentTime < START_HOLD_LINE_TIME )
        return;

    double	w = 7.5;
    double	scale = 25.0 / w;
    double	avoidSMaxA = 0.00075 * scale;
    double	avoidSMaxV = 0.005 * scale;
    double	avoidTMaxA = 0.0003 * scale;
    double	avoidTMaxV = 0.2 * scale;

    m_attractor = target.x;

    double	targetS = 1 - target.y;

    if ((m_avoidS != 1       && m_attractor == 0) ||
        (m_avoidS != targetS && m_attractor != 0))
    {
        targetS = (m_attractor == 0) ? 1 : 0;//0.35;
        double	avoidA = targetS > m_avoidS ? avoidSMaxA : -avoidSMaxA;

        double	dist = targetS - m_avoidS;
        if( fabs(dist) < 0.0005 )
            m_avoidSVel = 0;
        else
        {
            double	slowS = (m_avoidSVel * m_avoidSVel) / (2 * avoidSMaxA);
            if( fabs(dist) <= slowS )
            {
                avoidA = -(m_avoidSVel * m_avoidSVel) / (2 * dist);
            }

            m_avoidSVel += avoidA;
        }
    }
    else
        m_avoidSVel = 0;

    if( m_avoidSVel > avoidSMaxV )
        m_avoidSVel = avoidSMaxV;
    else if( m_avoidSVel < -avoidSMaxV )
        m_avoidSVel = -avoidSMaxV;

    double	oldAvoidS = m_avoidS;
    m_avoidS += m_avoidSVel;

    if( m_avoidS < 0.0005 && m_avoidSVel < 0 )
    {
        m_avoidS = 0;
        m_avoidSVel = 0;
    }
    else if( m_avoidS >= 0.9995 && m_avoidSVel > 0 )
    {
        m_avoidS = 1;
        m_avoidSVel = 0;
    }
    else if ((oldAvoidS < targetS && m_avoidS >= targetS) ||
             (oldAvoidS > targetS && m_avoidS <= targetS) ||
             fabs(targetS - m_avoidS) < 0.0005)
    {
        m_avoidS = targetS;
        m_avoidSVel = 0;
    }

    double	attractT = m_attractor;
    double	avoidA = 0;

    if( attractT != m_avoidT )
    {
        double	tMaxA = avoidTMaxA / MX(0.2, 1 - m_avoidS);
        avoidA = attractT > m_avoidT ? tMaxA : -tMaxA;
        double	dist = attractT - m_avoidT;
        double	slowS = (m_avoidTVel * m_avoidTVel) / (2 * avoidTMaxA);

        if( dist * m_avoidTVel > 0 && fabs(dist) <= slowS )
        {
            avoidA = -(m_avoidTVel * m_avoidTVel) / (2 * dist);
        }

        if( avoidA > avoidTMaxA )
            avoidA = avoidTMaxA;
        else if( avoidA < -avoidTMaxA )
            avoidA = -avoidTMaxA;

        m_avoidTVel += avoidA;
    }
    else
        m_avoidTVel = 0;

    double	tMaxV = avoidTMaxV / MX(0.2, 1 - m_avoidS);
    double	pos_tMaxV =  tMaxV;
    double	neg_tMaxV = -tMaxV;
    if( target.y != 0 &&
            (ai.flags & Opponent::F_DANGEROUS) == 0 &&
            (ai.flags & Opponent::F_TO_SIDE)   == 0 )
    {
        if( k >  0.0025 && mySpd + 3 > pi.spd )
        {
            pos_tMaxV = 0;//*= 0.001;
            LogSHADOW.debug( "-- right movement disallowed\n" );
        }
        if( k < -0.0025 && mySpd + 3 > pi.spd )
        {
            neg_tMaxV = 0;//*= 0.001;
            LogSHADOW.debug( "-- left movement disallowed\n" );
        }
    }

    if( m_avoidTVel > pos_tMaxV )
        m_avoidTVel = pos_tMaxV;
    else if( m_avoidTVel < neg_tMaxV )
        m_avoidTVel = neg_tMaxV;

    double	oldAvoidT = m_avoidT;
    m_avoidT += m_avoidTVel;

    if( m_avoidT < -1 )
    {
        m_avoidT = -1;
        m_avoidTVel = 0;
    }
    else if( m_avoidT > 1 )
    {
        m_avoidT = 1;
        m_avoidTVel = 0;
    }
    else if ((oldAvoidT < attractT && m_avoidT >= attractT) ||
             (oldAvoidT > attractT && m_avoidT <= attractT))
    {
        m_avoidT = attractT;
        m_avoidTVel = 0;
    }

    CalcPathOffset(pos, m_avoidS, m_avoidT);
}

int		Driver::CalcGear( tCarElt* car, double& acc )
{
    if( car->_gear <= 0 )
    {
        return 1;
    }

    const int	MAX_GEAR = car->_gearNb - 1;

    double	gr_dn = car->_gear > 1 ? car->_gearRatio[car->_gear + car->_gearOffset - 1] : 1e5;
    double	gr_this = car->_gearRatio[car->_gear + car->_gearOffset];
    double	wr = (car->_wheelRadius(2) + car->_wheelRadius(3)) / 2;
    double	rpm = gr_this * car->_speed_x / wr;
    double	rpmUp = m_gearUpRpm;
    double	rpmDn = rpmUp * gr_this * 0.95 / gr_dn;

    if( car->_gear < MAX_GEAR && rpm > rpmUp )
    {
        car->ctrl.clutchCmd = 0.5;//1.0;

        return car->_gear + 1;
    }
    else if( car->_gear > 1 && rpm < rpmDn )
    {
        car->ctrl.clutchCmd = 1.0;

        return car->_gear - 1;
    }

    return car->_gear;
}

double	Driver::ApplyAbs( tCarElt* car, double brake )
{
    if( car->_speed_x < 10 )
        return brake;

    double	slip = 0.0;
    for( int i = 0; i < 4; i++ )
        slip += car->_wheelSpinVel(i) * car->_wheelRadius(i) / car->_speed_x;
    slip /= 4.0;

    if( (m_cm[PATH_NORMAL].wheel(0).slipX() + m_cm[PATH_NORMAL].wheel(1).slipX()) * 0.5 > m_cm[PATH_NORMAL].TARGET_SLIP )
    {
        brake *= 0.5;
    }

    return brake;
}

double	Driver::ApplyTractionControl( tCarElt* car, double acc )
{
    double	spin = 0;
    double	wr = 0;
    int		count = 0;

    if( m_driveType == cDT_FWD || m_driveType == cDT_4WD )
    {
        spin += car->_wheelSpinVel(FRNT_LFT) * car->_wheelRadius(FRNT_LFT);
        spin += car->_wheelSpinVel(FRNT_RGT) * car->_wheelRadius(FRNT_RGT);
        wr += car->_wheelRadius(FRNT_LFT) + car->_wheelRadius(FRNT_RGT);
        count += 2;
    }

    if( m_driveType == cDT_RWD || m_driveType == cDT_4WD )
    {
        spin += car->_wheelSpinVel(REAR_LFT) * car->_wheelRadius(REAR_LFT);
        spin += car->_wheelSpinVel(REAR_RGT) * car->_wheelRadius(REAR_RGT);
        wr += car->_wheelRadius(REAR_LFT) + car->_wheelRadius(REAR_RGT);
        count += 2;
    }

    static double	tract = 1.0;

    spin /= count;

    if( car->_speed_x < 0.01 )
        return acc;

    double	slip = car->_speed_x / spin;

    if( slip > 1.1 )
    {
        tract = 0.1;

        wr /= count;
        // double	gr = car->_gearRatio[car->_gear + car->_gearOffset];
        // double rpmForSpd = gr * car->_speed_x / wr;
        acc = 0;
    }
    else
    {
        tract = MN(1.0, tract + 0.1);
    }

    acc = MN(acc, tract);

    return acc;
}

// Meteorology
//--------------------------------------------------------------------------*
void Driver::Meteorology(tTrack *t)
{
    tTrackSeg *Seg;
    tTrackSurface *Surf;
    rainintensity = 0;
    weathercode = GetWeather(t);
    Seg = t->seg;

    for ( int I = 0; I < t->nseg; I++)
    {
        Surf = Seg->surface;
        rainintensity = MAX(rainintensity, Surf->kFrictionDry / Surf->kFriction);
        LogSHADOW.debug("# %.4f, %.4f %s\n",Surf->kFriction, Surf->kRollRes, Surf->material);
        Seg = Seg->next;
    }

    rainintensity -= 1;

    if (rainintensity > 0)
    {
        rain = true;
        //mycardata->muscale *= 0.85;
        //mycardata->basebrake *= 0.75;
        //tcl_slip = MIN(tcl_slip, 2.0);
    }
    else
        rain = false;
}

//==========================================================================*
// Estimate weather
//--------------------------------------------------------------------------*
int Driver::GetWeather(tTrack *t)
{
    return (t->local.rain << 4) + t->local.water;
};

/*void Driver::calcSkill(tSituation *s)
{
    //if (RM_TYPE_PRACTICE != racetype)
    if (skill_adjust_timer == -1.0 || s->currentTime - skill_adjust_timer > skill_adjust_limit)
    {
        double rand1 = (double) getRandom() / 65536.0;  // how long we'll change speed for
        double rand2 = (double) getRandom() / 65536.0;  // the actual speed change
        double rand3 = (double) getRandom() / 65536.0;  // whether change is positive or negative
        LogSHADOW.debug(" # Random 1 = %.3f - Random 2 = %.3f - Random 3 = %.3f\n", rand1, rand2, rand3);

        // acceleration to use in current time limit
        decel_adjust_targ = (skill/4 * rand1);

        // brake to use - usually 1.0, sometimes less (more rarely on higher skill)
        brake_adjust_targ = MAX(0.85, 1.0 - MAX(0.0, skill/15 * (rand2-0.85)));

        // how long this skill mode to last for
        skill_adjust_limit = 5.0 + rand3 * 50.0;
        skill_adjust_timer = simtime;
    }

    if (decel_adjust_perc < decel_adjust_targ)
      decel_adjust_perc += MIN(deltaTime*4, decel_adjust_targ - decel_adjust_perc);
    else
      decel_adjust_perc -= MIN(deltaTime*4, decel_adjust_perc - decel_adjust_targ);

    if (brake_adjust_perc < brake_adjust_targ)
      brake_adjust_perc += MIN(deltaTime*2, brake_adjust_targ - brake_adjust_perc);
    else
      brake_adjust_perc -= MIN(deltaTime*2, brake_adjust_perc - brake_adjust_targ);

    LogSHADOW.debug("skill: decel %.3f - %.3f, brake %.3f - %.3f\n", decel_adjust_perc, decel_adjust_targ, brake_adjust_perc, brake_adjust_targ);
}*/

/*void Driver::SetRandomSeed(unsigned int seed)
{
    random_seed = seed ? seed : RANDOM_SEED;

    return;
}

unsigned int Driver::getRandom()
{
    random_seed = RANDOM_A * random_seed + RANDOM_C;
    LogSHADOW.debug("Random = %u\n", random_seed);

    return (random_seed >> 16);
}*/

//==========================================================================*
// Check if pit sharing is activated
//--------------------------------------------------------------------------*
bool Driver::CheckPitSharing(tCarElt *car)
{
  const tTrackOwnPit* OwnPit = car->_pit;           // Get my pit

  if (OwnPit == NULL)                            // If pit is NULL
  {                                              // nothing to do
      LogSHADOW.info(" #Pit = NULL\n\n");                 // here
    return false;
  }

  if (OwnPit->freeCarIndex > 1)
  {
      LogSHADOW.info(" #PitSharing = true\n\n");
      return true;
  }
  else
  {
      LogSHADOW.info(" #PitSharing = false\n\n");
      return false;
  }
}

