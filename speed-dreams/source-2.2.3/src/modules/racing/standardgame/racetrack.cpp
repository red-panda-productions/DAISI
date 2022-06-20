/***************************************************************************

    file        : racetrack.cpp
    copyright   : (C) 2010 by Xavier Bertaux
    web         : www.speed-dreams.org
    version     : $Id: racetrack.cpp 7165 2020-08-06 17:12:38Z torcs-ng $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** @file
            Track related functions
    @author	    Xavier Bertaux
    @version	$Id: racetrack.cpp 7165 2020-08-06 17:12:38Z torcs-ng $
*/

#include <cstdlib>
#include <cstdio>
#include <ctime>

#include <tgf.h>


#include <raceman.h>
#include <track.h>
#include <tracks.h>

#include "standardgame.h"
#include "racesituation.h"
#include "raceinit.h"
#include "racetrack.h"

#include "Mediator.h"


#ifdef WEBSERVER
#include <iomanip>
#include <fstream>
#include <sstream>
#include "racewebmetar.h"

static ReWebMetarCloud      webMetarCloud;
static ReWebMetarRunway     webMetarRunway;
static ReWebMetarVisibility webMetarVisibility;
static ReWebMetar           *webMetar = NULL;
#endif //WEBSERVER

// portability.h must be after curl.h (included by racewebmetar.h)
#include <portability.h>

// Local functions.
static void reTrackDump(const tTrack *track, int verbose);
static void reTrackInitTimeOfDay(void);
static void reTrackInitWeather(void);
static void reTrackInitWeatherValues(void);

#ifdef WEBSERVER
static void reTrackInitRealWeather(void);
static void reTrackInitSimuWeather(void);
#endif //WEBSERVER

static void reTrackUpdatePhysics(void);

/** Initialize the track for a race manager.
    @return <tt>0 ... </tt>Ok<br>
    <tt>-1 .. </tt>Error
*/
int
ReTrackInit(void)
{
    char buf[1024];

    // DAISI: Get the path for the track from the mediator
    SMediator* mediator = SMediator::GetInstance();
    const char* trackPath = mediator->GetEnvironmentFilePath();
    ReInfo->track = ReTrackLoader().load(trackPath);

    snprintf(buf, sizeof(buf), "Loading %s track", ReInfo->track->name);
    ReUI().addLoadingMessage(buf);

    reTrackInitTimeOfDay();

    const char* pszWeather =
        GfParmGetStr(ReInfo->params, ReInfo->_reRaceName, RM_ATTR_WEATHER, 0);

    if (!pszWeather)
         pszWeather = GfParmGetStr(ReInfo->params, RM_VAL_ANYRACE, RM_ATTR_WEATHER, RM_VAL_WEATHER_CONFIG);
#ifdef WEBSERVER
    if (!strcmp(pszWeather, RM_VAL_WEATHER_REAL))
        reTrackInitRealWeather();
    else if (!strcmp(pszWeather, RM_VAL_WEATHER_RECORDED))
        reTrackInitSimuWeather();
    else
#endif
        reTrackInitWeather();

    reTrackDump(ReInfo->track, 0);

    return 0;
}//ReTrackInit

/** Dump the track segments on screen
    @param  track track to dump
    @param  verbose if set to 1 all the segments are described (long)
    @ingroup  racemantools
 */
