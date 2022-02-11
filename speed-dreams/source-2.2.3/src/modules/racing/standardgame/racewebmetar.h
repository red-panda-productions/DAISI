/***************************************************************************
                    webmetar.h -- Interface file for Real Metar
                             -------------------
    created              : 09/03/2019
    copyright            : (C) 2019 by torcs-ng
    email                : bertauxx@gmail.com
    version              : $Id$
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef WEBSERVER

#ifndef _SD_REWEBMETAR_H_
#define _SD_REWEBMETAR_H_

#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <curl/curl.h>

// url https://tgftp.nws.noaa.gov/data/observations/metar/stations/
struct Token
{
    const char *id;
    const char *text;
};

const double WebMetarNaN = -1E20;

//class WebMetar;

class ReWebMetarVisibility
{
    friend class ReWebMetar;
public:
    ReWebMetarVisibility() :
        _distance(WebMetarNaN),
        _direction(-1),
        _modifier(EQUALS),
        _tendency(NONE) {}

    enum Modifier
    {
        NOGO,
        EQUALS,
        LESS_THAN,
        GREATER_THAN
    };

    enum Tendency
    {
        NONE,
        STABLE,
        INCREASING,
        DECREASING
    };

    void set(double dist, int dir = -1, int mod = -1, int tend = -1);

    inline double	getVisibility_m()	const { return _distance; }
    inline double	getVisibility_ft()	const { return _distance == WebMetarNaN ? WebMetarNaN : _distance * 3.28083989501312335958; }
    inline double	getVisibility_sm()	const { return _distance == WebMetarNaN ? WebMetarNaN : _distance * 0.0006213699494949496; }
    inline int	getDirection()		const { return _direction; }
    inline int	getModifier()		const { return _modifier; }
    inline int	getTendency()		const { return _tendency; }

protected:
    double	_distance;
    int	_direction;
    int	_modifier;
    int	_tendency;
};


// runway condition (surface and visibility)
class ReWebMetarRunway
{
    friend class ReWebMetar;
public:
    ReWebMetarRunway() :
        _deposit(-1),
        _deposit_string(0),
        _extent(-1),
        _extent_string(0),
        _depth(WebMetarNaN),
        _friction(WebMetarNaN),
        _friction_string(0),
        _comment(0),
        _wind_shear(false) {}

    inline int			    getDeposit()		const { return _deposit; }
    inline const char		*getDepositString()	const { return _deposit_string; }
    inline double			getExtent()		    const { return _extent; }
    inline const char		*getExtentString()	const { return _extent_string; }
    inline double			getDepth()		    const { return _depth; }
    inline double			getFriction()		const { return _friction; }
    inline const char		*getFrictionString()  const { return _friction_string; }
    inline const char		*getComment()		const { return _comment; }
    inline       bool		getWindShear()		const { return _wind_shear; }
    inline const            ReWebMetarVisibility&	getMinVisibility()	const { return _min_visibility; }
    inline const            ReWebMetarVisibility&	getMaxVisibility()	const { return _max_visibility; }

protected:
    ReWebMetarVisibility _min_visibility;
    ReWebMetarVisibility _max_visibility;
    int		           _deposit;
    const char*        _deposit_string;
    int		           _extent;
    const char*        _extent_string;
    double		       _depth;
    double		       _friction;
    const char*        _friction_string;
    const char*        _comment;
    bool		       _wind_shear;
};

// cloud layer
class ReWebMetarCloud
{
    friend class ReWebMetar;
public:
    enum Coverage
    {
        COVERAGE_NIL = -1,
        COVERAGE_CLEAR = 0,
        COVERAGE_CIRRUS = 1,
        COVERAGE_FEW = 2,
        COVERAGE_MANY = 3,
        COVERAGE_CUMULUS = 4,
        COVERAGE_SCATTERED = 5,
        COVERAGE_BROKEN = 6,
        COVERAGE_OVERCAST = 7
    };

    static const char * COVERAGE_NIL_STRING;
    static const char * COVERAGE_CLEAR_STRING;
    static const char * COVERAGE_CIRRUS_STRING;
    static const char * COVERAGE_FEW_STRING;
    static const char * COVERAGE_MANY_STRING;
    static const char * COVERAGE_CUMULUS_STRING;
    static const char * COVERAGE_SCATTERED_STRING;
    static const char * COVERAGE_BROKEN_STRING;
    static const char * COVERAGE_OVERCAST_STRING;

    ReWebMetarCloud() : _coverage(COVERAGE_NIL), _altitude(WebMetarNaN), _type(0), _type_long(0) {}

    void set(double alt, Coverage cov = COVERAGE_NIL );

    inline Coverage		getCoverage()          const { return _coverage; }
    static Coverage		getCoverage(           const std::string & coverage );
    inline double		getAltitude_m()          const { return _altitude; }
    inline double		getAltitude_ft()         const { return _altitude == WebMetarNaN ? WebMetarNaN : _altitude * 3.28083989501312335958; }
    inline const char*	getTypeString()     const { return _type; }
    inline const char*	getTypeLongString() const { return _type_long; }

protected:
    Coverage			_coverage;     // quarters: 0 -> clear ... 4 -> overcast
    double				_altitude;       // 1000 m
    const char*			_type;      // CU
    const char*			_type_long; // cumulus
};

class ReWebMetar
{
public:
    ReWebMetar();
    ~ReWebMetar();

    bool ReWebMetarFtp(const std::string& m);
    void ReWebMetarLoad(const std::string& m);

    enum ReportType
    {
        NONE,
        AUTO,
        COR,
        RTD
    };

    enum Intensity
    {
        NIL = 0,
        LIGHT = 1,
        MODERATE = 2,
        HEAVY = 3
    };

    struct Weather
    {
        Weather() { intensity = NIL; vincinity = false; }
        Intensity intensity;
        bool      vincinity;
        std::vector<std::string> descriptions;
        std::vector<std::string> phenomena;
    };

    inline const char	*getData()			const { return _data; }
    inline const char	*getUnusedData()	const { return _m; }
    inline       bool	getProxy()			const { return _x_proxy; }
    inline const char	*getId()		    const { return _icao; }
    inline int			getYear()		    const { return _year; }
    inline int			getMonth()		    const { return _month; }
    inline int			getDay()		    const { return _day; }
    inline int			getHour()		    const { return _hour; }
    inline int			getMinute()		    const { return _minute; }
    inline int			getReportType()		const { return _report_type; }
    inline int			getCloudNumber()	const { return _cloudnumber; }
    inline int			getCloud1()      	const { return _cloud1; }
    inline int			getCloud2()        	const { return _cloud2; }
    inline int			getCloud3()       	const { return _cloud3; }

    inline double       getAltitude1()        const { return _cloud_altitude1; }
    inline double       getAltitude2()        const { return _cloud_altitude2; }
    inline double       getAltitude3()        const { return _cloud_altitude3; }

    inline int			getWindDir()		const { return _wind_dir; }
    inline double		getWindSpeed_mps()	const { return _wind_speed; }
    inline double		getWindSpeed_kmh()	const { return _wind_speed == WebMetarNaN ? WebMetarNaN : _wind_speed * 3.6; }
    inline double		getWindSpeed_kt()	const { return _wind_speed == WebMetarNaN ? WebMetarNaN : _wind_speed * 1.9438444924406046432; }
    inline double		getWindSpeed_mph()	const { return _wind_speed == WebMetarNaN ? WebMetarNaN : _wind_speed * 2.2369362920544020312; }

    inline double		getGustSpeed_mps()	const { return _gust_speed; }
    inline double		getGustSpeed_kmh()	const { return _gust_speed == WebMetarNaN ? WebMetarNaN : _gust_speed * 3.6; }
    inline double		getGustSpeed_kt()	const { return _gust_speed == WebMetarNaN ? WebMetarNaN : _gust_speed * 1.9438444924406046432; }
    inline double		getGustSpeed_mph()	const { return _gust_speed == WebMetarNaN ? WebMetarNaN : _gust_speed * 2.2369362920544020312; }

    inline double		getVisibility_m()	const { return _distance; }

    inline int			getWindRangeFrom()	const { return _wind_range_from; }
    inline int			getWindRangeTo()	const { return _wind_range_to; }

    inline const ReWebMetarVisibility&    getMinVisibility()	const { return _min_visibility; }
    inline const ReWebMetarVisibility&    getMaxVisibility()	const { return _max_visibility; }
    inline const ReWebMetarVisibility&    getVertVisibility()	const { return _vert_visibility; }
    inline const ReWebMetarVisibility*    getDirVisibility()	const { return _dir_visibility; }

    inline double		getTemperature_C()	const { return _temp; }
    inline double		getTemperature_F()	const { return _temp == WebMetarNaN ? WebMetarNaN : 1.8 * _temp + 32; }
    inline double		getDewpoint_C()		const { return _dewp; }
    inline double		getDewpoint_F()		const { return _dewp == WebMetarNaN ? WebMetarNaN : 1.8 * _dewp + 32; }
    inline double		getPressure_hPa()	const { return _pressure == WebMetarNaN ? WebMetarNaN : _pressure / 100; }
    inline double		getPressure_inHg()	const { return _pressure == WebMetarNaN ? WebMetarNaN : _pressure * 0.0002952998330101010; }
    inline double       getDensity_C()      const { return _density; }

    inline int			getRain()			const { return _rain; }
    inline int			getHail()			const { return _hail; }
    inline int			getSnow()			const { return _snow; }
    inline bool			getCAVOK()			const { return _cavok; }

    double				getRelHumidity()	const;

    inline const std::vector<ReWebMetarCloud>& getClouds()				const	{ return _clouds; }
    inline const std::map<std::string, ReWebMetarRunway>& getRunways()	const	{ return _runways; }
    inline const std::vector<std::string>& getWeather()					const	{ return _weather; }
    inline const std::vector<struct Weather> getWeather2()				const   { return _weather2; }

protected:
    std::string	_url;
    int			_grpcount;
    bool		_x_proxy;
    char*		_data;
    char*		_m;
    char		_icao[5];
    int			_year;
    int			_month;
    int			_day;
    int			_hour;
    int			_minute;
    int			_report_type;
    int			_wind_dir;
    double		_wind_speed;
    double		_gust_speed;
    int			_wind_range_from;
    int			_wind_range_to;
    double		_temp;
    double		_dewp;
    double      _distance;
    double		_pressure;
    double      _density;
    int			_rain;
    int			_hail;
    int			_snow;
    int         _cloudnumber;
    int         _cloud1;
    double      _cloud_altitude1;
    int         _cloud2;
    double      _cloud_altitude2;
    int         _cloud3;
    double      _cloud_altitude3;
    bool		_cavok;
    std::vector<struct Weather> _weather2;

    ReWebMetarVisibility				_min_visibility;
    ReWebMetarVisibility				_max_visibility;
    ReWebMetarVisibility				_vert_visibility;
    ReWebMetarVisibility				_dir_visibility[8];
    std::vector<ReWebMetarCloud>		_clouds;
    std::map<std::string, ReWebMetarRunway>	_runways;
    std::vector<std::string>		    _weather;

    bool	scanPreambleDate();
    bool	scanPreambleTime();
    void	useCurrentDate();

    bool	scanType();
    bool	scanId();
    bool	scanDate();
    bool	scanModifier();
    bool	scanWind();
    bool	scanVariability();
    bool	scanVisibility();
    bool	scanRwyVisRange();
    bool	scanSkyCondition();
    bool	scanWeather();
    bool	scanTemperature();
    bool	scanPressure();
    bool	scanRunwayReport();
    bool	scanWindShear();
    bool	scanTrendForecast();
    bool	scanColorState();
    bool	scanRemark();
    bool	scanRemainder();

    int	    scanNumber(char **str, int *num, int min, int max = 0);
    bool	scanBoundary(char **str);
    const struct Token *scanToken(char **str, const struct Token *list);
    void	normalizeData();
    void    density();
};

#endif //_SD_REWEBMETAR_H_
#endif //WEBSERVER
