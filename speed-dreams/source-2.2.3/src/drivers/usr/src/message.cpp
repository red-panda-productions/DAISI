/***************************************************************************

    file                 : message.cpp
    created              : 2018-12-30 06:35:00 UTC
    copyright            : (C) Daniel Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "message.h"

#include <iostream>
#include <string.h>
#include <sstream>
#include <iomanip> // std::setprecision
#include <fstream>

Message::Message()
{
}

void Message::init(std::string dir, MyCar* car)
{
/*    mDir = dir;
    mFile = dir + car->car()->_name + "_msg.txt";
    mCar = car;*/
}

void Message::clearPanel() const
{
    //memset((void*)&mCar->car()->_msgCmd, 0, 4 * 32);
}

void Message::displayOnPanel(const std::string& line1, const std::string& line2) const
{
    /*float color[] = {1.0, 1.0, 0.0, 1.0};
    memcpy((void*)mCar->car()->_msgColorCmd, color, sizeof(mCar->car()->_msgColorCmd));
    memcpy((void*)mCar->car()->_msgCmd[0], line1.c_str(), line1.size());
    memcpy((void*)mCar->car()->_msgCmd[1], line2.c_str(), line2.size());*/
}

void Message::print(const double time, const std::string& flags, const std::string& desc, const double value)
{
    /*std::stringstream msgss;
    msgss << std::fixed
          << std::setprecision ( 0 ) << flags << " "
          << std::setprecision ( 3 ) << time << "s" << " "
          << std::setprecision ( 0 ) << mCar->car()->_distFromStartLine << "m" << " "
          << std::setprecision ( 0 ) << desc << " "
          << std::setprecision ( 3 ) << value;*/
    //std::cout << msgss.str() << std::endl;

   /* if (mMsgLog.size() < mMaxLines)
    {
        mMsgLog.push_back(msgss.str());
    }
    else
    {
        mMsgLog[mLogLine] = msgss.str();
    }

    mLogLine++;
    mLogLine = mLogLine % mMaxLines;*/
}

void Message::write()
{
    /*if (GfCreateDir(strdup(mDir.c_str())) == GF_DIR_CREATED) {
    std::ofstream myfile(mFile);
    for (unsigned i = 0; i < mMsgLog.size(); i++) {
      myfile << mMsgLog[i] << " ";
      myfile << std::endl;
    }
  } else {
    std::cout << "Error writeMsgLog: unable to create dir" << std::endl;
  }*/
}