static void
reTrackDump(const tTrack *track, int verbose)
{
    char buf[128];

    snprintf(buf, sizeof(buf), "  by %s (%.0f m long, %.0f m wide) ...",
             track->authors, track->length, track->width);
    ReUI().addLoadingMessage(buf);

    GfLogInfo("++++++++++++ Track ++++++++++++\n");
    GfLogInfo("Name     = %s\n", track->name);
    GfLogInfo("Authors  = %s\n", track->authors);
    GfLogInfo("Filename = %s\n", track->filename);
    GfLogInfo("NSeg     = %d\n", track->nseg);
    GfLogInfo("Version  = %d\n", track->version);
    GfLogInfo("Length   = %f m\n", track->length);
    GfLogInfo("Width    = %f m\n", track->width);
    GfLogInfo("XSize    = %f m\n", track->max.x);
    GfLogInfo("YSize    = %f m\n", track->max.y);
    GfLogInfo("ZSize    = %f m\n", track->max.z);

    switch (track->pits.type)
    {
        case TR_PIT_NONE:
            GfLogInfo("Pits     = none\n");
            break;

        case TR_PIT_ON_TRACK_SIDE:
            GfLogInfo("Pits     = present on track side\n");
            break;

        case TR_PIT_ON_SEPARATE_PATH:
            GfLogInfo("Pits     = present on separate path\n");
            break;

        case TR_PIT_NO_BUILDING:
            GfLogInfo("Pits     = present, no building style\n");
            break;
    }//switch pits.type

    const int seconds = (int)track->local.timeofday;
    GfLogInfo("TimeOfDay= %02d:%02d:%02d\n", seconds / 3600, (seconds % 3600) / 60, seconds % 60);
    GfLogInfo("Sun asc. = %.1f d\n", RAD2DEG(track->local.sunascension));
    GfLogInfo("Clouds   = %d (0=none, 1=cirrus, 2=few, 3=many, 7=full)\n", track->local.clouds);
    GfLogInfo("Rain     = %d (0=none, 1=little, 2=medium, 3=heavy)\n", track->local.rain);
    GfLogInfo("Water    = %d (0=none, 1=some, 2=more, 3=swampy)\n", track->local.water);

    if (verbose)
    {
        int i;
        tTrackSeg *seg;
#ifdef SD_DEBUG
        const char  *stype[4] = { "", "RGT", "LFT", "STR" };
#endif

        for (i = 0, seg = track->seg->next; i < track->nseg; i++, seg = seg->next)
        {
            GfLogTrace("  segment %d -------------- \n", seg->id);
#ifdef SD_DEBUG
            GfLogTrace("        type    %s\n", stype[seg->type]);
#endif
            GfLogTrace("        length  %f m\n", seg->length);
            GfLogTrace("  radius  %f m\n", seg->radius);
            GfLogTrace("  arc %f d Zs %f d Ze %f d Zcs %f d\n", RAD2DEG(seg->arc),
                       RAD2DEG(seg->angle[TR_ZS]),
                       RAD2DEG(seg->angle[TR_ZE]),
                       RAD2DEG(seg->angle[TR_CS]));
            GfLogTrace(" Za  %f d\n", RAD2DEG(seg->angle[TR_ZS]));
            GfLogTrace("  vertices: %-8.8f %-8.8f %-8.8f ++++ ",
                       seg->vertex[TR_SR].x,
                       seg->vertex[TR_SR].y,
                       seg->vertex[TR_SR].z);
            GfLogTrace("%-8.8f %-8.8f %-8.8f\n",
                       seg->vertex[TR_SL].x,
                       seg->vertex[TR_SL].y,
                       seg->vertex[TR_SL].z);
            GfLogTrace("  vertices: %-8.8f %-8.8f %-8.8f ++++ ",
                       seg->vertex[TR_ER].x,
                       seg->vertex[TR_ER].y,
                       seg->vertex[TR_ER].z);
            GfLogTrace("%-8.8f %-8.8f %-8.8f\n",
                       seg->vertex[TR_EL].x,
                       seg->vertex[TR_EL].y,
                       seg->vertex[TR_EL].z);
            GfLogTrace("  prev    %d\n", seg->prev->id);
            GfLogTrace("  next    %d\n", seg->next->id);
        }//for i

        GfLogTrace("From Last To First\n");
        GfLogTrace("Dx = %-8.8f  Dy = %-8.8f Dz = %-8.8f\n",
                   track->seg->next->vertex[TR_SR].x - track->seg->vertex[TR_ER].x,
                   track->seg->next->vertex[TR_SR].y - track->seg->vertex[TR_ER].y,
                   track->seg->next->vertex[TR_SR].z - track->seg->vertex[TR_ER].z);
    }//if verbose
}//reTrackDump

