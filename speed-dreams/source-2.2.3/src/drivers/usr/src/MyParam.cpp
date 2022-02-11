/***************************************************************************

    file        : MyParam.cpp
    created     : 18 Mar 2019
    copyright   : (C) 2019 D.Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "MyParam.h"

#include <cassert>
#include <iostream>
#include <robot.h>
#include <tgf.h>
#include <portability.h>

// The "AXIOM" logger instance.
extern GfLogger* PLogUSR;
#define LogUSR (*PLogUSR)

MyParam::MyParam(void** carparmhandle, const std::string& datadir, const std::string& trackname, unsigned int weather)
{
    if (weather < 1)
    {
        std::string trackfile = datadir + trackname + ".xml";
        std::string defaultfile = datadir + "default.xml";
        LogUSR.info(" Load track file : %s\n", trackfile.c_str());
        LogUSR.info(" Load default file : %s\n", defaultfile.c_str());

        *carparmhandle = GfParmReadFile(trackfile.c_str(), GFPARM_RMODE_STD);

        if (*carparmhandle == NULL)
        {
            LogUSR.info("Couldn't load : %s\n", trackfile.c_str());
            *carparmhandle = GfParmReadFile(defaultfile.c_str(), GFPARM_RMODE_STD);

            if (*carparmhandle == NULL)
                LogUSR.fatal("Couldn't load : %s\n", defaultfile.c_str());
            else
                LogUSR.info("Loaded : %s\n", defaultfile.c_str());
        }
        else
            LogUSR.info("Loaded : %s\n", trackfile.c_str());
    }
    else
    {
        char weatherStr[8];
        snprintf(weatherStr, 8, "%d", weather);
        std::string weatherstring = std::string(weatherStr);
        std::string trackfilerain = datadir + trackname + "-" + weatherstring + ".xml";
        std::string defaultfilerain = datadir + "default" + "-" + weatherstring + ".xml";
        std::string defaultfile = datadir + "default.xml";
        LogUSR.info(" Load track file rain : %s\n", trackfilerain.c_str());
        LogUSR.info(" Load default file rain : %s\n", defaultfilerain.c_str());
        LogUSR.info(" Load default file : %s\n", defaultfile.c_str());

        *carparmhandle = GfParmReadFile(trackfilerain.c_str(), GFPARM_RMODE_STD);

        if (*carparmhandle == NULL)
        {
            LogUSR.info("Couldn't load : %s\n", trackfilerain.c_str());
            *carparmhandle = GfParmReadFile(defaultfilerain.c_str(), GFPARM_RMODE_STD);

            if (*carparmhandle == NULL)
            {
                LogUSR.info("Couldn't load : %s\n", defaultfilerain.c_str());
                *carparmhandle = GfParmReadFile(defaultfile.c_str(), GFPARM_RMODE_STD);

                if (*carparmhandle == NULL)
                    LogUSR.fatal("Couldn't load : %s\n", defaultfile.c_str());
                else
                    LogUSR.info("Loaded : %s\n", defaultfile.c_str());
            }
            else
                LogUSR.info("Loaded : %s\n", defaultfilerain.c_str());
        }
        else
            LogUSR.info("Loaded : %s\n", trackfilerain.c_str());
    }

    mCarParmHandle = *carparmhandle;
}

double MyParam::getNum(const std::string& sect, const std::string& att) const
{
  double value = GfParmGetNum(mCarParmHandle, sect.c_str(), att.c_str(), (char*)NULL, 0.0);

  if (value == 0.0)
  {
    LogUSR.info("Get %s : %s = 0 Check if this parameter should be 0\n", sect.c_str(), att.c_str());
  }
  else
  {
    //std::cout << "Get " << sect << " " << att << " " << value << std::endl;
    LogUSR.info("Get %s : %s = %.3f\n", sect.c_str(), att.c_str(), value);
  }

  return value;
}

void MyParam::setNum(const std::string& sect, const std::string& att, double value) const
{
  GfParmSetNum(mCarParmHandle, sect.c_str(), att.c_str(), (char*)NULL, (tdble)value); // (tdble) for VS 2013 compatibility
  //std::cout << "Set " << sect << " " << att << " " << value << std::endl;
  LogUSR.info("Get %s : %s = %.7f\n", sect.c_str(), att.c_str(), value);
}
