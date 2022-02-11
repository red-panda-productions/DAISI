/***************************************************************************

    file                 : datalog.cpp
    created              : 2011-07-20 06:35:00 UTC
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

#include "datalog.h"

#include <iostream>
#include <string.h>
#include <fstream>

#include <tgf.h>

DataLog::DataLog() :
    mLogLine(0),
    mMaxLines(3000)
{
}

void DataLog::init(const std::string &dir, const std::string &carname)
{
    mDir = dir;
    mFile = dir + carname + "_log.dat";
}

void DataLog::add(const std::string &name, double* dataptr, double scale)
{
    DataLogHead hd;
    hd.name = name;
    hd.dataptr = dataptr;
    hd.scale = scale;
    mHead.push_back(hd);
}

void DataLog::update()
{
    for (unsigned i = 0; i < mHead.size(); i++)
    {
        if (mData.size() < mMaxLines * mHead.size())
        {
            mData.push_back(*mHead[i].dataptr * mHead[i].scale);
        }
        else
        {
            mData[mLogLine * mHead.size() + i] = *mHead[i].dataptr * mHead[i].scale;
        }
    }

    mLogLine++;
    mLogLine = mLogLine % mMaxLines;
}

void DataLog::write()
{
    /*if (GfLocalDir(strdup(mDir.c_str())) == GF_DIR_CREATED)
  {
    std::ofstream myfile(mFile);

    for (unsigned i = 0; i < mHead.size(); i++)
    {
      myfile << mHead[i].name << " ";
    }

    myfile << std::endl;

    for (unsigned i = 0; i < mData.size(); i++)
    {
      myfile << mData[i] << " ";

      if (((i + 1) % mHead.size()) == 0)
      {
        myfile << std::endl;
      }
    }
  }
  else
  {
    std::cout << "Error writeLog: unable to create dir" << std::endl;
  }*/
}