// Initialize track time of day from race settings
void
reTrackInitTimeOfDay(void)
{
    static const char *TimeOfDayValues[] = RM_VALS_TIME;
    static const int NTimeOfDayValues = sizeof(TimeOfDayValues) / sizeof(const char*);

    tTrackLocalInfo *trackLocal = &ReInfo->track->local;

    // Load time of day settings for the session
    // (defaults to  "All sesions" one, or else "afternoon").
    int timeofday = RM_IND_TIME_AFTERNOON;
    const char* pszTimeOfDay =
        GfParmGetStr(ReInfo->params, ReInfo->_reRaceName, RM_ATTR_TIME_OF_DAY, 0);
    if (!pszTimeOfDay)
         pszTimeOfDay =
             GfParmGetStr(ReInfo->params, RM_VAL_ANYRACE, RM_ATTR_TIME_OF_DAY, RM_VAL_TIME_AFTERNOON);
    for (int i = 0; i < NTimeOfDayValues; i++)
        if (!strcmp(pszTimeOfDay, TimeOfDayValues[i]))
        {
            timeofday = i;
            break;
        }

    trackLocal->timeofdayindex = timeofday;
    switch (timeofday)
    {
        case RM_IND_TIME_DAWN:
            trackLocal->timeofday = 6 * 3600 + 13 * 60 + 20; // 06:13:20
            break;

        case RM_IND_TIME_MORNING:
            trackLocal->timeofday = 10 * 3600 + 0 * 60 + 0; // 10:00:00
            break;

        case RM_IND_TIME_NOON:
        case RM_IND_TIME_24HR:
            trackLocal->timeofday = 12 * 3600 + 0 * 60 + 0; // 12:00:00
            break;

        case RM_IND_TIME_AFTERNOON:
            trackLocal->timeofday = 15 * 3600 + 0 * 60 + 0; // 15:00:00
            break;

        case RM_IND_TIME_DUSK:
            trackLocal->timeofday = 17 * 3600 + 46 * 60 + 40; // 17:46:40
            break;

        case RM_IND_TIME_NIGHT:
            trackLocal->timeofday = 0 * 3600 + 0 * 60 + 0; // Midnight = 00:00:00
            break;
        case RM_IND_TIME_REAL:
        case RM_IND_TIME_NOW:
        {
            time_t t = time(0);
            struct tm *ptm = localtime(&t);
            trackLocal->timeofday = ptm->tm_hour * 3600.0f + ptm->tm_min * 60.0f + ptm->tm_sec;
            GfLogDebug("  Now time of day\n");
            break;
        }

        case RM_IND_TIME_TRACK:
            // Already loaded by the track loader (or else default value).
            GfLogDebug("  Track-defined time of day\n");
            break;

        case RM_IND_TIME_RANDOM:
            trackLocal->timeofday = (tdble)(rand() % (24*60*60));
            break;

        default:
            trackLocal->timeofday = 15 * 3600 + 0 * 60 + 0; // 15:00:00
            trackLocal->timeofdayindex = RM_IND_TIME_AFTERNOON;
            GfLogError("Unsupported value %d for user timeofday (assuming 15:00)\n",
                       timeofday);
            break;

    }//switch timeofday

}

// Initialize track weather info from race settings
void
reTrackInitWeather(void)
{
    static const char* CloudsValues[] = RM_VALS_CLOUDS;
    static const int NCloudsValues = sizeof(CloudsValues) / sizeof(const char*);

    static const char *RainValues[] = RM_VALS_RAIN;
    static const int NRainValues = sizeof(RainValues) / sizeof(const char*);

    tTrackLocalInfo *trackLocal = &ReInfo->track->local;

    // Load cloud cover settings for the session
    // (defaults to  "All sesions" one, or else "none").
    int clouds = TR_CLOUDS_NONE;
    const char* pszClouds =
        GfParmGetStr(ReInfo->params, ReInfo->_reRaceName, RM_ATTR_CLOUDS, 0);
    if (!pszClouds)
        pszClouds =
            GfParmGetStr(ReInfo->params, RM_VAL_ANYRACE, RM_ATTR_CLOUDS, RM_VAL_CLOUDS_NONE);
    for (int i = 0; i < NCloudsValues; i++)
        if (!strcmp(pszClouds, CloudsValues[i]))
        {
            clouds = i;
            break;
        }

    // Load rain fall (and track dry/wet conditions) settings for the session
    // if feature supported (defaults to  "All sesions" one, or else "none").
    int rain = TR_RAIN_NONE;
    if (ReInfo->s->_features & RM_FEATURE_WETTRACK)
    {
        const char* pszRain =
            GfParmGetStr(ReInfo->params, ReInfo->_reRaceName, RM_ATTR_RAIN, 0);
        if (!pszRain)
            pszRain =
                GfParmGetStr(ReInfo->params, RM_VAL_ANYRACE, RM_ATTR_RAIN, RM_VAL_RAIN_NONE);
        for (int i = 0; i < NRainValues; i++)
            if (!strcmp(pszRain, RainValues[i]))
            {
                rain = i;
                break;
            }
    }

    // Take care of the random case for rain falls and ground water.
    const bool bRandomRain = (rain == TR_RAIN_RANDOM);
    if (bRandomRain)
    {
        // Force random clouds, in case there is no rain at the end.
        clouds = TR_CLOUDS_RANDOM;

        // Random rain (if random[0,1] < trackLocal->anyrainlkhood, then it rains).
        const tdble randDraw = (tdble)(rand()/(double)RAND_MAX);

        GfLogTrace("Rain likelyhoods : overall=%.2f, little=%.2f, medium=%.2f\n",
                   trackLocal->anyrainlkhood, trackLocal->littlerainlkhood,
                   trackLocal->mediumrainlkhood);
        GfLogDebug("Overall rain random draw = %.2f,\n", randDraw);
        if (randDraw < trackLocal->anyrainlkhood)
        {
            // Now, let's determine how much it rains :
            // if random[0,1] < little rain likelyhood => rain = little rain
            const tdble randDraw2 = (tdble)(rand()/(double)RAND_MAX);
            GfLogDebug("Specific rain random draw = %.2f,\n", randDraw2);
            if (randDraw2 < trackLocal->littlerainlkhood)
                rain = TR_RAIN_LITTLE;
            // else if random[0,1] < medium + little rain likelyhood => rain = medium rain
            else if (randDraw2 <  trackLocal->littlerainlkhood + trackLocal->mediumrainlkhood)
                rain = TR_RAIN_MEDIUM;
            // otherwise, random[0,1] >= medium + little rain likelyhood => rain = Heavy rain
            else
                rain = TR_RAIN_HEAVY;
        }
        else
        {
            // No Rain.
            rain = TR_RAIN_NONE;
        }
    }

    // Take care of the random case for clouds cover.
    const bool bRandomClouds = (clouds == TR_CLOUDS_RANDOM);
    if (bRandomClouds)
    {
        if (rain != TR_RAIN_NONE)
        {
            // If any rain level, heavy clouds.
            clouds = TR_CLOUDS_FULL;
        }
        else
        {
            // Really random clouds.
            clouds = rand() % (TR_CLOUDS_FULL);
        }
    }

    // Ground water = rain for the moment (might change in the future).
    const int water = rain;

    GfLogInfo("Weather : Using %s rain (%d) and ground water (%d) + %s clouds (%d) settings\n",
              bRandomRain ? "random" : "user defined", rain, water,
              bRandomClouds ? "random" : "user defined", clouds);

    // Update track local info.
    trackLocal->rain = rain;
    trackLocal->hail = 0;
    trackLocal->snow = 0;
    trackLocal->clouds = clouds;
    trackLocal->cloud_altitude = 5500.0 * 0.3048;
    trackLocal->water = water;
    trackLocal->airtemperature = 15.0f;
    trackLocal->dewp = 5.0f;
    trackLocal->airpressure = 101300;
    trackLocal->airdensity = 1.219f;
    trackLocal->windspeed = (tdble)(rand() % 100);
    trackLocal->winddir = (tdble)(rand() % 359);
    trackLocal->relativehumidity = 65.0f;
    trackLocal->visibility = (tdble)(rand() % 12000);
    trackLocal->config = 2;

    reTrackInitWeatherValues();

    if ((trackLocal->visibility < 300) && (rain < 1))
        trackLocal->visibility = 300;

    GfLogDebug("Visibility = %.3f\n", trackLocal->visibility);
    GfLogDebug("Wind Speed = %.3f\n", trackLocal->windspeed);
    GfLogDebug("Wind direction = %.3f\n", trackLocal->winddir);
    GfLogDebug("Air Temperature = %.3f\n", trackLocal->airtemperature);
    GfLogDebug("Dew point = %.3f\n", trackLocal->dewp);
    GfLogDebug("Air pressure = %.3f\n", trackLocal->airpressure);
    GfLogDebug("Rain = %i\n", trackLocal->rain);
    GfLogDebug("Snow = %i\n", trackLocal->snow);
    GfLogDebug("Hail = %i\n", trackLocal->hail);
    GfLogDebug("Relative Humidity = %.3f\n", trackLocal->relativehumidity);

    // Update track physics from computed local info.
    ReTrackUpdate();
}

// Initialize Air temperature
void
reTrackInitWeatherValues(void)
{
    tTrackLocalInfo *trackLocal = &ReInfo->track->local;
    GfLogDebug("Start use current date ...\n");
    struct tm now;
    time_t now_sec = time(0);
#ifdef _WIN32
    now = *gmtime(&now_sec);
#else
    gmtime_r(&now_sec, &now);
#endif
    int month = now.tm_mon + 1;
    tdble temp = 0.0;

    switch (month)
    {
    case 1:
        temp = (tdble)(rand() % 10);
        temp -= 10.0;
        break;
    case 2:
        temp = (tdble)(rand() % 15);
        temp -= 10.0;
        break;
    case 3:
        temp = (tdble)(rand() % 18);
        temp -= 8.0;
        break;
    case 4:
        temp = (tdble)(rand() % 19);
        temp -= 5.0;
        break;
    case 5:
        temp = (tdble)(rand() % 22);
        temp -= 4.0;
        break;
    case 6:
        temp = (tdble)(rand() % 25);
        temp -= 3.0;
        break;
    case 7:
        temp = (tdble)(rand() % 30);
        temp-= 3.0;
        break;
    case 8:
        temp = (tdble)(rand() % 35);
        temp -= 3.0;
        break;
    case 9:
        temp = (tdble)(rand() % 30);
        break;
    case 10:
        temp = (tdble)(rand() % 25);
        break;
    case 11:
        temp = (tdble)(rand() % 20);
        temp -= 5.0;
        break;
    case 12:
        temp = (tdble)(rand() % 15);
        temp -= 10.0;
        break;
    default:
        temp = (tdble)(rand() % 25);
    }

    trackLocal->airtemperature = temp;
}

#ifdef WEBSERVER
// Initialize track weather info from race settings
void
reTrackInitRealWeather(void)
{
    std::string url = "ftp://tgftp.nws.noaa.gov/data/observations/metar/stations/";
    webMetar = new ReWebMetar;
    tTrackLocalInfo *trackLocal = &ReInfo->track->local;

    url += trackLocal->station;
    url += ".TXT";

    GfLogInfo("URL WEATHER : %s\n", url.c_str());
    bool w = webMetar->ReWebMetarFtp(url);

    if (w == false)
        reTrackInitSimuWeather();
    else
    {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "%sconfig/weather.txt", GetLocalDir());

        std::string data = buffer;
        GfLogDebug("Path weather.txt : %s\n", data.c_str());

        std::ifstream file(data.c_str());

        if (!file.is_open()) {
            GfLogError("Failed to open %s\n", data.c_str());
            return;
        }

        // compter le nombre de lignes
        int count = 0;
        std::string line;
        std::string weather;
        while (true)
        {
            getline(file, line);
            //GfLogInfo("Line = %s\n", line.c_str());

            if (file.eof())
            {
                break;
            }

            weather += line;
            ++count;
        }

        file.close();

        GfLogDebug("Contenu weather.txt = %s\n", weather.c_str());

        webMetar->ReWebMetarLoad(weather);

        if (webMetar->getCAVOK())
        {
            if (webMetar->getVisibility_m() == WebMetarNaN || webMetar->getVisibility_m() < 0)
                webMetarVisibility.set(12000.0);

            if (webMetar->getCloudNumber() > 0)
            {
                trackLocal->clouds = 0;
                trackLocal->altitude = (tdble)(1676.40);
                //_clouds.push_back(cl);
            }
        }

        // visibility
        tdble _wind_range_from = 0.0;
        tdble _wind_range_to = 0.0;
        tdble d = (tdble)(webMetar->getVisibility_m());

        if (d < 0.0)
            d = 12000.0;

        GfLogDebug("WebMetar Visibility in racetrack = %.3f\n", webMetar->getVisibility_m());

        if (d == WebMetarNaN )
            d = 10000.0;

        if (webMetarVisibility.getModifier() == ReWebMetarVisibility::GREATER_THAN)
            d += 2000.0;// * sg_random();

        if(d > 15000)
            d = 12000.0;

        if (d < 350)
            d = 350.0;

        trackLocal->visibility = d;

        // wind
        if (webMetar->getWindDir() == -1)
        {
            if (webMetar->getWindRangeTo() == -1)
            {
                trackLocal->winddir = 0;
                _wind_range_from = 0;
                _wind_range_to = 359;
            }
            else
            {
                trackLocal->winddir = (_wind_range_from + _wind_range_to) / 2;
            }
        }
        else if (webMetar->getWindRangeFrom() == -1)
        {
            _wind_range_from = _wind_range_to = trackLocal->winddir;
        }

        if (webMetar->getWindSpeed_kmh() == WebMetarNaN)
            trackLocal->windspeed = 0.0;
        else
            trackLocal->windspeed = (tdble)(webMetar->getWindSpeed_kmh());

        // clouds
        int cn = webMetar->getCloudNumber();

        if(cn > 0)
        {
            for (int i = 0; i < cn; i++)
            {
                switch (i)
                {
                case 0:
                    trackLocal->clouds = webMetar->getCloud1();
                    trackLocal->cloud_altitude = webMetar->getAltitude1();
                    GfLogDebug("Clouds 1 = %i - Alitude cloud 1 = %.3f\n", trackLocal->clouds, trackLocal->cloud_altitude);
                    break;
                case 1:
                    trackLocal->clouds2 = webMetar->getCloud2();
                    trackLocal->cloud_altitude2 = webMetar->getAltitude2();
                    GfLogDebug("Clouds 2 = %i - Alitude cloud 2 = %.3f\n", trackLocal->clouds2, trackLocal->cloud_altitude2);
                    break;
                case 3:
                    trackLocal->clouds3 = webMetar->getCloud3();
                    trackLocal->cloud_altitude3 = webMetar->getAltitude3();
                    GfLogDebug("Clouds 3 = %i - Alitude cloud 3 = %.3f\n", trackLocal->clouds3, trackLocal->cloud_altitude3);
                    break;
                }
            }
        }

        // temperature/pressure/density
        if (webMetar->getTemperature_C() == WebMetarNaN)
            trackLocal->airtemperature = 15.0;
        else
            trackLocal->airtemperature = (tdble)(webMetar->getTemperature_C());

        if (webMetar->getDewpoint_C() == WebMetarNaN)
            trackLocal->dewp = 0.0;
        else
            trackLocal->dewp = (tdble)(webMetar->getDewpoint_C());

        if (webMetar->getPressure_hPa() == WebMetarNaN)
            trackLocal->airpressure = (tdble)(30.0 * 3386.388640341);
        else
            trackLocal->airpressure = (tdble)(webMetar->getPressure_hPa());

        trackLocal->airpressure = (tdble)(trackLocal->airpressure * 100);

        if (webMetar->getDensity_C() == WebMetarNaN)
            trackLocal->airdensity = 1.219f;
        else
            trackLocal->airdensity = (tdble)(webMetar->getDensity_C());

        if (ReInfo->s->_features & RM_FEATURE_WETTRACK)
        {
            trackLocal->rain = webMetar->getRain();
            trackLocal->water = trackLocal->rain;

            trackLocal->snow = webMetar->getSnow();

            if (trackLocal->snow > 0)
                trackLocal->water = trackLocal->snow;

            trackLocal->hail = webMetar->getHail();

            if (trackLocal->hail > 0)
                trackLocal->water = trackLocal->hail;

            trackLocal->relativehumidity = (tdble)(webMetar->getRelHumidity());
        }
        else
        {
            trackLocal->rain = TR_RAIN_NONE;
            trackLocal->snow = TR_RAIN_NONE;
            trackLocal->hail = TR_RAIN_NONE;
            trackLocal->relativehumidity = TR_RAIN_NONE;
            trackLocal->water = TR_RAIN_NONE;
        }

        trackLocal->config = 0;

        GfLogDebug("Visibility = %.3f\n", trackLocal->visibility);
        GfLogDebug("Wind Speed = %.3f\n", trackLocal->windspeed);
        GfLogDebug("Wind direction = %.3f\n", trackLocal->winddir);
        GfLogDebug("Air Temperature = %.3f\n", trackLocal->airtemperature);
        GfLogDebug("Dew point = %.3f\n", trackLocal->dewp);
        GfLogDebug("Air pressure = %.3f\n", trackLocal->airpressure);
        GfLogDebug("Air Density = %.3f\n", trackLocal->airdensity);
        GfLogDebug("Rain = %i\n", trackLocal->rain);
        GfLogDebug("Snow = %i\n", trackLocal->snow);
        GfLogDebug("Hail = %i\n", trackLocal->hail);
        GfLogDebug("Water track = %d\n", trackLocal->water);
        GfLogDebug("Relative Humidity = %.3f\n", trackLocal->relativehumidity);

        ReTrackUpdate();
    }
}

// Initialize track weather info from race settings
void
reTrackInitSimuWeather(void)
{
    webMetar = new ReWebMetar;
    tTrackLocalInfo *trackLocal = &ReInfo->track->local;

    struct tm now;
    time_t now_sec = time(0);
#ifdef _WIN32
    now = *gmtime(&now_sec);
#else
    gmtime_r(&now_sec, &now);
#endif

    std::ostringstream weatherfile;
    weatherfile << GetDataDir() << "data/weather/" << trackLocal->station << "/"
                << std::setw(2) << std::setfill('0') << std::right << now.tm_mday << "-"
                << std::setw(2) << std::setfill('0') << std::right << (now.tm_mon + 1) << ".txt";

    GfLogDebug("Path file weather : %s\n", weatherfile.str().c_str());

    std::ifstream file(weatherfile.str().c_str());

    if (!file.is_open())
    {
        GfLogError("Failed to open %s\n", weatherfile.str().c_str());
        int clouds = TR_CLOUDS_NONE;
        int rain = TR_RAIN_NONE;
        // Really random clouds.
        clouds = rand() % (TR_CLOUDS_FULL);

        // Update track local info.
        trackLocal->rain = rain;
        trackLocal->hail = 0;
        trackLocal->snow = 0;
        trackLocal->clouds = clouds;
        trackLocal->cloud_altitude = 5500.0 * 0.3048;
        trackLocal->water = rain;
        trackLocal->airtemperature = 15.0f;
        trackLocal->dewp = 5.0f;
        trackLocal->airpressure = 101300;
        trackLocal->airdensity = 1.219f;
        trackLocal->windspeed = (tdble)(rand() % 100);
        trackLocal->winddir = (tdble)(rand() % 359);
        trackLocal->relativehumidity = 65.0f;
        trackLocal->visibility = (tdble)(rand() % 12000);

        reTrackInitWeatherValues();

        if ((trackLocal->visibility < 300) && (rain < 1))
            trackLocal->visibility = 300;

        trackLocal->config = 1;

        ReTrackUpdate();

        return;
    }

    // compter le nombre de lignes
    int count = 0;
    std::string line;
    std::string weather;
    while (true)
    {
        getline(file, line);
        //GfLogInfo("Line = %s\n", line.c_str());

        if (file.eof())
        {
            break;
        }

        weather += line;
        ++count;
    }

    file.close();
    GfLogDebug("Contenu weather.txt = %s\n", weather.c_str());

    webMetar->ReWebMetarLoad(weather);

    if (webMetar->getCAVOK())
    {
        if (webMetar->getVisibility_m() == WebMetarNaN)
            webMetarVisibility.set(12000.0);

        if (webMetar->getCloudNumber() > 0)
        {
            trackLocal->clouds = 0;
            trackLocal->altitude = (tdble)(5500 * 0.3048);
            //_clouds.push_back(cl);
        }
    }

    // visibility
    tdble _wind_range_from = 0.0;
    tdble _wind_range_to = 0.0;
    tdble d = (tdble)(webMetar->getVisibility_m());
    GfLogDebug("WebMetar Visibility in racetrack = %.3f\n", webMetar->getVisibility_m());

    if (d < 0.0)
        d = 12000.0;

    if (d == WebMetarNaN )
        d = 10000.0;

    if (webMetarVisibility.getModifier() == ReWebMetarVisibility::GREATER_THAN)
        d += 2000.0;// * sg_random();

    if(d > 15000)
        d = 12000.0;

    if (d < 350)
        d = 350.0;

    trackLocal->visibility = d;

    // wind
    if (webMetar->getWindDir() == -1)
    {
        if (webMetar->getWindRangeTo() == -1)
        {
            trackLocal->winddir = 0.0;
            _wind_range_from = 0.0;
            _wind_range_to = 359.0;
        }
        else
        {
            trackLocal->winddir = (_wind_range_from + _wind_range_to) / 2;
        }
    }
    else if (webMetar->getWindRangeFrom() == -1)
    {
        _wind_range_from = _wind_range_to = trackLocal->winddir;
    }

    if (webMetar->getWindSpeed_kmh() == WebMetarNaN)
        trackLocal->windspeed = 0.0;
    else
        trackLocal->windspeed = (tdble)(webMetar->getWindSpeed_kmh());

    // clouds
    int cn = webMetar->getCloudNumber();

    if(cn > 0)
    {
        for (int i = 0; i < cn; i++)
        {
            switch (i)
            {
            case 0:
                trackLocal->clouds = webMetar->getCloud1();
                trackLocal->cloud_altitude = webMetar->getAltitude1();
                GfLogDebug("Clouds 1 = %i - Alitude cloud 1 = %.3f\n", trackLocal->clouds, trackLocal->cloud_altitude);
                break;
            case 1:
                trackLocal->clouds2 = webMetar->getCloud2();
                trackLocal->cloud_altitude2 = webMetar->getAltitude2();
                GfLogDebug("Clouds 2 = %i - Alitude cloud 2 = %.3f\n", trackLocal->clouds2, trackLocal->cloud_altitude2);
                break;
            case 3:
                trackLocal->clouds3 = webMetar->getCloud3();
                trackLocal->cloud_altitude3 = webMetar->getAltitude3();
                GfLogDebug("Clouds 3 = %i - Alitude cloud 3 = %.3f\n", trackLocal->clouds3, trackLocal->cloud_altitude3);
                break;
            }
        }
    }

    // temperature/pressure
    if (webMetar->getTemperature_C() == WebMetarNaN)
        trackLocal->airtemperature = 15.0f;
    else
        trackLocal->airtemperature = (tdble)(webMetar->getTemperature_C());

    if (webMetar->getDewpoint_C() == WebMetarNaN)
        trackLocal->dewp = 0.0;
    else
        trackLocal->dewp = (tdble)(webMetar->getDewpoint_C());

    if (webMetar->getPressure_hPa() == WebMetarNaN)
        trackLocal->airpressure = (tdble)(30.0 * 3386.388640341);
    else
        trackLocal->airpressure = (tdble)(webMetar->getPressure_hPa());

    trackLocal->airpressure = (tdble)(trackLocal->airpressure * 100);

    if (webMetar->getDensity_C() == WebMetarNaN)
        trackLocal->airdensity = 1.219f;
    else
        trackLocal->airdensity = (tdble)(webMetar->getDensity_C());

    if (ReInfo->s->_features & RM_FEATURE_WETTRACK)
    {
        trackLocal->rain = webMetar->getRain();
        trackLocal->water = trackLocal->rain;

        trackLocal->snow = webMetar->getSnow();

        if (trackLocal->snow > 0)
            trackLocal->water = trackLocal->snow;

        trackLocal->hail = webMetar->getHail();

        if (trackLocal->hail > 0)
            trackLocal->water = trackLocal->hail;

        trackLocal->relativehumidity = (tdble)(webMetar->getRelHumidity());
    }
    else
    {
        trackLocal->rain = TR_RAIN_NONE;
        trackLocal->snow = TR_RAIN_NONE;
        trackLocal->hail = TR_RAIN_NONE;
        trackLocal->relativehumidity = TR_RAIN_NONE;
    }

    GfLogDebug("Visibility = %.3f\n", trackLocal->visibility);
    GfLogDebug("Wind Speed = %.3f\n", trackLocal->windspeed);
    GfLogDebug("Wind direction = %.3f\n", trackLocal->winddir);
    GfLogDebug("Air Temperature = %.3f\n", trackLocal->airtemperature);
    GfLogDebug("Dew point = %.3f\n", trackLocal->dewp);
    GfLogDebug("Air pressure = %.3f\n", trackLocal->airpressure);
    GfLogDebug("Air Density = %.3f\n", trackLocal->airdensity);
    GfLogDebug("Rain = %i\n", trackLocal->rain);
    GfLogDebug("Snow = %i\n", trackLocal->snow);
    GfLogDebug("Hail = %i\n", trackLocal->hail);
    GfLogDebug("Relative Humidity = %.3f\n", trackLocal->relativehumidity);
    GfLogDebug("Water track = %d\n", trackLocal->water);

    ReTrackUpdate();
}
#endif //WEBSERVER

// Update track info ...
void
ReTrackUpdate(void)
{
    // TODO: New weather conditions when starting a new event ?

    reTrackUpdatePhysics();
}

// Update Track Physics (compute kFriction from current "water level" on ground).
void
reTrackUpdatePhysics(void)
{
    tTrackLocalInfo *trackLocal = &ReInfo->track->local;

    // Get the wet / dry friction coefficients ratio.
    void* hparmTrackConsts =
        GfParmReadFile(TRK_PHYSICS_FILE, GFPARM_RMODE_STD | GFPARM_RMODE_CREAT);
    const tdble kFrictionWetDryRatio =
        GfParmGetNum(hparmTrackConsts, TRKP_SECT_SURFACES, TRKP_VAL_FRICTIONWDRATIO, (char*)NULL, 0.5f);
    GfParmReleaseHandle(hparmTrackConsts);

    // Determine the "wetness" of the track (inside  [0, 1]).
    const tdble wetness = (tdble)trackLocal->water / TR_WATER_MUCH;

    GfLogDebug("ReTrackUpdate : water = %d, wetness = %.2f, wet/dry mu = %.4f\n",
               trackLocal->water, wetness, kFrictionWetDryRatio);

    // Set the actual friction for each _ground_ surface of the track.
    GfLogDebug("ReTrackUpdate : kFriction | kRollRes | Surface :\n");
    tTrackSurface *curSurf;
    curSurf = ReInfo->track->surfaces;
    do
    {
        // Linear interpolation of kFriction from dry to wet according to wetness.
        curSurf->kFriction =
            curSurf->kFrictionDry * (1 - wetness)
            + curSurf->kFrictionDry * kFrictionWetDryRatio * wetness;

        // For the moment, we don't change curSurf->kRollRes (might change in the future).

        GfLogDebug("                   %.4f |   %.4f | %s\n",
                   curSurf->kFriction, curSurf->kRollRes, curSurf->material);

        curSurf = curSurf->next;

    } while ( curSurf );
}

/** Shutdown the track for a race manager.
    @return <tt>0 ... </tt>Ok<br>
    <tt>-1 .. </tt>Error
*/
int
ReTrackShutdown(void)
{
#ifdef WEBSERVER
    if(webMetar)
    {
        delete webMetar;
        webMetar = 0;
    }
#endif //WEBSERVER
    return 0;
}
